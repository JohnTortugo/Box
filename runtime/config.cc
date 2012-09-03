/////////////////////////////////////////////////////////////////////////
// $Id: config.cc 11347 2012-08-19 11:45:50Z vruppert $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2012  The Bochs Project
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

#include "bochs.h"
#include "bxversion.h"
#include "iodev/iodev.h"
#include "param_names.h"
#include <assert.h>

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#if defined(macintosh)
// Work around a bug in SDL 1.2.4 on MacOS X, which redefines getenv to
// SDL_getenv, but then neglects to provide SDL_getenv.  It happens
// because we are defining -Dmacintosh.
#undef getenv
#endif


int bochsrc_include_count = 0;
#if BX_PLUGINS
Bit8u bx_user_plugin_count = 0;
#endif

#define LOG_THIS genlog->

extern bx_debug_t bx_dbg;

static const char *get_builtin_variable(const char *varname);
static int parse_line_unformatted(const char *context, char *line);
static int parse_line_formatted(const char *context, int num_params, char *params[]);
static int parse_bochsrc(const char *rcfile);
static int get_floppy_type_from_image(const char *filename);

static Bit64s bx_param_handler(bx_param_c *param, int set, Bit64s val)
{
  char pname[BX_PATHNAME_LEN];
  Bit8u device;
//Bit8u channel;

  bx_list_c *base = (bx_list_c*) param->get_parent();
  base->get_param_path(pname, BX_PATHNAME_LEN);
  if (!strncmp(pname, "ata.", 4)) {
//  channel = pname[4] - '0';
    if (!strcmp(base->get_name(), "master")) {
      device = 0;
    } else {
      device = 1;
    }
    if (!strcmp(param->get_name(), "type")) {
      if (set) {
        switch (val) {
          case BX_ATA_DEVICE_DISK:
            ((bx_param_filename_c*)SIM->get_param("path", base))->set_extension("img");
            break;
          case BX_ATA_DEVICE_CDROM:
            ((bx_param_filename_c*)SIM->get_param("path", base))->set_extension("iso");
            break;
        }
      }
    } else {
      BX_PANIC(("bx_param_handler called with unknown parameter '%s.%s'", pname, param->get_name()));
      return -1;
    }
  } else {
    param->get_param_path(pname, BX_PATHNAME_LEN);
    if ((!strcmp(pname, BXPN_FLOPPYA_TYPE)) ||
        (!strcmp(pname, BXPN_FLOPPYB_TYPE))) {
      if (set) {
        if (val == BX_FLOPPY_AUTO) {
          val = get_floppy_type_from_image(SIM->get_param_string("path", base)->getptr());
          SIM->get_param_enum("type", base)->set(val);
        } else if (!SIM->get_init_done() && (val != BX_FLOPPY_NONE)) {
          switch (val) {
            case BX_FLOPPY_2_88:
              device = BX_FDD_350ED;
              break;
            case BX_FLOPPY_720K:
            case BX_FLOPPY_1_44:
              device = BX_FDD_350HD;
              break;
            default:
              device = BX_FDD_525HD;
          }
          SIM->get_param_enum("devtype", base)->set(device);
        }
      }
    } else {
      BX_PANIC(("bx_param_handler called with unknown parameter '%s'", pname));
      return -1;
    }
  }
  return val;
}

const char *bx_param_string_handler(bx_param_string_c *param, int set,
                                    const char *oldval, const char *val, int maxlen)
{
  char pname[BX_PATHNAME_LEN];

  param->get_param_path(pname, BX_PATHNAME_LEN);
  if (!strcmp(pname, BXPN_SCREENMODE)) {
    if (set==1) {
      BX_INFO(("Screen mode changed to %s", val));
    }
#if BX_PLUGINS
  } else if (!strncmp(pname, "misc.user_plugin", 16)) {
    if ((strlen(oldval) > 0) && (strcmp(oldval, "none"))) {
      PLUG_unload_user_plugin(oldval);
    }
    if ((strlen(val) > 0) && (strcmp(val, "none"))) {
      PLUG_load_user_plugin(val);
    }
#endif
  } else {
    BX_PANIC(("bx_param_string_handler called with unknown parameter '%s'", pname));
  }
  return val;
}

void bx_init_std_nic_options(const char *name, bx_list_c *menu)
{
  // networking module choices
  static const char *eth_module_list[] = {
    "null",
#if BX_NETMOD_LINUX
    "linux",
#endif
#if BX_NETMOD_TAP
    "tap",
#endif
#if BX_NETMOD_TUNTAP
    "tuntap",
#endif
#if BX_NETMOD_WIN32
    "win32",
#endif
#if BX_NETMOD_FBSD
    "fbsd",
#endif
#if BX_NETMOD_VDE
    "vde",
#endif
#if BX_NETMOD_SLIRP
    "slirp",
#endif
    "vnet",
    NULL
  };

  bx_param_enum_c *ethmod;
  bx_param_string_c *macaddr;
  bx_param_filename_c *path, *bootrom;
  char descr[120];

  sprintf(descr, "MAC address of the %s device. Don't use an address of a machine on your net.", name);
  macaddr = new bx_param_string_c(menu,
    "macaddr",
    "MAC Address",
    descr,
    "", 6);
  macaddr->set_options(macaddr->RAW_BYTES);
  macaddr->set_initial_val("\xfe\xfd\xde\xad\xbe\xef");
  macaddr->set_separator(':');
  ethmod = new bx_param_enum_c(menu,
    "ethmod",
    "Ethernet module",
    "Module used for the connection to the real net.",
    eth_module_list,
    0,
    0);
  ethmod->set_by_name("null");
  ethmod->set_ask_format("Choose ethernet module for the device [%s] ");
  new bx_param_string_c(menu,
    "ethdev",
    "Ethernet device",
    "Device used for the connection to the real net. This is only valid if an ethernet module other than 'null' is used.",
    "xl0", BX_PATHNAME_LEN);
  path = new bx_param_filename_c(menu,
    "script",
    "Device configuration script",
    "Name of the script that is executed after Bochs initializes the network interface (optional).",
    "none", BX_PATHNAME_LEN);
  path->set_ask_format("Enter new script name, or 'none': [%s] ");
  bootrom = new bx_param_filename_c(menu,
    "bootrom",
    "Boot ROM image",
    "Pathname of network boot ROM image to load",
    "", BX_PATHNAME_LEN);
  bootrom->set_format("Name of boot ROM image: %s");
}

void bx_init_usb_options(const char *usb_name, const char *pname, int maxports)
{
  char group[16], name[8], descr[512], label[512];

  bx_param_c *usb = SIM->get_param("ports.usb");
  sprintf(group, "USB %s", usb_name);
  sprintf(label, "%s Configuration", usb_name);
  bx_list_c *menu = new bx_list_c(usb, pname, label);
  menu->set_options(menu->SHOW_PARENT);
  sprintf(label, "Enable %s emulation", usb_name);
  sprintf(descr, "Enables the %s emulation", usb_name);
  bx_param_bool_c *enabled = new bx_param_bool_c(menu, "enabled", label, descr, 0);
  bx_list_c *deplist = new bx_list_c(NULL);
  for (int i = 0; i < maxports; i++) {
    sprintf(name, "port%d", i+1);
    sprintf(label, "Port #%d Configuration", i+1);
    sprintf(descr, "Device connected to %s port #%d and it's options", usb_name, i+1);
    bx_list_c *port = new bx_list_c(menu, name, label);
    port->set_options(port->SERIES_ASK | port->USE_BOX_TITLE);
    sprintf(descr, "Device connected to %s port #%d", usb_name, i+1);
    bx_param_string_c *device = new bx_param_string_c(port, "device", "Device",
                                                      descr, "", BX_PATHNAME_LEN);
    sprintf(descr, "Options for device connected to %s port #%d", usb_name, i+1);
    bx_param_string_c *options = new bx_param_string_c(port, "options", "Options",
                                                       descr, "", BX_PATHNAME_LEN);
    port->set_group(group);
    deplist->add(port);
    deplist->add(device);
    deplist->add(options);
  }
  enabled->set_dependent_list(deplist);
}

void bx_plugin_ctrl_reset(bx_bool init_done)
{
  bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_PLUGIN_CTRL);
  if (init_done) {
    for (int i = 0; i < base->get_size(); i++) {
      ((bx_param_bool_c*)base->get(i))->set(0);
    }
    SIM->opt_plugin_ctrl("*", 0);
  }
  // add the default set of plugins to the list
  new bx_param_bool_c(base, "unmapped", "", "", 1);
  new bx_param_bool_c(base, "biosdev", "", "", 1);
  new bx_param_bool_c(base, "speaker", "", "", 1);
  new bx_param_bool_c(base, "extfpuirq", "", "", 1);
  new bx_param_bool_c(base, "parallel", "", "", 1);
  new bx_param_bool_c(base, "serial", "", "", 1);
#if BX_SUPPORT_GAMEPORT
  new bx_param_bool_c(base, "gameport", "", "", 1);
#endif
#if BX_SUPPORT_IODEBUG && BX_DEBUGGER
  new bx_param_bool_c(base, "iodebug", "", "", 1);
#endif
  if (init_done) {
    SIM->opt_plugin_ctrl("*", 1);
  }
}

void bx_init_options()
{
  int i;
  bx_list_c *menu;
  bx_list_c *deplist;
  bx_param_num_c *ioaddr, *ioaddr2, *irq;
  bx_param_bool_c *enabled, *readonly, *status;
  bx_param_enum_c *mode, *type, *toggle;
  bx_param_filename_c *path;
  char name[BX_PATHNAME_LEN], descr[512], label[512];

  bx_param_c *root_param = SIM->get_param(".");

  // general options subtree
  menu = new bx_list_c(root_param, "general", "");

 // config interface option, set in bochsrc or command line
  static const char *config_interface_list[] = {
#ifdef WIN32
    "win32config",
#endif
#if BX_USE_TEXTCONFIG
    "textconfig",
#endif
#if BX_WITH_WX
    "wx",
#endif
    NULL
  };
  bx_param_enum_c *sel_config = new bx_param_enum_c(menu,
    "config_interface", "Configuration interface",
    "Select configuration interface",
    config_interface_list,
    0,
    0);
  sel_config->set_by_name(BX_DEFAULT_CONFIG_INTERFACE);

  static const char *bochs_start_names[] = { "quick", "load", "edit", "run" };

  // quick start option, set by command line arg
  new bx_param_enum_c(menu,
      "start_mode",
      "Bochs start types",
      "Bochs start types",
      bochs_start_names,
      BX_RUN_START,
      BX_QUICK_START);

  new bx_param_bool_c(menu,
      "restore",
      "Restore Bochs session",
      "Restore Bochs session",
      0);
  new bx_param_string_c(menu,
    "restore_path",
    "Path to data for restore",
    "Path to data for restore",
    "",
    BX_PATHNAME_LEN);

  // benchmarking mode, set by command line arg
  new bx_param_num_c(menu,
      "benchmark",
      "benchmark mode",
      "set benchmark mode",
      0, BX_MAX_BIT32U, 0);

  // subtree for setting up log actions by device in bochsrc
  bx_list_c *logfn = new bx_list_c(menu, "logfn", "Logfunctions");
  new bx_list_c(logfn, "debug", "");
  new bx_list_c(logfn, "info", "");
  new bx_list_c(logfn, "error", "");
  new bx_list_c(logfn, "panic", "");

  // optional plugin control
  new bx_list_c(menu, "plugin_ctrl", "Optional Plugin Control");
  bx_plugin_ctrl_reset(0);

  // subtree for special menus
  bx_list_c *special_menus = new bx_list_c(root_param, "menu", "");

#if BX_SUPPORT_SMP
  #define BX_CPU_PROCESSORS_LIMIT 255
  #define BX_CPU_CORES_LIMIT 8
  #define BX_CPU_HT_THREADS_LIMIT 4
#else
  #define BX_CPU_PROCESSORS_LIMIT 1
  #define BX_CPU_CORES_LIMIT 1
  #define BX_CPU_HT_THREADS_LIMIT 1
#endif

  // cpu subtree
  bx_list_c *cpu_param = new bx_list_c(root_param, "cpu", "CPU Options");

  static const char *cpu_names[] = {
#define bx_define_cpudb(model) #model,
#include "cpudb.h"
    NULL
  };
#undef bx_define_cpudb

  new bx_param_enum_c(cpu_param,
      "model", "CPU configuration",
      "Choose pre-defined CPU configuration",
      cpu_names, 0, 0);

  // cpu options
  bx_param_num_c *nprocessors = new bx_param_num_c(cpu_param,
      "n_processors", "Number of processors in SMP mode",
      "Sets the number of processors for multiprocessor emulation",
      1, BX_CPU_PROCESSORS_LIMIT,
      1);
  nprocessors->set_enabled(BX_CPU_PROCESSORS_LIMIT > 1);
  bx_param_num_c *ncores = new bx_param_num_c(cpu_param,
      "n_cores", "Number of cores in each processor in SMP mode",
      "Sets the number of cores per processor for multiprocessor emulation",
      1, BX_CPU_CORES_LIMIT,
      1);
  ncores->set_enabled(BX_CPU_CORES_LIMIT > 1);
  bx_param_num_c *nthreads = new bx_param_num_c(cpu_param,
      "n_threads", "Number of HT threads per each core in SMP mode",
      "Sets the number of HT (Intel(R) HyperThreading Technology) threads per core for multiprocessor emulation",
      1, BX_CPU_HT_THREADS_LIMIT,
      1);
  nthreads->set_enabled(BX_CPU_HT_THREADS_LIMIT > 1);
  new bx_param_num_c(cpu_param,
      "ips", "Emulated instructions per second (IPS)",
      "Emulated instructions per second, used to calibrate bochs emulated time with wall clock time.",
      BX_MIN_IPS, BX_MAX_BIT32U,
      4000000);
#if BX_SUPPORT_SMP
  new bx_param_num_c(cpu_param,
      "quantum", "Quantum ticks in SMP simulation",
      "Maximum amount of instructions allowed to execute before returning control to another CPU.",
      BX_SMP_QUANTUM_MIN, BX_SMP_QUANTUM_MAX,
      16);
#endif
  new bx_param_bool_c(cpu_param,
      "reset_on_triple_fault", "Enable CPU reset on triple fault",
      "Enable CPU reset if triple fault occured (highly recommended)",
      1);
#if BX_CPU_LEVEL >= 5
  new bx_param_bool_c(cpu_param,
      "ignore_bad_msrs", "Ignore RDMSR / WRMSR to unknown MSR register",
      "Ignore RDMSR/WRMSR to unknown MSR register",
      1);
#endif
  new bx_param_bool_c(cpu_param,
      "cpuid_limit_winnt", "Limit max CPUID function to 3",
      "Limit max CPUID function reported to 3 to workaround WinNT issue",
      0);
#if BX_SUPPORT_MONITOR_MWAIT
  new bx_param_bool_c(cpu_param,
      "mwait_is_nop", "Don't put CPU to sleep state by MWAIT",
      "Don't put CPU to sleep state by MWAIT",
      0);
#endif
#if BX_CONFIGURE_MSRS
  new bx_param_filename_c(cpu_param,
      "msrs",
      "Configurable MSR definition file",
      "Set path to the configurable MSR definition file",
      "", BX_PATHNAME_LEN);
#endif

  cpu_param->set_options(menu->SHOW_PARENT);

  // cpuid subtree
#if BX_CPU_LEVEL >= 4
  bx_list_c *cpuid_param = new bx_list_c(root_param, "cpuid", "CPUID Options");

  new bx_param_string_c(cpuid_param,
      "vendor_string",
      "CPUID vendor string",
      "Set the CPUID vendor string",
#if BX_CPU_VENDOR_INTEL
      "GenuineIntel", 
#else
      "AuthenticAMD", 
#endif
      BX_CPUID_VENDOR_LEN+1);
  new bx_param_string_c(cpuid_param,
      "brand_string",
      "CPUID brand string",
      "Set the CPUID brand string",
#if BX_CPU_VENDOR_INTEL
      "              Intel(R) Pentium(R) 4 CPU        ", 
#else
      "AMD Athlon(tm) processor",
#endif
      BX_CPUID_BRAND_LEN+1);

  new bx_param_num_c(cpuid_param,
      "stepping", "Stepping ID",
      "Processor 4-bits stepping ID",
      0, 15,
      3);

  new bx_param_num_c(cpuid_param,
      "model", "Model ID",
      "Processor model ID, extended model ID",
      0, 255,
      3);

  new bx_param_num_c(cpuid_param,
      "family", "Family ID",
      "Processor family ID, extended family ID",
      BX_CPU_LEVEL, (BX_CPU_LEVEL >= 6) ? 4095 : BX_CPU_LEVEL,
      BX_CPU_LEVEL);

#if BX_CPU_LEVEL >= 5
  new bx_param_bool_c(cpuid_param,
      "mmx", "Support for MMX instruction set",
      "Support for MMX instruction set",
      1);

  // configure defaults to XAPIC enabled
  static const char *apic_names[] = {
    "legacy",
    "xapic",
#if BX_CPU_LEVEL >= 6
    "xapic_ext",
    "x2apic",
#endif
    NULL
  };

  new bx_param_enum_c(cpuid_param,
      "apic", "APIC configuration",
      "Select APIC configuration (Legacy APIC/XAPIC/XAPIC_EXT/X2APIC)",
      apic_names,
      BX_CPUID_SUPPORT_XAPIC,
      BX_CPUID_SUPPORT_LEGACY_APIC);
#endif

#if BX_CPU_LEVEL >= 6
  // configure defaults to CPU_LEVEL = 6 with SSE2 enabled
  static const char *sse_names[] = { "none", "sse", "sse2", "sse3", "ssse3", "sse4_1", "sse4_2", NULL };
  new bx_param_enum_c(cpuid_param,
      "sse", "Support for SSE instruction set",
      "Support for SSE/SSE2/SSE3/SSSE3/SSE4_1/SSE4_2 instruction set",
      sse_names,
      BX_CPUID_SUPPORT_SSE2,
      BX_CPUID_SUPPORT_NOSSE);

  new bx_param_bool_c(cpuid_param,
      "sse4a", "Support for AMD SSE4A instructions",
      "Support for AMD SSE4A instructions",
      0);

  new bx_param_bool_c(cpuid_param,
      "sep", "Support for SYSENTER/SYSEXIT instructions",
      "Support for SYSENTER/SYSEXIT instructions",
      1);
  new bx_param_bool_c(cpuid_param,
      "movbe", "Support for MOVBE instruction",
      "Support for MOVBE instruction",
      0);
  new bx_param_bool_c(cpuid_param,
      "adx", "Support for ADX instructions",
      "Support for ADCX/ADOX instructions",
      0);
  new bx_param_bool_c(cpuid_param,
      "aes", "Support for AES instruction set",
      "Support for AES instruction set",
      0);
  new bx_param_bool_c(cpuid_param,
      "xsave", "Support for XSAVE extensions",
      "Support for XSAVE extensions",
      0);
  new bx_param_bool_c(cpuid_param,
      "xsaveopt", "Support for XSAVEOPT instruction",
      "Support for XSAVEOPT instruction",
      0);
#if BX_SUPPORT_AVX
  new bx_param_num_c(cpuid_param,
      "avx", "Support for AVX instruction set",
      "Support for AVX instruction set",
      0, 2,
      0);
  new bx_param_bool_c(cpuid_param,
      "avx_f16c", "Support for AVX F16 convert instructions",
      "Support for AVX F16 convert instructions",
      0);
  new bx_param_bool_c(cpuid_param,
      "avx_fma", "Support for AVX FMA instructions",
      "Support for AVX FMA instructions",
      0);
  new bx_param_num_c(cpuid_param,
      "bmi", "Support for BMI instructions",
      "Support for Bit Manipulation Instructions (BMI)",
      0, 2,
      0);
  new bx_param_bool_c(cpuid_param,
      "xop", "Support for AMD XOP instructions",
      "Support for AMD XOP instructions",
      0);
  new bx_param_bool_c(cpuid_param,
      "fma4", "Support for AMD four operand FMA instructions",
      "Support for AMD FMA4 instructions",
      0);
  new bx_param_bool_c(cpuid_param,
      "tbm", "Support for AMD TBM instructions",
      "Support for AMD Trailing Bit Manipulation (TBM) instructions",
      0);
#endif
#if BX_SUPPORT_X86_64
  new bx_param_bool_c(cpuid_param,
      "x86_64", "x86-64 and long mode",
      "Support for x86-64 and long mode",
      1);
  new bx_param_bool_c(cpuid_param,
      "1g_pages", "1G pages support in long mode",
      "Support for 1G pages in long mode",
      0);
  new bx_param_bool_c(cpuid_param,
      "pcid", "PCID support in long mode",
      "Support for process context ID (PCID) in long mode",
      0);
  new bx_param_bool_c(cpuid_param,
      "fsgsbase", "FS/GS BASE access instructions support",
      "FS/GS BASE access instructions support in long mode",
      0);
#endif
  new bx_param_bool_c(cpuid_param,
      "smep", "Supervisor Mode Execution Protection support",
      "Supervisor Mode Execution Protection support",
      0);
#if BX_SUPPORT_MONITOR_MWAIT
  new bx_param_bool_c(cpuid_param,
      "mwait", "MONITOR/MWAIT instructions support",
      "MONITOR/MWAIT instructions support",
      BX_SUPPORT_MONITOR_MWAIT);
#endif
#if BX_SUPPORT_VMX
  new bx_param_num_c(cpuid_param,
      "vmx", "Support for Intel VMX extensions emulation",
      "Support for Intel VMX extensions emulation",
      0, BX_SUPPORT_VMX,
      1);
#endif
#if BX_SUPPORT_SVM
  new bx_param_bool_c(cpuid_param,
      "svm", "Secure Virtual Machine (SVM) emulation support",
      "Secure Virtual Machine (SVM) emulation support",
      0);
#endif
#endif // CPU_LEVEL >= 6

  cpuid_param->set_options(menu->SHOW_PARENT);

  // CPUID subtree depends on CPU model
  SIM->get_param_enum(BXPN_CPU_MODEL)->set_dependent_list(cpuid_param->clone(), 0);
  // enable CPUID subtree only for CPU model choice #0
  SIM->get_param_enum(BXPN_CPU_MODEL)->set_dependent_bitmap(0, BX_MAX_BIT64U);

#endif // CPU_LEVEL >= 4

  // memory subtree
  bx_list_c *memory = new bx_list_c(root_param, "memory", "Memory Options");
  bx_list_c *stdmem = new bx_list_c(memory, "standard", "Standard Options");
  bx_list_c *optrom = new bx_list_c(memory, "optrom", "Optional ROM Images");
  bx_list_c *optram = new bx_list_c(memory, "optram", "Optional RAM Images");
  bx_list_c *ram = new bx_list_c(stdmem, "ram", "RAM size options");
  bx_list_c *rom = new bx_list_c(stdmem, "rom", "BIOS ROM options");
  bx_list_c *vgarom = new bx_list_c(stdmem, "vgarom", "VGABIOS ROM options");

  // memory options (ram & rom)
  bx_param_num_c *ramsize = new bx_param_num_c(ram,
      "size",
      "Memory size (megabytes)",
      "Amount of RAM in megabytes",
      1, ((Bit64u)(1) << BX_PHY_ADDRESS_WIDTH) / (1024*1024),
      BX_DEFAULT_MEM_MEGS);
  ramsize->set_ask_format("Enter memory size (MB): [%d] ");
  ramsize->set_options(ramsize->USE_SPIN_CONTROL);

  bx_param_num_c *host_ramsize = new bx_param_num_c(ram,
      "host_size",
      "Host allocated memory size (megabytes)",
      "Amount of host allocated memory in megabytes",
      1, 2048,
      BX_DEFAULT_MEM_MEGS);
  host_ramsize->set_ask_format("Enter host memory size (MB): [%d] ");
  host_ramsize->set_options(ramsize->USE_SPIN_CONTROL);
  ram->set_options(ram->SERIES_ASK);

  path = new bx_param_filename_c(rom,
      "path",
      "ROM BIOS image",
      "Pathname of ROM image to load",
      "", BX_PATHNAME_LEN);
  path->set_format("Name of ROM BIOS image: %s");
  sprintf(name, "%s/BIOS-bochs-latest", (char *)get_builtin_variable("BXSHARE"));
  path->set_initial_val(name);
  bx_param_num_c *romaddr = new bx_param_num_c(rom,
      "addr",
      "ROM BIOS address",
      "The address at which the ROM image should be loaded",
      0, BX_MAX_BIT32U,
      0);
  romaddr->set_base(16);
  romaddr->set_format("0x%05x");
  romaddr->set_long_format("ROM BIOS address: 0x%05x");
  rom->set_options(rom->SERIES_ASK);

  path = new bx_param_filename_c(vgarom,
      "path",
      "VGA BIOS image",
      "Pathname of VGA ROM image to load",
      "", BX_PATHNAME_LEN);
  path->set_format("Name of VGA BIOS image: %s");
  sprintf(name, "%s/VGABIOS-lgpl-latest", get_builtin_variable("BXSHARE"));
  path->set_initial_val(name);
  vgarom->set_options(vgarom->SERIES_ASK);

  bx_param_num_c *optaddr;

  for (i=0; i<BX_N_OPTROM_IMAGES; i++) {
    sprintf(name, "%d", i+1);
    sprintf(descr, "Pathname of optional ROM image #%d to load", i+1);
    sprintf(label, "Optional ROM image #%d", i+1);
    bx_list_c *optnum1 = new bx_list_c(optrom, name, label);
    path = new bx_param_filename_c(optnum1,
      "path",
      "Path",
      descr,
      "", BX_PATHNAME_LEN);
    sprintf(label, "Name of optional ROM image #%d", i+1);
    strcat(label, " : %s");
    path->set_format(strdup(label));
    sprintf(descr, "The address at which the optional ROM image #%d should be loaded", i+1);
    optaddr = new bx_param_num_c(optnum1,
      "addr",
      "Address",
      descr,
      0, BX_MAX_BIT32U,
      0);
    optaddr->set_base(16);
    optaddr->set_format("0x%05x");
    sprintf(label, "Optional ROM #%d address:", i+1);
    strcat(label, " 0x%05x");
    optaddr->set_long_format(strdup(label));
    optnum1->set_options(optnum1->SERIES_ASK | optnum1->USE_BOX_TITLE);
  }
  optrom->set_options(optrom->SHOW_PARENT);

  for (i=0; i<BX_N_OPTRAM_IMAGES; i++) {
    sprintf(name, "%d", i+1);
    sprintf(descr, "Pathname of optional RAM image #%d to load", i+1);
    sprintf(label, "Optional RAM image #%d", i+1);
    bx_list_c *optnum2 = new bx_list_c(optram, name, label);
    path = new bx_param_filename_c(optnum2,
      "path",
      "Path",
      descr,
      "", BX_PATHNAME_LEN);
    sprintf(label, "Name of optional RAM image #%d", i+1);
    strcat(label, " : %s");
    path->set_format(strdup(label));
    sprintf(descr, "The address at which the optional RAM image #%d should be loaded", i+1);
    optaddr = new bx_param_num_c(optnum2,
      "addr",
      "Address",
      descr,
      0, BX_MAX_BIT32U,
      0);
    optaddr->set_base(16);
    optaddr->set_format("0x%05x");
    sprintf(label, "Optional RAM #%d address:", i+1);
    strcat(label, " 0x%05x");
    optaddr->set_long_format(strdup(label));
    optnum2->set_options(optnum2->SERIES_ASK | optnum2->USE_BOX_TITLE);
  }
  optram->set_options(optram->SHOW_PARENT);
  memory->set_options(memory->SHOW_PARENT | memory->USE_TAB_WINDOW);

  // clock & cmos subtree
  bx_list_c *clock_cmos = new bx_list_c(root_param, "clock_cmos", "Clock & CMOS Options");

  // clock & cmos options
  static const char *clock_sync_names[] = { "none", "realtime", "slowdown", "both", NULL };

  bx_param_enum_c *clock_sync = new bx_param_enum_c(clock_cmos,
      "clock_sync", "Synchronisation method",
      "Host to guest time synchronization method",
      clock_sync_names,
      BX_CLOCK_SYNC_NONE,
      BX_CLOCK_SYNC_NONE);
  bx_param_num_c *time0 = new bx_param_num_c(clock_cmos,
      "time0",
      "Initial CMOS time for Bochs\n(1:localtime, 2:utc, other:time in seconds)",
      "Initial time for Bochs CMOS clock, used if you really want two runs to be identical",
      0, BX_MAX_BIT32U,
      BX_CLOCK_TIME0_LOCAL);
  bx_param_bool_c *rtc_sync = new bx_param_bool_c(clock_cmos,
      "rtc_sync", "Sync RTC speed with realtime",
      "If enabled, the RTC runs at realtime speed",
      0);
  deplist = new bx_list_c(NULL);
  deplist->add(rtc_sync);
  clock_sync->set_dependent_list(deplist, 0);
  clock_sync->set_dependent_bitmap(BX_CLOCK_SYNC_REALTIME, 1);
  clock_sync->set_dependent_bitmap(BX_CLOCK_SYNC_BOTH, 1);

  bx_list_c *cmosimage = new bx_list_c(clock_cmos, "cmosimage", "CMOS Image Options");
  bx_param_bool_c *use_cmosimage = new bx_param_bool_c(cmosimage,
      "enabled", "Use a CMOS image",
      "Controls the usage of a CMOS image",
      0);
  path = new bx_param_filename_c(cmosimage,
      "path", "Pathname of CMOS image",
      "Pathname of CMOS image",
      "", BX_PATHNAME_LEN);
  bx_param_bool_c *rtc_init = new bx_param_bool_c(cmosimage,
      "rtc_init", "Initialize RTC from image",
      "Controls whether to initialize the RTC with values stored in the image",
      0);
  deplist = new bx_list_c(NULL);
  deplist->add(path);
  deplist->add(rtc_init);
  use_cmosimage->set_dependent_list(deplist);

  time0->set_ask_format("Enter Initial CMOS time (1:localtime, 2:utc, other:time in seconds): [%d] ");
  clock_sync->set_ask_format("Enter Synchronisation method: [%s] ");
  clock_cmos->set_options(clock_cmos->SHOW_PARENT);
  cmosimage->set_options(cmosimage->SHOW_PARENT);

  // pci subtree
  bx_list_c *pci = new bx_list_c(root_param, "pci", "PCI Options");

  // pci options
  deplist = new bx_list_c(NULL);

  bx_param_bool_c *i440fx_support = new bx_param_bool_c(pci,
      "i440fx_support",
      "Enable i440FX PCI Support",
      "Controls whether to emulate the i440FX PCI chipset",
      BX_SUPPORT_PCI);
  // pci slots
  bx_list_c *slot = new bx_list_c(pci, "slot", "PCI Slots");
  deplist->add(slot);
  for (i=0; i<BX_N_PCI_SLOTS; i++) {
    sprintf(name, "%d", i+1);
    sprintf (descr, "Name of the device connected to PCI slot #%d", i+1);
    sprintf (label, "PCI slot #%d device", i+1);
    bx_param_string_c *devname = new bx_param_string_c(slot,
        name,
        label,
        descr,
        "", BX_PATHNAME_LEN);
    deplist->add(devname);
  }
  i440fx_support->set_dependent_list(deplist);
  pci->set_options(pci->SHOW_PARENT);
  slot->set_options(slot->SHOW_PARENT);

  // display subtree
  bx_list_c *display = new bx_list_c(root_param, "display", "Bochs Display & Interface Options");

  // this is a list of gui libraries that are known to be available at
  // compile time.  The one that is listed first will be the default,
  // which is used unless the user overrides it on the command line or
  // in a configuration file.
  static const char *display_library_list[] = {
#if BX_WITH_X11
    "x",
#endif
#if BX_WITH_WIN32
    "win32",
#endif
#if BX_WITH_CARBON
    "carbon",
#endif
#if BX_WITH_MACOS
    "macos",
#endif
#if BX_WITH_AMIGAOS
    "amigaos",
#endif
#if BX_WITH_SDL
    "sdl",
#endif
#if BX_WITH_SVGA
    "svga",
#endif
#if BX_WITH_TERM
    "term",
#endif
#if BX_WITH_RFB
    "rfb",
#endif
#if BX_WITH_WX
    "wx",
#endif
#if BX_WITH_NOGUI
    "nogui",
#endif
    NULL
  };
  bx_param_enum_c *sel_displaylib = new bx_param_enum_c(display,
    "display_library", "VGA Display Library",
    "Select VGA Display Library",
    display_library_list,
    0,
    0);
  sel_displaylib->set_by_name(BX_DEFAULT_DISPLAY_LIBRARY);
  sel_displaylib->set_ask_format("Choose which library to use for the Bochs display: [%s] ");

  new bx_param_string_c(display,
    "displaylib_options", "Display Library options",
    "Options passed to Display Library",
    "",
    BX_PATHNAME_LEN);

  new bx_param_bool_c(display,
      "private_colormap", "Use a private colormap",
      "Request that the GUI create and use it's own non-shared colormap. This colormap will be used when in the bochs window. If not enabled, a shared colormap scheme may be used. Not implemented on all GUI's.",
      0);

  bx_param_bool_c *fullscreen = new bx_param_bool_c(display,
      "fullscreen", "Use full screen mode",
      "When enabled, bochs occupies the whole screen instead of just a window.",
      0);
  bx_param_string_c *screenmode = new bx_param_string_c(display,
      "screenmode",
      "Screen mode name",
      "Screen mode name",
      "", BX_PATHNAME_LEN);
  screenmode->set_handler(bx_param_string_handler);
#if !BX_WITH_AMIGAOS
  fullscreen->set_enabled(0);
  screenmode->set_enabled(0);
#endif

  bx_param_num_c *vga_update_freq = new bx_param_num_c(display,
      "vga_update_frequency",
      "VGA Update Frequency",
      "Number of VGA updates per emulated second",
      1, 60,
      5);
  vga_update_freq->set_ask_format ("Type a new value for VGA update frequency: [%d] ");

  bx_param_string_c *vga_extension = new bx_param_string_c(display,
                "vga_extension",
                "VGA Extension",
                "Name of the VGA extension",
                "none", BX_PATHNAME_LEN);
  vga_extension->set_initial_val("vbe");
  display->set_options(display->SHOW_PARENT);

  // keyboard & mouse subtree
  bx_list_c *kbd_mouse = new bx_list_c(root_param, "keyboard_mouse", "Keyboard & Mouse Options");
  bx_list_c *keyboard = new bx_list_c(kbd_mouse, "keyboard", "Keyboard Options");
  bx_list_c *mouse = new bx_list_c(kbd_mouse, "mouse", "Mouse Options");

  static const char *keyboard_type_names[] = { "xt", "at", "mf", NULL };

  // keyboard & mouse options
  type = new bx_param_enum_c(keyboard,
      "type", "Keyboard type",
      "Keyboard type reported by the 'identify keyboard' command",
      keyboard_type_names,
      BX_KBD_MF_TYPE,
      BX_KBD_XT_TYPE);
  type->set_ask_format ("Enter keyboard type: [%s] ");

  new bx_param_num_c(keyboard,
      "serial_delay", "Keyboard serial delay",
      "Approximate time in microseconds that it takes one character to be transferred from the keyboard to controller over the serial path.",
      1, BX_MAX_BIT32U,
      250);
  new bx_param_num_c(keyboard,
      "paste_delay", "Keyboard paste delay",
      "Approximate time in microseconds between attemps to paste characters to the keyboard controller.",
      1000, BX_MAX_BIT32U,
      100000);
  bx_param_bool_c *use_kbd_mapping = new bx_param_bool_c(keyboard,
      "use_mapping", "Use keyboard mapping",
      "Controls whether to use the keyboard mapping feature",
      0);
  bx_param_filename_c *keymap = new bx_param_filename_c(keyboard,
      "keymap", "Keymap filename",
      "Pathname of the keymap file used",
      "", BX_PATHNAME_LEN);
  keymap->set_extension("map");
  deplist = new bx_list_c(NULL);
  deplist->add(keymap);
  use_kbd_mapping->set_dependent_list(deplist);

  bx_param_string_c *user_shortcut = new bx_param_string_c(keyboard,
      "user_shortcut",
      "Userbutton shortcut",
      "Defines the keyboard shortcut to be sent when you press the 'user' button in the headerbar.",
      "none", 20);
  user_shortcut->set_runtime_param(1);

  static const char *mouse_type_list[] = {
    "none",
    "ps2",
    "imps2",
#if BX_SUPPORT_BUSMOUSE
    "bus",
#endif
    "serial",
    "serial_wheel",
    "serial_msys",
    NULL
  };
  type = new bx_param_enum_c(mouse,
      "type", "Mouse type",
      "The mouse type can be one of these: 'none', 'ps2', 'imps2', 'serial', 'serial_wheel'"
#if BX_SUPPORT_BUSMOUSE
      ", 'bus'"
#endif
      ,
      mouse_type_list,
      BX_MOUSE_TYPE_PS2,
      BX_MOUSE_TYPE_NONE);
  type->set_ask_format("Choose the type of mouse [%s] ");

  new bx_param_bool_c(mouse,
      "enabled", "Enable mouse capture",
      "Controls whether the mouse sends events to the guest. The hardware emulation is always enabled.",
      0);

  static const char *mouse_toggle_list[] = {
    "ctrl+mbutton",
    "ctrl+f10",
    "ctrl+alt",
    "f12",
    NULL
  };
  toggle = new bx_param_enum_c(mouse,
      "toggle", "Mouse toggle method",
      "The mouse toggle method can be one of these: 'ctrl+mbutton', 'ctrl+f10', 'ctrl+alt'",
      mouse_toggle_list,
      BX_MOUSE_TOGGLE_CTRL_MB,
      BX_MOUSE_TOGGLE_CTRL_MB);
  toggle->set_ask_format("Choose the mouse toggle method [%s] ");

  kbd_mouse->set_options(kbd_mouse->SHOW_PARENT);
  keyboard->set_options(keyboard->SHOW_PARENT);
  mouse->set_options(mouse->SHOW_PARENT);

  // boot parameter subtree
  bx_list_c *boot_params = new bx_list_c(root_param, "boot_params", "Boot Options");
  // boot sequence
  for (i=0; i<3; i++) {
    sprintf(name, "boot_drive%d", i+1);
    sprintf(label, "Boot drive #%d", i+1);
    sprintf(descr, "Name of drive #%d in boot sequence (A, C or CD)", i+1);
    bx_param_enum_c *bootdrive = new bx_param_enum_c(boot_params,
        name,
        label,
        descr,
        &bochs_bootdisk_names[(i==0)?BX_BOOT_FLOPPYA:BX_BOOT_NONE],
        (i==0)?BX_BOOT_FLOPPYA:BX_BOOT_NONE,
        (i==0)?BX_BOOT_FLOPPYA:BX_BOOT_NONE);
    bootdrive->set_ask_format("Boot from floppy drive, hard drive or cdrom ? [%s] ");
  }

  new bx_param_bool_c(boot_params,
      "floppy_sig_check",
      "Skip Floppy Boot Signature Check",
      "Skips check for the 0xaa55 signature on floppy boot device.",
      0);

  // loader hack
  bx_list_c *load32bitos = new bx_list_c(boot_params, "load32bitos", "32-bit OS Loader Hack");

  static const char *loader_os_names[] = { "none", "linux", "nullkernel", NULL };

  bx_param_enum_c *whichOS = new bx_param_enum_c(load32bitos,
      "which",
      "Which operating system?",
      "Which OS to boot",
      loader_os_names,
      Load32bitOSNone,
      Load32bitOSNone);
  path = new bx_param_filename_c(load32bitos,
      "path",
      "Pathname of OS to load",
      "Pathname of the 32-bit OS to load",
      "", BX_PATHNAME_LEN);
  bx_param_filename_c *iolog = new bx_param_filename_c(load32bitos,
      "iolog",
      "Pathname of I/O log file",
      "I/O logfile used for initializing the hardware",
      "", BX_PATHNAME_LEN);
  bx_param_filename_c *initrd = new bx_param_filename_c(load32bitos,
      "initrd",
      "Pathname of initrd",
      "Pathname of the initial ramdisk",
      "", BX_PATHNAME_LEN);
  whichOS->set_ask_format("Enter OS to load: [%s] ");
  path->set_ask_format("Enter pathname of OS: [%s]");
  iolog->set_ask_format("Enter pathname of I/O log: [%s] ");
  initrd->set_ask_format("Enter pathname of initrd: [%s] ");
  load32bitos->set_options(menu->SERIES_ASK);
  whichOS->set_dependent_list(load32bitos->clone(), 1);
  whichOS->set_dependent_bitmap(Load32bitOSNone, 0);
  whichOS->set(Load32bitOSNone);
  boot_params->set_options(menu->SHOW_PARENT);

  // floppy subtree
  bx_list_c *floppy = new bx_list_c(root_param, "floppy", "Floppy Options");
  new bx_list_c(floppy, "0", "First Floppy Drive");
  new bx_list_c(floppy, "1", "Second Floppy Drive");

  bx_param_enum_c *devtype;
  // floppy options
  for (i = 0; i < 2; i++) {

    bx_list_c *floppyX = (bx_list_c*)floppy->get(i);

    devtype = new bx_param_enum_c(floppyX,
      "devtype",
      "Type of floppy drive",
      "Type of floppy drive",
      floppy_devtype_names,
      BX_FDD_NONE,
      BX_FDD_NONE);
    devtype->set_ask_format("What type of floppy drive? [%s] ");

    if (i == 0) {
      strcpy(label, "First floppy image/device");
      strcpy(descr, "Pathname of first floppy image file or device.  If you're booting from floppy, this should be a bootable floppy.");
    } else {
      strcpy(label, "Second floppy image/device");
      strcpy(descr, "Pathname of second floppy image file or device.");
    }
    path = new bx_param_filename_c(floppyX, "path", label, descr, "", BX_PATHNAME_LEN);
    path->set_ask_format("Enter new filename, or 'none' for no disk: [%s] ");
    path->set_extension("img");
    path->set_initial_val("none");

    type = new bx_param_enum_c(floppyX,
      "type",
      "Type of floppy media",
      "Type of floppy media",
      floppy_type_names,
      BX_FLOPPY_NONE,
      BX_FLOPPY_NONE);
    type->set_ask_format("What type of floppy media? (auto=detect) [%s] ");
    type->set_handler(bx_param_handler);
    type->set_runtime_param(1);

    readonly = new bx_param_bool_c(floppyX,
      "readonly",
      "Write Protection",
      "Floppy media write protection",
      0);
    readonly->set_ask_format("Is media write protected? [%s] ");

    status = new bx_param_bool_c(floppyX,
      "status",
      "Inserted",
      "Floppy media status (inserted / ejected)",
      0);
    status->set_ask_format("Is media inserted in drive? [%s] ");

    deplist = new bx_list_c(NULL);
    deplist->add(path);
    devtype->set_dependent_list(deplist, 1);
    devtype->set_dependent_bitmap(BX_FDD_NONE, 0);

    deplist = new bx_list_c(NULL);
    deplist->add(type);
    deplist->add(readonly);
    deplist->add(status);
    path->set_dependent_list(deplist);

    floppyX->set_options(floppyX->SERIES_ASK | floppyX->USE_BOX_TITLE);
  }

  floppy->set_options(floppy->SHOW_PARENT);

  // ATA/ATAPI subtree
  bx_list_c *ata = new bx_list_c(root_param, "ata", "ATA/ATAPI Options");
  ata->set_options(ata->USE_TAB_WINDOW);

  // disk options
  const char *s_atachannel[] = {
    "ATA channel 0",
    "ATA channel 1",
    "ATA channel 2",
    "ATA channel 3",
    };
  const char *s_atadevname[2] = {
    "master",
    "slave",
  };
  const char *s_atadevice[4][2] = {
    { "First HD/CD on channel 0",
      "Second HD/CD on channel 0" },
    { "First HD/CD on channel 1",
    "Second HD/CD on channel 1" },
    { "First HD/CD on channel 2",
    "Second HD/CD on channel 2" },
    { "First HD/CD on channel 3",
    "Second HD/CD on channel 3" }
    };
  Bit16u ata_default_ioaddr1[4] = {
    0x1f0, 0x170, 0x1e8, 0x168
  };
  Bit16u ata_default_ioaddr2[4] = {
    0x3f0, 0x370, 0x3e0, 0x360
  };
  Bit8u ata_default_irq[4] = {
    14, 15, 11, 9
  };

  #define BXP_PARAMS_PER_ATA_DEVICE 12

  bx_list_c *ata_menu[BX_MAX_ATA_CHANNEL];
  bx_list_c *ata_res[BX_MAX_ATA_CHANNEL];

  for (Bit8u channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {

    sprintf(name, "%d", channel);
    ata_menu[channel] = new bx_list_c(ata, name, s_atachannel[channel]);
    ata_menu[channel]->set_options(bx_list_c::USE_TAB_WINDOW);
    ata_res[channel] = new bx_list_c(ata_menu[channel], "resources", s_atachannel[channel]);
    ata_res[channel]->set_options(bx_list_c::SERIES_ASK);

    enabled = new bx_param_bool_c(ata_res[channel],
      "enabled",
      "Enable ATA channel",
      "Controls whether ata channel is installed or not",
      0);
    enabled->set_ask_format("Channel is enabled: [%s] ");

    ioaddr = new bx_param_num_c(ata_res[channel],
      "ioaddr1",
      "I/O Address 1",
      "IO adress of ata command block",
      0, 0xffff,
      ata_default_ioaddr1[channel]);
    ioaddr->set_base(16);
    ioaddr->set_ask_format("Enter new ioaddr1: [0x%x] ");

    ioaddr2 = new bx_param_num_c(ata_res[channel],
      "ioaddr2",
      "I/O Address 2",
      "IO adress of ata control block",
      0, 0xffff,
      ata_default_ioaddr2[channel]);
    ioaddr2->set_base(16);
    ioaddr2->set_ask_format("Enter new ioaddr2: [0x%x] ");

    irq = new bx_param_num_c(ata_res[channel],
      "irq",
      "IRQ",
      "IRQ used by this ata channel",
      0, 15,
      ata_default_irq[channel]);
    irq->set_ask_format("Enter new IRQ: [%d] ");
    irq->set_options(irq->USE_SPIN_CONTROL);

    // all items in the ata[channel] menu depend on the enabled flag.
    // The menu list is complete, but a few dependent_list items will
    // be added later.  Use clone() to make a copy of the dependent_list
    // so that it can be changed without affecting the menu.
    enabled->set_dependent_list(ata_res[channel]->clone());

    for (Bit8u slave=0; slave<2; slave++) {
      menu = new bx_list_c(ata_menu[channel],
          s_atadevname[slave],
          s_atadevice[channel][slave]);
      menu->set_options(menu->SERIES_ASK);

      bx_param_bool_c *present = new bx_param_bool_c(menu,
        "present",
        "Enable this device",
        "Controls whether ata device is installed or not",
        0);
      present->set_ask_format("Device is enabled: [%s] ");

      static const char *atadevice_type_names[] = { "disk", "cdrom", NULL };

      type = new bx_param_enum_c(menu,
        "type",
        "Type of ATA device",
        "Type of ATA device (disk or cdrom)",
        atadevice_type_names,
        BX_ATA_DEVICE_DISK,
        BX_ATA_DEVICE_DISK);
      type->set_ask_format("Enter type of ATA device, disk or cdrom: [%s] ");

      path = new bx_param_filename_c(menu,
        "path",
        "Path or physical device name",
        "Pathname of the image or physical device (cdrom only)",
        "", BX_PATHNAME_LEN);
      path->set_ask_format("Enter new filename: [%s] ");
      path->set_extension("img");

      mode = new bx_param_enum_c(menu,
        "mode",
        "Type of disk image",
        "Mode of the ATA harddisk",
        hdimage_mode_names,
        BX_HDIMAGE_MODE_FLAT,
        BX_HDIMAGE_MODE_FLAT);
      mode->set_ask_format("Enter mode of ATA device, (flat, concat, etc.): [%s] ");

      status = new bx_param_bool_c(menu,
        "status",
        "Inserted",
        "CD-ROM media status (inserted / ejected)",
        0);
      status->set_ask_format("Is media inserted in drive? [%s] ");

      bx_param_filename_c *journal = new bx_param_filename_c(menu,
        "journal",
        "Path of journal file",
        "Pathname of the journal file",
        "", BX_PATHNAME_LEN);
      journal->set_ask_format("Enter path of journal file: [%s]");
      deplist = new bx_list_c(NULL);
      deplist->add(journal);
      mode->set_dependent_list(deplist, 0);
      mode->set_dependent_bitmap(BX_HDIMAGE_MODE_UNDOABLE, 1);
      mode->set_dependent_bitmap(BX_HDIMAGE_MODE_VOLATILE, 1);
      mode->set_dependent_bitmap(BX_HDIMAGE_MODE_VVFAT, 1);

      bx_param_num_c *cylinders = new bx_param_num_c(menu,
        "cylinders",
        "Cylinders",
        "Number of cylinders",
        0, 262143,
        0);
      cylinders->set_ask_format("Enter number of cylinders: [%d] ");
      bx_param_num_c *heads = new bx_param_num_c(menu,
        "heads",
        "Heads",
        "Number of heads",
        0, 16,
        0);
      heads->set_ask_format("Enter number of heads: [%d] ");
      bx_param_num_c *spt = new bx_param_num_c(menu,
        "spt",
        "Sectors per track",
        "Number of sectors per track",
        0, 63,
        0);
      spt->set_ask_format("Enter number of sectors per track: [%d] ");

      bx_param_string_c *model = new bx_param_string_c(menu,
        "model",
        "Model name",
        "String returned by the 'identify device' command",
        "Generic 1234", 41);
      model->set_ask_format("Enter new model name: [%s]");

      static const char *atadevice_biosdetect_names[] = { "none", "auto", "cmos", NULL };

      bx_param_enum_c *biosdetect = new bx_param_enum_c(menu,
        "biosdetect",
        "BIOS Detection",
        "Type of bios detection",
        atadevice_biosdetect_names,
        BX_ATA_BIOSDETECT_AUTO,
        BX_ATA_BIOSDETECT_NONE);
      biosdetect->set_ask_format("Enter bios detection type: [%s]");

      static const char *atadevice_translation_names[] = { "none", "lba", "large", "rechs", "auto", NULL };

      bx_param_enum_c *translation = new bx_param_enum_c(menu,
        "translation",
        "Translation type",
        "How the ata-disk translation is done by the bios",
        atadevice_translation_names,
        BX_ATA_TRANSLATION_AUTO,
        BX_ATA_TRANSLATION_NONE);
      translation->set_ask_format("Enter translation type: [%s]");

      // the menu and all items on it depend on the present flag
      deplist = new bx_list_c(NULL);
      deplist->add(type);
      deplist->add(path);
      deplist->add(model);
      deplist->add(biosdetect);
      present->set_dependent_list(deplist);
      // the master/slave menu depends on the ATA channel's enabled flag
      enabled->get_dependent_list()->add(menu);
      // the present flag depends on the ATA channel's enabled flag
      enabled->get_dependent_list()->add(present);

      // some items depend on the drive type
      bx_param_c *type_deplist[] = {
        mode,
        status,
        cylinders,
        heads,
        spt,
        translation,
        NULL
      };
      deplist = new bx_list_c(NULL, "deplist", "", type_deplist);
      type->set_dependent_list(deplist, 0);
      type->set_dependent_bitmap(BX_ATA_DEVICE_DISK, 0x3d);
      type->set_dependent_bitmap(BX_ATA_DEVICE_CDROM, 0x02);

      type->set_handler(bx_param_handler);
    }

    // Enable two ATA interfaces by default, disable the others.
    // Now that the dependence relationships are established, call set() on
    // the ata device present params to set all enables correctly.
    enabled->set_initial_val(channel<2);
    enabled->set(channel<2);
  }

  // disk menu
  bx_param_c *disk_menu_init_list[] = {
    SIM->get_param(BXPN_FLOPPYA),
    SIM->get_param(BXPN_FLOPPYB),
    SIM->get_param(BXPN_ATA0_RES),
    SIM->get_param(BXPN_ATA0_MASTER),
    SIM->get_param(BXPN_ATA0_SLAVE),
#if BX_MAX_ATA_CHANNEL>1
    SIM->get_param(BXPN_ATA1_RES),
    SIM->get_param(BXPN_ATA1_MASTER),
    SIM->get_param(BXPN_ATA1_SLAVE),
#endif
#if BX_MAX_ATA_CHANNEL>2
    SIM->get_param(BXPN_ATA2_RES),
    SIM->get_param(BXPN_ATA2_MASTER),
    SIM->get_param(BXPN_ATA2_SLAVE),
#endif
#if BX_MAX_ATA_CHANNEL>3
    SIM->get_param(BXPN_ATA3_RES),
    SIM->get_param(BXPN_ATA3_MASTER),
    SIM->get_param(BXPN_ATA3_SLAVE),
#endif
    SIM->get_param("boot_params"),
    NULL
  };
  menu = new bx_list_c(special_menus, "disk", "Bochs Disk Options", disk_menu_init_list);
  menu->set_options(menu->SHOW_PARENT);

  // disk menu for win32paramdlg
  bx_param_c *disk_menu2_init_list[] = {
    SIM->get_param("floppy"),
    SIM->get_param("ata.0"),
#if BX_MAX_ATA_CHANNEL>1
    SIM->get_param("ata.1"),
#endif
#if BX_MAX_ATA_CHANNEL>2
    SIM->get_param("ata.2"),
#endif
#if BX_MAX_ATA_CHANNEL>3
    SIM->get_param("ata.3"),
#endif
    SIM->get_param("boot_params"),
    NULL
  };
  menu = new bx_list_c(special_menus, "disk_win32", "Bochs Disk Options", disk_menu2_init_list);
  menu->set_options(menu->USE_TAB_WINDOW);

  // ports subtree
  bx_list_c *ports = new bx_list_c(root_param, "ports", "Serial and Parallel Port Options");
  ports->set_options(ports->USE_TAB_WINDOW | ports->SHOW_PARENT);

  // parallel ports
  bx_list_c *parallel = new bx_list_c(ports, "parallel", "Parallel Port Options");
  parallel->set_options(parallel->SHOW_PARENT);
  // parport options initialized in the devive plugin code

  // serial ports
  bx_list_c *serial = new bx_list_c(ports, "serial", "Serial Port Options");
  serial->set_options(serial->SHOW_PARENT);
  // serial port options initialized in the devive plugin code

  // usb subtree
  bx_list_c *usb = new bx_list_c(ports, "usb", "USB Configuration");
  usb->set_options(usb->USE_TAB_WINDOW | usb->SHOW_PARENT);
  // USB host controller options initialized in the devive plugin code

  // network subtree
  bx_list_c *network = new bx_list_c(root_param, "network", "Network Configuration");
  network->set_options(network->USE_TAB_WINDOW | network->SHOW_PARENT);
  // network device options initialized in the devive plugin code

  // sound subtree
  bx_list_c *sound = new bx_list_c(root_param, "sound", "Sound Configuration");
  sound->set_options(sound->USE_TAB_WINDOW | sound->SHOW_PARENT);
  // sound device options initialized in the devive plugin code

  // misc options subtree
  bx_list_c *misc = new bx_list_c(root_param, "misc", "Configure Everything Else");
  misc->set_options(misc->SHOW_PARENT);

  // port e9 hack
  new bx_param_bool_c(misc,
      "port_e9_hack",
      "Enable port 0xE9 hack",
      "Debug messages written to i/o port 0xE9 will be displayed on console",
      0);

  // GDB stub
  menu = new bx_list_c(misc, "gdbstub", "GDB Stub Options");
  menu->set_options(menu->SHOW_PARENT | menu->USE_BOX_TITLE);
  menu->set_enabled(BX_GDBSTUB);
  enabled = new bx_param_bool_c(menu,
    "enabled",
    "Enable GDB stub",
    "",
    0);
  enabled->set_enabled(BX_GDBSTUB);
  new bx_param_num_c(menu,
    "port",
    "Port",
    "TCP/IP port for GDB stub",
    0, 65535,
    1234);
  new bx_param_num_c(menu,
    "text_base",
    "Text base",
    "",
    0, BX_MAX_BIT32U,
    0);
  new bx_param_num_c(menu,
    "data_base",
    "Data base",
    "",
    0, BX_MAX_BIT32U,
    0);
  new bx_param_num_c(menu,
    "bss_base",
    "BSS base",
    "",
    0, BX_MAX_BIT32U,
    0);
  enabled->set_dependent_list(menu->clone());

#if BX_PLUGINS
  // user plugin options
  menu = new bx_list_c(misc, "user_plugin", "User Plugin Options");
  menu->set_options(menu->SHOW_PARENT | menu->USE_BOX_TITLE);
  for (i=0; i<BX_N_USER_PLUGINS; i++) {
    sprintf(name, "%d", i+1);
    sprintf(label, "User Plugin #%d", i+1);
    sprintf(descr, "User-defined plugin device #%d", i+1);
    bx_param_string_c *plugin = new bx_param_string_c(menu, name, label, descr,
      "", BX_PATHNAME_LEN);
    plugin->set_handler(bx_param_string_handler);
  }
  // user-defined options subtree
  bx_list_c *user = new bx_list_c(root_param, "user", "User-defined options");
  user->set_options(user->SHOW_PARENT);
#endif

  // log options subtree
  menu = new bx_list_c(root_param, "log", "Logfile Options");
  menu->set_options(menu->SHOW_PARENT);

  // log options
  path = new bx_param_filename_c(menu,
      "filename",
      "Log filename",
      "Pathname of bochs log file",
      "-", BX_PATHNAME_LEN);
  path->set_ask_format("Enter log filename: [%s] ");
  path->set_extension("txt");

  bx_param_string_c *prefix = new bx_param_string_c(menu,
      "prefix",
      "Log output prefix",
      "Prefix prepended to log output",
      "%t%e%d", BX_PATHNAME_LEN);
  prefix->set_ask_format("Enter log prefix: [%s] ");

  path = new bx_param_filename_c(menu,
      "debugger_filename",
      "Debugger Log filename",
      "Pathname of debugger log file",
      "-", BX_PATHNAME_LEN);
  path->set_ask_format("Enter debugger log filename: [%s] ");
  path->set_extension("log");
  path->set_enabled(BX_DEBUGGER);

  // runtime options
  menu = new bx_list_c(special_menus, "runtime", "Runtime options");
  bx_list_c *cdrom = new bx_list_c(menu, "cdrom", "CD-ROM options");
  cdrom->set_options(cdrom->SHOW_PARENT);
  usb = new bx_list_c(menu, "usb", "USB options");
  usb->set_options(usb->SHOW_PARENT | usb->USE_TAB_WINDOW);
  // misc runtime options
  misc = new bx_list_c(menu, "misc", "Misc options");
  misc->add(SIM->get_param(BXPN_VGA_UPDATE_FREQUENCY));
  misc->add(SIM->get_param(BXPN_MOUSE_ENABLED));
  misc->add(SIM->get_param(BXPN_KBD_PASTE_DELAY));
  misc->add(SIM->get_param(BXPN_USER_SHORTCUT));
  misc->set_options(misc->SHOW_PARENT | misc->SHOW_GROUP_NAME);
}

void bx_reset_options()
{
  // optional plugin control
  bx_plugin_ctrl_reset(1);

  // cpu
  SIM->get_param("cpu")->reset();

#if BX_CPU_LEVEL >= 4
  // cpuid
  SIM->get_param("cpuid")->reset();
#endif

  // memory (ram & rom)
  SIM->get_param("memory")->reset();

  // clock & cmos
  SIM->get_param("clock_cmos")->reset();

  // pci
  SIM->get_param("pci")->reset();

  // display & interface
  SIM->get_param("display")->reset();

  // keyboard & mouse
  SIM->get_param("keyboard_mouse")->reset();

  // boot
  SIM->get_param("boot_params")->reset();

  // floppy drives
  SIM->get_param("floppy")->reset();

  // ata/atapi drives
  SIM->get_param("ata")->reset();

  // serial/parallel/usb
  SIM->get_param("ports")->reset();

  // network devices
  SIM->get_param("network")->reset();

  // sound devices
  SIM->get_param("sound")->reset();

  // misc
  SIM->get_param("misc")->reset();

  // logfile
  SIM->get_param("log")->reset();

#if BX_PLUGINS
  // user-defined options
  SIM->get_param("user")->reset();
  bx_user_plugin_count = 0;
#endif
}

int bx_read_configuration(const char *rcfile)
{
  // parse rcfile first, then parse arguments in order.
  BX_INFO (("reading configuration from %s", rcfile));
  if (parse_bochsrc(rcfile) < 0) {
    BX_PANIC (("reading from %s failed", rcfile));
    return -1;
  }
  // update log actions
  for (int level=0; level<N_LOGLEV; level++) {
    int action = SIM->get_default_log_action(level);
    io->set_log_action(level, action);
  }
  return 0;
}

int bx_parse_cmdline(int arg, int argc, char *argv[])
{
  //if (arg < argc) BX_INFO (("parsing command line arguments"));

  while (arg < argc) {
    BX_INFO (("parsing arg %d, %s", arg, argv[arg]));
    parse_line_unformatted("cmdline args", argv[arg]);
    arg++;
  }
  // update log actions
  for (int level=0; level<N_LOGLEV; level++) {
    int action = SIM->get_default_log_action(level);
    io->set_log_action(level, action);
  }
  return 0;
}

char *bx_find_bochsrc()
{
  FILE *fd = NULL;
  char rcfile[512];
  Bit32u retry = 0, found = 0;
  // try several possibilities for the bochsrc before giving up
  while (!found) {
    rcfile[0] = 0;
    switch (retry++) {
    case 0: strcpy (rcfile, ".bochsrc"); break;
    case 1: strcpy (rcfile, "bochsrc"); break;
    case 2: strcpy (rcfile, "bochsrc.txt"); break;
#ifdef WIN32
    case 3: strcpy (rcfile, "bochsrc.bxrc"); break;
#elif !BX_WITH_MACOS
      // only try this on unix
    case 3:
      {
        char *ptr = getenv("HOME");
        if (ptr) snprintf (rcfile, sizeof(rcfile), "%s/.bochsrc", ptr);
      }
      break;
    case 4: strcpy (rcfile, "/etc/bochsrc"); break;
#endif
    default:
      return NULL;
    }
    if (rcfile[0]) {
      BX_DEBUG (("looking for configuration in %s", rcfile));
      fd = fopen(rcfile, "r");
      if (fd) found = 1;
    }
  }
  assert (fd != NULL && rcfile[0] != 0);
  fclose (fd);
  return strdup(rcfile);
}

static int parse_bochsrc(const char *rcfile)
{
  FILE *fd = NULL;
  char *ret;
  char line[512];
  char context[BX_PATHNAME_LEN];
  Bit32u linenum = 1;

  // try several possibilities for the bochsrc before giving up

  bochsrc_include_count++;

  fd = fopen (rcfile, "r");
  if (fd == NULL) return -1;

  int retval = 0;
  do {
    ret = fgets(line, sizeof(line)-1, fd);
    line[sizeof(line) - 1] = '\0';
    size_t len = strlen(line);
    if ((len>0) && (line[len-1] < ' '))
      line[len-1] = '\0';
    if ((ret != NULL) && strlen(line)) {
      sprintf(context, "%s:%u", rcfile, linenum);
      if (parse_line_unformatted(context, line) < 0) {
        retval = -1;
        break;  // quit parsing after first error
      }
    }
    linenum++;
  } while (!feof(fd));
  fclose(fd);
  bochsrc_include_count--;
  return retval;
}

static const char *get_builtin_variable(const char *varname)
{
#ifdef WIN32
  int code;
  DWORD size;
  DWORD type = 0;
  HKEY hkey;
  char keyname[80];
  static char data[MAX_PATH];
#endif

  if (strlen(varname)<1) return NULL;
  else {
    if (!strcmp(varname, "BXSHARE")) {
#ifdef WIN32
      wsprintf(keyname, "Software\\Bochs %s", VER_STRING);
      code = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyname, 0, KEY_READ, &hkey);
      if (code == ERROR_SUCCESS) {
        data[0] = 0;
        size = MAX_PATH;
        if (RegQueryValueEx(hkey, "", NULL, (LPDWORD)&type, (LPBYTE)data,
                            (LPDWORD)&size) == ERROR_SUCCESS) {
          RegCloseKey(hkey);
          return data;
        } else {
          RegCloseKey(hkey);
          return NULL;
        }
      } else {
        return NULL;
      }
#else
      return BX_SHARE_PATH;
#endif
    }
    return NULL;
  }
}

static int parse_line_unformatted(const char *context, char *line)
{
#define MAX_PARAMS_LEN 40
  char *ptr;
  unsigned i, string_i = 0;
  char string[512];
  char *params[MAX_PARAMS_LEN];
  int num_params;
  bx_bool inquotes = 0;
  bx_bool comment = 0;

  memset(params, 0, sizeof(params));
  if (line == NULL) return 0;

  // if passed nothing but whitespace, just return
  for (i=0; i<strlen(line); i++) {
    if (!isspace(line[i])) break;
  }
  if (i>=strlen(line))
    return 0;

  num_params = 0;

  if (!strncmp(line, "#include", 8))
    ptr = strtok(line, " ");
  else
    ptr = strtok(line, ":");
  while ((ptr) && (!comment)) {
    if (!inquotes) {
      string_i = 0;
    } else {
      string[string_i++] = ',';
    }
    for (i=0; i<strlen(ptr); i++) {
      if (ptr[i] == '"')
        inquotes = !inquotes;
      else if ((ptr[i] == '#') && (strncmp(line+i, "#include", 8)) && !inquotes) {
        comment = 1;
        break;
      } else {
#if BX_HAVE_GETENV
        // substitute environment variables.
        if (ptr[i] == '$') {
          char varname[512];
          char *pv = varname;
          const char *value;
          *pv = 0;
          i++;
          while (isalpha(ptr[i]) || ptr[i]=='_') {
            *pv = ptr[i]; pv++; i++;
          }
          *pv = 0;
          if (strlen(varname)<1 || !(value = getenv(varname))) {
            if ((value = get_builtin_variable(varname))) {
              // append value to the string
              for (pv=(char *)value; *pv; pv++)
                  string[string_i++] = *pv;
            } else {
              BX_PANIC (("could not look up environment variable '%s'", varname));
            }
          } else {
            // append value to the string
            for (pv=(char *)value; *pv; pv++)
                string[string_i++] = *pv;
          }
        }
#endif
        if (!isspace(ptr[i]) || inquotes) {
          string[string_i++] = ptr[i];
        }
      }
    }
    string[string_i] = '\0';
    if (string_i == 0) break;
    if (!inquotes) {
      if (params[num_params] != NULL) {
        free(params[num_params]);
        params[num_params] = NULL;
      }
      if (num_params < MAX_PARAMS_LEN) {
        params[num_params++] = strdup(string);
      } else {
        BX_PANIC (("too many parameters, max is %d\n", MAX_PARAMS_LEN));
      }
    }
    ptr = strtok(NULL, ",");
  }
  Bit32s retval = parse_line_formatted(context, num_params, &params[0]);
  for (i=0; i < MAX_PARAMS_LEN; i++)
  {
    if (params[i] != NULL)
    {
        free(params[i]);
        params[i] = NULL;
    }
  }
  return retval;
}

// These macros are called for all parse errors, so that we can easily
// change the behavior of all occurrences.
#define PARSE_ERR(x)  \
  do { BX_PANIC(x); return -1; } while (0)
#define PARSE_WARN(x)  \
  BX_ERROR(x)

/*
 * this supports the "floppyx: image=" option.
 * the functions returns the type of the floppy
 * image (1.44, 360, etc.), based on the image file size.
 */
int get_floppy_type_from_image(const char *filename)
{
  struct stat stat_buf;

  if (!strncmp(filename, "vvfat:", 6)) {
    return BX_FLOPPY_1_44;
  }
  if (stat(filename, &stat_buf)) {
    return BX_FLOPPY_NONE;
  }

  switch (stat_buf.st_size) {
    case 163840:
      return BX_FLOPPY_160K;

    case 184320:
      return BX_FLOPPY_180K;

    case 327680:
      return BX_FLOPPY_320K;

    case 368640:
      return BX_FLOPPY_360K;

    case 737280:
      return BX_FLOPPY_720K;

    case 1228800:
      return BX_FLOPPY_1_2;

    case 1474560:
    case 1720320:
    case 1763328:
    case 1884160:
      return BX_FLOPPY_1_44;

    case 2949120:
      return BX_FLOPPY_2_88;

    default:
      return BX_FLOPPY_UNKNOWN;
  }
}

static Bit32s parse_log_options(const char *context, int num_params, char *params[])
{
  int level, action, i;
  bx_bool def_action = 0;
  char *param, *module, *actstr;
  char pname[20];
  bx_list_c *base;
  bx_param_num_c *mparam;

  if (!strcmp(params[0], "panic")) {
    level = LOGLEV_PANIC;
  } else if (!strcmp(params[0], "error")) {
    level = LOGLEV_ERROR;
  } else if (!strcmp(params[0], "info")) {
    level = LOGLEV_INFO;
  } else { /* debug */
    level = LOGLEV_DEBUG;
  }
  for (i = 1; i < num_params; i++) {
    param = strdup(params[i]);
    module = strtok(param, "=");
    actstr = strtok(NULL, "");
    if (actstr != NULL) {
      def_action = !strcmp(module, "action");
      if (!strcmp(actstr, "fatal"))
        action = ACT_FATAL;
      else if (!strcmp (actstr, "report"))
        action = ACT_REPORT;
      else if (!strcmp (actstr, "ignore"))
        action = ACT_IGNORE;
      else if (!strcmp (actstr, "ask"))
        action = ACT_ASK;
      else {
        PARSE_ERR(("%s: %s directive malformed.", context, params[0]));
        free(param);
        return -1;
      }
      if (def_action) {
        SIM->set_default_log_action(level, action);
      } else {
        sprintf(pname, "general.logfn.%s", params[0]);
        base = (bx_list_c*) SIM->get_param(pname);
        mparam = (bx_param_num_c*) base->get_by_name(module);
        if (mparam != NULL) {
          mparam->set(action);
        } else {
          mparam = new bx_param_num_c(base, module, "", "", -1, BX_MAX_BIT32U, action);
          if (mparam == NULL) {
            PARSE_ERR(("%s: %s: failed to add log module.", context, params[0]));
          }
        }
      }
    } else {
      PARSE_ERR(("%s: %s directive malformed.", context, params[0]));
      free(param);
      return -1;
    }
    free(param);
  }
  return 0;
}

static int parse_debug_symbols(const char *context, const char **params, int num_params)
{
#if BX_DEBUGGER
  Bit32u offset = 0;
  const char *filename = 0;

  while (num_params > 0)
  {
    if (!strncmp(*params, "file=", 5)) {
      filename = *params + 5;
    }
    else if (!strncmp(*params, "offset=", 7)) {
      char* end;
      offset = strtoul(*params + 7, &end, 0);
      if (*end)
        PARSE_ERR(("%s: debug_symbols: invalid parameter %s", context, *params));
    }
    else {
      PARSE_ERR(("%s: debug_symbols: invalid parameter %s", context, *params));
    }
    params++; num_params--;
  }

  if (!filename)
    PARSE_ERR(("%s: debug_symbols: missing file name", context));

  if (bx_dbg_symbol_command(filename, 1, offset) < 0)
    PARSE_ERR(("%s: debug_symbols: failed to load symbols from '%s'", context, filename));
#endif
  return 0;
}

static int parse_param_bool(const char *input, int len, const char *param)
{
  if (input[len] == '0' || input[len] == '1') {
    SIM->get_param_bool(param)->set(input[len] - '0');
    return 0;
  }

  return -1;
}

int bx_parse_usb_port_params(const char *context, bx_bool devopt, const char *param, int maxports, bx_list_c *base)
{
  int idx, plen;
  char tmpname[20];

  if (!devopt) {
    plen = 4;
  } else {
    plen = 7;
  }
  idx = param[plen];
  if ((idx < '1') || (idx > '9') || (param[plen + 1] != '=')) {
    PARSE_ERR(("%s: usb_%s: portX / optionsX parameter malformed.", context, base->get_name()));
    return -1;
  }
  idx -= '0';
  if (idx > maxports) {
    PARSE_ERR(("%s: usb_%s: port number out of range.", context, base->get_name()));
    return -1;
  }
  sprintf(tmpname, "port%d.%s", idx, devopt ? "options" : "device");
  SIM->get_param_string(tmpname, base)->set(&param[plen + 2]);
  return 0;
}

int bx_parse_nic_params(const char *context, const char *param, bx_list_c *base)
{
  int tmp[6];
  char tmpchar[6];
  int valid = 0;
  int n;

  if (!strncmp(param, "enabled=", 8)) {
    if (atol(&param[8]) == 0) valid |= 0x80;
  } else if (!strncmp(param, "mac=", 4)) {
    n = sscanf(&param[4], "%x:%x:%x:%x:%x:%x",
               &tmp[0],&tmp[1],&tmp[2],&tmp[3],&tmp[4],&tmp[5]);
    if (n != 6) {
      PARSE_ERR(("%s: '%s' mac address malformed.", context, base->get_name()));
    }
    for (n=0;n<6;n++)
      tmpchar[n] = (unsigned char)tmp[n];
    SIM->get_param_string("macaddr", base)->set(tmpchar);
    valid |= 0x04;
  } else if (!strncmp(param, "ethmod=", 7)) {
    if (!SIM->get_param_enum("ethmod", base)->set_by_name(&param[7]))
      PARSE_ERR(("%s: ethernet module '%s' not available", context, &param[7]));
  } else if (!strncmp(param, "ethdev=", 7)) {
    SIM->get_param_string("ethdev", base)->set(&param[7]);
  } else if (!strncmp(param, "script=", 7)) {
    SIM->get_param_string("script", base)->set(&param[7]);
  } else if (!strncmp(param, "bootrom=", 8)) {
    SIM->get_param_string("bootrom", base)->set(&param[8]);
  } else {
    PARSE_WARN(("%s: unknown parameter '%s' for '%s' ignored.", context, param, base->get_name()));
    return -1;
  }
  return valid;
}

static int parse_line_formatted(const char *context, int num_params, char *params[])
{
  int i, slot, t;
  bx_list_c *base;

  if (num_params < 1) return 0;
  if (num_params < 2) {
    PARSE_ERR(("%s: a bochsrc option needs at least one parameter", context));
  }

#if BX_SUPPORT_PCIPNIC
  // Temporary HACK for the PCI Pseudo NIC's bochsrc option name change
  if (!strcmp(params[0], "pnic")) {
    strcpy(params[0], "pcipnic");
  }
#endif

  if (!strcmp(params[0], "#include")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: ignoring malformed #include directive.", context));
    }
    if (!strcmp(params[1], context)) {
      PARSE_ERR(("%s: cannot include this file again.", context));
    }
    if (bochsrc_include_count == 2) {
      PARSE_ERR(("%s: include directive in an included file not supported yet.", context));
    }
    bx_read_configuration(params[1]);
  }
  else if ((!strcmp(params[0], "floppya")) ||
           (!strcmp(params[0], "floppyb"))) {
    if (!strcmp(params[0], "floppya")) {
      base = (bx_list_c*) SIM->get_param(BXPN_FLOPPYA);
    } else {
      base = (bx_list_c*) SIM->get_param(BXPN_FLOPPYB);
    }
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "type=", 5)) {
        if (!strcmp(params[i]+5, "2_88")) {
          SIM->get_param_enum("devtype", base)->set(BX_FDD_350ED);
        }
        else if (!strcmp(params[i]+5, "1_44")) {
          SIM->get_param_enum("devtype", base)->set(BX_FDD_350HD);
        }
        else if (!strcmp(params[i]+5, "1_2")) {
          SIM->get_param_enum("devtype", base)->set(BX_FDD_525HD);
        }
        else if (!strcmp(params[i]+5, "720k")) {
          SIM->get_param_enum("devtype", base)->set(BX_FDD_350DD);
        }
        else if (!strcmp(params[i]+5, "360k")) {
          SIM->get_param_enum("devtype", base)->set(BX_FDD_525DD);
        }
        else {
          PARSE_ERR(("%s: %s: unknown type '%s'.", context, params[0],
            params[i]+5));
        }
      }
      else if (!strncmp(params[i], "2_88=", 5)) {
        SIM->get_param_string("path", base)->set(&params[i][5]);
        SIM->get_param_enum("type", base)->set(BX_FLOPPY_2_88);
      }
      else if (!strncmp(params[i], "1_44=", 5)) {
        SIM->get_param_string("path", base)->set(&params[i][5]);
        SIM->get_param_enum("type", base)->set(BX_FLOPPY_1_44);
      }
      else if (!strncmp(params[i], "1_2=", 4)) {
        SIM->get_param_string("path", base)->set(&params[i][4]);
        SIM->get_param_enum("type", base)->set(BX_FLOPPY_1_2);
      }
      else if (!strncmp(params[i], "720k=", 5)) {
        SIM->get_param_string("path", base)->set(&params[i][5]);
        SIM->get_param_enum("type", base)->set(BX_FLOPPY_720K);
      }
      else if (!strncmp(params[i], "360k=", 5)) {
        SIM->get_param_string("path", base)->set(&params[i][5]);
        SIM->get_param_enum("type", base)->set(BX_FLOPPY_360K);
      }
      // use CMOS reserved types?
      else if (!strncmp(params[i], "160k=", 5)) {
        SIM->get_param_string("path", base)->set(&params[i][5]);
        SIM->get_param_enum("type", base)->set(BX_FLOPPY_160K);
      }
      else if (!strncmp(params[i], "180k=", 5)) {
        SIM->get_param_string("path", base)->set(&params[i][5]);
        SIM->get_param_enum("type", base)->set(BX_FLOPPY_180K);
      }
      else if (!strncmp(params[i], "320k=", 5)) {
        SIM->get_param_string("path", base)->set(&params[i][5]);
        SIM->get_param_enum("type", base)->set(BX_FLOPPY_320K);
      }
      else if (!strncmp(params[i], "image=", 6)) {
        /* "image=" means we should get floppy type from image */
        SIM->get_param_string("path", base)->set(&params[i][6]);
        t = get_floppy_type_from_image(&params[i][6]);
        if (t != BX_FLOPPY_UNKNOWN)
          SIM->get_param_enum("type", base)->set(t);
        else
          PARSE_ERR(("%s: %s image size doesn't match one of the supported types.",
            context, params[0]));
      }
      else if (!strcmp(params[i], "status=inserted")) {
        SIM->get_param_bool("status", base)->set(1);
      }
      else if (!strcmp(params[i], "status=ejected")) {
        SIM->get_param_bool("status", base)->set(0);
      }
      else if (!strncmp(params[i], "write_protected=", 16)) {
        SIM->get_param_bool("readonly", base)->set(atol(&params[i][16]));
      }
      else {
        PARSE_ERR(("%s: %s attribute '%s' not understood.", context, params[0],
          params[i]));
      }
    }
  }

  else if ((!strncmp(params[0], "ata", 3)) && (strlen(params[0]) == 4)) {
    char tmpname[80];
    Bit8u channel = params[0][3];

    if ((channel < '0') || (channel > '9')) {
      PARSE_ERR(("%s: ataX directive malformed.", context));
    }
    channel-='0';
    if (channel >= BX_MAX_ATA_CHANNEL) {
      PARSE_ERR(("%s: ataX directive malformed.", context));
    }

    if ((num_params < 2) || (num_params > 5)) {
      PARSE_ERR(("%s: ataX directive malformed.", context));
    }
    sprintf(tmpname, "ata.%d.resources", channel);
    base = (bx_list_c*) SIM->get_param(tmpname);
    if (strncmp(params[1], "enabled=", 8)) {
      PARSE_ERR(("%s: ataX directive malformed.", context));
    } else {
      SIM->get_param_bool("enabled", base)->set(atol(&params[1][8]));
    }

    if (num_params > 2) {
      if (strncmp(params[2], "ioaddr1=", 8)) {
        PARSE_ERR(("%s: ataX directive malformed.", context));
      }
      else {
        if ((params[2][8] == '0') && (params[2][9] == 'x'))
          SIM->get_param_num("ioaddr1", base)->set(strtoul(&params[2][8], NULL, 16));
        else
          SIM->get_param_num("ioaddr1", base)->set(strtoul(&params[2][8], NULL, 10));
      }
    }

    if (num_params > 3) {
      if (strncmp(params[3], "ioaddr2=", 8)) {
        PARSE_ERR(("%s: ataX directive malformed.", context));
      }
      else {
        if ((params[3][8] == '0') && (params[3][9] == 'x'))
          SIM->get_param_num("ioaddr2", base)->set(strtoul(&params[3][8], NULL, 16));
        else
          SIM->get_param_num("ioaddr2", base)->set(strtoul(&params[3][8], NULL, 10));
      }
    }

    if (num_params > 4) {
      if (strncmp(params[4], "irq=", 4)) {
        PARSE_ERR(("%s: ataX directive malformed.", context));
      }
      else {
        SIM->get_param_num("irq", base)->set(atol(&params[4][4]));
      }
    }
  }

  // ataX-master, ataX-slave
  else if ((!strncmp(params[0], "ata", 3)) && (strlen(params[0]) > 4)) {
    Bit8u channel = params[0][3];
    int type = -1, mode = BX_HDIMAGE_MODE_FLAT, biosdetect = BX_ATA_BIOSDETECT_AUTO;
    Bit32u cylinders = 0, heads = 0, sectors = 0;
    char tmpname[80];

    if ((channel < '0') || (channel > '9')) {
      PARSE_ERR(("%s: ataX-master/slave directive malformed.", context));
    }
    channel-='0';
    if (channel >= BX_MAX_ATA_CHANNEL) {
      PARSE_ERR(("%s: ataX-master/slave directive malformed.", context));
    }

    if ((strcmp(&params[0][4], "-slave")) &&
        (strcmp(&params[0][4], "-master"))) {
      PARSE_ERR(("%s: ataX-master/slave directive malformed.", context));
    }

    sprintf(tmpname, "ata.%d.%s", channel, &params[0][5]);
    base = (bx_list_c*) SIM->get_param(tmpname);
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "type=", 5)) {
        type = SIM->get_param_enum("type", base)->find_by_name(&params[i][5]);
        if (type < 0) {
          PARSE_ERR(("%s: ataX-master/slave: unknown type '%s'", context, &params[i][5]));
        }
      } else if (!strncmp(params[i], "mode=", 5)) {
        mode = SIM->get_param_enum("mode", base)->find_by_name(&params[i][5]);
        if (mode < 0) {
          PARSE_ERR(("%s: ataX-master/slave: unknown mode '%s'", context, &params[i][5]));
        }
      } else if (!strncmp(params[i], "path=", 5)) {
        SIM->get_param_string("path", base)->set(&params[i][5]);
      } else if (!strncmp(params[i], "cylinders=", 10)) {
        cylinders = atol(&params[i][10]);
      } else if (!strncmp(params[i], "heads=", 6)) {
        heads = atol(&params[i][6]);
      } else if (!strncmp(params[i], "spt=", 4)) {
        sectors = atol(&params[i][4]);
      } else if (!strncmp(params[i], "model=", 6)) {
        SIM->get_param_string("model", base)->set(&params[i][6]);
      } else if (!strncmp(params[i], "biosdetect=", 11)) {
        biosdetect = SIM->get_param_enum("biosdetect", base)->find_by_name(&params[i][11]);
        if (biosdetect < 0) {
          PARSE_ERR(("%s: ataX-master/slave: unknown biosdetect '%s'", context, &params[i][11]));
        }
      } else if (!strcmp(params[i], "translation=none")) {
        SIM->get_param_enum("translation", base)->set(BX_ATA_TRANSLATION_NONE);
      } else if (!strcmp(params[i], "translation=lba")) {
        SIM->get_param_enum("translation", base)->set(BX_ATA_TRANSLATION_LBA);
      } else if (!strcmp(params[i], "translation=large")) {
        SIM->get_param_enum("translation", base)->set(BX_ATA_TRANSLATION_LARGE);
      } else if (!strcmp(params[i], "translation=echs")) { // synonym of large
        SIM->get_param_enum("translation", base)->set(BX_ATA_TRANSLATION_LARGE);
      } else if (!strcmp(params[i], "translation=rechs")) {
        SIM->get_param_enum("translation", base)->set(BX_ATA_TRANSLATION_RECHS);
      } else if (!strcmp(params[i], "translation=auto")) {
        SIM->get_param_enum("translation", base)->set(BX_ATA_TRANSLATION_AUTO);
      } else if (!strcmp(params[i], "status=ejected")) {
        SIM->get_param_bool("status", base)->set(0);
      } else if (!strcmp(params[i], "status=inserted")) {
        SIM->get_param_bool("status", base)->set(1);
      } else if (!strncmp(params[i], "journal=", 8)) {
        SIM->get_param_string("journal", base)->set(&params[i][8]);
      } else {
        PARSE_ERR(("%s: ataX-master/slave directive malformed.", context));
      }
    }

    // Enables the ata device
    if (type >= 0) {
      SIM->get_param_enum("type", base)->set(type);
      SIM->get_param_num("biosdetect", base)->set(biosdetect);
      if (type == BX_ATA_DEVICE_DISK) {
        if (strlen(SIM->get_param_string("path", base)->getptr()) > 0) {
          SIM->get_param_bool("present", base)->set(1);
          SIM->get_param_enum("mode", base)->set(mode);
          SIM->get_param_num("cylinders", base)->set(cylinders);
          if ((cylinders == 0) && (heads == 0) && (sectors == 0)) {
            PARSE_WARN(("%s: ataX-master/slave CHS set to 0/0/0 - autodetection enabled", context));
            // using heads = 16 and spt = 63 for autodetection (bximage defaults)
            SIM->get_param_num("heads", base)->set(16);
            SIM->get_param_num("spt", base)->set(63);
          } else {
            SIM->get_param_num("heads", base)->set(heads);
            SIM->get_param_num("spt", base)->set(sectors);
          }
        } else {
          SIM->get_param_bool("present", base)->set(0);
        }
      } else if (type == BX_ATA_DEVICE_CDROM) {
        SIM->get_param_bool("present", base)->set(1);
      }
    } else {
      SIM->get_param_bool("present", base)->set(0);
    }

  } else if (!strcmp(params[0], "boot")) {
    char tmppath[80];
    if (num_params < 2) {
      PARSE_ERR(("%s: boot directive malformed.", context));
    }
    for (i=1; i<num_params; i++) {
      sprintf(tmppath, "boot_params.boot_drive%d", i);
      if (!strcmp(params[i], "none")) {
        SIM->get_param_enum(tmppath)->set(BX_BOOT_NONE);
      } else if (!strcmp(params[i], "a")) {
        SIM->get_param_enum(tmppath)->set(BX_BOOT_FLOPPYA);
      } else if (!strcmp(params[i], "floppy")) {
        SIM->get_param_enum(tmppath)->set(BX_BOOT_FLOPPYA);
      } else if (!strcmp(params[i], "c")) {
        SIM->get_param_enum(tmppath)->set(BX_BOOT_DISKC);
      } else if (!strcmp(params[i], "disk")) {
        SIM->get_param_enum(tmppath)->set(BX_BOOT_DISKC);
      } else if (!strcmp(params[i], "cdrom")) {
        SIM->get_param_enum(tmppath)->set(BX_BOOT_CDROM);
      } else if (!strcmp(params[i], "network")) {
        SIM->get_param_enum(tmppath)->set(BX_BOOT_NETWORK);
      } else {
        PARSE_ERR(("%s: boot directive with unknown boot drive '%s'.  use 'floppy', 'disk', 'cdrom' or 'network'.", context, params[i]));
      }
    }
    if (SIM->get_param_enum(BXPN_BOOTDRIVE1)->get() == BX_BOOT_NONE) {
      PARSE_ERR(("%s: first boot drive must be one of 'floppy', 'disk' or 'cdrom'.", context));
    }
    if ((SIM->get_param_enum(BXPN_BOOTDRIVE1)->get() == SIM->get_param_enum(BXPN_BOOTDRIVE2)->get()) ||
        (SIM->get_param_enum(BXPN_BOOTDRIVE1)->get() == SIM->get_param_enum(BXPN_BOOTDRIVE3)->get()) ||
        ((SIM->get_param_enum(BXPN_BOOTDRIVE3)->get() != BX_BOOT_NONE) &&
        (SIM->get_param_enum(BXPN_BOOTDRIVE2)->get() == SIM->get_param_enum(BXPN_BOOTDRIVE3)->get()))) {
      PARSE_ERR(("%s: a boot drive appears twice in boot sequence.", context));
    }
  } else if (!strcmp(params[0], "floppy_bootsig_check")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: floppy_bootsig_check directive malformed.", context));
    }
    if (strncmp(params[1], "disabled=", 9)) {
      PARSE_ERR(("%s: floppy_bootsig_check directive malformed.", context));
    }
    if (parse_param_bool(params[1], 9, BXPN_FLOPPYSIGCHECK) < 0) {
      PARSE_ERR(("%s: floppy_bootsig_check directive malformed.", context));
    }
  } else if (!strcmp(params[0], "log")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: log directive has wrong # args.", context));
    }
    SIM->get_param_string(BXPN_LOG_FILENAME)->set(params[1]);
  } else if (!strcmp(params[0], "logprefix")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: logprefix directive has wrong # args.", context));
    }
    SIM->get_param_string(BXPN_LOG_PREFIX)->set(params[1]);
  } else if (!strcmp(params[0], "debugger_log")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: debugger_log directive has wrong # args.", context));
    }
    SIM->get_param_string(BXPN_DEBUGGER_LOG_FILENAME)->set(params[1]);
  } else if (!strcmp(params[0], "panic")) {
    if (num_params < 2) {
      PARSE_ERR(("%s: panic directive malformed.", context));
    }
    if (parse_log_options(context, num_params, params) < 0) {
      return -1;
    }
  } else if (!strcmp(params[0], "error")) {
    if (num_params < 2) {
      PARSE_ERR(("%s: error directive malformed.", context));
    }
    if (parse_log_options(context, num_params, params) < 0) {
      return -1;
    }
  } else if (!strcmp(params[0], "info")) {
    if (num_params < 2) {
      PARSE_ERR(("%s: info directive malformed.", context));
    }
    if (parse_log_options(context, num_params, params) < 0) {
      return -1;
    }
  } else if (!strcmp(params[0], "debug")) {
    if (num_params < 2) {
      PARSE_ERR(("%s: debug directive malformed.", context));
    }
    if (parse_log_options(context, num_params, params) < 0) {
      return -1;
    }
  } else if (!strcmp(params[0], "cpu")) {
    if (num_params < 2) {
      PARSE_ERR(("%s: cpu directive malformed.", context));
    }
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "count=", 6)) {
        unsigned processors = 1, cores = 1, threads = 1;
        sscanf(&params[i][6], "%u:%u:%u", &processors, &cores, &threads);
        unsigned smp_threads = cores*threads*processors;
        if (smp_threads < 1) {
          PARSE_ERR(("%s: at least one CPU thread should be defined, cpu directive malformed", context));
        }
        SIM->get_param_num(BXPN_CPU_NPROCESSORS)->set(processors);
        SIM->get_param_num(BXPN_CPU_NCORES)->set(cores);
        SIM->get_param_num(BXPN_CPU_NTHREADS)->set(threads);
      } else if (!strncmp(params[i], "model=", 6)) {
        if (! SIM->get_param_enum(BXPN_CPU_MODEL)->set_by_name(&params[i][6]))
          PARSE_ERR(("%s: unsupported CPU model option.", context));
      } else if (!strncmp(params[i], "ips=", 4)) {
        SIM->get_param_num(BXPN_IPS)->set(atol(&params[i][4]));
#if BX_SUPPORT_SMP
      } else if (!strncmp(params[i], "quantum=", 8)) {
        SIM->get_param_num(BXPN_SMP_QUANTUM)->set(atol(&params[i][8]));
#endif
      } else if (!strncmp(params[i], "reset_on_triple_fault=", 22)) {
        if (parse_param_bool(params[i], 22, BXPN_RESET_ON_TRIPLE_FAULT) < 0) {
          PARSE_ERR(("%s: cpu directive malformed.", context));
        }
#if BX_CPU_LEVEL >= 5
      } else if (!strncmp(params[i], "ignore_bad_msrs=", 16)) {
        if (parse_param_bool(params[i], 16, BXPN_IGNORE_BAD_MSRS) < 0) {
          PARSE_ERR(("%s: cpu directive malformed.", context));
        }
#endif
#if BX_SUPPORT_MONITOR_MWAIT
      } else if (!strncmp(params[i], "mwait_is_nop=", 13)) {
        if (parse_param_bool(params[i], 13, BXPN_MWAIT_IS_NOP) < 0) {
          PARSE_ERR(("%s: cpu directive malformed.", context));
        }
#endif
      } else if (!strncmp(params[i], "msrs=", 5)) {
        SIM->get_param_string(BXPN_CONFIGURABLE_MSRS_PATH)->set(&params[i][5]);
      } else if (!strncmp(params[i], "cpuid_limit_winnt=", 18)) {
        if (parse_param_bool(params[i], 18, BXPN_CPUID_LIMIT_WINNT) < 0) {
          PARSE_ERR(("%s: cpu directive malformed.", context));
        }
      } else {
        PARSE_ERR(("%s: cpu directive malformed.", context));
      }
    }
#if BX_CPU_LEVEL >= 4
  } else if (!strcmp(params[0], "cpuid")) {
    if (num_params < 2) {
      PARSE_ERR(("%s: cpuid directive malformed.", context));
    }
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "vendor_string=", 14)) {
        if (strlen(&params[i][14]) != BX_CPUID_VENDOR_LEN) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
        SIM->get_param_string(BXPN_VENDOR_STRING)->set(&params[i][14]);
      } else if (!strncmp(params[i], "brand_string=", 13)) {
        if (strlen(&params[i][13]) > BX_CPUID_BRAND_LEN) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        } 
        SIM->get_param_string(BXPN_BRAND_STRING)->set(&params[i][13]);
      } else if (!strncmp(params[i], "stepping=", 9)) {
        SIM->get_param_num(BXPN_CPUID_STEPPING)->set(atol(&params[i][9]));
      } else if (!strncmp(params[i], "model=", 6)) {
        SIM->get_param_num(BXPN_CPUID_MODEL)->set(strtoul(&params[i][6], NULL, 0));
      } else if (!strncmp(params[i], "family=", 7)) {
        SIM->get_param_num(BXPN_CPUID_FAMILY)->set(strtoul(&params[i][7], NULL, 0));
#if BX_CPU_LEVEL >= 5
      } else if (!strncmp(params[i], "mmx=", 4)) {
        if (parse_param_bool(params[i], 4, BXPN_CPUID_MMX) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
      } else if (!strncmp(params[i], "apic=", 5)) {
        if (! SIM->get_param_enum(BXPN_CPUID_APIC)->set_by_name(&params[i][5]))
          PARSE_ERR(("%s: unsupported apic option.", context));
      } else if (!strncmp(params[i], "xapic=", 6)) {
        PARSE_ERR(("%s: unsupported xapic option (deprecated).", context));
#endif
#if BX_CPU_LEVEL >= 6
      } else if (!strncmp(params[i], "sse=", 4)) {
        if (! SIM->get_param_enum(BXPN_CPUID_SSE)->set_by_name(&params[i][4]))
          PARSE_ERR(("%s: unsupported sse option.", context));
      } else if (!strncmp(params[i], "sse4a=", 6)) {
        if (parse_param_bool(params[i], 6, BXPN_CPUID_SSE4A) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
      } else if (!strncmp(params[i], "aes=", 4)) {
        if (parse_param_bool(params[i], 4, BXPN_CPUID_AES) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
      } else if (!strncmp(params[i], "movbe=", 6)) {
        if (parse_param_bool(params[i], 6, BXPN_CPUID_MOVBE) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
      } else if (!strncmp(params[i], "adx=", 4)) {
        if (parse_param_bool(params[i], 4, BXPN_CPUID_ADX) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
      } else if (!strncmp(params[i], "sep=", 4)) {
        if (parse_param_bool(params[i], 4, BXPN_CPUID_SEP) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
      } else if (!strncmp(params[i], "xsave=", 6)) {
        if (parse_param_bool(params[i], 6, BXPN_CPUID_XSAVE) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
      } else if (!strncmp(params[i], "xsaveopt=", 9)) {
        if (parse_param_bool(params[i], 9, BXPN_CPUID_XSAVEOPT) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
#if BX_SUPPORT_AVX
      } else if (!strncmp(params[i], "avx=", 4)) {
        SIM->get_param_num(BXPN_CPUID_AVX)->set(atol(&params[i][4]));
      } else if (!strncmp(params[i], "avx_f16c=", 9)) {
        if (parse_param_bool(params[i], 9, BXPN_CPUID_AVX_F16CVT) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
      } else if (!strncmp(params[i], "avx_fma=", 8)) {
        if (parse_param_bool(params[i], 8, BXPN_CPUID_AVX_FMA) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
      } else if (!strncmp(params[i], "bmi=", 4)) {
        SIM->get_param_num(BXPN_CPUID_BMI)->set(atol(&params[i][4]));
      } else if (!strncmp(params[i], "xop=", 4)) {
        if (parse_param_bool(params[i], 4, BXPN_CPUID_XOP) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
      } else if (!strncmp(params[i], "tbm=", 4)) {
        if (parse_param_bool(params[i], 4, BXPN_CPUID_TBM) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
      } else if (!strncmp(params[i], "fma4=", 5)) {
        if (parse_param_bool(params[i], 5, BXPN_CPUID_FMA4) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
#endif
#if BX_SUPPORT_VMX
      } else if (!strncmp(params[i], "vmx=", 4)) {
        SIM->get_param_num(BXPN_CPUID_VMX)->set(atol(&params[i][4]));
#endif
#if BX_SUPPORT_SVM
      } else if (!strncmp(params[i], "svm=", 4)) {
        if (parse_param_bool(params[i], 4, BXPN_CPUID_SVM) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
#endif
#if BX_SUPPORT_X86_64
      } else if (!strncmp(params[i], "x86_64=", 7)) {
        if (parse_param_bool(params[i], 7, BXPN_CPUID_X86_64) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
      } else if (!strncmp(params[i], "1g_pages=", 9)) {
        if (parse_param_bool(params[i], 9, BXPN_CPUID_1G_PAGES) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
      } else if (!strncmp(params[i], "pcid=", 5)) {
        if (parse_param_bool(params[i], 5, BXPN_CPUID_PCID) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
      } else if (!strncmp(params[i], "fsgsbase=", 9)) {
        if (parse_param_bool(params[i], 9, BXPN_CPUID_FSGSBASE) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
#endif
      } else if (!strncmp(params[i], "smep=", 5)) {
        if (parse_param_bool(params[i], 5, BXPN_CPUID_SMEP) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
#if BX_SUPPORT_MONITOR_MWAIT
      } else if (!strncmp(params[i], "mwait=", 6)) {
        if (parse_param_bool(params[i], 6, BXPN_CPUID_MWAIT) < 0) {
          PARSE_ERR(("%s: cpuid directive malformed.", context));
        }
#endif
#endif
      } else if (!strncmp(params[i], "cpuid_limit_winnt=", 18)) {
        PARSE_ERR(("%s: cpuid_limit_winnt moved to CPU .bochsrc option.", context));
      } else {
        PARSE_ERR(("%s: cpuid directive malformed.", context));
      }
    }
#endif
  } else if (!strcmp(params[0], "megs")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: megs directive: wrong # args.", context));
    }
    SIM->get_param_num(BXPN_MEM_SIZE)->set(atol(params[1]));
    SIM->get_param_num(BXPN_HOST_MEM_SIZE)->set(atol(params[1]));
  } else if (!strcmp(params[0], "memory")) {
    if (num_params < 3) {
      PARSE_ERR(("%s: memory directive malformed.", context));
    }
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "host=", 5)) {
        SIM->get_param_num(BXPN_HOST_MEM_SIZE)->set(atol(&params[i][5]));
      } else if (!strncmp(params[i], "guest=", 6)) {
        SIM->get_param_num(BXPN_MEM_SIZE)->set(atol(&params[i][6]));
      } else {
        PARSE_ERR(("%s: memory directive malformed.", context));
      }
    }
  } else if (!strcmp(params[0], "romimage")) {
    if ((num_params < 2) || (num_params > 3)) {
      PARSE_ERR(("%s: romimage directive: wrong # args.", context));
    }
    if (!strncmp(params[1], "file=", 5)) {
      SIM->get_param_string(BXPN_ROM_PATH)->set(&params[1][5]);
    } else {
      PARSE_ERR(("%s: romimage directive malformed.", context));
    }
    if (num_params == 3) {
      if (!strncmp(params[2], "address=", 8)) {
        if ((params[2][8] == '0') && (params[2][9] == 'x'))
          SIM->get_param_num(BXPN_ROM_ADDRESS)->set(strtoul(&params[2][8], NULL, 16));
        else
          SIM->get_param_num(BXPN_ROM_ADDRESS)->set(strtoul(&params[2][8], NULL, 10));
      } else {
        PARSE_ERR(("%s: romimage directive malformed.", context));
      }
    } else {
      SIM->get_param_num(BXPN_ROM_ADDRESS)->set(0);
    }
  } else if (!strcmp(params[0], "vgaromimage")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: vgaromimage directive: wrong # args.", context));
    }
    if (!strncmp(params[1], "file=", 5)) {
      SIM->get_param_string(BXPN_VGA_ROM_PATH)->set(&params[1][5]);
    } else {
      PARSE_ERR(("%s: vgaromimage directive malformed.", context));
    }
  } else if (!strncmp(params[0], "optromimage", 11)) {
    int num = atoi(&params[0][11]);
    char tmppath[80], tmpaddr[80];
    if ((num < 1) || (num > BX_N_OPTROM_IMAGES)) {
      PARSE_ERR(("%s: optromimage%d: not supported", context, num));
    }
    if (num_params != 3) {
      PARSE_ERR(("%s: optromimage%d directive: wrong # args.", context, num));
    }
    sprintf(tmppath, "memory.optrom.%d.path", num);
    sprintf(tmpaddr, "memory.optrom.%d.addr", num);
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "file=", 5)) {
        SIM->get_param_string(tmppath)->set(&params[i][5]);
      } else if (!strncmp(params[i], "address=", 8)) {
        if ((params[i][8] == '0') && (params[2][9] == 'x'))
          SIM->get_param_num(tmpaddr)->set(strtoul(&params[i][8], NULL, 16));
        else
          SIM->get_param_num(tmpaddr)->set(strtoul(&params[i][8], NULL, 10));
      } else {
        PARSE_ERR(("%s: optromimage%d directive malformed.", context, num));
      }
    }
  } else if (!strncmp(params[0], "optramimage", 11)) {
    int num = atoi(&params[0][11]);
    char tmppath[80], tmpaddr[80];
    if ((num < 1) || (num > BX_N_OPTRAM_IMAGES)) {
      PARSE_ERR(("%s: optramimage%d: not supported", context, num));
    }
    if (num_params != 3) {
      PARSE_ERR(("%s: optramimage%d directive: wrong # args.", context, num));
    }
    sprintf(tmppath, "memory.optram.%d.path", num);
    sprintf(tmpaddr, "memory.optram.%d.addr", num);
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "file=", 5)) {
        SIM->get_param_string(tmppath)->set(&params[i][5]);
      } else if (!strncmp(params[i], "address=", 8)) {
        if ((params[i][8] == '0') && (params[2][9] == 'x'))
          SIM->get_param_num(tmpaddr)->set(strtoul(&params[i][8], NULL, 16));
        else
          SIM->get_param_num(tmpaddr)->set(strtoul(&params[i][8], NULL, 10));
      } else {
        PARSE_ERR(("%s: optramimage%d directive malformed.", context, num));
      }
    }
  } else if (!strcmp(params[0], "vga_update_interval")) {
    Bit64u value;
    if (num_params != 2) {
      PARSE_ERR(("%s: vga_update_interval directive: wrong # args.", context));
    }
    value = atol(params[1]);
    if (value > 0) {
      SIM->get_param_num(BXPN_VGA_UPDATE_FREQUENCY)->set(1000000 / value);
      PARSE_WARN(("%s: 'vga_update_interval' will be replaced by new 'vga: update_freq' option.", context));
    } else {
      PARSE_ERR(("%s: invalid value for vga_update_interval", context));
    }
  } else if (!strcmp(params[0], "vga")) {
    if (num_params < 2) {
      PARSE_ERR(("%s: vga directive malformed.", context));
    }
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "extension=", 10)) {
        SIM->get_param_string(BXPN_VGA_EXTENSION)->set(&params[i][10]);
      } else if (!strncmp(params[i], "update_freq=", 12)) {
        SIM->get_param_num(BXPN_VGA_UPDATE_FREQUENCY)->set(atol(&params[i][12]));
      } else {
        PARSE_ERR(("%s: vga directive malformed.", context));
      }
    }
  } else if (!strcmp(params[0], "keyboard")) {
    if (num_params < 2) {
      PARSE_ERR(("%s: keyboard directive malformed.", context));
    }
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "serial_delay=", 13)) {
        SIM->get_param_num(BXPN_KBD_SERIAL_DELAY)->set(atol(&params[i][13]));
        if (SIM->get_param_num(BXPN_KBD_SERIAL_DELAY)->get() < 5) {
          PARSE_ERR (("%s: keyboard serial delay not big enough!", context));
        }
      } else if (!strncmp(params[i], "paste_delay=", 12)) {
        SIM->get_param_num(BXPN_KBD_PASTE_DELAY)->set(atol(&params[i][12]));
        if (SIM->get_param_num(BXPN_KBD_PASTE_DELAY)->get() < 1000) {
          PARSE_ERR (("%s: keyboard paste delay not big enough!", context));
        }
      } else if (!strncmp(params[i], "type=", 5)) {
        if (!SIM->get_param_enum(BXPN_KBD_TYPE)->set_by_name(&params[i][5])) {
          PARSE_ERR(("%s: keyboard_type directive: wrong arg '%s'.", context,params[1]));
        }
      } else if (!strncmp(params[i], "keymap=", 7)) {
        SIM->get_param_bool(BXPN_KBD_USEMAPPING)->set(strlen(params[i]) > 7);
        SIM->get_param_string(BXPN_KBD_KEYMAP)->set(&params[i][7]);
      } else {
        PARSE_ERR(("%s: keyboard directive malformed.", context));
      }
    }
  } else if (!strcmp(params[0], "keyboard_serial_delay")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: keyboard_serial_delay directive: wrong # args.", context));
    }
    SIM->get_param_num(BXPN_KBD_SERIAL_DELAY)->set(atol(params[1]));
    if (SIM->get_param_num(BXPN_KBD_SERIAL_DELAY)->get() < 5) {
      PARSE_ERR (("%s: keyboard_serial_delay not big enough!", context));
    }
    PARSE_WARN(("%s: 'keyboard_serial_delay' will be replaced by new 'keyboard' option.", context));
  } else if (!strcmp(params[0], "keyboard_paste_delay")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: keyboard_paste_delay directive: wrong # args.", context));
    }
    SIM->get_param_num(BXPN_KBD_PASTE_DELAY)->set(atol(params[1]));
    if (SIM->get_param_num(BXPN_KBD_PASTE_DELAY)->get() < 1000) {
      PARSE_ERR (("%s: keyboard_paste_delay not big enough!", context));
    }
    PARSE_WARN(("%s: 'keyboard_paste_delay' will be replaced by new 'keyboard' option.", context));
  } else if (!strcmp(params[0], "text_snapshot_check")) {
    PARSE_ERR(("%s: the 'text_snapshot_check' feature has been removed.", context));
  } else if (!strcmp(params[0], "mouse")) {
    if (num_params < 2) {
      PARSE_ERR(("%s: mouse directive malformed.", context));
    }
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        if (parse_param_bool(params[1], 8, BXPN_MOUSE_ENABLED) < 0) {
          PARSE_ERR(("%s: mouse directive malformed.", context));
        }
      } else if (!strncmp(params[i], "type=", 5)) {
        if (!SIM->get_param_enum(BXPN_MOUSE_TYPE)->set_by_name(&params[i][5]))
          PARSE_ERR(("%s: mouse type '%s' not available", context, &params[i][5]));
      } else if (!strncmp(params[i], "toggle=", 7)) {
        if (!SIM->get_param_enum(BXPN_MOUSE_TOGGLE)->set_by_name(&params[i][7]))
          PARSE_ERR(("%s: mouse toggle method '%s' not available", context, &params[i][7]));
      } else {
        PARSE_ERR(("%s: mouse directive malformed.", context));
      }
    }
  } else if (!strcmp(params[0], "private_colormap")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: private_colormap directive malformed.", context));
    }
    if (strncmp(params[1], "enabled=", 8)) {
      PARSE_ERR(("%s: private_colormap directive malformed.", context));
    }
    if (parse_param_bool(params[1], 8, BXPN_PRIVATE_COLORMAP) < 0) {
      PARSE_ERR(("%s: private_colormap directive malformed.", context));
    }
  } else if (!strcmp(params[0], "fullscreen")) {
#if BX_WITH_AMIGAOS
    if (num_params != 2) {
      PARSE_ERR(("%s: fullscreen directive malformed.", context));
    }
    if (strncmp(params[1], "enabled=", 8)) {
      PARSE_ERR(("%s: fullscreen directive malformed.", context));
    }
    if (parse_param_bool(params[1], 8, BXPN_FULLSCREEN) < 0) {
      PARSE_ERR(("%s: fullscreen directive malformed.", context));
    }
#endif
  } else if (!strcmp(params[0], "screenmode")) {
#if BX_WITH_AMIGAOS
    if (num_params != 2) {
      PARSE_ERR(("%s: screenmode directive malformed.", context));
    }
    if (strncmp(params[1], "name=", 5)) {
      PARSE_ERR(("%s: screenmode directive malformed.", context));
    }
    SIM->get_param_string(BXPN_SCREENMODE)->set(&params[1][5]);
#endif
  } else if ((!strcmp(params[0], "pci")) ||
             (!strcmp(params[0], "i440fxsupport"))) {
    // new option 'pci' for future extensions
    char tmpdev[80];
    int enabled = -1;
    int chipset = -1;
    if (!strcmp(params[0], "i440fxsupport")) {
      chipset = 1;
      PARSE_WARN(("%s: 'i440fxsupport' will be replaced by new 'pci' option.", context));
    }
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        enabled = atol(&params[i][8]);
      } else if (!strncmp(params[i], "chipset=", 8)) {
        if (!strcmp(&params[i][8], "i440fx")) {
          chipset = 1;
        } else {
          PARSE_ERR(("%s: pci: unknown chipset '%s'", context, &params[i][8]));
        }
      } else if ((!strncmp(params[i], "slot", 4)) && (params[i][5] == '=')) {
        slot = atol(&params[i][4]);
        if ((slot > 0) && (slot < 6)) {
          sprintf(tmpdev, "pci.slot.%d", slot);
          SIM->get_param_string(tmpdev)->set(&params[i][6]);
        } else {
          BX_ERROR(("%s: unknown pci slot number ignored.", context));
        }
      } else {
        PARSE_ERR(("%s: pci: unknown parameter '%s'.", context, params[i]));
      }
    }
    if (enabled == 0) {
      SIM->get_param_bool(BXPN_I440FX_SUPPORT)->set(0);
    } else if (enabled == 1) {
      if (chipset == 1) {
        SIM->get_param_bool(BXPN_I440FX_SUPPORT)->set(1);
      } else {
        PARSE_ERR(("%s: pci: chipset not specified", context));
      }
    }
  } else if (!strcmp(params[0], "cmosimage")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "file=", 5)) {
        SIM->get_param_string(BXPN_CMOSIMAGE_PATH)->set(&params[i][5]);
      } else if (!strcmp(params[i], "rtc_init=time0")) {
        SIM->get_param_bool(BXPN_CMOSIMAGE_RTC_INIT)->set(0);
      } else if (!strcmp(params[i], "rtc_init=image")) {
        SIM->get_param_bool(BXPN_CMOSIMAGE_RTC_INIT)->set(1);
      } else {
        // for backward compatiblity
        SIM->get_param_string(BXPN_CMOSIMAGE_PATH)->set(params[i]);
      }
    }
    if (strlen(SIM->get_param_string(BXPN_CMOSIMAGE_PATH)->getptr()) > 0) {
      SIM->get_param_bool(BXPN_CMOSIMAGE_ENABLED)->set(1);
    }
  } else if (!strcmp(params[0], "clock")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "sync=", 5)) {
        SIM->get_param_enum(BXPN_CLOCK_SYNC)->set_by_name(&params[i][5]);
      }
      else if (!strncmp(params[i], "rtc_sync=", 9)) {
        SIM->get_param_bool(BXPN_CLOCK_RTC_SYNC)->set(atol(&params[i][9]));
      }
      else if (!strcmp(params[i], "time0=local")) {
        SIM->get_param_num(BXPN_CLOCK_TIME0)->set(BX_CLOCK_TIME0_LOCAL);
      }
      else if (!strcmp(params[i], "time0=utc")) {
        SIM->get_param_num(BXPN_CLOCK_TIME0)->set(BX_CLOCK_TIME0_UTC);
      }
      else if (!strncmp(params[i], "time0=", 6)) {
        SIM->get_param_num(BXPN_CLOCK_TIME0)->set(atoi(&params[i][6]));
      }
      else {
        BX_ERROR(("%s: unknown parameter for clock ignored.", context));
      }
    }
  }
  else if (!strcmp(params[0], "gdbstub")) {
#if BX_GDBSTUB
    if (num_params < 2) {
      PARSE_ERR(("%s: gdbstub directive: wrong # args.", context));
    }
    base = (bx_list_c*) SIM->get_param(BXPN_GDBSTUB);
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        if (params[i][8] == '0') {
          SIM->get_param_bool("enabled", base)->set(0);
          BX_INFO(("Disabled gdbstub"));
          bx_dbg.gdbstub_enabled = 0;
        }
        else if (params[i][8] == '1') {
          SIM->get_param_bool("enabled", base)->set(1);
          BX_INFO(("Enabled gdbstub"));
          bx_dbg.gdbstub_enabled = 1;
        }
        else {
          PARSE_ERR(("%s: gdbstub directive malformed.", context));
        }
      }
      else if (!strncmp(params[i], "port=", 5)) {
        SIM->get_param_num("port", base)->set(atoi(&params[i][5]));
      }
      else if (!strncmp(params[i], "text_base=", 10)) {
        SIM->get_param_num("text_base", base)->set(atoi(&params[i][10]));
      }
      else if (!strncmp(params[i], "data_base=", 10)) {
        SIM->get_param_num("data_base", base)->set(atoi(&params[i][10]));
      }
      else if (!strncmp(params[i], "bss_base=", 9)) {
        SIM->get_param_num("bss_base", base)->set(atoi(&params[i][9]));
      }
      else {
        PARSE_ERR(("%s: gdbstub directive malformed.", context));
      }
    }
#else
    PARSE_ERR(("%s: Bochs is not compiled with gdbstub support", context));
#endif
  }
  else if (!strcmp(params[0], "magic_break")) {
#if BX_DEBUGGER
    if (num_params != 2) {
      PARSE_ERR(("%s: magic_break directive: wrong # args.", context));
    }
    if (strncmp(params[1], "enabled=", 8)) {
      PARSE_ERR(("%s: magic_break directive malformed.", context));
    }
    if (params[1][8] == '0') {
      BX_INFO(("Ignoring magic break points"));
      bx_dbg.magic_break_enabled = 0;
    }
    else if (params[1][8] == '1') {
      BX_INFO(("Stopping on magic break points"));
      bx_dbg.magic_break_enabled = 1;
    }
    else {
      PARSE_ERR(("%s: magic_break directive malformed.", context));
    }
#else
    PARSE_WARN(("%s: Bochs is not compiled with internal debugger support", context));
#endif
  }
  else if (!strcmp(params[0], "debug_symbols")) {
    if (parse_debug_symbols(context, (const char **)(params + 1), num_params - 1) < 0) {
      return -1;
    }
  }
  else if (!strcmp(params[0], "print_timestamps")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: print_timestamps directive: wrong # args.", context));
    }
    if (strncmp(params[1], "enabled=", 8)) {
      PARSE_ERR(("%s: print_timestamps directive malformed.", context));
    }
    if (params[1][8] == '0' || params[1][8] == '1') {
      bx_dbg.print_timestamps = params[1][8] - '0';
    }
    else {
      PARSE_ERR(("%s: print_timestamps directive malformed.", context));
    }
  }
  else if (!strcmp(params[0], "port_e9_hack")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: port_e9_hack directive: wrong # args.", context));
    }
    if (strncmp(params[1], "enabled=", 8)) {
      PARSE_ERR(("%s: port_e9_hack directive malformed.", context));
    }
    if (parse_param_bool(params[1], 8, BXPN_PORT_E9_HACK) < 0) {
      PARSE_ERR(("%s: port_e9_hack directive malformed.", context));
    }
  }
  else if (!strcmp(params[0], "load32bitOSImage")) {
    if ((num_params!=4) && (num_params!=5)) {
      PARSE_ERR(("%s: load32bitOSImage directive: wrong # args.", context));
    }
    if (strncmp(params[1], "os=", 3)) {
      PARSE_ERR(("%s: load32bitOSImage: directive malformed.", context));
    }
    if (!strcmp(&params[1][3], "nullkernel")) {
      SIM->get_param_enum(BXPN_LOAD32BITOS_WHICH)->set(Load32bitOSNullKernel);
    }
    else if (!strcmp(&params[1][3], "linux")) {
      SIM->get_param_enum(BXPN_LOAD32BITOS_WHICH)->set(Load32bitOSLinux);
    }
    else {
      PARSE_ERR(("%s: load32bitOSImage: unsupported OS.", context));
    }
    if (strncmp(params[2], "path=", 5)) {
      PARSE_ERR(("%s: load32bitOSImage: directive malformed.", context));
    }
    if (strncmp(params[3], "iolog=", 6)) {
      PARSE_ERR(("%s: load32bitOSImage: directive malformed.", context));
    }
    SIM->get_param_string(BXPN_LOAD32BITOS_PATH)->set(&params[2][5]);
    SIM->get_param_string(BXPN_LOAD32BITOS_IOLOG)->set(&params[3][6]);
    if (num_params == 5) {
      if (strncmp(params[4], "initrd=", 7)) {
        PARSE_ERR(("%s: load32bitOSImage: directive malformed.", context));
      }
      SIM->get_param_string(BXPN_LOAD32BITOS_INITRD)->set(&params[4][7]);
    }
  }
  else if (!strcmp(params[0], "keyboard_type")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: keyboard_type directive: wrong # args.", context));
    }
    if (!SIM->get_param_enum(BXPN_KBD_TYPE)->set_by_name(params[1])) {
      PARSE_ERR(("%s: keyboard_type directive: wrong arg '%s'.", context,params[1]));
    }
    PARSE_WARN(("%s: 'keyboard_type' will be replaced by new 'keyboard' option.", context));
  }
  else if (!strcmp(params[0], "keyboard_mapping")
         ||!strcmp(params[0], "keyboardmapping")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        SIM->get_param_bool(BXPN_KBD_USEMAPPING)->set(atol(&params[i][8]));
      }
      else if (!strncmp(params[i], "map=", 4)) {
        SIM->get_param_string(BXPN_KBD_KEYMAP)->set(&params[i][4]);
      }
    }
    PARSE_WARN(("%s: '%s' will be replaced by new 'keyboard' option.", context, params[0]));
  }
  else if (!strcmp(params[0], "user_shortcut"))
  {
    if (num_params != 2) {
      PARSE_ERR(("%s: user_shortcut directive: wrong # args.", context));
    }
    if(!strncmp(params[1], "keys=", 4)) {
      SIM->get_param_string(BXPN_USER_SHORTCUT)->set(&params[1][5]);
      if ((strchr(&params[1][5], '-') == NULL) && (strlen(&params[1][5]) > 5))
        PARSE_WARN(("user_shortcut: old-style syntax detected"));
    } else {
      PARSE_ERR(("%s: user_shortcut directive malformed.", context));
    }
  }
  else if (!strcmp(params[0], "config_interface"))
  {
    if (num_params != 2) {
      PARSE_ERR(("%s: config_interface directive: wrong # args.", context));
    }
    if (!SIM->get_param_enum(BXPN_SEL_CONFIG_INTERFACE)->set_by_name(params[1]))
      PARSE_ERR(("%s: config_interface '%s' not available", context, params[1]));
  }
  else if (!strcmp(params[0], "display_library")) {
    if ((num_params < 2) || (num_params > 3)) {
      PARSE_ERR(("%s: display_library directive: wrong # args.", context));
    }
    if (!SIM->get_param_enum(BXPN_SEL_DISPLAY_LIBRARY)->set_by_name(params[1]))
      PARSE_ERR(("%s: display library '%s' not available", context, params[1]));
    if (num_params == 3) {
      if (!strncmp(params[2], "options=", 8)) {
        SIM->get_param_string(BXPN_DISPLAYLIB_OPTIONS)->set(&params[2][8]);
      }
    }
  }
#if BX_PLUGINS
  else if (!strcmp(params[0], "user_plugin")) {
    char tmpname[80];
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "name=", 5)) {
        if (bx_user_plugin_count < BX_N_USER_PLUGINS) {
          sprintf(tmpname, "misc.user_plugin.%d", ++bx_user_plugin_count);
          SIM->get_param_string(tmpname)->set(&params[i][5]);
        } else {
          PARSE_ERR(("%s: too many user plugins", context));
        }
      } else {
        PARSE_ERR(("%s: unknown user plugin parameter '%s'", context, params[i]));
      }
    }
  }
#endif
  else if (!strcmp(params[0], "plugin_ctrl")) {
    char *param, *pname, *val;
    for (i=1; i<num_params; i++) {
      param = strdup(params[i]);
      pname = strtok(param, "=");
      val = strtok(NULL, "");
      if (val != NULL) {
        if (isdigit(val[0])) {
          SIM->opt_plugin_ctrl(pname, atoi(val));
        } else {
          PARSE_ERR(("%s: plugin_ctrl directive malformed", context));
        }
      } else {
        PARSE_ERR(("%s: plugin_ctrl directive malformed", context));
      }
      free(param);
    }
  }
  // add-on options handled by registered functions
  else if (SIM->is_addon_option(params[0]))
  {
    return SIM->parse_addon_option(context, num_params, &params[0]);
  }
  // treat unknown option as plugin name and try to load it
  else if (SIM->opt_plugin_ctrl(params[0], 1))
  {
    // after loading the plugin a bochsrc option with it's name must exist
    if (SIM->is_addon_option(params[0])) {
      return SIM->parse_addon_option(context, num_params, &params[0]);
    } else {
      PARSE_ERR(("%s: directive '%s' not understood", context, params[0]));
    }
  }
  else
  {
    PARSE_ERR(("%s: directive '%s' not understood", context, params[0]));
  }
  return 0;
}


static const char *fdtypes[] = {
  "none", "1_2", "1_44", "2_88", "720k", "360k", "160k", "180k", "320k"
};

int bx_write_floppy_options(FILE *fp, int drive)
{
  char devtype[80], path[80], type[80], status[80], readonly[80];
  int ftype;

  BX_ASSERT(drive==0 || drive==1);
  sprintf(devtype, "floppy.%d.devtype", drive);
  sprintf(path, "floppy.%d.path", drive);
  sprintf(type, "floppy.%d.type", drive);
  sprintf(status, "floppy.%d.status", drive);
  sprintf(readonly, "floppy.%d.readonly", drive);
  ftype = SIM->get_param_enum(devtype)->get();
  if (ftype == BX_FDD_NONE) {
    fprintf(fp, "# no floppy%c\n", (char)'a'+drive);
    return 0;
  } else {
    fprintf(fp, "floppy%c: type=", (char)'a'+drive);
    if (ftype == BX_FDD_350ED) {
      fprintf(fp, "2_88");
    } else if (ftype == BX_FDD_350HD) {
      fprintf(fp, "1_44");
    } else if (ftype == BX_FDD_525HD) {
      fprintf(fp, "1_2");
    } else if (ftype == BX_FDD_350DD) {
      fprintf(fp, "720k");
    } else if (ftype == BX_FDD_525DD) {
      fprintf(fp, "360k");
    }
  }
  if ((SIM->get_param_enum(type)->get() > BX_FLOPPY_NONE) &&
      (SIM->get_param_enum(type)->get() <= BX_FLOPPY_LAST)) {
    fprintf(fp, ", %s=\"%s\", status=%s, write_protected=%d",
      fdtypes[SIM->get_param_enum(type)->get() - BX_FLOPPY_NONE],
      SIM->get_param_string(path)->getptr(),
      SIM->get_param_bool(status)->get() ? "inserted":"ejected",
      SIM->get_param_bool(readonly)->get());
  }
  fprintf(fp, "\n");
  return 0;
}

int bx_write_ata_options(FILE *fp, Bit8u channel, bx_list_c *base)
{
  fprintf(fp, "ata%d: enabled=%d", channel, SIM->get_param_bool("enabled", base)->get());

  if (SIM->get_param_bool("enabled", base)->get()) {
    fprintf(fp, ", ioaddr1=0x%x, ioaddr2=0x%x, irq=%d", SIM->get_param_num("ioaddr1", base)->get(),
      SIM->get_param_num("ioaddr2", base)->get(), SIM->get_param_num("irq", base)->get());
    }

  fprintf(fp, "\n");
  return 0;
}

int bx_write_atadevice_options(FILE *fp, Bit8u channel, Bit8u drive, bx_list_c *base)
{
  if (SIM->get_param_bool("present", base)->get()) {
    fprintf(fp, "ata%d-%s: ", channel, drive==0?"master":"slave");

    if (SIM->get_param_enum("type", base)->get() == BX_ATA_DEVICE_DISK) {
      fprintf(fp, "type=disk");

      fprintf(fp, ", mode=%s", SIM->get_param_enum("mode", base)->get_selected());
      fprintf(fp, ", translation=%s", SIM->get_param_enum("translation", base)->get_selected());
      fprintf(fp, ", path=\"%s\", cylinders=%d, heads=%d, spt=%d",
          SIM->get_param_string("path", base)->getptr(),
          SIM->get_param_num("cylinders", base)->get(),
          SIM->get_param_num("heads", base)->get(),
          SIM->get_param_num("spt", base)->get());

      if (SIM->get_param_string("journal", base)->getptr() != NULL)
        if (strcmp(SIM->get_param_string("journal", base)->getptr(), "") != 0)
          fprintf(fp, ", journal=\"%s\"", SIM->get_param_string("journal", base)->getptr());

    } else if (SIM->get_param_enum("type", base)->get() == BX_ATA_DEVICE_CDROM) {
      fprintf(fp, "type=cdrom, path=\"%s\", status=%s",
        SIM->get_param_string("path", base)->getptr(),
        SIM->get_param_bool("status", base)->get() ? "inserted":"ejected");
    }

    fprintf(fp, ", biosdetect=%s", SIM->get_param_enum("biosdetect", base)->get_selected());

    if (SIM->get_param_string("model", base)->getptr()>0) {
        fprintf(fp, ", model=\"%s\"", SIM->get_param_string("model", base)->getptr());
    }

    fprintf(fp, "\n");
  }
  return 0;
}

int bx_write_usb_options(FILE *fp, int maxports, bx_list_c *base)
{
  int i;
  char tmpname[20];

  fprintf(fp, "usb_%s: enabled=%d", base->get_name(), SIM->get_param_bool("enabled", base)->get());
  if (SIM->get_param_bool("enabled", base)->get()) {
    for (i = 1; i <= maxports; i++) {
      sprintf(tmpname, "port%d.device", i);
      fprintf(fp, ", port%d=%s", i, SIM->get_param_string(tmpname, base)->getptr());
      sprintf(tmpname, "port%d.options", i);
      fprintf(fp, ", options%d=%s", i, SIM->get_param_string(tmpname, base)->getptr());
    }
  }
  fprintf(fp, "\n");
  return 0;
}

int bx_write_pci_nic_options(FILE *fp, bx_list_c *base)
{
  fprintf (fp, "%s: enabled=%d", base->get_name(), SIM->get_param_bool("enabled", base)->get());
  if (SIM->get_param_bool("enabled", base)->get()) {
    char *ptr = SIM->get_param_string("macaddr", base)->getptr();
    fprintf (fp, ", mac=%02x:%02x:%02x:%02x:%02x:%02x, ethmod=%s, ethdev=%s, script=%s, bootrom=%s",
      (unsigned int)(0xff & ptr[0]),
      (unsigned int)(0xff & ptr[1]),
      (unsigned int)(0xff & ptr[2]),
      (unsigned int)(0xff & ptr[3]),
      (unsigned int)(0xff & ptr[4]),
      (unsigned int)(0xff & ptr[5]),
      SIM->get_param_enum("ethmod", base)->get_selected(),
      SIM->get_param_string("ethdev", base)->getptr(),
      SIM->get_param_string("script", base)->getptr(),
      SIM->get_param_string("bootrom", base)->getptr());
  }
  fprintf (fp, "\n");
  return 0;
}

int bx_write_loader_options(FILE *fp)
{
  if (SIM->get_param_enum(BXPN_LOAD32BITOS_WHICH)->get() == Load32bitOSNone) {
    fprintf(fp, "# no loader\n");
    return 0;
  }
  BX_ASSERT((SIM->get_param_enum(BXPN_LOAD32BITOS_WHICH)->get() == Load32bitOSLinux) ||
            (SIM->get_param_enum(BXPN_LOAD32BITOS_WHICH)->get() == Load32bitOSNullKernel));
  fprintf (fp, "load32bitOSImage: os=%s, path=%s, iolog=%s, initrd=%s\n",
      (SIM->get_param_enum(BXPN_LOAD32BITOS_WHICH)->get() == Load32bitOSLinux) ? "linux" : "nullkernel",
      SIM->get_param_string(BXPN_LOAD32BITOS_PATH)->getptr(),
      SIM->get_param_string(BXPN_LOAD32BITOS_IOLOG)->getptr(),
      SIM->get_param_string(BXPN_LOAD32BITOS_INITRD)->getptr());
  return 0;
}

int bx_write_clock_cmos_options(FILE *fp)
{
  fprintf(fp, "clock: ");

  switch (SIM->get_param_enum(BXPN_CLOCK_SYNC)->get()) {
    case BX_CLOCK_SYNC_NONE:
      fprintf(fp, "sync=none");
      break;
    case BX_CLOCK_SYNC_REALTIME:
      fprintf(fp, "sync=realtime");
      break;
    case BX_CLOCK_SYNC_SLOWDOWN:
      fprintf(fp, "sync=slowdown");
      break;
    case BX_CLOCK_SYNC_BOTH:
      fprintf(fp, "sync=both");
      break;
    default:
      BX_PANIC(("Unknown value for sync method"));
  }

  switch (SIM->get_param_num(BXPN_CLOCK_TIME0)->get()) {
    case 0: break;
    case BX_CLOCK_TIME0_LOCAL:
      fprintf(fp, ", time0=local");
      break;
    case BX_CLOCK_TIME0_UTC:
      fprintf(fp, ", time0=utc");
      break;
    default:
      fprintf(fp, ", time0=%u", SIM->get_param_num(BXPN_CLOCK_TIME0)->get());
  }

  fprintf(fp, ", rtc_sync=%d\n", SIM->get_param_bool(BXPN_CLOCK_RTC_SYNC)->get());

  if (strlen(SIM->get_param_string(BXPN_CMOSIMAGE_PATH)->getptr()) > 0) {
    fprintf(fp, "cmosimage: file=%s, ", SIM->get_param_string(BXPN_CMOSIMAGE_PATH)->getptr());
    fprintf(fp, "rtc_init=%s\n", SIM->get_param_bool(BXPN_CMOSIMAGE_RTC_INIT)->get()?"image":"time0");
  } else {
    fprintf(fp, "# no cmosimage\n");
  }
  return 0;
}

int bx_write_log_options(FILE *fp, bx_list_c *base)
{
  fprintf(fp, "log: %s\n", SIM->get_param_string("filename", base)->getptr());
  fprintf(fp, "logprefix: %s\n", SIM->get_param_string("prefix", base)->getptr());
  fprintf(fp, "panic: action=%s\n",
      io->getaction(logfunctions::get_default_action(LOGLEV_PANIC)));
  fprintf(fp, "error: action=%s\n",
      io->getaction(logfunctions::get_default_action(LOGLEV_ERROR)));
  fprintf(fp, "info: action=%s\n",
      io->getaction(logfunctions::get_default_action(LOGLEV_INFO)));
  fprintf(fp, "debug: action=%s\n",
      io->getaction(logfunctions::get_default_action(LOGLEV_DEBUG)));
  return 0;
}

int bx_write_keyboard_options(FILE *fp)
{
  fprintf(fp, "keyboard: type=%s, serial_delay=%u, paste_delay=%u, ",
    SIM->get_param_enum(BXPN_KBD_TYPE)->get_selected(),
    SIM->get_param_num(BXPN_KBD_SERIAL_DELAY)->get(),
    SIM->get_param_num(BXPN_KBD_PASTE_DELAY)->get());
  if (SIM->get_param_bool(BXPN_KBD_USEMAPPING)->get()) {
    fprintf(fp, "keymap=%s\n", SIM->get_param_string(BXPN_KBD_KEYMAP)->getptr());
  } else {
    fprintf(fp, "keymap=\n");
  }
  fprintf(fp, "user_shortcut: keys=%s\n", SIM->get_param_string(BXPN_USER_SHORTCUT)->getptr());
  return 0;
}

int bx_write_debugger_options(FILE *fp)
{
#if BX_DEBUGGER
  fprintf(fp, "debugger_log: %s\n", SIM->get_param_string(BXPN_DEBUGGER_LOG_FILENAME)->getptr());
  fprintf(fp, "magic_break: enabled=%d\n", bx_dbg.magic_break_enabled);
  // TODO: debug symbols
#endif
#if BX_GDBSTUB
  bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_GDBSTUB);
  bx_bool enabled = SIM->get_param_bool("enabled", base)->get();
  if (enabled) {
    fprintf(fp, "gdbstub: enabled=%d, port=%d, text_base=%d, data_base=%d, bss_base=%d\n",
            enabled, SIM->get_param_num("port", base)->get(), SIM->get_param_num("text_base", base)->get(),
            SIM->get_param_num("data_base", base)->get(), SIM->get_param_num("bss_base", base)->get());
  } else {
    fprintf(fp, "# no gdb stub\n");
  }
#endif
  return 0;
}

// return values:
//   0: written ok
//  -1: failed
//  -2: already exists, and overwrite was off
int bx_write_configuration(const char *rc, int overwrite)
{
  int i;
  char *strptr, tmppath[80], tmpaddr[80], tmpdev[80];
  bx_list_c *base;
  BX_INFO(("write current configuration to %s", rc));
  // check if it exists.  If so, only proceed if overwrite is set.
  FILE *fp = fopen(rc, "r");
  if (fp != NULL) {
    fclose(fp);
    if (!overwrite) return -2;
  }
  fp = fopen(rc, "w");
  if (fp == NULL) return -1;
  // finally it's open and we can start writing.
  fprintf(fp, "# configuration file generated by Bochs\n");
  base = (bx_list_c*) SIM->get_param(BXPN_PLUGIN_CTRL);
  if (base->get_size() > 0) {
    fprintf(fp, "plugin_ctrl: ");
    for (i = 0; i < base->get_size(); i++) {
      if (i > 0) fprintf(fp, ", ");
      bx_param_bool_c *plugin = (bx_param_bool_c*)(base->get(i));
      fprintf(fp, "%s=1", plugin->get_name());
    }
    fprintf(fp, "\n");
  }
#if BX_PLUGINS
  // user plugins
  for (i=0; i<BX_N_USER_PLUGINS; i++) {
    sprintf(tmpdev, "misc.user_plugin.%d", i+1);
    if (strlen(SIM->get_param_string(tmpdev)->getptr()) > 0) {
      fprintf(fp, "user_plugin: name=%s\n",
        SIM->get_param_string(tmpdev)->getptr());
    }
  }
#endif
  fprintf(fp, "config_interface: %s\n", SIM->get_param_enum(BXPN_SEL_CONFIG_INTERFACE)->get_selected());
  fprintf(fp, "display_library: %s", SIM->get_param_enum(BXPN_SEL_DISPLAY_LIBRARY)->get_selected());
  strptr = SIM->get_param_string(BXPN_DISPLAYLIB_OPTIONS)->getptr();
  if (strlen(strptr) > 0)
    fprintf(fp, ", options=\"%s\"\n", strptr);
  else
    fprintf(fp, "\n");
  fprintf(fp, "memory: host=%d, guest=%d\n", SIM->get_param_num(BXPN_HOST_MEM_SIZE)->get(),
    SIM->get_param_num(BXPN_MEM_SIZE)->get());
  strptr = SIM->get_param_string(BXPN_ROM_PATH)->getptr();
  if (strlen(strptr) > 0) {
    fprintf(fp, "romimage: file=\"%s\"", strptr);
    if (SIM->get_param_num(BXPN_ROM_ADDRESS)->get() != 0)
      fprintf(fp, ", address=0x%08x\n", (unsigned int) SIM->get_param_num(BXPN_ROM_ADDRESS)->get());
    else
      fprintf(fp, "\n");
  }
  else {
    fprintf(fp, "# no romimage\n");
  }
  strptr = SIM->get_param_string(BXPN_VGA_ROM_PATH)->getptr();
  if (strlen(strptr) > 0)
    fprintf(fp, "vgaromimage: file=\"%s\"\n", strptr);
  else
    fprintf(fp, "# no vgaromimage\n");
  fprintf(fp, "boot: %s", SIM->get_param_enum(BXPN_BOOTDRIVE1)->get_selected());
  for (i=1; i<3; i++) {
    sprintf(tmppath, "boot_params.boot_drive%d", i+1);
    if (SIM->get_param_enum(tmppath)->get() != BX_BOOT_NONE) {
      fprintf(fp, ", %s", SIM->get_param_enum(tmppath)->get_selected());
    }
  }
  fprintf(fp, "\n");
  fprintf(fp, "floppy_bootsig_check: disabled=%d\n", SIM->get_param_bool(BXPN_FLOPPYSIGCHECK)->get());
  // it would be nice to put this type of function as methods on
  // the structs like bx_floppy_options::print or something.
  bx_write_floppy_options(fp, 0);
  bx_write_floppy_options(fp, 1);
  for (Bit8u channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    sprintf(tmppath, "ata.%d", channel);
    base = (bx_list_c*) SIM->get_param(tmppath);
    bx_write_ata_options(fp, channel, (bx_list_c*) SIM->get_param("resources", base));
    bx_write_atadevice_options(fp, channel, 0, (bx_list_c*) SIM->get_param("master", base));
    bx_write_atadevice_options(fp, channel, 1, (bx_list_c*) SIM->get_param("slave", base));
  }
  for (i=0; i<BX_N_OPTROM_IMAGES; i++) {
    sprintf(tmppath, "memory.optrom.%d.path", i+1);
    sprintf(tmpaddr, "memory.optrom.%d.addr", i+1);
    strptr = SIM->get_param_string(tmppath)->getptr();
    if (strlen(strptr) > 0)
      fprintf(fp, "optromimage%d: file=\"%s\", address=0x%05x\n", i+1, strptr,
              (unsigned int)SIM->get_param_num(tmpaddr)->get());
  }
  for (i=0; i<BX_N_OPTRAM_IMAGES; i++) {
    sprintf(tmppath, "memory.optram.%d.path", i+1);
    sprintf(tmpaddr, "memory.optram.%d.addr", i+1);
    strptr = SIM->get_param_string(tmppath)->getptr();
    if (strlen(strptr) > 0)
      fprintf(fp, "optramimage%d: file=\"%s\", address=0x%05x\n", i+1, strptr,
              (unsigned int)SIM->get_param_num(tmpaddr)->get());
  }
  // pci
  fprintf(fp, "pci: enabled=%d",
          SIM->get_param_bool(BXPN_I440FX_SUPPORT)->get());
  if (SIM->get_param_bool(BXPN_I440FX_SUPPORT)->get()) {
    fprintf(fp, ", chipset=i440fx");
    for (i=0; i<BX_N_PCI_SLOTS; i++) {
      sprintf(tmpdev, "pci.slot.%d", i+1);
      strptr = SIM->get_param_string(tmpdev)->getptr();
      if (strlen(strptr) > 0) {
        fprintf(fp, ", slot%d=%s", i+1, strptr);
      }
    }
  }
  fprintf(fp, "\n");
  fprintf(fp, "vga: extension=%s, update_freq=%u\n",
    SIM->get_param_string(BXPN_VGA_EXTENSION)->getptr(),
    SIM->get_param_num(BXPN_VGA_UPDATE_FREQUENCY)->get());
#if BX_SUPPORT_SMP
  fprintf(fp, "cpu: count=%u:%u:%u, ips=%u, quantum=%d, ",
    SIM->get_param_num(BXPN_CPU_NPROCESSORS)->get(), SIM->get_param_num(BXPN_CPU_NCORES)->get(),
    SIM->get_param_num(BXPN_CPU_NTHREADS)->get(), SIM->get_param_num(BXPN_IPS)->get(),
    SIM->get_param_num(BXPN_SMP_QUANTUM)->get());
#else
  fprintf(fp, "cpu: count=1, ips=%u, ", SIM->get_param_num(BXPN_IPS)->get());
#endif
  fprintf(fp, "model=%s, reset_on_triple_fault=%d, cpuid_limit_winnt=%d",
    SIM->get_param_enum(BXPN_CPU_MODEL)->get_selected(),
    SIM->get_param_bool(BXPN_RESET_ON_TRIPLE_FAULT)->get(),
    SIM->get_param_bool(BXPN_CPUID_LIMIT_WINNT)->get());
#if BX_CPU_LEVEL >= 5
  fprintf(fp, ", ignore_bad_msrs=%d", SIM->get_param_bool(BXPN_IGNORE_BAD_MSRS)->get());
#endif
#if BX_SUPPORT_MONITOR_MWAIT
  fprintf(fp, ", mwait_is_nop=%d", SIM->get_param_bool(BXPN_MWAIT_IS_NOP)->get());
#endif
#if BX_CONFIGURE_MSRS
  strptr = SIM->get_param_string(BXPN_CONFIGURABLE_MSRS_PATH)->getptr();
  if (strlen(strptr) > 0)
    fprintf(fp, ", msrs=\"%s\"", strptr);
#endif
  fprintf(fp, "\n");

#if BX_CPU_LEVEL >= 4
  if (! SIM->get_param_enum(BXPN_CPU_MODEL)->get()) {
    // dump only when using BX_GENERIC CPUDB profile
    fprintf(fp, "cpuid: family=%d, model=0x%02x, stepping=%d", 
      SIM->get_param_num(BXPN_CPUID_FAMILY)->get(),
      SIM->get_param_num(BXPN_CPUID_MODEL)->get(),
      SIM->get_param_num(BXPN_CPUID_STEPPING)->get());
#if BX_CPU_LEVEL >= 5
    fprintf(fp, ", mmx=%d, apic=%s", SIM->get_param_bool(BXPN_CPUID_MMX)->get(),
      SIM->get_param_enum(BXPN_CPUID_APIC)->get_selected());
#endif
#if BX_CPU_LEVEL >= 6
    fprintf(fp, ", sse=%s, sse4a=%d, sep=%d, aes=%d, xsave=%d, xsaveopt=%d, movbe=%d, adx=%d, smep=%d",
      SIM->get_param_enum(BXPN_CPUID_SSE)->get_selected(),
      SIM->get_param_bool(BXPN_CPUID_SSE4A)->get(),
      SIM->get_param_bool(BXPN_CPUID_SEP)->get(),
      SIM->get_param_bool(BXPN_CPUID_AES)->get(),
      SIM->get_param_bool(BXPN_CPUID_XSAVE)->get(),
      SIM->get_param_bool(BXPN_CPUID_XSAVEOPT)->get(),
      SIM->get_param_bool(BXPN_CPUID_MOVBE)->get(),
      SIM->get_param_bool(BXPN_CPUID_ADX)->get(),
      SIM->get_param_bool(BXPN_CPUID_SMEP)->get());
#if BX_SUPPORT_AVX
    fprintf(fp, ", avx=%d, avx_f16c=%d, avx_fma=%d, bmi=%d, xop=%d, tbm=%d, fma4=%d", 
      SIM->get_param_num(BXPN_CPUID_AVX)->get(),
      SIM->get_param_bool(BXPN_CPUID_AVX_F16CVT)->get(),
      SIM->get_param_bool(BXPN_CPUID_AVX_FMA)->get(),
      SIM->get_param_num(BXPN_CPUID_BMI)->get(),
      SIM->get_param_bool(BXPN_CPUID_XOP)->get(),
      SIM->get_param_bool(BXPN_CPUID_TBM)->get(),
      SIM->get_param_bool(BXPN_CPUID_FMA4)->get());
#endif
#if BX_SUPPORT_VMX
    fprintf(fp, ", vmx=%d", SIM->get_param_num(BXPN_CPUID_VMX)->get());
#endif
#if BX_SUPPORT_SVM
    fprintf(fp, ", svm=%d", SIM->get_param_num(BXPN_CPUID_SVM)->get());
#endif
#if BX_SUPPORT_X86_64
    fprintf(fp, ", x86_64=%d, 1g_pages=%d, pcid=%d, fsgsbase=%d",
      SIM->get_param_bool(BXPN_CPUID_X86_64)->get(),
      SIM->get_param_bool(BXPN_CPUID_1G_PAGES)->get(),
      SIM->get_param_bool(BXPN_CPUID_PCID)->get(),
      SIM->get_param_bool(BXPN_CPUID_FSGSBASE)->get());
#endif
#if BX_SUPPORT_MONITOR_MWAIT
    fprintf(fp, ", mwait=%d",
      SIM->get_param_bool(BXPN_CPUID_MWAIT)->get());
#endif
#endif
    fprintf(fp, "\n");

    const char *vendor_string = SIM->get_param_string(BXPN_VENDOR_STRING)->getptr();
    if (vendor_string)
      fprintf(fp, "cpuid: vendor_string=\"%s\"\n", vendor_string);
    const char *brand_string = SIM->get_param_string(BXPN_BRAND_STRING)->getptr();
    if (brand_string)
      fprintf(fp, "cpuid: brand_string=\"%s\"\n", brand_string);
    fprintf(fp, "\n");
  }
#endif

  fprintf(fp, "print_timestamps: enabled=%d\n", bx_dbg.print_timestamps);
  bx_write_debugger_options(fp);
  fprintf(fp, "port_e9_hack: enabled=%d\n", SIM->get_param_bool(BXPN_PORT_E9_HACK)->get());
  fprintf(fp, "private_colormap: enabled=%d\n", SIM->get_param_bool(BXPN_PRIVATE_COLORMAP)->get());
#if BX_WITH_AMIGAOS
  fprintf(fp, "fullscreen: enabled=%d\n", SIM->get_param_bool(BXPN_FULLSCREEN)->get());
  fprintf(fp, "screenmode: name=\"%s\"\n", SIM->get_param_string(BXPN_SCREENMODE)->getptr());
#endif
  bx_write_clock_cmos_options(fp);
  bx_write_loader_options(fp);
  bx_write_log_options(fp, (bx_list_c*) SIM->get_param("log"));
  bx_write_keyboard_options(fp);
  fprintf(fp, "mouse: enabled=%d, type=%s, toggle=%s\n",
    SIM->get_param_bool(BXPN_MOUSE_ENABLED)->get(),
    SIM->get_param_enum(BXPN_MOUSE_TYPE)->get_selected(),
    SIM->get_param_enum(BXPN_MOUSE_TOGGLE)->get_selected());
  SIM->save_addon_options(fp);
  fclose(fp);
  return 0;
}
