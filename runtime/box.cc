/////////////////////////////////////////////////////////////////////////
// $Id: main.cc 11367 2012-08-25 13:20:55Z vruppert $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2011  The Bochs Project
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

#include "../bochs.h"
#include "../bxversion.h"
#include "../param_names.h"
#include "../gui/textconfig.h"
#include "../cpu/cpu.h"
#include "../iodev/iodev.h"

extern "C" {
#include <signal.h>
}

// some prototypes from iodev/
// I want to stay away from including iodev/iodev.h here
Bit32u bx_unmapped_io_read_handler(Bit32u address, unsigned io_len);
void   bx_unmapped_io_write_handler(Bit32u address, Bit32u value,
                                    unsigned io_len);

void bx_init_hardware(void);
void bx_init_options(void);
void bx_init_bx_dbg(void);

static const char *divider = "========================================================================";
static logfunctions thePluginLog;
logfunctions *pluginlog = &thePluginLog;

bx_startup_flags_t bx_startup_flags;
bx_bool bx_user_quit;
Bit8u bx_cpu_count;
#if BX_SUPPORT_APIC
Bit32u apic_id_mask; // determinted by XAPIC option
bx_bool simulate_xapic;
#endif

/* typedefs */

#define LOG_THIS genlog->

bx_pc_system_c bx_pc_system;

bx_debug_t bx_dbg;

typedef BX_CPU_C *BX_CPU_C_PTR;

#if BX_SUPPORT_SMP
// multiprocessor simulation, we need an array of cpus
BOCHSAPI BX_CPU_C_PTR *bx_cpu_array = NULL;
#else
// single processor simulation, so there's one of everything
BOCHSAPI BX_CPU_C bx_cpu;
#endif

BOCHSAPI BX_MEM_C bx_mem;

char *bochsrc_filename = NULL;

void bx_print_header()
{
  printf("%s\n", divider);
  char buffer[128];
  sprintf (buffer, "Box IA32 Process Emulator %s\n", VER_STRING);
  bx_center_print(stdout, buffer, 72);
  if (REL_STRING[0]) {
    sprintf(buffer, "%s\n", REL_STRING);
    bx_center_print(stdout, buffer, 72);
#ifdef __DATE__
#ifdef __TIME__
    sprintf(buffer, "Compiled on %s at %s\n", __DATE__, __TIME__);
#else
    sprintf(buffer, "Compiled on %s\n", __DATE__);
#endif
    bx_center_print(stdout, buffer, 72);
#endif
  }
  printf("%s\n", divider);
}

#if BX_DEBUGGER
void print_tree(bx_param_c *node, int level)
{
  int i;
  char tmpstr[BX_PATHNAME_LEN], tmpbyte[4];

  for (i=0; i<level; i++)
    dbg_printf("  ");
  if (node == NULL) {
      dbg_printf("NULL pointer\n");
      return;
  }
  switch (node->get_type()) {
    case BXT_PARAM_NUM:
      if (((bx_param_num_c*)node)->get_base() == BASE_DEC) {
        dbg_printf("%s = " FMT_LL "d (number)\n", node->get_name(), ((bx_param_num_c*)node)->get64());
      } else {
        dbg_printf("%s = 0x" FMT_LL "x (hex number)\n", node->get_name(), ((bx_param_num_c*)node)->get64());
      }
      break;
    case BXT_PARAM_BOOL:
      dbg_printf("%s = %s (boolean)\n", node->get_name(), ((bx_param_bool_c*)node)->get()?"true":"false");
      break;
    case BXT_PARAM_ENUM:
      dbg_printf("%s = '%s' (enum)\n", node->get_name(), ((bx_param_enum_c*)node)->get_selected());
      break;
    case BXT_PARAM_STRING:
      if (((bx_param_string_c*)node)->get_options() & bx_param_string_c::RAW_BYTES) {
        tmpstr[0] = 0;
        for (i = 0; i < ((bx_param_string_c*)node)->get_maxsize(); i++) {
          if (i > 0) {
            tmpbyte[0] = ((bx_param_string_c*)node)->get_separator();
            tmpbyte[1] = 0;
            strcat(tmpstr, tmpbyte);
          }
          sprintf(tmpbyte, "%02x", (Bit8u)((bx_param_string_c*)node)->getptr()[i]);
          strcat(tmpstr, tmpbyte);
        }
        dbg_printf("%s = '%s' (raw byte string)\n", node->get_name(), tmpstr);
      } else {
        dbg_printf("%s = '%s' (string)\n", node->get_name(), ((bx_param_string_c*)node)->getptr());
      }
      break;
    case BXT_LIST:
      {
	dbg_printf("%s = \n", node->get_name());
	bx_list_c *list = (bx_list_c*)node;
	for (i=0; i < list->get_size(); i++) {
	   print_tree(list->get(i), level+1);
	}
	break;
      }
    case BXT_PARAM_DATA:
      dbg_printf("%s = 'size=%d' (binary data)\n", node->get_name(), ((bx_shadow_data_c*)node)->get_size());
      break;
    default:
      dbg_printf("%s (unknown parameter type)\n", node->get_name());
  }
}
#endif

int bxmain(void) {


	cpu_loop();

/*
  bx_user_quit = 0;

  bx_init_siminterface();   // create the SIM object

  static jmp_buf context;

  if (setjmp(context) == 0) {
    SIM->set_quit_context (&context);

    if (bx_init_main(bx_startup_flags.argc, bx_startup_flags.argv) < 0) {
      BX_INSTR_EXIT_ENV();
      return 0;
    }

    // read a param to decide which config interface to start.
    // If one exists, start it.  If not, just begin.
    bx_param_enum_c *ci_param = SIM->get_param_enum(BXPN_SEL_CONFIG_INTERFACE);
    const char *ci_name = ci_param->get_selected();

    if (!strcmp(ci_name, "textconfig")) {
      init_text_config_interface();   // in textconfig.h
    }
    else {
      BX_PANIC(("unsupported configuration interface '%s'", ci_name));
    }

    ci_param->set_enabled(0);
    int status = SIM->configuration_interface(ci_name, CI_START);

    if (status == CI_ERR_NO_TEXT_CONSOLE)
      BX_PANIC(("Bochs needed the text console, but it was not usable"));
    // user quit the config interface, so just quit
  } 
  else {
    // quit via longjmp
  }

  SIM->set_quit_context(NULL);

  return SIM->get_exit_code();
*/

	return 0;
}

void print_usage(void)
{
  fprintf(stderr,
    "Usage: bochs [flags] [bochsrc options]\n\n"
    "  -n               no configuration file\n"
    "  -f configfile    specify configuration file\n"
    "  -q               quick start (skip configuration interface)\n"
    "  -benchmark n     run bochs in benchmark mode for millions of emulated ticks\n"
    "  -r path          restore the Bochs state from path\n"
    "  -log filename    specify Bochs log file name\n"
#if BX_DEBUGGER
    "  -rc filename     execute debugger commands stored in file\n"
    "  -dbglog filename specify Bochs internal debugger log file name\n"
#endif
#ifdef WIN32
    "  -noconsole       disable console window\n"
#endif
    "  --help           display this help and exit\n"
    "  --help features  display available features / devices and exit\n"
#if BX_CPU_LEVEL > 4
    "  --help cpu       display supported CPU models and exit\n"
#endif
    "\nFor information on Bochs configuration file arguments, see the\n"
#if (!defined(WIN32)) && !BX_WITH_MACOS
    "bochsrc section in the user documentation or the man page of bochsrc.\n");
#else
    "bochsrc section in the user documentation.\n");
#endif
}

int bx_init_main(int argc, char *argv[])
{
  // To deal with initialization order problems inherent in C++, use the macros
  // SAFE_GET_IOFUNC and SAFE_GET_GENLOG to retrieve "io" and "genlog" in all
  // constructors or functions called by constructors.  The macros test for
  // NULL and create the object if necessary, then return it.  Ensure that io
  // and genlog get created, by making one reference to each macro right here.
  // All other code can reference io and genlog directly.  Because these
  // objects are required for logging, and logging is so fundamental to
  // knowing what the program is doing, they are never free()d.
//  SAFE_GET_IOFUNC();  // never freed
//  SAFE_GET_GENLOG();  // never freed

  // initalization must be done early because some destructors expect
  // the bochs config options to exist by the time they are called.
//  bx_init_bx_dbg();
  bx_init_options();

  bx_print_header();

  SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_RUN_START);

  // interpret the args that start with -, like -q, -f, etc.
  int arg = 1, load_rcfile=1, i = 0;
  while (arg < argc) {
    // parse next arg
    if (!strcmp("--help", argv[arg]) || !strncmp("-h", argv[arg], 2)
       ) {
      if ((arg+1) < argc) {
        if (!strcmp("features", argv[arg+1])) {
          fprintf(stderr, "Supported features:\n\n");
          fprintf(stderr, "\n");
          arg++;
        }
#if BX_CPU_LEVEL > 4
        else if (!strcmp("cpu", argv[arg+1])) {
          fprintf(stderr, "Supported CPU models:\n\n");
          do {
            fprintf(stderr, "%s\n", SIM->get_param_enum(BXPN_CPU_MODEL)->get_choice(i));
          } while (i++ < SIM->get_param_enum(BXPN_CPU_MODEL)->get_max());
          fprintf(stderr, "\n");
          arg++;
        }
#endif
      } else {
        print_usage();
      }
      SIM->quit_sim(0);
    }
    else if (!strcmp("-n", argv[arg])) {
      load_rcfile = 0;
    }
    else if (!strcmp("-q", argv[arg])) {
      SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_QUICK_START);
    }
    else if (!strcmp("-log", argv[arg])) {
      if (++arg >= argc) BX_PANIC(("-log must be followed by a filename"));
      else SIM->get_param_string(BXPN_LOG_FILENAME)->set(argv[arg]);
    }
    else if (!strcmp("-f", argv[arg])) {
      if (++arg >= argc) BX_PANIC(("-f must be followed by a filename"));
      else bochsrc_filename = argv[arg];
    }
    else if (!strcmp("-qf", argv[arg])) {
      SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_QUICK_START);
      if (++arg >= argc) BX_PANIC(("-qf must be followed by a filename"));
      else bochsrc_filename = argv[arg];
    }
    else if (!strcmp("-benchmark", argv[arg])) {
      SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_QUICK_START);
      if (++arg >= argc) BX_PANIC(("-benchmark must be followed by a number"));
      else SIM->get_param_num(BXPN_BOCHS_BENCHMARK)->set(atoi(argv[arg]));
    }
    else if (!strcmp("-r", argv[arg])) {
      if (++arg >= argc) BX_PANIC(("-r must be followed by a path"));
      else {
        SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_QUICK_START);
        SIM->get_param_bool(BXPN_RESTORE_FLAG)->set(1);
        SIM->get_param_string(BXPN_RESTORE_PATH)->set(argv[arg]);
      }
    }
    else if (argv[arg][0] == '-') {
      print_usage();
      BX_PANIC (("command line arg '%s' was not understood", argv[arg]));
    }
    else {
      // the arg did not start with -, so stop interpreting flags
      break;
    }
    arg++;
  }
#if BX_PLUGINS
  // set a default plugin path, in case the user did not specify one
#if BX_HAVE_GETENV && BX_HAVE_SETENV
  if (getenv("LTDL_LIBRARY_PATH") != NULL) {
    BX_INFO (("LTDL_LIBRARY_PATH is set to '%s'", getenv("LTDL_LIBRARY_PATH")));
  } else {
    BX_INFO (("LTDL_LIBRARY_PATH not set. using compile time default '%s'",
        BX_PLUGIN_PATH));
    setenv("LTDL_LIBRARY_PATH", BX_PLUGIN_PATH, 1);
  }
  if (getenv("BXSHARE") != NULL) {
    BX_INFO (("BXSHARE is set to '%s'", getenv("BXSHARE")));
  } else {
    BX_INFO (("BXSHARE not set. using compile time default '%s'",
        BX_SHARE_PATH));
    setenv("BXSHARE", BX_SHARE_PATH, 1);
  }
#else
  // we don't have getenv or setenv.  Do nothing.
#endif
#endif  /* if BX_PLUGINS */

  // initialize plugin system. This must happen before we attempt to
  // load any modules.
  //BOX:REMOVE plugin_startup();

  int norcfile = 1;

  if (SIM->get_param_bool(BXPN_RESTORE_FLAG)->get()) {
    load_rcfile = 0;
    norcfile = 0;
  }
  // load pre-defined optional plugins before parsing configuration
  SIM->opt_plugin_ctrl("*", 1);
  SIM->init_save_restore();
  if (load_rcfile) {
    // parse configuration file and command line arguments
    if (bochsrc_filename == NULL) bochsrc_filename = bx_find_bochsrc ();
    if (bochsrc_filename)
      norcfile = bx_read_configuration(bochsrc_filename);
  }

  if (norcfile) {
    // No configuration was loaded, so the current settings are unusable.
    // Switch off quick start so that we will drop into the configuration
    // interface.
    if (SIM->get_param_enum(BXPN_BOCHS_START)->get() == BX_QUICK_START) {
      if (!SIM->test_for_text_console())
        BX_PANIC(("Unable to start Bochs without a bochsrc.txt and without a text console"));
      else
        BX_ERROR(("Switching off quick start, because no configuration file was found."));
    }
    SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_LOAD_START);
  }

  if (SIM->get_param_bool(BXPN_RESTORE_FLAG)->get()) {
    if (arg < argc) {
      BX_ERROR(("WARNING: bochsrc options are ignored in restore mode!"));
    }
  }
  else {
    // parse the rest of the command line.  This is done after reading the
    // configuration file so that the command line arguments can override
    // the settings from the file.
    if (bx_parse_cmdline(arg, argc, argv)) {
      BX_PANIC(("There were errors while parsing the command line"));
      return -1;
    }
  }
  return 0;
}

int bx_begin_simulation (int argc, char *argv[])
{
  if (SIM->get_param_bool(BXPN_RESTORE_FLAG)->get()) {
    if (!SIM->restore_config()) {
      BX_PANIC(("cannot restore configuration"));
      SIM->get_param_bool(BXPN_RESTORE_FLAG)->set(0);
    }
  } else {
    // make sure all optional plugins have been loaded
    SIM->opt_plugin_ctrl("*", 1);
  }

  // deal with gui selection
  if (!load_and_init_display_lib ()) {
    BX_PANIC (("no gui module was loaded"));
    return 0;
  }

  bx_cpu_count = SIM->get_param_num(BXPN_CPU_NPROCESSORS)->get() *
                 SIM->get_param_num(BXPN_CPU_NCORES)->get() *
                 SIM->get_param_num(BXPN_CPU_NTHREADS)->get();

  BX_ASSERT(bx_cpu_count > 0);

  bx_init_hardware();

  /* BOX:REMOVE
  if (SIM->get_param_enum(BXPN_LOAD32BITOS_WHICH)->get()) {
    void bx_load32bitOSimagehack(void);
    bx_load32bitOSimagehack();
  }
  */
  SIM->set_init_done(1);

  // update headerbar buttons since drive status can change during init
  bx_gui->update_drive_status_buttons();

  // iniialize statusbar and set all items inactive
  if (!SIM->get_param_bool(BXPN_RESTORE_FLAG)->get())
  {
    bx_gui->statusbar_setitem(-1, 0);
  }

  // The set handler for mouse_enabled does not actually update the gui
  // until init_done is set.  This forces the set handler to be called,
  // which sets up the mouse enabled GUI-specific stuff correctly.
  // Not a great solution but it works. BBD
  SIM->get_param_bool(BXPN_MOUSE_ENABLED)->set(SIM->get_param_bool(BXPN_MOUSE_ENABLED)->get());

  BX_INFO(("cpu loop quit, shutting down simulator"));
  bx_atexit();
  return(0);
}

void bx_stop_simulation(void)
{
  // in wxWidgets, the whole simulator is running in a separate thread.
  // our only job is to end the thread as soon as possible, NOT to shut
  // down the whole application with an exit.
  BX_CPU(0)->async_event = 1;
  bx_pc_system.kill_bochs_request = 1;
  // the cpu loop will exit very soon after this condition is set.
}

void bx_sr_after_restore_state(void)
{
#if BX_SUPPORT_SMP == 0
  BX_CPU(0)->after_restore_state();
#else
  for (unsigned i=0; i<BX_SMP_PROCESSORS; i++) {
    BX_CPU(i)->after_restore_state();
  }
#endif
  DEV_after_restore_state();
}

void bx_set_log_actions_by_device(bx_bool panic_flag)
{
  int id, l, m, val;
  bx_list_c *loglev, *level;
  bx_param_num_c *action;

  loglev = (bx_list_c*) SIM->get_param("general.logfn");
  for (l = 0; l < loglev->get_size(); l++) {
    level = (bx_list_c*) loglev->get(l);
    for (m = 0; m < level->get_size(); m++) {
      action = (bx_param_num_c*) level->get(m);
      id = SIM->get_logfn_id(action->get_name());
      val = action->get();
      if (id < 0) {
        if (panic_flag) {
          BX_PANIC(("unknown log function module '%s'", action->get_name()));
        }
      } else if (val >= 0) {
        SIM->set_log_action(id, l, val);
        // mark as 'done'
        action->set(-1);
      }
    }
  }
}

void bx_init_hardware()
{
  // all configuration has been read, now initialize everything.

  if (SIM->get_param_enum(BXPN_BOCHS_START)->get()==BX_QUICK_START) {
    for (int level=0; level<N_LOGLEV; level++) {
      int action = SIM->get_default_log_action(level);
      io->set_log_action(level, action);
    }
  }

  bx_pc_system.initialize(SIM->get_param_num(BXPN_IPS)->get());

  if (SIM->get_param_string(BXPN_LOG_FILENAME)->getptr()[0]!='-') {
    BX_INFO (("using log file %s", SIM->get_param_string(BXPN_LOG_FILENAME)->getptr()));
    io->init_log(SIM->get_param_string(BXPN_LOG_FILENAME)->getptr());
  }

  io->set_log_prefix(SIM->get_param_string(BXPN_LOG_PREFIX)->getptr());

  // Output to the log file the cpu and device settings
  // This will by handy for bug reports
  BX_INFO(("Bochs x86 Emulator %s", VER_STRING));
  BX_INFO(("  %s", REL_STRING));
#ifdef __DATE__
#ifdef __TIME__
  BX_INFO(("Compiled on %s at %s", __DATE__, __TIME__));
#else
  BX_INFO(("Compiled on %s", __DATE__));
#endif
#endif
  BX_INFO(("System configuration"));
  BX_INFO(("  processors: %d (cores=%u, HT threads=%u)", BX_SMP_PROCESSORS,
    SIM->get_param_num(BXPN_CPU_NCORES)->get(), SIM->get_param_num(BXPN_CPU_NTHREADS)->get()));
  BX_INFO(("  A20 line support: %s", BX_SUPPORT_A20?"yes":"no"));
#if BX_CONFIGURE_MSRS
  const char *msrs_file = SIM->get_param_string(BXPN_CONFIGURABLE_MSRS_PATH)->getptr();
  if(strlen(msrs_file) > 0)
    BX_INFO(("  load configurable MSRs from file \"%s\"", msrs_file));
#endif
  BX_INFO(("IPS is set to %d", (Bit32u) SIM->get_param_num(BXPN_IPS)->get()));
  BX_INFO(("CPU configuration"));
  BX_INFO(("  level: %d", BX_CPU_LEVEL));
#if BX_SUPPORT_SMP
  BX_INFO(("  SMP support: yes, quantum=%d", SIM->get_param_num(BXPN_SMP_QUANTUM)->get()));
#else
  BX_INFO(("  SMP support: no"));
#endif

  unsigned cpu_model = SIM->get_param_enum(BXPN_CPU_MODEL)->get();
  if (! cpu_model) {
#if BX_CPU_LEVEL >= 5
    BX_INFO(("  APIC support: %s", SIM->get_param_enum(BXPN_CPUID_APIC)->get_selected()));
#else
    BX_INFO(("  APIC support: no"));
#endif
    BX_INFO(("  FPU support: %s", BX_SUPPORT_FPU?"yes":"no"));
#if BX_CPU_LEVEL >= 5
    bx_bool mmx_enabled = SIM->get_param_bool(BXPN_CPUID_MMX)->get();
    BX_INFO(("  MMX support: %s", mmx_enabled?"yes":"no"));
    BX_INFO(("  3dnow! support: %s", BX_SUPPORT_3DNOW?"yes":"no"));
#endif
#if BX_CPU_LEVEL >= 6
    bx_bool sep_enabled = SIM->get_param_bool(BXPN_CPUID_SEP)->get();
    BX_INFO(("  SEP support: %s", sep_enabled?"yes":"no"));
    bx_bool sse4a_enabled = SIM->get_param_bool(BXPN_CPUID_SSE4A)->get();
    BX_INFO(("  SSE support: %s%s", SIM->get_param_enum(BXPN_CPUID_SSE)->get_selected(),
      sse4a_enabled ? "+sse4a" : ""));
    bx_bool xsave_enabled = SIM->get_param_bool(BXPN_CPUID_XSAVE)->get();
    bx_bool xsaveopt_enabled = SIM->get_param_bool(BXPN_CPUID_XSAVEOPT)->get();
    BX_INFO(("  XSAVE support: %s %s",
      xsave_enabled?"xsave":"no", xsaveopt_enabled?"xsaveopt":""));
    bx_bool aes_enabled = SIM->get_param_bool(BXPN_CPUID_AES)->get();
    BX_INFO(("  AES support: %s", aes_enabled?"yes":"no"));
    bx_bool movbe_enabled = SIM->get_param_bool(BXPN_CPUID_MOVBE)->get();
    BX_INFO(("  MOVBE support: %s", movbe_enabled?"yes":"no"));
    bx_bool adx_enabled = SIM->get_param_bool(BXPN_CPUID_ADX)->get();
    BX_INFO(("  ADX support: %s", adx_enabled?"yes":"no"));
#if BX_SUPPORT_X86_64
    bx_bool x86_64_enabled = SIM->get_param_bool(BXPN_CPUID_X86_64)->get();
    BX_INFO(("  x86-64 support: %s", x86_64_enabled?"yes":"no"));
    bx_bool xlarge_enabled = SIM->get_param_bool(BXPN_CPUID_1G_PAGES)->get();
    BX_INFO(("  1G paging support: %s", xlarge_enabled?"yes":"no"));
#else
    BX_INFO(("  x86-64 support: no"));
#endif
#if BX_SUPPORT_MONITOR_MWAIT
    bx_bool mwait_enabled = SIM->get_param_bool(BXPN_CPUID_MWAIT)->get();
    BX_INFO(("  MWAIT support: %s", mwait_enabled?"yes":"no"));
#endif
#if BX_SUPPORT_AVX
    unsigned avx_enabled = SIM->get_param_num(BXPN_CPUID_AVX)->get();
    bx_bool avx_fma_enabled = SIM->get_param_bool(BXPN_CPUID_AVX_FMA)->get();
    if (avx_enabled) {
      BX_INFO(("  AVX support: %d%s", avx_enabled, avx_fma_enabled ? " (with FMA)" : ""));
    }
    else {
      BX_INFO(("  AVX support: no"));
    }
#endif
#if BX_SUPPORT_VMX
    unsigned vmx_enabled = SIM->get_param_num(BXPN_CPUID_VMX)->get();
    if (vmx_enabled) {
      BX_INFO(("  VMX support: %d", vmx_enabled));
    }
    else {
      BX_INFO(("  VMX support: no"));
    }
#endif
#if BX_SUPPORT_SVM
    bx_bool svm_enabled = SIM->get_param_bool(BXPN_CPUID_SVM)->get();
    BX_INFO(("  SVM support: %s", svm_enabled?"yes":"no"));
#endif
#endif // BX_CPU_LEVEL >= 6
  }
  else {
    BX_INFO(("  Using pre-defined CPU configuration: %s",
      SIM->get_param_enum(BXPN_CPU_MODEL)->get_selected()));
  }

  BX_INFO(("Optimization configuration"));
  BX_INFO(("  RepeatSpeedups support: %s", BX_SUPPORT_REPEAT_SPEEDUPS?"yes":"no"));
  BX_INFO(("  Fast function calls: %s", BX_FAST_FUNC_CALL?"yes":"no"));
  BX_INFO(("  Handlers Chaining speedups: %s", BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS?"yes":"no"));
  BX_INFO(("Devices configuration"));
  BX_INFO(("  NE2000 support: %s", BX_SUPPORT_NE2K?"yes":"no"));
  BX_INFO(("  PCI support: %s, enabled=%s", BX_SUPPORT_PCI?"yes":"no",
    SIM->get_param_bool(BXPN_I440FX_SUPPORT)->get() ? "yes" : "no"));
  BX_INFO(("  SB16 support: %s", BX_SUPPORT_SB16?"yes":"no"));
  BX_INFO(("  USB support: %s", BX_SUPPORT_PCIUSB?"yes":"no"));
  BX_INFO(("  VGA extension support: vbe %s", BX_SUPPORT_CLGD54XX?"cirrus":""));

  // Check if there is a romimage
  if (strcmp(SIM->get_param_string(BXPN_ROM_PATH)->getptr(),"") == 0) {
    BX_ERROR(("No romimage to load. Is your bochsrc file loaded/valid ?"));
  }

  // set one shot timer for benchmark mode if needed, the timer will fire
  // once and kill Bochs simulation after predefined amount of emulated
  // ticks
  int benchmark_mode = SIM->get_param_num(BXPN_BOCHS_BENCHMARK)->get();
  if (benchmark_mode) {
    BX_INFO(("Bochs benchmark mode is ON (~%d millions of ticks)", benchmark_mode));
    bx_pc_system.register_timer_ticks(&bx_pc_system, bx_pc_system_c::benchmarkTimer,
        (Bit64u) benchmark_mode * 1000000, 0, 1, "benchmark.timer");
  }

  // set up memory and CPU objects
  bx_param_num_c *bxp_memsize = SIM->get_param_num(BXPN_MEM_SIZE);
  Bit64u memSize = bxp_memsize->get64() * BX_CONST64(1024*1024);

  bx_param_num_c *bxp_host_memsize = SIM->get_param_num(BXPN_HOST_MEM_SIZE);
  Bit64u hostMemSize = bxp_host_memsize->get64() * BX_CONST64(1024*1024);

  // do not allocate more host memory than needed for emulation of guest RAM 
  if (memSize < hostMemSize) hostMemSize = memSize;

  BX_MEM(0)->init_memory(memSize, hostMemSize);

  // First load the system BIOS (VGABIOS loading moved to the vga code)
  BX_MEM(0)->load_ROM(SIM->get_param_string(BXPN_ROM_PATH)->getptr(),
                      SIM->get_param_num(BXPN_ROM_ADDRESS)->get(), 0);

  // Then load the optional ROM images
  if (strcmp(SIM->get_param_string(BXPN_OPTROM1_PATH)->getptr(), "") !=0)
    BX_MEM(0)->load_ROM(SIM->get_param_string(BXPN_OPTROM1_PATH)->getptr(), SIM->get_param_num(BXPN_OPTROM1_ADDRESS)->get(), 2);
  if (strcmp(SIM->get_param_string(BXPN_OPTROM2_PATH)->getptr(), "") !=0)
    BX_MEM(0)->load_ROM(SIM->get_param_string(BXPN_OPTROM2_PATH)->getptr(), SIM->get_param_num(BXPN_OPTROM2_ADDRESS)->get(), 2);
  if (strcmp(SIM->get_param_string(BXPN_OPTROM3_PATH)->getptr(), "") !=0)
    BX_MEM(0)->load_ROM(SIM->get_param_string(BXPN_OPTROM3_PATH)->getptr(), SIM->get_param_num(BXPN_OPTROM3_ADDRESS)->get(), 2);
  if (strcmp(SIM->get_param_string(BXPN_OPTROM4_PATH)->getptr(), "") !=0)
    BX_MEM(0)->load_ROM(SIM->get_param_string(BXPN_OPTROM4_PATH)->getptr(), SIM->get_param_num(BXPN_OPTROM4_ADDRESS)->get(), 2);

  // Then load the optional RAM images
  if (strcmp(SIM->get_param_string(BXPN_OPTRAM1_PATH)->getptr(), "") !=0)
    BX_MEM(0)->load_RAM(SIM->get_param_string(BXPN_OPTRAM1_PATH)->getptr(), SIM->get_param_num(BXPN_OPTRAM1_ADDRESS)->get(), 2);
  if (strcmp(SIM->get_param_string(BXPN_OPTRAM2_PATH)->getptr(), "") !=0)
    BX_MEM(0)->load_RAM(SIM->get_param_string(BXPN_OPTRAM2_PATH)->getptr(), SIM->get_param_num(BXPN_OPTRAM2_ADDRESS)->get(), 2);
  if (strcmp(SIM->get_param_string(BXPN_OPTRAM3_PATH)->getptr(), "") !=0)
    BX_MEM(0)->load_RAM(SIM->get_param_string(BXPN_OPTRAM3_PATH)->getptr(), SIM->get_param_num(BXPN_OPTRAM3_ADDRESS)->get(), 2);
  if (strcmp(SIM->get_param_string(BXPN_OPTRAM4_PATH)->getptr(), "") !=0)
    BX_MEM(0)->load_RAM(SIM->get_param_string(BXPN_OPTRAM4_PATH)->getptr(), SIM->get_param_num(BXPN_OPTRAM4_ADDRESS)->get(), 2);

#if BX_SUPPORT_SMP == 0
  BX_CPU(0)->initialize();
  BX_CPU(0)->sanity_checks();
  BX_CPU(0)->register_state();
  BX_INSTR_INITIALIZE(0);
#else
  bx_cpu_array = new BX_CPU_C_PTR[BX_SMP_PROCESSORS];

  for (unsigned i=0; i<BX_SMP_PROCESSORS; i++) {
    BX_CPU(i) = new BX_CPU_C(i);
    BX_CPU(i)->initialize();  // assign local apic id in 'initialize' method
    BX_CPU(i)->sanity_checks();
    BX_CPU(i)->register_state();
    BX_INSTR_INITIALIZE(i);
  }
#endif

  DEV_init_devices();
  // unload optional plugins which are unused and marked for removal
  SIM->opt_plugin_ctrl("*", 0);
  bx_pc_system.register_state();
  DEV_register_state();
  if (SIM->get_param_bool(BXPN_RESTORE_FLAG)->get()) {
    if (!SIM->restore_logopts()) {
      BX_PANIC(("cannot restore log options"));
      SIM->get_param_bool(BXPN_RESTORE_FLAG)->set(0);
    }
  } else {
    bx_set_log_actions_by_device(1);
  }

  // will enable A20 line and reset CPU and devices
  bx_pc_system.Reset(BX_RESET_HARDWARE);

  if (SIM->get_param_bool(BXPN_RESTORE_FLAG)->get()) {
    if (SIM->restore_hardware()) {
      bx_sr_after_restore_state();
    } else {
      BX_PANIC(("cannot restore hardware state"));
      SIM->get_param_bool(BXPN_RESTORE_FLAG)->set(0);
    }
  }

  bx_gui->init_signal_handlers();
  bx_pc_system.start_timers();

  BX_DEBUG(("bx_init_hardware is setting signal handlers"));
// if not using debugger, then we can take control of SIGINT.
#if !BX_DEBUGGER
  signal(SIGINT, bx_signal_handler);
#endif

#if BX_SHOW_IPS
#if !defined(WIN32)
  if (!SIM->is_wx_selected()) {
    signal(SIGALRM, bx_signal_handler);
    alarm(1);
  }
#endif
#endif
}

void bx_init_bx_dbg(void)
{
#if BX_DEBUGGER
  bx_dbg_init_infile();
#endif
  memset(&bx_dbg, 0, sizeof(bx_debug_t));
}

int bx_atexit(void)
{
  if (!SIM->get_init_done()) return 1; // protect from reentry

  // in case we ended up in simulation mode, change back to config mode
  // so that the user can see any messages left behind on the console.
  SIM->set_display_mode(DISP_MODE_CONFIG);

#if BX_DEBUGGER == 0
  if (SIM && SIM->get_init_done()) {
    for (int cpu=0; cpu<BX_SMP_PROCESSORS; cpu++)
#if BX_SUPPORT_SMP
      if (BX_CPU(cpu))
#endif
        BX_CPU(cpu)->atexit();
  }
#endif

  BX_MEM(0)->cleanup_memory();

  bx_pc_system.exit();

  // restore signal handling to defaults
#if BX_DEBUGGER == 0
  BX_INFO(("restoring default signal behavior"));
  signal(SIGINT, SIG_DFL);
#endif

#if BX_SHOW_IPS
#if !defined(__MINGW32__) && !defined(_MSC_VER)
  if (!SIM->is_wx_selected()) {
    alarm(0);
    signal(SIGALRM, SIG_DFL);
  }
#endif
#endif

  SIM->cleanup_save_restore();
  SIM->set_init_done(0);

  return 0;
}

void CDECL bx_signal_handler(int signum)
{
  // in a multithreaded environment, a signal such as SIGINT can be sent to all
  // threads.  This function is only intended to handle signals in the
  // simulator thread.  It will simply return if called from any other thread.
  // Otherwise the BX_PANIC() below can be called in multiple threads at
  // once, leading to multiple threads trying to display a dialog box,
  // leading to GUI deadlock.
  if (!SIM->is_sim_thread()) {
    BX_INFO (("bx_signal_handler: ignored sig %d because it wasn't called from the simulator thread", signum));
    return;
  }

#if BX_SHOW_IPS
  static Bit64u ticks_count = 0;
  static Bit64u counts = 0;

  if (signum == SIGALRM)
  {
    // amount of system ticks passed from last time the handler was called
    Bit64u ips_count = bx_pc_system.time_ticks() - ticks_count;
    if (ips_count) {
      bx_gui->show_ips((Bit32u) ips_count);
      ticks_count = bx_pc_system.time_ticks();
      counts++;
      if (bx_dbg.print_timestamps) {
        printf("IPS: %u\taverage = %u\t\t(%us)\n",
           (unsigned) ips_count, (unsigned) (ticks_count/counts), (unsigned) counts);
        fflush(stdout);
      }
    }
    return;
  }
#endif

  if ( signum == 2 ) 
   exit(0);
  else
   BX_PANIC(("SIGNAL %u caught", signum));
}
