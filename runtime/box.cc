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

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "debug.h"
#include "bxversion.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "syscall/syscall.h"

extern "C" {
#include <signal.h>
}

void bx_init_hardware(void);
void bx_init_options(void);

typedef BX_CPU_C *BX_CPU_C_PTR;

extern const char* cpu_mode_string(unsigned cpu_mode);


void bx_init_hardware()
{
/*
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
*/
}

int bx_atexit(void)
{
/*
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
*/
}

void CDECL bx_signal_handler(int signum)
{
/*
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
*/
}
