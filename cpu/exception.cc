/////////////////////////////////////////////////////////////////////////
// $Id: exception.cc 11330 2012-08-09 13:11:25Z sshwarts $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2012  The Bochs Project
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "param_names.h"

#if BX_SUPPORT_X86_64==0
// Make life easier merging cpu64 & cpu code.
#define RIP EIP
#define RSP ESP
#endif

void BX_CPU_C::protected_mode_int(Bit8u vector, unsigned soft_int, bx_bool push_error, Bit16u error_code)
{
  bx_descriptor_t gate_descriptor, cs_descriptor;
  bx_selector_t cs_selector;

  Bit16u raw_tss_selector;
  bx_selector_t   tss_selector;
  bx_descriptor_t tss_descriptor;

  Bit16u gate_dest_selector;
  Bit32u gate_dest_offset;

  BX_INFO(("protected_mode_int"));

  // interrupt vector must be within IDT table limits,
  // else #GP(vector*8 + 2 + EXT)
  if ((vector*8 + 7) > BX_CPU_THIS_PTR idtr.limit) {
    BX_ERROR(("interrupt(): vector must be within IDT table limits, IDT.limit = 0x%x",
    		           BX_CPU_THIS_PTR idtr.limit));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  Bit64u desctmp = system_read_qword(BX_CPU_THIS_PTR idtr.base + vector*8);

  Bit32u dword1 = GET32L(desctmp);
  Bit32u dword2 = GET32H(desctmp);

  parse_descriptor(dword1, dword2, &gate_descriptor);

  if ((gate_descriptor.valid==0) || gate_descriptor.segment) {
    BX_ERROR(("interrupt(): gate descriptor is not valid sys seg (vector=0x%02x)", vector));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  // descriptor AR byte must indicate interrupt gate, trap gate,
  // or task gate, else #GP(vector*8 + 2 + EXT)
  switch (gate_descriptor.type) {
  case BX_TASK_GATE:
  case BX_286_INTERRUPT_GATE:
  case BX_286_TRAP_GATE:
  case BX_386_INTERRUPT_GATE:
  case BX_386_TRAP_GATE:
    break;
  default:
    printf("interrupt(): gate.type(%u) != {5,6,7,14,15}",
      (unsigned) gate_descriptor.type);
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  // if software interrupt, then gate descripor DPL must be >= CPL,
  // else #GP(vector * 8 + 2 + EXT)
  if (soft_int && gate_descriptor.dpl < CPL) {
    printf("interrupt(): soft_int && (gate.dpl < CPL)");
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  // Gate must be present, else #NP(vector * 8 + 2 + EXT)
  if (! IS_PRESENT(gate_descriptor)) {
    printf("interrupt(): gate not present");
    exception(BX_NP_EXCEPTION, vector*8 + 2);
  }

  switch (gate_descriptor.type) {
  case BX_TASK_GATE:
    // examine selector to TSS, given in task gate descriptor
    raw_tss_selector = gate_descriptor.u.taskgate.tss_selector;
    parse_selector(raw_tss_selector, &tss_selector);

    // must specify global in the local/global bit,
    //      else #GP(TSS selector)
    if (tss_selector.ti) {
      printf("interrupt(): tss_selector.ti=1 from gate descriptor - #GP(tss_selector)");
      exception(BX_GP_EXCEPTION, raw_tss_selector & 0xfffc);
    }

    // index must be within GDT limits, else #TS(TSS selector)
    fetch_raw_descriptor(&tss_selector, &dword1, &dword2, BX_GP_EXCEPTION);

    parse_descriptor(dword1, dword2, &tss_descriptor);

    // AR byte must specify available TSS,
    //   else #GP(TSS selector)
    if (tss_descriptor.valid==0 || tss_descriptor.segment) {
      printf("interrupt(): TSS selector points to invalid or bad TSS - #GP(tss_selector)");
      exception(BX_GP_EXCEPTION, raw_tss_selector & 0xfffc);
    }

    if (tss_descriptor.type!=BX_SYS_SEGMENT_AVAIL_286_TSS &&
        tss_descriptor.type!=BX_SYS_SEGMENT_AVAIL_386_TSS)
    {
      printf("interrupt(): TSS selector points to bad TSS - #GP(tss_selector)");
      exception(BX_GP_EXCEPTION, raw_tss_selector & 0xfffc);
    }

    // TSS must be present, else #NP(TSS selector)
    if (! IS_PRESENT(tss_descriptor)) {
      printf("interrupt(): TSS descriptor.p == 0");
      exception(BX_NP_EXCEPTION, raw_tss_selector & 0xfffc);
    }

    // switch tasks with nesting to TSS
    task_switch(0, &tss_selector, &tss_descriptor,
                    BX_TASK_FROM_INT, dword1, dword2, push_error, error_code);
    return;

  case BX_286_INTERRUPT_GATE:
  case BX_286_TRAP_GATE:
  case BX_386_INTERRUPT_GATE:
  case BX_386_TRAP_GATE:
    gate_dest_selector = gate_descriptor.u.gate.dest_selector;
    gate_dest_offset   = gate_descriptor.u.gate.dest_offset;

    // examine CS selector and descriptor given in gate descriptor
    // selector must be non-null else #GP(EXT)
    if ((gate_dest_selector & 0xfffc) == 0) {
      printf("int_trap_gate(): selector null");
      exception(BX_GP_EXCEPTION, 0);
    }

    parse_selector(gate_dest_selector, &cs_selector);

    // selector must be within its descriptor table limits
    // else #GP(selector+EXT)
    fetch_raw_descriptor(&cs_selector, &dword1, &dword2, BX_GP_EXCEPTION);
    parse_descriptor(dword1, dword2, &cs_descriptor);

    // descriptor AR byte must indicate code seg
    // and code segment descriptor DPL<=CPL, else #GP(selector+EXT)
    if (cs_descriptor.valid==0 || cs_descriptor.segment==0 ||
        IS_DATA_SEGMENT(cs_descriptor.type) ||
        cs_descriptor.dpl > CPL)
    {
      printf("interrupt(): not accessible or not code segment cs=0x%04x", cs_selector.value);
      exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc);
    }

    // segment must be present, else #NP(selector + EXT)
    if (! IS_PRESENT(cs_descriptor)) {
      printf("interrupt(): segment not present");
      exception(BX_NP_EXCEPTION, cs_selector.value & 0xfffc);
    }

    // if code segment is non-conforming and DPL < CPL then
    // INTERRUPT TO INNER PRIVILEGE
    if(IS_CODE_SEGMENT_NON_CONFORMING(cs_descriptor.type) && cs_descriptor.dpl < CPL)
    {
      Bit16u old_SS, old_CS, SS_for_cpl_x;
      Bit32u ESP_for_cpl_x, old_EIP, old_ESP;
      bx_descriptor_t ss_descriptor;
      bx_selector_t   ss_selector;
      int is_v8086_mode = v8086_mode();

      printf("interrupt(): INTERRUPT TO INNER PRIVILEGE");

      // check selector and descriptor for new stack in current TSS
      get_SS_ESP_from_TSS(cs_descriptor.dpl,
                              &SS_for_cpl_x, &ESP_for_cpl_x);

      if (is_v8086_mode && cs_descriptor.dpl != 0) {
        // if code segment DPL != 0 then #GP(new code segment selector)
        printf("interrupt(): code segment DPL(%d) != 0 in v8086 mode", cs_descriptor.dpl);
        exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc);
      }

      // Selector must be non-null else #TS(EXT)
      if ((SS_for_cpl_x & 0xfffc) == 0) {
        printf("interrupt(): SS selector null");
        exception(BX_TS_EXCEPTION, 0); /* TS(ext) */
      }

      // selector index must be within its descriptor table limits
      // else #TS(SS selector + EXT)
      parse_selector(SS_for_cpl_x, &ss_selector);
      // fetch 2 dwords of descriptor; call handles out of limits checks
      fetch_raw_descriptor(&ss_selector, &dword1, &dword2, BX_TS_EXCEPTION);
      parse_descriptor(dword1, dword2, &ss_descriptor);

      // selector rpl must = dpl of code segment,
      // else #TS(SS selector + ext)
      if (ss_selector.rpl != cs_descriptor.dpl) {
        printf("interrupt(): SS.rpl != CS.dpl");
        exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc);
      }

      // stack seg DPL must = DPL of code segment,
      // else #TS(SS selector + ext)
      if (ss_descriptor.dpl != cs_descriptor.dpl) {
        printf("interrupt(): SS.dpl != CS.dpl");
        exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc);
      }

      // descriptor must indicate writable data segment,
      // else #TS(SS selector + EXT)
      if (ss_descriptor.valid==0 || ss_descriptor.segment==0 ||
           IS_CODE_SEGMENT(ss_descriptor.type) ||
          !IS_DATA_SEGMENT_WRITEABLE(ss_descriptor.type))
      {
        printf("interrupt(): SS is not writable data segment");
        exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc);
      }

      // seg must be present, else #SS(SS selector + ext)
      if (! IS_PRESENT(ss_descriptor)) {
        printf("interrupt(): SS not present");
        exception(BX_SS_EXCEPTION, SS_for_cpl_x & 0xfffc);
      }

      // IP must be within CS segment boundaries, else #GP(0)
      if (gate_dest_offset > cs_descriptor.u.segment.limit_scaled) {
        printf("interrupt(): gate EIP > CS.limit");
        exception(BX_GP_EXCEPTION, 0);
      }

      old_ESP = ESP;
      old_SS  = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
      old_EIP = EIP;
      old_CS  = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;

      // Prepare new stack segment
      bx_segment_reg_t new_stack;
      new_stack.selector = ss_selector;
      new_stack.cache = ss_descriptor;
      new_stack.selector.rpl = cs_descriptor.dpl;
      // add cpl to the selector value
      new_stack.selector.value = (0xfffc & new_stack.selector.value) |
        new_stack.selector.rpl;

      if (ss_descriptor.u.segment.d_b) {
        Bit32u temp_ESP = ESP_for_cpl_x;

        if (is_v8086_mode)
        {
          if (gate_descriptor.type>=14) { // 386 int/trap gate
            write_new_stack_dword_32(&new_stack, temp_ESP-4,  cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
            write_new_stack_dword_32(&new_stack, temp_ESP-8,  cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
            write_new_stack_dword_32(&new_stack, temp_ESP-12, cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
            write_new_stack_dword_32(&new_stack, temp_ESP-16, cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
            temp_ESP -= 16;
          }
          else {
            write_new_stack_word_32(&new_stack, temp_ESP-2, cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
            write_new_stack_word_32(&new_stack, temp_ESP-4, cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
            write_new_stack_word_32(&new_stack, temp_ESP-6, cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
            write_new_stack_word_32(&new_stack, temp_ESP-8, cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
            temp_ESP -= 8;
          }
        }

        if (gate_descriptor.type>=14) { // 386 int/trap gate
          // push long pointer to old stack onto new stack
          write_new_stack_dword_32(&new_stack, temp_ESP-4,  cs_descriptor.dpl, old_SS);
          write_new_stack_dword_32(&new_stack, temp_ESP-8,  cs_descriptor.dpl, old_ESP);
          write_new_stack_dword_32(&new_stack, temp_ESP-12, cs_descriptor.dpl, read_eflags());
          write_new_stack_dword_32(&new_stack, temp_ESP-16, cs_descriptor.dpl, old_CS);
          write_new_stack_dword_32(&new_stack, temp_ESP-20, cs_descriptor.dpl, old_EIP);
          temp_ESP -= 20;

          if (push_error) {
            temp_ESP -= 4;
            write_new_stack_dword_32(&new_stack, temp_ESP, cs_descriptor.dpl, error_code);
          }
        }
        else {                          // 286 int/trap gate
          // push long pointer to old stack onto new stack
          write_new_stack_word_32(&new_stack, temp_ESP-2,  cs_descriptor.dpl, old_SS);
          write_new_stack_word_32(&new_stack, temp_ESP-4,  cs_descriptor.dpl, (Bit16u) old_ESP);
          write_new_stack_word_32(&new_stack, temp_ESP-6,  cs_descriptor.dpl, (Bit16u) read_eflags());
          write_new_stack_word_32(&new_stack, temp_ESP-8,  cs_descriptor.dpl, old_CS);
          write_new_stack_word_32(&new_stack, temp_ESP-10, cs_descriptor.dpl, (Bit16u) old_EIP);
          temp_ESP -= 10;

          if (push_error) {
            temp_ESP -= 2;
            write_new_stack_word_32(&new_stack, temp_ESP, cs_descriptor.dpl, error_code);
          }
        }

        ESP = temp_ESP;
      }
      else {
        Bit16u temp_SP = (Bit16u) ESP_for_cpl_x;

        if (is_v8086_mode)
        {
          if (gate_descriptor.type>=14) { // 386 int/trap gate
            write_new_stack_dword_32(&new_stack, (Bit16u)(temp_SP-4),  cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
            write_new_stack_dword_32(&new_stack, (Bit16u)(temp_SP-8),  cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
            write_new_stack_dword_32(&new_stack, (Bit16u)(temp_SP-12), cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
            write_new_stack_dword_32(&new_stack, (Bit16u)(temp_SP-16), cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
            temp_SP -= 16;
          }
          else {
            write_new_stack_word_32(&new_stack, (Bit16u)(temp_SP-2), cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
            write_new_stack_word_32(&new_stack, (Bit16u)(temp_SP-4), cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
            write_new_stack_word_32(&new_stack, (Bit16u)(temp_SP-6), cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
            write_new_stack_word_32(&new_stack, (Bit16u)(temp_SP-8), cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
            temp_SP -= 8;
          }
        }

        if (gate_descriptor.type>=14) { // 386 int/trap gate
          // push long pointer to old stack onto new stack
          write_new_stack_dword_32(&new_stack, (Bit16u)(temp_SP-4),  cs_descriptor.dpl, old_SS);
          write_new_stack_dword_32(&new_stack, (Bit16u)(temp_SP-8),  cs_descriptor.dpl, old_ESP);
          write_new_stack_dword_32(&new_stack, (Bit16u)(temp_SP-12), cs_descriptor.dpl, read_eflags());
          write_new_stack_dword_32(&new_stack, (Bit16u)(temp_SP-16), cs_descriptor.dpl, old_CS);
          write_new_stack_dword_32(&new_stack, (Bit16u)(temp_SP-20), cs_descriptor.dpl, old_EIP);
          temp_SP -= 20;

          if (push_error) {
            temp_SP -= 4;
            write_new_stack_dword_32(&new_stack, temp_SP, cs_descriptor.dpl, error_code);
          }
        }
        else {                          // 286 int/trap gate
          // push long pointer to old stack onto new stack
          write_new_stack_word_32(&new_stack, (Bit16u)(temp_SP-2),  cs_descriptor.dpl, old_SS);
          write_new_stack_word_32(&new_stack, (Bit16u)(temp_SP-4),  cs_descriptor.dpl, (Bit16u) old_ESP);
          write_new_stack_word_32(&new_stack, (Bit16u)(temp_SP-6),  cs_descriptor.dpl, (Bit16u) read_eflags());
          write_new_stack_word_32(&new_stack, (Bit16u)(temp_SP-8),  cs_descriptor.dpl, old_CS);
          write_new_stack_word_32(&new_stack, (Bit16u)(temp_SP-10), cs_descriptor.dpl, (Bit16u) old_EIP);
          temp_SP -= 10;

          if (push_error) {
            temp_SP -= 2;
            write_new_stack_word_32(&new_stack, temp_SP, cs_descriptor.dpl, error_code);
          }
        }

        SP = temp_SP;
      }

      // load new CS:eIP values from gate
      // set CPL to new code segment DPL
      // set RPL of CS to CPL
      load_cs(&cs_selector, &cs_descriptor, cs_descriptor.dpl);

      // load new SS:eSP values from TSS
      load_ss(&ss_selector, &ss_descriptor, cs_descriptor.dpl);

      if (is_v8086_mode)
      {
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.valid = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.valid = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.valid = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value = 0;
      }
    }
    else
    {
      printf("interrupt(): INTERRUPT TO SAME PRIVILEGE");

      if (v8086_mode() && (IS_CODE_SEGMENT_CONFORMING(cs_descriptor.type) || cs_descriptor.dpl != 0)) {
        // if code segment DPL != 0 then #GP(new code segment selector)
        printf("interrupt(): code segment conforming or DPL(%d) != 0 in v8086 mode", cs_descriptor.dpl);
        exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc);
      }

      // EIP must be in CS limit else #GP(0)
      if (gate_dest_offset > cs_descriptor.u.segment.limit_scaled) {
        printf("interrupt(): IP > CS descriptor limit");
        exception(BX_GP_EXCEPTION, 0);
      }

      // push flags onto stack
      // push current CS selector onto stack
      // push return offset onto stack
      if (gate_descriptor.type >= 14) { // 386 gate
        push_32(read_eflags());
        push_32(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
        push_32(EIP);
        if (push_error)
          push_32(error_code);
      }
      else { // 286 gate
        push_16((Bit16u) read_eflags());
        push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
        push_16(IP);
        if (push_error)
          push_16(error_code);
      }

      // load CS:IP from gate
      // load CS descriptor
      // set the RPL field of CS to CPL
      load_cs(&cs_selector, &cs_descriptor, CPL);
    }

    EIP = gate_dest_offset;

    // if interrupt gate then set IF to 0
    if (!(gate_descriptor.type & 1)) // even is int-gate
      BX_CPU_THIS_PTR clear_IF();
    BX_CPU_THIS_PTR clear_TF();
    BX_CPU_THIS_PTR clear_NT();
    BX_CPU_THIS_PTR clear_VM();
    BX_CPU_THIS_PTR clear_RF();
    return;

  default:
    printf("bad descriptor type in interrupt()!");
    break;
  }
}

void BX_CPU_C::real_mode_int(Bit8u vector, bx_bool push_error, Bit16u error_code)
{

  BX_INFO(("real_mode_int"));

  if ((vector*4+3) > BX_CPU_THIS_PTR idtr.limit) {
    BX_INFO(("interrupt(real mode) vector > idtr.limit"));
    exception(BX_GP_EXCEPTION, 0);
  }

  push_16((Bit16u) read_eflags());
  push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
  push_16(IP);

  Bit16u new_ip = system_read_word(BX_CPU_THIS_PTR idtr.base + 4 * vector);
  // CS.LIMIT can't change when in real/v8086 mode
  if (new_ip > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
    BX_ERROR(("interrupt(real mode): instruction pointer not within code segment limits"));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit16u cs_selector = system_read_word(BX_CPU_THIS_PTR idtr.base + 4 * vector + 2);
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_selector);
  EIP = new_ip;

  /* INT affects the following flags: I,T */
  BX_CPU_THIS_PTR clear_IF();
  BX_CPU_THIS_PTR clear_TF();
#if BX_CPU_LEVEL >= 4
  BX_CPU_THIS_PTR clear_AC();
#endif
  BX_CPU_THIS_PTR clear_RF();
}

void BX_CPU_C::interrupt(Bit8u vector, unsigned type, bx_bool push_error, Bit16u error_code)
{

BX_INFO(("interrupt (%x)",vector));

#if printfGER
  BX_CPU_THIS_PTR show_flag |= Flag_intsig;
#if printf_LINUX
  if (bx_dbg.linux_syscall) {
    if (vector == 0x80) bx_dbg_linux_syscall(BX_CPU_ID);
  }
#endif
  bx_dbg_interrupt(BX_CPU_ID, vector, error_code);
#endif

  BX_INSTR_INTERRUPT(BX_CPU_ID, vector);

  invalidate_prefetch_q();

  bx_bool soft_int = 0;
  switch(type) {
    case BX_SOFTWARE_INTERRUPT:
    case BX_SOFTWARE_EXCEPTION:
      soft_int = 1;
      break;
    case BX_PRIVILEGED_SOFTWARE_INTERRUPT:
    case BX_EXTERNAL_INTERRUPT:
    case BX_NMI:
    case BX_HARDWARE_EXCEPTION:
      break;
    default:
      BX_INFO(("interrupt(): unknown exception type %d", type));
  }

  BX_INFO(("interrupt(): vector = %02x, TYPE = %u, EXT = %u",
      vector, type, (unsigned) BX_CPU_THIS_PTR EXT));

  // Discard any traps and inhibits for new context; traps will
  // resume upon return.
  BX_CPU_THIS_PTR debug_trap = 0;
  BX_CPU_THIS_PTR inhibit_mask = 0;

  RSP_SPECULATIVE;

   // software interrupt can be redirefcted in v8086 mode
   if (type != BX_SOFTWARE_INTERRUPT || !v8086_mode() || !v86_redirect_interrupt(vector))
   {
     if(real_mode()) {
       real_mode_int(vector, push_error, error_code);
     }
     else {
       protected_mode_int(vector, soft_int, push_error, error_code);
     }
   }

   RSP_COMMIT;

#if BX_X86_DEBUGGER
  BX_CPU_THIS_PTR in_repeat = 0;
#endif

#if BX_SUPPORT_VMX || BX_SUPPORT_SVM
  BX_CPU_THIS_PTR in_event = 0;
#endif
}

/* Exception classes.  These are used as indexes into the 'is_exception_OK'
 * array below, and are stored in the 'exception' array also
 */
#define BX_ET_BENIGN       0
#define BX_ET_CONTRIBUTORY 1
#define BX_ET_PAGE_FAULT   2

#define BX_ET_DOUBLE_FAULT 10

static const bx_bool is_exception_OK[3][3] = {
    { 1, 1, 1 }, /* 1st exception is BENIGN */
    { 1, 0, 1 }, /* 1st exception is CONTRIBUTORY */
    { 1, 0, 0 }  /* 1st exception is PAGE_FAULT */
};

#define BX_EXCEPTION_CLASS_TRAP  0
#define BX_EXCEPTION_CLASS_FAULT 1
#define BX_EXCEPTION_CLASS_ABORT 2

struct BxExceptionInfo exceptions_info[BX_CPU_HANDLED_EXCEPTIONS] = {
  /* DE */ { BX_ET_CONTRIBUTORY, BX_EXCEPTION_CLASS_FAULT, 0 },
  /* DB */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 02 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 }, // NMI
  /* BP */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_TRAP,  0 },
  /* OF */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_TRAP,  0 },
  /* BR */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* UD */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* NM */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* DF */ { BX_ET_DOUBLE_FAULT, BX_EXCEPTION_CLASS_FAULT, 1 },
             // coprocessor segment overrun (286,386 only)
  /* 09 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* TS */ { BX_ET_CONTRIBUTORY, BX_EXCEPTION_CLASS_FAULT, 1 },
  /* NP */ { BX_ET_CONTRIBUTORY, BX_EXCEPTION_CLASS_FAULT, 1 },
  /* SS */ { BX_ET_CONTRIBUTORY, BX_EXCEPTION_CLASS_FAULT, 1 },
  /* GP */ { BX_ET_CONTRIBUTORY, BX_EXCEPTION_CLASS_FAULT, 1 },
  /* PF */ { BX_ET_PAGE_FAULT,   BX_EXCEPTION_CLASS_FAULT, 1 },
  /* 15 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 }, // reserved
  /* MF */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* AC */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 1 },
  /* MC */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_ABORT, 0 },
  /* XM */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 20 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 21 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 22 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 23 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 24 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 25 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 26 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 27 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 28 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 29 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 30 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 }, // FIXME: SVM #SF
  /* 31 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 }
};

// vector:     0..255: vector in IDT
// error_code: if exception generates and error, push this error code
// trap:       override exception class to TRAP
void BX_CPU_C::exception(unsigned vector, Bit16u error_code)
{
  BX_INSTR_EXCEPTION(BX_CPU_ID, vector, error_code);

#if printfGER
  bx_dbg_exception(BX_CPU_ID, vector, error_code);
#endif

  BX_INFO(("exception(0x%02x): error_code=%04x", vector, error_code));

  unsigned exception_type = 0;
  unsigned exception_class = BX_EXCEPTION_CLASS_FAULT;
  bx_bool push_error = 0;

  if (vector < BX_CPU_HANDLED_EXCEPTIONS) {
     push_error = exceptions_info[vector].push_error;
     exception_class = exceptions_info[vector].exception_class;
     exception_type = exceptions_info[vector].exception_type;
  }
  else {
     BX_ERROR(("exception(%u): bad vector", vector));
  }

  if (vector != BX_PF_EXCEPTION && vector != BX_DF_EXCEPTION) {
    // Page faults have different format
    error_code = (error_code & 0xfffe) | BX_CPU_THIS_PTR EXT;
  }

  if (BX_CPU_THIS_PTR errorno > 0) {
    if (BX_CPU_THIS_PTR errorno > 2 || BX_CPU_THIS_PTR curr_exception == BX_ET_DOUBLE_FAULT) {
      // restore RIP/RSP to value before error occurred
      RIP = BX_CPU_THIS_PTR prev_rip;
      if (BX_CPU_THIS_PTR speculative_rsp)
        RSP = BX_CPU_THIS_PTR prev_rsp;

      debug(BX_CPU_THIS_PTR prev_rip); // print debug information to the log
#if printfGER
      // trap into debugger (similar as done when PANIC occured)
      bx_debug_break();
#endif
      BX_PANIC(("exception(): 3rd (%d) exception with no resolution \n WARNING: Any simulation after this point is completely bogus !", vector));
      longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1); // go back to main decode loop
    }
  }

  // note: fault-class exceptions _except_ #DB set RF in
  //       eflags image.
  if (exception_class == BX_EXCEPTION_CLASS_FAULT)
  {
    // restore RIP/RSP to value before error occurred
    RIP = BX_CPU_THIS_PTR prev_rip;
    if (BX_CPU_THIS_PTR speculative_rsp)
      RSP = BX_CPU_THIS_PTR prev_rsp;

    if (vector != BX_DB_EXCEPTION) BX_CPU_THIS_PTR assert_RF();
  }

  if (vector == BX_DB_EXCEPTION) {
    // Commit debug events to DR6: preserve DR5.BS and DR6.BD values,
    // only software can clear them
    BX_CPU_THIS_PTR dr6.val32 = (BX_CPU_THIS_PTR dr6.val32 & 0xffff6ff0) |
                          (BX_CPU_THIS_PTR debug_trap & 0x0000e00f);

    // clear GD flag in the DR7 prior entering debug exception handler
    BX_CPU_THIS_PTR dr7.set_GD(0);
  }

  BX_CPU_THIS_PTR EXT = 1;

  // if we've already had 1st exception, see if 2nd causes a
  // Double Fault instead.  Otherwise, just record 1st exception
  if (BX_CPU_THIS_PTR errorno > 0 && exception_type != BX_ET_DOUBLE_FAULT) {
    if (! is_exception_OK[BX_CPU_THIS_PTR curr_exception][exception_type]) {
      exception(BX_DF_EXCEPTION, 0);
    }
  }

  BX_CPU_THIS_PTR curr_exception = exception_type;
  BX_CPU_THIS_PTR errorno++;

  if (real_mode()) {
    push_error = 0; // not INT, no error code pushed
    error_code = 0;
  }

  interrupt(vector, BX_HARDWARE_EXCEPTION, push_error, error_code);
  BX_CPU_THIS_PTR errorno = 0; // error resolved
  longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1); // go back to main decode loop
}
