/////////////////////////////////////////////////////////////////////////
// $Id: cpu.h 11356 2012-08-21 19:58:41Z sshwarts $
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
/////////////////////////////////////////////////////////////////////////

#ifndef BX_CPU_H
#  define BX_CPU_H 1

#include "config.h"
#include "syscall/syscall.h"
#include <setjmp.h>
#include <vector>

// segment register encoding
#define BX_SEG_REG_ES    0
#define BX_SEG_REG_CS    1
#define BX_SEG_REG_SS    2
#define BX_SEG_REG_DS    3
#define BX_SEG_REG_FS    4
#define BX_SEG_REG_GS    5
// NULL now has to fit in 3 bits.
#define BX_SEG_REG_NULL  7
#define BX_NULL_SEG_REG(seg) ((seg) == BX_SEG_REG_NULL)

#define BX_16BIT_REG_AX 0
#define BX_16BIT_REG_CX 1
#define BX_16BIT_REG_DX 2
#define BX_16BIT_REG_BX 3
#define BX_16BIT_REG_SP 4
#define BX_16BIT_REG_BP 5
#define BX_16BIT_REG_SI 6
#define BX_16BIT_REG_DI 7

#define BX_32BIT_REG_EAX 0
#define BX_32BIT_REG_ECX 1
#define BX_32BIT_REG_EDX 2
#define BX_32BIT_REG_EBX 3
#define BX_32BIT_REG_ESP 4
#define BX_32BIT_REG_EBP 5
#define BX_32BIT_REG_ESI 6
#define BX_32BIT_REG_EDI 7

# define BX_GENERAL_REGISTERS 8

#define BX_TMP_REGISTER (BX_GENERAL_REGISTERS)

#define BX_16BIT_REG_IP  (BX_GENERAL_REGISTERS+1)
#define BX_32BIT_REG_EIP (BX_GENERAL_REGISTERS+1)

#define BX_NIL_REGISTER  (BX_GENERAL_REGISTERS+2)

#if defined(NEED_CPU_REG_SHORTCUTS)

/* WARNING:
   Only BX_CPU_C member functions can use these shortcuts safely!
   Functions that use the shortcuts outside of BX_CPU_C might work
   when BX_USE_CPU_SMF=1 but will fail when BX_USE_CPU_SMF=0
   (for example in SMP mode).
*/

// access to 8 bit general registers
#define AL (bx_cpu.gen_reg[0].word.byte.rl)
#define CL (bx_cpu.gen_reg[1].word.byte.rl)
#define DL (bx_cpu.gen_reg[2].word.byte.rl)
#define BL (bx_cpu.gen_reg[3].word.byte.rl)
#define AH (bx_cpu.gen_reg[0].word.byte.rh)
#define CH (bx_cpu.gen_reg[1].word.byte.rh)
#define DH (bx_cpu.gen_reg[2].word.byte.rh)
#define BH (bx_cpu.gen_reg[3].word.byte.rh)

#define TMP8L (bx_cpu.gen_reg[BX_TMP_REGISTER].word.byte.rl)

// access to 16 bit general registers
#define AX (bx_cpu.gen_reg[0].word.rx)
#define CX (bx_cpu.gen_reg[1].word.rx)
#define DX (bx_cpu.gen_reg[2].word.rx)
#define BX (bx_cpu.gen_reg[3].word.rx)
#define SP (bx_cpu.gen_reg[4].word.rx)
#define BP (bx_cpu.gen_reg[5].word.rx)
#define SI (bx_cpu.gen_reg[6].word.rx)
#define DI (bx_cpu.gen_reg[7].word.rx)

// access to 16 bit instruction pointer
#define IP (bx_cpu.gen_reg[BX_16BIT_REG_IP].word.rx)

#define TMP16 (bx_cpu.gen_reg[BX_TMP_REGISTER].word.rx)

// accesss to 32 bit general registers
#define EAX (bx_cpu.gen_reg[0].dword.erx)
#define ECX (bx_cpu.gen_reg[1].dword.erx)
#define EDX (bx_cpu.gen_reg[2].dword.erx)
#define EBX (bx_cpu.gen_reg[3].dword.erx)
#define ESP (bx_cpu.gen_reg[4].dword.erx)
#define EBP (bx_cpu.gen_reg[5].dword.erx)
#define ESI (bx_cpu.gen_reg[6].dword.erx)
#define EDI (bx_cpu.gen_reg[7].dword.erx)

// access to 32 bit instruction pointer
#define EIP (bx_cpu.gen_reg[BX_32BIT_REG_EIP].dword.erx)

#define TMP32 (bx_cpu.gen_reg[BX_TMP_REGISTER].dword.erx)


#define RAX EAX
#define RCX ECX
#define RDX EDX
#define RBX EBX
#define RSP ESP
#define RBP EBP
#define RSI ESI
#define RDI EDI
#define RIP EIP


#define BX_READ_8BIT_REG(index)  (((index) & 4) ? \
  (bx_cpu.gen_reg[(index)-4].word.byte.rh) : \
  (bx_cpu.gen_reg[index].word.byte.rl))
#define BX_READ_8BIT_REGx(index,ext) BX_READ_8BIT_REG(index)

#define BX_READ_8BIT_REGH(index) (bx_cpu.gen_reg[index].word.byte.rh)
#define BX_READ_16BIT_REG(index) (bx_cpu.gen_reg[index].word.rx)
#define BX_READ_32BIT_REG(index) (bx_cpu.gen_reg[index].dword.erx)

#define BX_WRITE_8BIT_REGH(index, val) {\
  bx_cpu.gen_reg[index].word.byte.rh = val; \
}

#define BX_WRITE_16BIT_REG(index, val) {\
  bx_cpu.gen_reg[index].word.rx = val; \
}

/*
#define BX_WRITE_32BIT_REG(index, val) {\
  bx_cpu.gen_reg[index].dword.erx = val; \
}
*/


#define BX_WRITE_8BIT_REG(index, val) {\
  if ((index) & 4) \
    bx_cpu.gen_reg[(index)-4].word.byte.rh = val; \
  else \
    bx_cpu.gen_reg[index].word.byte.rl = val; \
}
#define BX_WRITE_8BIT_REGx(index, ext, val) BX_WRITE_8BIT_REG(index, val)

// For x86-32, I just pretend this one is like the macro above,
// so common code can be used.
#define BX_WRITE_32BIT_REGZ(index, val) {\
  bx_cpu.gen_reg[index].dword.erx = (Bit32u) val; \
}

#define BX_CLEAR_64BIT_HIGH(index)

#define CPL       (bx_cpu.sregs[BX_SEG_REG_CS].selector.rpl)
#define USER_PL   (bx_cpu.user_pl) /* CPL == 3 */
#define BX_CPU_ID (0)

#endif  // defined(NEED_CPU_REG_SHORTCUTS)

// <TAG-INSTRUMENTATION_COMMON-BEGIN>

// possible types passed to BX_INSTR_TLB_CNTRL()
#define BX_INSTR_MOV_CR0        10
#define BX_INSTR_MOV_CR3        11
#define BX_INSTR_MOV_CR4        12
#define BX_INSTR_TASK_SWITCH    13
#define BX_INSTR_CONTEXT_SWITCH 14
#define BX_INSTR_INVLPG         15
#define BX_INSTR_INVEPT         16
#define BX_INSTR_INVVPID        17
#define BX_INSTR_INVPCID        18

// possible types passed to BX_INSTR_CACHE_CNTRL()
#define BX_INSTR_INVD           10
#define BX_INSTR_WBINVD         11

// possible types passed to BX_INSTR_FAR_BRANCH() and BX_INSTR_UCNEAR_BRANCH()
#define BX_INSTR_IS_JMP            10
#define BX_INSTR_IS_JMP_INDIRECT   11
#define BX_INSTR_IS_CALL           12
#define BX_INSTR_IS_CALL_INDIRECT  13
#define BX_INSTR_IS_RET            14
#define BX_INSTR_IS_IRET           15
#define BX_INSTR_IS_INT            16
#define BX_INSTR_IS_SYSCALL        17
#define BX_INSTR_IS_SYSRET         18
#define BX_INSTR_IS_SYSENTER       19
#define BX_INSTR_IS_SYSEXIT        20

// possible types passed to BX_INSTR_PREFETCH_HINT()
#define BX_INSTR_PREFETCH_NTA   0
#define BX_INSTR_PREFETCH_T0    1
#define BX_INSTR_PREFETCH_T1    2
#define BX_INSTR_PREFETCH_T2    3

// <TAG-INSTRUMENTATION_COMMON-END>

// passed to internal debugger together with BX_READ/BX_WRITE/BX_EXECUTE/BX_RW
enum {
  BX_PDPTR0_ACCESS = 1,
  BX_PDPTR1_ACCESS,
  BX_PDPTR2_ACCESS,
  BX_PDPTR3_ACCESS,
  BX_PTE_ACCESS,
  BX_PDE_ACCESS,
  BX_PDTE_ACCESS,
  BX_PML4E_ACCESS,
  BX_EPT_PTE_ACCESS,
  BX_EPT_PDE_ACCESS,
  BX_EPT_PDTE_ACCESS,
  BX_EPT_PML4E_ACCESS,
  BX_VMCS_ACCESS,
  BX_MSR_BITMAP_ACCESS,
  BX_IO_BITMAP_ACCESS,
  BX_VMX_LOAD_MSR_ACCESS,
  BX_VMX_STORE_MSR_ACCESS,
  BX_VMX_VTPR_ACCESS,
  BX_SMRAM_ACCESS
};

struct BxExceptionInfo {
  unsigned exception_type;
  unsigned exception_class;
  bx_bool push_error;
};

#define BX_DE_EXCEPTION   0 // Divide Error (fault)
#define BX_DB_EXCEPTION   1 // Debug (fault/trap)
#define BX_BP_EXCEPTION   3 // Breakpoint (trap)
#define BX_OF_EXCEPTION   4 // Overflow (trap)
#define BX_BR_EXCEPTION   5 // BOUND (fault)
#define BX_UD_EXCEPTION   6
#define BX_NM_EXCEPTION   7
#define BX_DF_EXCEPTION   8
#define BX_TS_EXCEPTION  10
#define BX_NP_EXCEPTION  11
#define BX_SS_EXCEPTION  12
#define BX_GP_EXCEPTION  13
#define BX_PF_EXCEPTION  14
#define BX_MF_EXCEPTION  16
#define BX_AC_EXCEPTION  17
#define BX_MC_EXCEPTION  18
#define BX_XM_EXCEPTION  19

#define BX_CPU_HANDLED_EXCEPTIONS  32

/* MSR registers */
#define BX_MSR_TSC                 0x010
#define BX_MSR_APICBASE            0x01b

#if BX_CPU_LEVEL >= 6
  #define BX_MSR_SYSENTER_CS       0x174
  #define BX_MSR_SYSENTER_ESP      0x175
  #define BX_MSR_SYSENTER_EIP      0x176
#endif

#define BX_MSR_DEBUGCTLMSR         0x1d9
#define BX_MSR_LASTBRANCHFROMIP    0x1db
#define BX_MSR_LASTBRANCHTOIP      0x1dc
#define BX_MSR_LASTINTOIP          0x1dd

#if BX_CPU_LEVEL >= 6
  #define BX_MSR_MTRRCAP           0x0fe
  #define BX_MSR_MTRRPHYSBASE0     0x200
  #define BX_MSR_MTRRPHYSMASK0     0x201
  #define BX_MSR_MTRRPHYSBASE1     0x202
  #define BX_MSR_MTRRPHYSMASK1     0x203
  #define BX_MSR_MTRRPHYSBASE2     0x204
  #define BX_MSR_MTRRPHYSMASK2     0x205
  #define BX_MSR_MTRRPHYSBASE3     0x206
  #define BX_MSR_MTRRPHYSMASK3     0x207
  #define BX_MSR_MTRRPHYSBASE4     0x208
  #define BX_MSR_MTRRPHYSMASK4     0x209
  #define BX_MSR_MTRRPHYSBASE5     0x20a
  #define BX_MSR_MTRRPHYSMASK5     0x20b
  #define BX_MSR_MTRRPHYSBASE6     0x20c
  #define BX_MSR_MTRRPHYSMASK6     0x20d
  #define BX_MSR_MTRRPHYSBASE7     0x20e
  #define BX_MSR_MTRRPHYSMASK7     0x20f
  #define BX_MSR_MTRRFIX64K_00000  0x250
  #define BX_MSR_MTRRFIX16K_80000  0x258
  #define BX_MSR_MTRRFIX16K_A0000  0x259
  #define BX_MSR_MTRRFIX4K_C0000   0x268
  #define BX_MSR_MTRRFIX4K_C8000   0x269
  #define BX_MSR_MTRRFIX4K_D0000   0x26a
  #define BX_MSR_MTRRFIX4K_D8000   0x26b
  #define BX_MSR_MTRRFIX4K_E0000   0x26c
  #define BX_MSR_MTRRFIX4K_E8000   0x26d
  #define BX_MSR_MTRRFIX4K_F0000   0x26e
  #define BX_MSR_MTRRFIX4K_F8000   0x26f
  #define BX_MSR_PAT               0x277
  #define BX_MSR_MTRR_DEFTYPE      0x2ff
#endif

#define BX_MSR_TSC_DEADLINE        0x6E0

#define BX_MSR_MAX_INDEX          0x1000

enum {
  BX_MEMTYPE_UC = 0,
  BX_MEMTYPE_WC,
  BX_MEMTYPE_RESERVED2,
  BX_MEMTYPE_RESERVED3,
  BX_MEMTYPE_WT,
  BX_MEMTYPE_WP,
  BX_MEMTYPE_WB
};

#if BX_SUPPORT_VMX
  #define BX_MSR_VMX_BASIC                0x480
  #define BX_MSR_VMX_PINBASED_CTRLS       0x481
  #define BX_MSR_VMX_PROCBASED_CTRLS      0x482
  #define BX_MSR_VMX_VMEXIT_CTRLS         0x483
  #define BX_MSR_VMX_VMENTRY_CTRLS        0x484
  #define BX_MSR_VMX_MISC                 0x485
  #define BX_MSR_VMX_CR0_FIXED0           0x486
  #define BX_MSR_VMX_CR0_FIXED1           0x487
  #define BX_MSR_VMX_CR4_FIXED0           0x488
  #define BX_MSR_VMX_CR4_FIXED1           0x489
  #define BX_MSR_VMX_VMCS_ENUM            0x48a
  #define BX_MSR_VMX_PROCBASED_CTRLS2     0x48b
  #define BX_MSR_VMX_EPT_VPID_CAP         0x48c
  #define BX_MSR_VMX_TRUE_PINBASED_CTRLS  0x48d
  #define BX_MSR_VMX_TRUE_PROCBASED_CTRLS 0x48e
  #define BX_MSR_VMX_TRUE_VMEXIT_CTRLS    0x48f
  #define BX_MSR_VMX_TRUE_VMENTRY_CTRLS   0x490
  #define BX_MSR_VMX_VMFUNC               0x491
  #define BX_MSR_IA32_FEATURE_CONTROL     0x03A
  #define BX_MSR_IA32_SMM_MONITOR_CTL     0x09B
#endif

#define BX_MSR_EFER             0xc0000080
#define BX_MSR_STAR             0xc0000081
#define BX_MSR_LSTAR            0xc0000082
#define BX_MSR_CSTAR            0xc0000083
#define BX_MSR_FMASK            0xc0000084
#define BX_MSR_FSBASE           0xc0000100
#define BX_MSR_GSBASE           0xc0000101
#define BX_MSR_KERNELGSBASE     0xc0000102
#define BX_MSR_TSC_AUX          0xc0000103

#define BX_SVM_VM_CR_MSR        0xc0010114
#define BX_SVM_IGNNE_MSR        0xc0010115
#define BX_SVM_SMM_CTL_MSR      0xc0010116
#define BX_SVM_HSAVE_PA_MSR     0xc0010117

#define BX_MODE_IA32_REAL       0x0   // CR0.PE=0                |
#define BX_MODE_IA32_V8086      0x1   // CR0.PE=1, EFLAGS.VM=1   | EFER.LMA=0
#define BX_MODE_IA32_PROTECTED  0x2   // CR0.PE=1, EFLAGS.VM=0   |
#define BX_MODE_LONG_COMPAT     0x3   // EFER.LMA = 1, CR0.PE=1, CS.L=0
#define BX_MODE_LONG_64         0x4   // EFER.LMA = 1, CR0.PE=1, CS.L=1

extern const char* cpu_mode_string(unsigned cpu_mode);


#define IsValidPhyAddr(addr) ((addr & BX_PHY_ADDRESS_RESERVED_BITS) == 0)

#define BX_CPU_INTR  (bx_cpu.INTR)

#define CACHE_LINE_SIZE 64

class BX_CPU_C;
class BX_MEM_C;
class BX_RUNTIME;

// normal member functions.  This can ONLY be used within BX_CPU_C classes.
// Anyone on the outside should use the BX_CPU macro (defined in bochs.h)
// instead.
#  define BX_CPU_THIS_PTR  this->
#  define BX_CPU_THIS      this
#  define BX_SMF
// with normal member functions, calling a member fn pointer looks like
// object->*(fnptr)(arg, ...);
// Since this is different from when SMF=1, encapsulate it in a macro.
#  define BX_CPU_CALL_METHOD(func, args) \
            (this->*((BxExecutePtr_tR) (func))) args
#  define BX_CPU_CALL_METHODR(func, args) \
            (this->*((BxResolvePtr_tR) (func))) args
#  define BX_CPU_CALL_REP_ITERATION(func, args) \
            (this->*((BxRepIterationPtr_tR) (func))) args

// single processor simulation, so there's one of everything
extern BX_CPU_C   bx_cpu;
extern BX_MEM_C   bx_mem;
extern BX_SYSCALL bx_sys;
extern BX_RUNTIME bx_rnt;

// notify internal debugger/instrumentation about memory access
#define BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, paddr, size, pl, rw, dataptr) {              \
  BX_INSTR_LIN_ACCESS(BX_CPU_ID, (laddr), (paddr), (size), (rw));                       \
  BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, (laddr), (paddr), (size), (pl), (rw), (dataptr)); \
}

#define BX_NOTIFY_PHY_MEMORY_ACCESS(paddr, size, rw, why, dataptr) {            \
  BX_INSTR_PHY_ACCESS(BX_CPU_ID, (paddr), (size), (rw));                        \
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, (paddr), (size), (rw), (why), (dataptr)); \
}

// accessors for all eflags in bx_flags_reg_t
// The macro is used once for each flag bit
// Do not use for arithmetic flags !
#define DECLARE_EFLAG_ACCESSOR(name,bitnum)                     \
  BX_CPP_INLINE Bit32u  get_##name ();                   \
  BX_CPP_INLINE bx_bool getB_##name ();                  \
  BX_CPP_INLINE void assert_##name ();                   \
  BX_CPP_INLINE void clear_##name ();                    \
  BX_CPP_INLINE void set_##name (bx_bool val);

#define IMPLEMENT_EFLAG_ACCESSOR(name,bitnum)                   \
  BX_CPP_INLINE bx_bool BX_CPU_C::getB_##name () {              \
    return 1 & (bx_cpu.eflags >> bitnum);              \
  }                                                             \
  BX_CPP_INLINE Bit32u  BX_CPU_C::get_##name () {               \
    return bx_cpu.eflags & (1 << bitnum);              \
  }

#define IMPLEMENT_EFLAG_SET_ACCESSOR(name,bitnum)               \
  BX_CPP_INLINE void BX_CPU_C::assert_##name () {               \
    bx_cpu.eflags |= (1<<bitnum);                      \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::clear_##name () {                \
    bx_cpu.eflags &= ~(1<<bitnum);                     \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::set_##name (bx_bool val) {       \
    bx_cpu.eflags =                                    \
      (bx_cpu.eflags&~(1<<bitnum))|((val)<<bitnum);    \
  }

#if BX_CPU_LEVEL >= 4

#define IMPLEMENT_EFLAG_SET_ACCESSOR_AC(bitnum)                 \
  BX_CPP_INLINE void BX_CPU_C::assert_AC () {                   \
    bx_cpu.eflags |= (1<<bitnum);                      \
    handleAlignmentCheck();                                     \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::clear_AC() {                     \
    bx_cpu.eflags &= ~(1<<bitnum);                     \
    handleAlignmentCheck();                                     \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::set_AC(bx_bool val) {            \
    bx_cpu.eflags =                                    \
      (bx_cpu.eflags&~(1<<bitnum))|((val)<<bitnum);    \
    handleAlignmentCheck();                                     \
  }

#endif

#define IMPLEMENT_EFLAG_SET_ACCESSOR_VM(bitnum)                 \
  BX_CPP_INLINE void BX_CPU_C::assert_VM() {                    \
    set_VM(1);                                                  \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::clear_VM() {                     \
    set_VM(0);                                                  \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::set_VM(bx_bool val) {            \
    if (!long_mode()) {                                         \
      bx_cpu.eflags =                                  \
        (bx_cpu.eflags&~(1<<bitnum))|((val)<<bitnum);  \
      handleCpuModeChange();                                    \
    }                                                           \
  }

// assert async_event when IF or TF is set
#define IMPLEMENT_EFLAG_SET_ACCESSOR_IF_TF(name,bitnum)         \
  BX_CPP_INLINE void BX_CPU_C::assert_##name() {                \
    bx_cpu.async_event = 1;                            \
    bx_cpu.eflags |= (1<<bitnum);                      \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::clear_##name() {                 \
    bx_cpu.eflags &= ~(1<<bitnum);                     \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::set_##name(bx_bool val) {        \
    if (val) bx_cpu.async_event = 1;                   \
    bx_cpu.eflags =                                    \
      (bx_cpu.eflags&~(1<<bitnum))|((val)<<bitnum);    \
  }

// invalidate prefetch queue and call prefetch() when RF is set
#define IMPLEMENT_EFLAG_SET_ACCESSOR_RF(name,bitnum)            \
  BX_CPP_INLINE void BX_CPU_C::assert_##name() {                \
    invalidate_prefetch_q();                                    \
    bx_cpu.eflags |= (1<<bitnum);                      \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::clear_##name() {                 \
    bx_cpu.eflags &= ~(1<<bitnum);                     \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::set_##name(bx_bool val) {        \
    if (val) invalidate_prefetch_q();                           \
    bx_cpu.eflags =                                    \
      (bx_cpu.eflags&~(1<<bitnum))|((val)<<bitnum);    \
  }

#define DECLARE_EFLAG_ACCESSOR_IOPL(bitnum)                     \
  BX_CPP_INLINE void set_IOPL(Bit32u val);               \
  BX_CPP_INLINE Bit32u  get_IOPL(void);

#define IMPLEMENT_EFLAG_ACCESSOR_IOPL(bitnum)                   \
  BX_CPP_INLINE void BX_CPU_C::set_IOPL(Bit32u val) {           \
    bx_cpu.eflags &= ~(3<<12);                         \
    bx_cpu.eflags |= ((3&val) << 12);                  \
  }                                                             \
  BX_CPP_INLINE Bit32u BX_CPU_C::get_IOPL() {                   \
    return 3 & (bx_cpu.eflags >> 12);                  \
  }

#define EFlagsCFMask   (1 <<  0)
#define EFlagsPFMask   (1 <<  2)
#define EFlagsAFMask   (1 <<  4)
#define EFlagsZFMask   (1 <<  6)
#define EFlagsSFMask   (1 <<  7)
#define EFlagsTFMask   (1 <<  8)
#define EFlagsIFMask   (1 <<  9)
#define EFlagsDFMask   (1 << 10)
#define EFlagsOFMask   (1 << 11)
#define EFlagsIOPLMask (3 << 12)
#define EFlagsNTMask   (1 << 14)
#define EFlagsRFMask   (1 << 16)
#define EFlagsVMMask   (1 << 17)
#define EFlagsACMask   (1 << 18)
#define EFlagsVIFMask  (1 << 19)
#define EFlagsVIPMask  (1 << 20)
#define EFlagsIDMask   (1 << 21)

#define EFlagsOSZAPCMask \
    (EFlagsCFMask | EFlagsPFMask | EFlagsAFMask | EFlagsZFMask | EFlagsSFMask | EFlagsOFMask)

#define EFlagsOSZAPMask  \
    (EFlagsPFMask | EFlagsAFMask | EFlagsZFMask | EFlagsSFMask | EFlagsOFMask)

#define EFlagsValidMask  0x003f7fd5	// only supported bits for EFLAGS

#if BX_CPU_LEVEL >= 5
typedef struct
{

#define MSR_STAR   (bx_cpu.msr.star)

  // SYSCALL/SYSRET instruction msr's
  Bit64u star;

#if BX_CPU_LEVEL >= 6
  // SYSENTER/SYSEXIT instruction msr's
  Bit32u sysenter_cs_msr;
  bx_address sysenter_esp_msr;
  bx_address sysenter_eip_msr;

  Bit64u mtrrphys[16];
  Bit64u mtrrfix64k_00000;
  Bit64u mtrrfix16k[2];
  Bit64u mtrrfix4k[8];
  Bit16u mtrr_deftype;
  Bit64u pat;
#endif

#if BX_SUPPORT_VMX
  Bit32u ia32_feature_ctrl;
#endif

#if BX_SUPPORT_SVM
  Bit64u svm_hsave_pa;
#endif

  /* TODO finish of the others */
} bx_regs_msr_t;
#endif

#include "cpuid.h"
#include "crregs.h"
#include "descriptor.h"
#include "instr.h"
#include "lazy_flags.h"

// BX_TLB_SIZE: Number of entries in TLB
// BX_TLB_INDEX_OF(lpf): This macro is passed the linear page frame
//   (top 20 bits of the linear address.  It must map these bits to
//   one of the TLB cache slots, given the size of BX_TLB_SIZE.
//   There will be a many-to-one mapping to each TLB cache slot.
//   When there are collisions, the old entry is overwritten with
//   one for the newest access.

#define BX_TLB_SIZE 1024
#define BX_TLB_MASK ((BX_TLB_SIZE-1) << 12)
#define BX_TLB_INDEX_OF(lpf, len) ((((unsigned)(lpf) + (len)) & BX_TLB_MASK) >> 12)

typedef bx_ptr_equiv_t bx_hostpageaddr_t;

typedef struct {
  bx_address lpf;       // linear page frame
  bx_phy_address ppf;   // physical page frame
  bx_hostpageaddr_t hostPageAddr;
  Bit32u accessBits;
  Bit32u lpf_mask;      // linear address mask of the page size
} bx_TLB_entry;

  #define LPF_MASK (0xfffff000)

#if BX_PHY_ADDRESS_LONG
  #define PPF_MASK BX_CONST64(0xfffffffffffff000)
#else
  #define PPF_MASK (0xfffff000)
#endif

#define LPFOf(laddr)               ((laddr) & LPF_MASK)
#define PPFOf(laddr)               ((laddr) & PPF_MASK)

#define AlignedAccessLPFOf(laddr, alignment_mask) \
                  ((laddr) & (LPF_MASK | (alignment_mask)))

#define PAGE_OFFSET(laddr) ((Bit32u)(laddr) & 0xfff)

#include "icache.h"

// general purpose register
#ifdef BX_BIG_ENDIAN
typedef struct {
  union {
    struct {
      Bit32u erx;
    } dword;
    struct {
      Bit16u word_filler;
      union {
        Bit16u rx;
        struct {
          Bit8u rh;
          Bit8u rl;
        } byte;
      };
    } word;
  };
} bx_gen_reg_t;
#else
typedef struct {
  union {
    struct {
      Bit32u erx;
    } dword;
    struct {
      union {
        Bit16u rx;
        struct {
          Bit8u rl;
          Bit8u rh;
        } byte;
      };
      Bit16u word_filler;
    } word;
  };
} bx_gen_reg_t;
#endif

#if BX_SUPPORT_FPU
#include "i387.h"
#include "xmm.h"
#endif

#if BX_SUPPORT_VMX
#include "vmx.h"
#endif

#if BX_SUPPORT_SVM
#include "svm.h"
#endif

#if BX_SUPPORT_MONITOR_MWAIT
struct monitor_addr_t {

    bx_phy_address monitor_addr;
    bx_bool armed;

    monitor_addr_t(): monitor_addr(0xffffffff), armed(0) {}

    BX_CPP_INLINE void arm(bx_phy_address addr) {
      // align to cache line
      monitor_addr = addr & ~((bx_phy_address)(CACHE_LINE_SIZE - 1));
      armed = 1;
    }

    BX_CPP_INLINE void reset_monitor(void) { armed = 0; }
};
#endif

class BX_SMM_State;

struct DiCacheEntry {
    Bit32u tag;
    Bit8u *ptr;
    int ret;

    bxInstruction_c ins;
};

class BX_CPU_C {

public: // for now...

  unsigned bx_cpuid;

  void push_16(Bit16u value16);
  void push_32(Bit32u value32);
  Bit16u pop_16(void);
  Bit32u pop_32(void);

#if BX_CPU_LEVEL >= 4
  bx_cpuid_t *cpuid;
#endif

  Bit64u isa_extensions_bitmask;
  Bit32u cpu_extensions_bitmask;
#if BX_SUPPORT_VMX
  Bit32u vmx_extensions_bitmask;
#endif
#if BX_SUPPORT_SVM
  Bit32u svm_extensions_bitmask;
#endif

#define BX_CPUID_SUPPORT_ISA_EXTENSION(feature) \
   (bx_cpu.isa_extensions_bitmask & (feature))

#define BX_CPUID_SUPPORT_CPU_EXTENSION(feature) \
   (bx_cpu.cpu_extensions_bitmask & (feature))

#define BX_SUPPORT_VMX_EXTENSION(feature) \
   (bx_cpu.vmx_extensions_bitmask & (feature))

#define BX_SUPPORT_SVM_EXTENSION(feature) \
   (bx_cpu.svm_extensions_bitmask & (feature))

  std::vector<DiCacheEntry> diCache;

  void initializeDiCache() {
      diCache.resize(512);
  }

  // General register set
  // rax: accumulator
  // rbx: base
  // rcx: count
  // rdx: data
  // rbp: base pointer
  // rsi: source index
  // rdi: destination index
  // esp: stack pointer
  // r8..r15 x86-64 extended registers
  // rip: instruction pointer
  // nil: null register
  // tmp: temp register
  bx_gen_reg_t gen_reg[BX_GENERAL_REGISTERS+3];

  /* 31|30|29|28| 27|26|25|24| 23|22|21|20| 19|18|17|16
   * ==|==|=====| ==|==|==|==| ==|==|==|==| ==|==|==|==
   *  0| 0| 0| 0|  0| 0| 0| 0|  0| 0|ID|VP| VF|AC|VM|RF
   *
   * 15|14|13|12| 11|10| 9| 8|  7| 6| 5| 4|  3| 2| 1| 0
   * ==|==|=====| ==|==|==|==| ==|==|==|==| ==|==|==|==
   *  0|NT| IOPL| OF|DF|IF|TF| SF|ZF| 0|AF|  0|PF| 1|CF
   */
  Bit32u eflags; // Raw 32-bit value in x86 bit position.

  // lazy arithmetic flags state
  bx_lf_flags_entry oszapc;

  // so that we can back up when handling faults, exceptions, etc.
  // we need to store the value of the instruction pointer, before
  // each fetch/execute cycle.
  bx_address prev_rip;
  bx_address prev_rsp;
  bx_bool    speculative_rsp;

  Bit64u icount;
  Bit64u icount_last_sync;

#define BX_INHIBIT_INTERRUPTS        0x01
#define BX_INHIBIT_DEBUG             0x02

#define BX_INHIBIT_INTERRUPTS_BY_MOVSS        \
    (BX_INHIBIT_INTERRUPTS | BX_INHIBIT_DEBUG)

  // What events to inhibit at any given time.  Certain instructions
  // inhibit interrupts, some debug exceptions and single-step traps.
  unsigned inhibit_mask;
  Bit64u inhibit_icount;

  /* user segment register set */
  bx_segment_reg_t  sregs[6];

  /* system segment registers */
  bx_global_segment_reg_t gdtr; /* global descriptor table register */
  bx_global_segment_reg_t idtr; /* interrupt descriptor table register */
  bx_segment_reg_t        ldtr; /* local descriptor table register */
  bx_segment_reg_t        tr;   /* task register */

  /* debug registers DR0-DR7 */
  bx_address dr[4]; /* DR0-DR3 */
  bx_dr6_t   dr6;
  bx_dr7_t   dr7;

  Bit32u debug_trap; // holds DR6 value (16bit) to be set

  /* Control registers */
  bx_cr0_t   cr0;
  bx_address cr2;
  bx_address cr3;
#if BX_CPU_LEVEL >= 5
  bx_cr4_t   cr4;
  Bit32u cr4_suppmask;

  bx_efer_t efer;
  Bit32u efer_suppmask;
#endif

#if BX_CPU_LEVEL >= 5
  // TSC: Time Stamp Counter
  // Instead of storing a counter and incrementing it every instruction, we
  // remember the time in ticks that it was reset to zero.  With a little
  // algebra, we can also support setting it to something other than zero.
  // Don't read this directly; use get_TSC and set_TSC to access the TSC.
  Bit64u tsc_last_reset;
#if BX_SUPPORT_VMX || BX_SUPPORT_SVM
  Bit64s tsc_offset;
#endif
#endif

#if BX_CPU_LEVEL >= 6
  xcr0_t xcr0;
  Bit32u xcr0_suppmask;
#endif

  /* SMM base register */
  Bit32u smbase;

#if BX_CPU_LEVEL >= 5
  bx_regs_msr_t msr;
#endif

#if BX_CONFIGURE_MSRS
  MSR *msrs[BX_MSR_MAX_INDEX];
#endif

#if BX_SUPPORT_FPU
  i387_t the_i387;
#endif

#if BX_CPU_LEVEL >= 6
#if BX_SUPPORT_AVX
  bx_avx_reg_t vmm[BX_XMM_REGISTERS+1];
#else
  bx_xmm_reg_t vmm[BX_XMM_REGISTERS+1];
#endif
  bx_mxcsr_t mxcsr;
  Bit32u mxcsr_mask;
#endif

#if BX_SUPPORT_MONITOR_MWAIT
  monitor_addr_t monitor;
#endif


#if BX_SUPPORT_VMX
  bx_bool in_vmx;
  bx_bool in_vmx_guest;
  bx_bool in_smm_vmx; // save in_vmx and in_vmx_guest flags when in SMM mode
  bx_bool in_smm_vmx_guest;
  bx_bool vmx_interrupt_window;
#if BX_SUPPORT_VMX >= 2
  bx_bool pending_vmx_timer_expired;
#endif
  Bit64u  vmcsptr;
  bx_hostpageaddr_t vmcshostptr;
  Bit64u  vmxonptr;
  
  VMCS_CACHE vmcs;
  VMX_CAP vmx_cap;
#endif

#if BX_SUPPORT_SVM
  bx_bool in_svm_guest;
  bx_bool svm_gif; /* global interrupt enable flag, when zero all external interrupt disabled */
  bx_phy_address  vmcbptr;
  bx_hostpageaddr_t vmcbhostptr;
  VMCB_CACHE vmcb;

// make SVM integration easier
#define SVM_GIF (bx_cpu.svm_gif)

#else

#define SVM_GIF (1)

#endif

#if BX_SUPPORT_VMX || BX_SUPPORT_SVM
  bx_bool in_event;
#endif

  bx_bool EXT; /* 1 if processing external interrupt or exception
                * or if not related to current instruction,
                * 0 if current CS:IP caused exception */
  unsigned errorno;   /* signal exception during instruction emulation */

#define BX_ACTIVITY_STATE_ACTIVE        (0)
#define BX_ACTIVITY_STATE_HLT           (1)
#define BX_ACTIVITY_STATE_SHUTDOWN      (2)
#define BX_ACTIVITY_STATE_WAIT_FOR_SIPI (3)
#define BX_ACTIVITY_STATE_MWAIT         (4)
#define BX_ACTIVITY_STATE_MWAIT_IF      (5)
  unsigned activity_state;

  Bit32u  async_event;

#define BX_ASYNC_EVENT_STOP_TRACE (0x80000000)

#if BX_X86_DEBUGGER
  bx_bool  in_repeat;
  bx_bool  codebp;
#endif
  bx_bool  in_smm;
  unsigned cpu_mode;
  bx_bool  user_pl;
  bx_bool  INTR;
  bx_bool  pending_SMI;
  bx_bool  pending_NMI;
  bx_bool  pending_INIT;
  bx_bool  disable_SMI;
  bx_bool  disable_NMI;
  bx_bool  disable_INIT;
#if BX_CPU_LEVEL >= 5
  bx_bool  ignore_bad_msrs;
#endif
#if BX_CPU_LEVEL >= 6
  unsigned sse_ok;
#if BX_SUPPORT_AVX
  unsigned avx_ok;
#endif
#endif

  // for exceptions
  jmp_buf jmp_buf_env;
  Bit8u curr_exception;

  // Boundaries of current code page, based on EIP
  bx_address eipPageBias;
  Bit32u     eipPageWindowSize;
  const Bit8u *eipFetchPtr;
  bx_phy_address pAddrFetchPage; // Guest physical address of current instruction page

  // Boundaries of current stack page, based on ESP
  bx_address espPageBias;        // Linear address of current stack page
  Bit32u     espPageWindowSize;
  const Bit8u *espHostPtr;
  bx_phy_address pAddrStackPage; // Guest physical address of current stack page

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
  unsigned alignment_check_mask;
#endif

#if BX_DEBUGGER
  bx_phy_address watchpoint;
  Bit8u break_point;
  Bit8u magic_break;
  Bit8u stop_reason;
  bx_bool trace;
  bx_bool trace_reg;
  bx_bool trace_mem;
  bx_bool mode_break;
#if BX_SUPPORT_VMX
  bx_bool vmexit_break;
#endif
  unsigned show_flag;
  bx_guard_found_t guard_found;
#endif

  // for paging
  struct {
    bx_TLB_entry entry[BX_TLB_SIZE] BX_CPP_AlignN(16);
#if BX_CPU_LEVEL >= 5
    bx_bool split_large;
#endif
  } TLB;

#if BX_CPU_LEVEL >= 6
  struct {
    Bit64u entry[4];
  } PDPTR_CACHE;
#endif

  // An instruction cache.  Each entry should be exactly 32 bytes, and
  // this structure should be aligned on a 32-byte boundary to be friendly
  // with the host cache lines.
  bxICache_c iCache BX_CPP_AlignN(32);
  Bit32u fetchModeMask;

  struct {
    bx_address rm_addr;       // The address offset after resolution
    bx_phy_address paddress1; // physical address after translation of 1st len1 bytes of data
    bx_phy_address paddress2; // physical address after translation of 2nd len2 bytes of data
    Bit32u len1;              // Number of bytes in page 1
    Bit32u len2;              // Number of bytes in page 2
    bx_ptr_equiv_t pages;     // Number of pages access spans (1 or 2).  Also used
                              // for the case when a native host pointer is
                              // available for the R-M-W instructions.  The host
                              // pointer is stuffed here.  Since this field has
                              // to be checked anyways (and thus cached), if it
                              // is greated than 2 (the maximum possible for
                              // normal cases) it is a native pointer and is used
                              // for a direct write access.
  } address_xlation;

  void setEFlags(Bit32u val) BX_CPP_AttrRegparmN(1);

  BX_CPP_INLINE void setEFlagsOSZAPC(Bit32u flags32) {
    set_OF(1 & ((flags32) >> 11));
    set_SF(1 & ((flags32) >> 7));
    set_ZF(1 & ((flags32) >> 6));
    set_AF(1 & ((flags32) >> 4));
    set_PF(1 & ((flags32) >> 2));
    set_CF(1 & ((flags32) >> 0));
  }
 
  BX_CPP_INLINE void SET_FLAGS_OxxxxC(Bit32u new_of, Bit32u new_cf) {
    Bit32u temp_po = new_of ^ new_cf;
    bx_cpu.oszapc.auxbits &= ~(LF_MASK_PO | LF_MASK_CF);
    bx_cpu.oszapc.auxbits |=
                  (temp_po << LF_BIT_PO) | (new_cf << LF_BIT_CF);
  }
 
  BX_CPP_INLINE void ASSERT_FLAGS_OxxxxC() {
    SET_FLAGS_OxxxxC(1, 1);
  }
 
  // ZF
  BX_CPP_INLINE bx_bool getB_ZF(void) {
    return (0 == bx_cpu.oszapc.result);
  }

  BX_CPP_INLINE bx_bool get_ZF(void) { return getB_ZF(); }

  BX_CPP_INLINE void set_ZF(bx_bool val) {
    if (val) assert_ZF();
    else clear_ZF();
  }

  BX_CPP_INLINE void clear_ZF(void) {
    bx_cpu.oszapc.result |= (1 << 8);
  }

  BX_CPP_INLINE void assert_ZF(void) {
    // merge the sign bit into the Sign Delta

    bx_cpu.oszapc.auxbits ^=
      (((bx_cpu.oszapc.result >> BX_LF_SIGN_BIT) & 1) << LF_BIT_SD);

    // merge the parity bits into the Parity Delta Byte

    Bit32u temp_pdb = (255 & bx_cpu.oszapc.result);
    bx_cpu.oszapc.auxbits ^= (temp_pdb << LF_BIT_PDB);

    // now zero the .result value

    bx_cpu.oszapc.result = 0;
  }

  // SF
  BX_CPP_INLINE bx_bool getB_SF(void) {
    return ((bx_cpu.oszapc.result >> BX_LF_SIGN_BIT) ^
            (bx_cpu.oszapc.auxbits >> LF_BIT_SD)) & 1;
  }

  BX_CPP_INLINE bx_bool get_SF(void) { return getB_SF(); }

  BX_CPP_INLINE void set_SF(bx_bool val) {
    bx_bool temp_sf = getB_SF();

    bx_cpu.oszapc.auxbits ^= (temp_sf ^ val) << LF_BIT_SD;
  }

  BX_CPP_INLINE void clear_SF(void) {
    set_SF(0);
  }

  BX_CPP_INLINE void assert_SF(void) {
    set_SF(1);
  }

  // PF - bit 2 in EFLAGS, represented by lower 8 bits of oszapc.result
  BX_CPP_INLINE bx_bool getB_PF(void) {
    Bit32u temp = (255 & bx_cpu.oszapc.result);
    temp = temp ^ (255 & (bx_cpu.oszapc.auxbits >> LF_BIT_PDB));
    temp = (temp ^ (temp >> 4)) & 0x0F;
    return (0x9669U >> temp) & 1;
  }

  BX_CPP_INLINE bx_bool get_PF(void) { return getB_PF(); }

  BX_CPP_INLINE void set_PF(bx_bool val) {
    Bit32u temp_pdb = (255 & bx_cpu.oszapc.result) ^ (!val);

    bx_cpu.oszapc.auxbits &= ~(LF_MASK_PDB);
    bx_cpu.oszapc.auxbits |= (temp_pdb << LF_BIT_PDB);
  }

  BX_CPP_INLINE void clear_PF(void) {
    set_PF(0);
  }

  BX_CPP_INLINE void assert_PF(void) {
    set_PF(1);
  }

  // AF - bit 4 in EFLAGS, represented by bit LF_BIT_AF of oszapc.auxbits
  BX_CPP_INLINE bx_bool getB_AF(void) {
    return ((bx_cpu.oszapc.auxbits >> LF_BIT_AF) & 1);
  }

  BX_CPP_INLINE bx_bool get_AF(void) { return getB_AF(); }

  BX_CPP_INLINE void set_AF(bx_bool val) {
    bx_cpu.oszapc.auxbits &= ~(LF_MASK_AF);
    bx_cpu.oszapc.auxbits |= (val) << LF_BIT_AF;
  }
  
  BX_CPP_INLINE void clear_AF(void) {
    bx_cpu.oszapc.auxbits &= ~(LF_MASK_AF);
  }

  BX_CPP_INLINE void assert_AF(void) {
    bx_cpu.oszapc.auxbits |= (LF_MASK_AF);
  }

  // CF
  BX_CPP_INLINE bx_bool getB_CF(void) {
    return ((bx_cpu.oszapc.auxbits >> LF_BIT_CF) & 1);
  }

  BX_CPP_INLINE bx_bool get_CF(void) {
    return bx_cpu.oszapc.auxbits & (1U << LF_BIT_CF);
  }

  BX_CPP_INLINE void set_CF(bx_bool val) {
    bx_bool temp_of = getB_OF();
    SET_FLAGS_OxxxxC(temp_of, (val));
  }

  BX_CPP_INLINE void clear_CF(void) {
    bx_bool temp_of = getB_OF();
    SET_FLAGS_OxxxxC(temp_of, (0));
  }

  BX_CPP_INLINE void assert_CF(void) {
    bx_bool temp_of = getB_OF();
    SET_FLAGS_OxxxxC(temp_of, (1));
  }

  // OF
  BX_CPP_INLINE bx_bool getB_OF(void) {
    return ((bx_cpu.oszapc.auxbits + (1U << LF_BIT_PO)) >> LF_BIT_CF) & 1;
  }

  BX_CPP_INLINE bx_bool get_OF(void) {
    return (bx_cpu.oszapc.auxbits + (1U << LF_BIT_PO)) & (1U << LF_BIT_CF);
  }

  BX_CPP_INLINE void set_OF(bx_bool val) {
    bx_bool temp_cf = getB_CF();
    SET_FLAGS_OxxxxC((val), temp_cf);
  }
 
  BX_CPP_INLINE void clear_OF(void) {
    bx_bool temp_cf = getB_CF();
    SET_FLAGS_OxxxxC((0), temp_cf);
  }
 
  BX_CPP_INLINE void assert_OF(void) {
    bx_bool temp_cf = getB_CF();
    SET_FLAGS_OxxxxC((1), temp_cf);
  }

  // constructors & destructors...
  BX_CPU_C(unsigned id = 0);
 ~BX_CPU_C();

  void initialize(void);
  void after_restore_state(void);
  void register_state(void);

// <TAG-CLASS-CPU-START>
  // prototypes for CPU instructions...
  BX_INSF_TYPE PUSH16_Sw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE POP16_Sw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUSH32_Sw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE POP32_Sw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE DAA(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE DAS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AAA(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AAS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AAM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AAD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE PUSHAD32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUSHAD16(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE POPAD32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE POPAD16(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ARPL_EwGw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUSH_Id(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUSH_Iw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  void INSB32_YbDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void INSB16_YbDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void INSW32_YwDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void INSW16_YwDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void INSD32_YdDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void INSD16_YdDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void OUTSB32_DXXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void OUTSB16_DXXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void OUTSW32_DXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void OUTSW16_DXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void OUTSD32_DXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void OUTSD16_DXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE REP_INSB_YbDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE REP_INSW_YwDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE REP_INSD_YdDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE REP_OUTSB_DXXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE REP_OUTSW_DXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE REP_OUTSD_DXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE BOUND_GwMa(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BOUND_GdMa(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE TEST_EbGbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE TEST_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE TEST_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE TEST_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE TEST_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE TEST_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE XCHG_EbGbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XCHG_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XCHG_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE XCHG_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XCHG_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XCHG_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE MOV_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_GbEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_GwEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE MOV32_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV32_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE MOV32S_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV32S_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE MOV_EwSwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_EwSwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_SwEw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE LEA_GdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LEA_GwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE CBW(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CWD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CALL32_Ap(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CALL16_Ap(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUSHF_Fw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE POPF_Fw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUSHF_Fd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE POPF_Fd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SAHF(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LAHF(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE MOV_ALOd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_EAXOd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_AXOd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_OdAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_OdEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_OdAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // repeatable instructions
  BX_INSF_TYPE REP_MOVSB_XbYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE REP_MOVSW_XwYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE REP_MOVSD_XdYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE REP_CMPSB_XbYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE REP_CMPSW_XwYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE REP_CMPSD_XdYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE REP_STOSB_YbAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE REP_LODSB_ALXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE REP_SCASB_ALXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE REP_STOSW_YwAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE REP_LODSW_AXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE REP_SCASW_AXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE REP_STOSD_YdEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE REP_LODSD_EAXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE REP_SCASD_EAXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // qualified by address size
  void CMPSB16_XbYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void CMPSW16_XwYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void CMPSD16_XdYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void CMPSB32_XbYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void CMPSW32_XwYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void CMPSD32_XdYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  void SCASB16_ALXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void SCASW16_AXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void SCASD16_EAXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void SCASB32_ALXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void SCASW32_AXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void SCASD32_EAXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  void LODSB16_ALXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void LODSW16_AXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void LODSD16_EAXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void LODSB32_ALXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void LODSW32_AXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void LODSD32_EAXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  void STOSB16_YbAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void STOSW16_YwAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void STOSD16_YdEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void STOSB32_YbAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void STOSW32_YwAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void STOSD32_YdEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  void MOVSB16_XbYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void MOVSW16_XwYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void MOVSD16_XdYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void MOVSB32_XbYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void MOVSW32_XwYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void MOVSD32_XdYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE MOV_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ENTER16_IwIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ENTER32_IwIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LEAVE16(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LEAVE32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE INT1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE INT3(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE INT_Ib(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE INTO(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE IRET32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE IRET16(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE SALC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XLAT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE LOOPNE16_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LOOPE16_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LOOP16_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LOOPNE32_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LOOPE32_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LOOP32_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JCXZ_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JECXZ_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE IN_ALIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE IN_AXIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE IN_EAXIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OUT_IbAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OUT_IbAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OUT_IbEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CALL_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CALL_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JMP_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JMP_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JMP_Ap(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE IN_ALDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE IN_AXDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE IN_EAXDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OUT_DXAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OUT_DXAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OUT_DXEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE HLT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CLC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE STC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CLI(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE STI(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CLD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE STD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE LAR_GvEw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LSL_GvEw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CLTS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE INVD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE WBINVD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CLFLUSH(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE MOV_CR0Rd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_CR2Rd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_CR3Rd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_CR4Rd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_RdCR0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_RdCR2(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_RdCR3(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_RdCR4(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_DdRd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_RdDd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE JO_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JNO_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JB_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JNB_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JZ_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JNZ_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JBE_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JNBE_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JS_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JNS_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JP_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JNP_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JL_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JNL_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JLE_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JNLE_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE JO_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JNO_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JB_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JNB_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JZ_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JNZ_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JBE_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JNBE_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JS_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JNS_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JP_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JNP_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JL_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JNL_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JLE_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JNLE_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE SETO_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETNO_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETB_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETNB_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETZ_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETNZ_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETBE_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETNBE_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETS_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETNS_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETP_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETNP_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETL_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETNL_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETLE_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETNLE_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE SETO_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETNO_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETB_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETNB_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETZ_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETNZ_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETBE_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETNBE_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETS_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETNS_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETP_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETNP_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETL_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETNL_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETLE_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SETNLE_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE CPUID(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE SHRD_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHRD_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHLD_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHLD_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHRD_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHRD_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHLD_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHLD_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE BSF_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BSF_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BSR_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BSR_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE BT_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BT_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTS_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTS_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTR_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTR_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTC_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTC_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE BT_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BT_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTS_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTS_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTR_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTR_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTC_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTC_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE BT_EwIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BT_EdIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTS_EwIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTS_EdIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTR_EwIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTR_EdIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTC_EwIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTC_EdIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE BT_EwIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BT_EdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTS_EwIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTS_EdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTR_EwIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTR_EdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTC_EwIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BTC_EdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE LES_GwMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LDS_GwMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LSS_GwMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LFS_GwMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LGS_GwMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LES_GdMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LDS_GdMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LSS_GdMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LFS_GdMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LGS_GdMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE MOVZX_GwEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVZX_GdEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVZX_GdEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVSX_GwEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVSX_GdEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVSX_GdEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE MOVZX_GwEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVZX_GdEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVZX_GdEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVSX_GwEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVSX_GdEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVSX_GdEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE BSWAP_RX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BSWAP_ERX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ADD_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OR_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADC_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SBB_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AND_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SUB_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XOR_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMP_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ADD_GbEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OR_GbEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADC_GbEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SBB_GbEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AND_GbEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SUB_GbEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XOR_GbEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMP_GbEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ADD_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OR_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADC_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SBB_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AND_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SUB_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XOR_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMP_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ADD_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OR_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADC_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SBB_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AND_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SUB_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XOR_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMP_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ADD_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OR_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADC_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SBB_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AND_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SUB_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XOR_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMP_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ADD_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OR_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADC_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SBB_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AND_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SUB_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XOR_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMP_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ADD_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OR_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADC_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SBB_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AND_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SUB_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XOR_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMP_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ADD_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OR_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADC_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SBB_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AND_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SUB_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XOR_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMP_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ADD_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OR_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADC_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SBB_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AND_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SUB_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XOR_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMP_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ADD_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OR_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADC_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SBB_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AND_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SUB_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XOR_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMP_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ADD_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OR_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADC_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SBB_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AND_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SUB_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XOR_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMP_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ADD_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OR_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADC_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SBB_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AND_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SUB_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XOR_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMP_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ADD_GwEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OR_GwEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADC_GwEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SBB_GwEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AND_GwEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SUB_GwEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XOR_GwEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMP_GwEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ADD_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OR_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADC_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SBB_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AND_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SUB_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMP_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XOR_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ADD_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE OR_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADC_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SBB_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AND_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SUB_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMP_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XOR_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE NOT_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE NOT_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE NOT_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE NOT_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE NOT_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE NOT_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE NEG_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE NEG_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE NEG_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE NEG_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE NEG_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE NEG_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ROL_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ROR_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RCL_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RCR_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHL_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHR_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SAR_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ROL_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ROR_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RCL_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RCR_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHL_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHR_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SAR_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ROL_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ROR_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RCL_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RCR_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHL_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHR_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SAR_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ROL_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ROR_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RCL_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RCR_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHL_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHR_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SAR_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ROL_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ROR_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RCL_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RCR_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHL_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHR_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SAR_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ROL_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ROR_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RCL_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RCR_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHL_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHR_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SAR_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE TEST_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE TEST_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE TEST_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE TEST_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE TEST_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE TEST_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE IMUL_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE IMUL_GdEdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE MUL_ALEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE IMUL_ALEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE DIV_ALEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE IDIV_ALEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE MUL_EAXEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE IMUL_EAXEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE DIV_EAXEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE IDIV_EAXEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE INC_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE DEC_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE INC_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE INC_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE INC_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE DEC_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE DEC_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE DEC_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE CALL_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CALL_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE CALL32_Ep(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CALL16_Ep(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JMP32_Ep(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JMP16_Ep(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE JMP_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE JMP_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE SLDT_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE STR_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LLDT_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LTR_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VERR_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VERW_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE SGDT_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SIDT_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LGDT_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LIDT_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SMSW_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SMSW_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LMSW_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // LOAD methods
  BX_INSF_TYPE LOAD_Eb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LOAD_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LOAD_Ed(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE LOADU_Wdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LOAD_Wdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LOAD_Wss(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LOAD_Wsd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LOAD_Ww(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LOAD_Wb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_AVX
  BX_INSF_TYPE LOAD_Vector(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LOAD_VectorQ(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

#if BX_SUPPORT_FPU == 0	// if FPU is disabled
  BX_INSF_TYPE FPU_ESC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

  BX_INSF_TYPE FWAIT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

#if BX_SUPPORT_FPU
  // load/store
  BX_INSF_TYPE FLD_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FLD_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FLD_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FLD_EXTENDED_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FILD_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FILD_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FILD_QWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FBLD_PACKED_BCD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE FST_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FST_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FST_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FSTP_EXTENDED_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FIST_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FIST_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FISTP_QWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FBSTP_PACKED_BCD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE FISTTP16(bxInstruction_c *) BX_CPP_AttrRegparmN(1); // SSE3
  BX_INSF_TYPE FISTTP32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FISTTP64(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // control
  BX_INSF_TYPE FNINIT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FNCLEX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE FRSTOR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FNSAVE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FLDENV(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FNSTENV(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE FLDCW(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FNSTCW(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FNSTSW(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FNSTSW_AX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // const
  BX_INSF_TYPE FLD1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FLDL2T(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FLDL2E(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FLDPI(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FLDLG2(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FLDLN2(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FLDZ(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // add
  BX_INSF_TYPE FADD_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FADD_STi_ST0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FADD_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FADD_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FIADD_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FIADD_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // mul
  BX_INSF_TYPE FMUL_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FMUL_STi_ST0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FMUL_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FMUL_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FIMUL_WORD_INTEGER (bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FIMUL_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // sub
  BX_INSF_TYPE FSUB_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FSUBR_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FSUB_STi_ST0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FSUBR_STi_ST0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FSUB_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FSUBR_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FSUB_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FSUBR_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE FISUB_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FISUBR_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FISUB_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FISUBR_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // div
  BX_INSF_TYPE FDIV_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FDIVR_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FDIV_STi_ST0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FDIVR_STi_ST0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FDIV_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FDIVR_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FDIV_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FDIVR_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE FIDIV_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FIDIVR_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FIDIV_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FIDIVR_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // compare
  BX_INSF_TYPE FCOM_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FUCOM_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FCOMI_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FUCOMI_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FCOM_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FCOM_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FICOM_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FICOM_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE FCOMPP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FUCOMPP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE FCMOVB_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FCMOVE_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FCMOVBE_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FCMOVU_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FCMOVNB_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FCMOVNE_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FCMOVNBE_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FCMOVNU_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // misc
  BX_INSF_TYPE FXCH_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FNOP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FPLEGACY(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FCHS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FABS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FTST(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FXAM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FDECSTP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FINCSTP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FFREE_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FFREEP_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE F2XM1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FYL2X(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FPTAN(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FPATAN(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FXTRACT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FPREM1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FPREM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FYL2XP1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FSQRT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FSINCOS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FRNDINT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#undef FSCALE            // <sys/param.h> is #included on Mac OS X from bochs.h
  BX_INSF_TYPE FSCALE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FSIN(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FCOS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

  /* MMX */
  BX_INSF_TYPE PUNPCKLBW_PqQd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUNPCKLWD_PqQd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUNPCKLDQ_PqQd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PACKSSWB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PCMPGTB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PCMPGTW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PCMPGTD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PACKUSWB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUNPCKHBW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUNPCKHWD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUNPCKHDQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PACKSSDW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVD_PqEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVD_PqEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVQ_PqQqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVQ_PqQqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PCMPEQB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PCMPEQW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PCMPEQD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE EMMS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVD_EdPdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVD_EdPdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVQ_QqPqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRLW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRLD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRLQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMULLW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSUBUSB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSUBUSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PAND_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PADDUSB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PADDUSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PANDN_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRAW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRAD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMULHW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSUBSB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSUBSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE POR_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PADDSB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PADDSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PXOR_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSLLW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSLLD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSLLQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMADDWD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSUBB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSUBW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSUBD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PADDB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PADDW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PADDD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRLW_PqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRAW_PqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSLLW_PqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRLD_PqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRAD_PqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSLLD_PqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRLQ_PqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSLLQ_PqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* MMX */

#if BX_SUPPORT_3DNOW
  BX_INSF_TYPE PFPNACC_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PI2FW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PI2FD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PF2IW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PF2ID_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PFNACC_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PFCMPGE_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PFMIN_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PFRCP_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PFRSQRT_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PFSUB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PFADD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PFCMPGT_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PFMAX_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PFRCPIT1_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PFRSQIT1_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PFSUBR_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PFACC_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PFCMPEQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PFMUL_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PFRCPIT2_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMULHRW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSWAPD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

  BX_INSF_TYPE SYSCALL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SYSRET(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  /* SSE */
  BX_INSF_TYPE FXSAVE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE FXRSTOR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LDMXCSR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE STMXCSR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PREFETCH(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSE */

  /* SSE */
  BX_INSF_TYPE ANDPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ORPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XORPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ANDNPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVUPS_VpsWpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVUPS_WpsVpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVSS_VssWssM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVSS_WssVssM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVSD_WsdVsdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVHLPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVLPS_VpsMq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVLHPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVHPS_VpsMq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVHPS_MqVps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVAPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVAPS_VpsWpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVAPS_WpsVpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTPI2PS_VpsQqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTPI2PS_VpsQqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTSI2SS_VssEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTTPS2PI_PqWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTTSS2SI_GdWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTPS2PI_PqWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTSS2SI_GdWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE UCOMISS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE COMISS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVMSKPS_GdVRps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SQRTPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SQRTSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RSQRTPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RSQRTSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RCPPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RCPSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADDPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADDSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MULPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MULSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SUBPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SUBSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MINPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MINSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE DIVPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE DIVSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MAXPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MAXSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSHUFW_PqQqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSHUFLW_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMPPS_VpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMPSS_VssWssIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PINSRW_PqEwIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PEXTRW_GdPqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHUFPS_VpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMOVMSKB_GdPRq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMINUB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMAXUB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PAVGB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PAVGW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMULHUW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMINSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMAXSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSADBW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MASKMOVQ_PqPRq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSE */

  /* SSE2 */
  BX_INSF_TYPE MOVSD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTPI2PD_VpdQqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTPI2PD_VpdQqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTSI2SD_VsdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTTPD2PI_PqWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTTSD2SI_GdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTPD2PI_PqWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTSD2SI_GdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE UCOMISD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE COMISD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVMSKPD_GdVRpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SQRTPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SQRTSD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADDPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADDSD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MULPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MULSD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SUBPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SUBSD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTPS2PD_VpdWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTPD2PS_VpsWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTSD2SS_VssWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTSS2SD_VsdWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTDQ2PS_VpsWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTPS2DQ_VdqWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTTPS2DQ_VdqWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MINPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MINSD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE DIVPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE DIVSD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MAXPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MAXSD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUNPCKLBW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUNPCKLWD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE UNPCKLPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PACKSSWB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PCMPGTB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PCMPGTW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PCMPGTD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PACKUSWB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUNPCKHBW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUNPCKHWD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE UNPCKHPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PACKSSDW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUNPCKLQDQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUNPCKHQDQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVD_VdqEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSHUFD_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSHUFHW_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PCMPEQB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PCMPEQW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PCMPEQD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVD_EdVdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVQ_VqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVQ_VqWqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMPPD_VpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMPSD_VsdWsdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PINSRW_VdqHdqEwIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PEXTRW_GdUdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHUFPD_VpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRLW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRLD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRLQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PADDQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PADDQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMULLW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVDQ2Q_PqVRq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVQ2DQ_VdqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMOVMSKB_GdUdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSUBUSB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSUBUSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMINUB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PADDUSB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PADDUSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMAXUB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PAVGB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRAW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRAD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PAVGW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMULHUW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMULHW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTTPD2DQ_VqWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTPD2DQ_VqWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CVTDQ2PD_VpdWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSUBSB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSUBSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMINSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PADDSB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PADDSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMAXSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSLLW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSLLD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSLLQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMULUDQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMULUDQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMADDWD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSADBW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MASKMOVDQU_VdqUdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSUBB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSUBW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSUBD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSUBQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSUBQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PADDB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PADDW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PADDD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRLW_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRAW_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSLLW_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRLD_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRAD_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSLLD_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRLQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSRLDQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSLLQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSLLDQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSE2 */

  /* SSE3 */
  BX_INSF_TYPE MOVDDUP_VpdWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVSLDUP_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVSHDUP_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE HADDPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE HADDPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE HSUBPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE HSUBPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADDSUBPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADDSUBPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSE3 */

#if BX_CPU_LEVEL >= 6
  /* SSSE3 */
  BX_INSF_TYPE PSHUFB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PHADDW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PHADDD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PHADDSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMADDUBSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PHSUBSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PHSUBW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PHSUBD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSIGNB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSIGNW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSIGND_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMULHRSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PABSB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PABSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PABSD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PALIGNR_PqQqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE PSHUFB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PHADDW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PHADDD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PHADDSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMADDUBSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PHSUBSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PHSUBW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PHSUBD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSIGNB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSIGNW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PSIGND_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMULHRSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PABSB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PABSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PABSD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PALIGNR_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSSE3 */

  /* SSE4.1 */
  BX_INSF_TYPE PBLENDVB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLENDVPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLENDVPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PTEST_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMULDQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PCMPEQQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PACKUSDW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMOVSXBW_VdqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMOVSXBD_VdqWdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMOVSXBQ_VdqWwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMOVSXWD_VdqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMOVSXWQ_VdqWdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMOVSXDQ_VdqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMOVZXBW_VdqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMOVZXBD_VdqWdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMOVZXBQ_VdqWwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMOVZXWD_VdqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMOVZXWQ_VdqWdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMOVZXDQ_VdqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMINSB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMINSD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMINUW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMINUD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMAXSB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMAXSD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMAXUW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMAXUD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PMULLD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PHMINPOSUW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ROUNDPS_VpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ROUNDPD_VpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ROUNDSS_VssWssIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ROUNDSD_VsdWsdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLENDPS_VpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLENDPD_VpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PBLENDW_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PEXTRB_EbdVdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PEXTRB_EbdVdqIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PEXTRW_EwdVdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PEXTRW_EwdVdqIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PEXTRD_EdVdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PEXTRD_EdVdqIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE EXTRACTPS_EdVpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE EXTRACTPS_EdVpsIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PINSRB_VdqHdqEbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PINSRB_VdqHdqEbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE INSERTPS_VpsHpsWssIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PINSRD_VdqHdqEdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PINSRD_VdqHdqEdIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE DPPS_VpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE DPPD_VpdHpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MPSADBW_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSE4.1 */

  /* SSE4.2 */
  BX_INSF_TYPE CRC32_GdEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CRC32_GdEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CRC32_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE PCMPGTQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PCMPESTRM_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PCMPESTRI_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PCMPISTRM_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PCMPISTRI_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSE4.2 */

  /* MOVBE Intel Atom(R) instruction */
  BX_INSF_TYPE MOVBE_GwMw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVBE_GdMd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVBE_MwGw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOVBE_MdGd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* MOVBE Intel Atom(R) instruction */
#endif

  /* XSAVE/XRSTOR extensions */
  BX_INSF_TYPE XSAVE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XRSTOR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XGETBV(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XSETBV(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* XSAVE/XRSTOR extensions */

#if BX_CPU_LEVEL >= 6
  /* AES instructions */
  BX_INSF_TYPE AESIMC_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AESENC_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AESENCLAST_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AESDEC_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AESDECLAST_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE AESKEYGENASSIST_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PCLMULQDQ_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* AES instructions */
#endif

  /* VMX instructions */
  BX_INSF_TYPE VMXON(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMXOFF(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMCALL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMLAUNCH(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMCLEAR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMPTRLD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMPTRST(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMREAD_EdGd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMWRITE_GdEd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMFUNC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* VMX instructions */

  /* SVM instructions */
  BX_INSF_TYPE VMRUN(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMMCALL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMLOAD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMSAVE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SKINIT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CLGI(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE STGI(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE INVLPGA(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SVM instructions */

  /* SMX instructions */
  BX_INSF_TYPE GETSEC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SMX instructions */

#if BX_CPU_LEVEL >= 6
  /* VMXx2 */
  BX_INSF_TYPE INVEPT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE INVVPID(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* VMXx2 */

  BX_INSF_TYPE INVPCID(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

#if BX_SUPPORT_AVX
  /* AVX */
  BX_INSF_TYPE VZEROUPPER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VZEROALL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE VMOVSS_VssHpsWssR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMOVSD_VsdHpdWsdR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMOVAPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMOVAPS_VpsWpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMOVUPS_VpsWpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMOVAPS_WpsVpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMOVUPS_WpsVpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMOVLPD_VpdHpdMq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMOVHPD_VpdHpdMq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMOVLHPS_VpsHpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMOVHLPS_VpsHpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMOVSHDUP_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMOVSLDUP_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMOVDDUP_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VUNPCKLPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VUNPCKHPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VUNPCKLPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VUNPCKHPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMOVMSKPS_GdVRps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMOVMSKPD_GdVRpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMOVMSKB_GdUdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VSQRTPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VSQRTPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VSQRTSS_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VSQRTSD_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VHADDPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VHADDPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VHSUBPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VHSUBPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VADDPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VADDPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VADDSS_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VADDSD_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMULPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMULPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMULSS_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMULSD_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VSUBPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VSUBPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VSUBSS_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VSUBSD_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VCVTSS2SD_VsdWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VCVTSD2SS_VssWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VCVTDQ2PS_VpsWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VCVTPS2DQ_VdqWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VCVTTPS2DQ_VdqWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VCVTPS2PD_VpdWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VCVTPD2PS_VpsWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VCVTPD2DQ_VqWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VCVTDQ2PD_VpdWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VCVTTPD2DQ_VqWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VCVTSI2SD_VsdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VCVTSI2SS_VssEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VCVTSI2SD_VsdEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VCVTSI2SS_VssEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMINPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMINPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMINSS_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMINSD_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VDIVPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VDIVPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VDIVSS_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VDIVSD_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMAXPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMAXPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMAXSS_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMAXSD_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VCMPPS_VpsHpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VCMPSS_VssHpsWssIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VCMPPD_VpdHpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VCMPSD_VsdHpdWsdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VADDSUBPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VADDSUBPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VROUNDPS_VpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VROUNDPD_VpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VROUNDSS_VssHpsWssIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VROUNDSD_VsdHpdWsdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VDPPS_VpsHpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VRSQRTPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VRSQRTSS_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VRCPPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VRCPSS_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VSHUFPS_VpsHpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VSHUFPD_VpdHpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VBLENDPS_VpsHpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VBLENDPD_VpdHpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPBLENDVB_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPTEST_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VTESTPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VTESTPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VANDPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VANDNPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VORPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VXORPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VBROADCASTF128_VdqMdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VBLENDVPS_VpsHpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VBLENDVPD_VpdHpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VINSERTF128_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VEXTRACTF128_WdqVdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VEXTRACTF128_WdqVdqIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPERMILPS_VpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPERMILPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPERMILPD_VpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPERMILPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPERM2F128_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMASKMOVPS_VpsHpsMps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMASKMOVPD_VpdHpdMpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMASKMOVPS_MpsHpsVps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMASKMOVPD_MpdHpdVpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE VCVTPH2PS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VCVTPS2PH_WpsVpsIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* AVX */

  /* AVX2 */
  BX_INSF_TYPE VPCMPEQB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPCMPEQW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPCMPEQD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPCMPEQQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPCMPGTB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPCMPGTW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPCMPGTD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPCMPGTQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMINSB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMINSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMINSD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMINUB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMINUW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMINUD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMAXSB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMAXSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMAXSD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMAXUB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMAXUW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMAXUD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSIGNB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSIGNW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSIGND_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPADDB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPADDW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPADDD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPADDQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSUBB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSUBW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSUBD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSUBQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPABSB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPABSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPABSD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSUBSB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSUBSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSUBUSB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSUBUSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPADDSB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPADDSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPADDUSB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPADDUSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPAVGB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPAVGW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHADDW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHADDD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHADDSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHSUBW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHSUBD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHSUBSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSHUFHW_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSHUFLW_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPACKUSWB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPACKSSWB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPACKUSDW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPACKSSDW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPUNPCKLBW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPUNPCKHBW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPUNPCKLWD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPUNPCKHWD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMULLD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMULLW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMULHW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMULHUW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMULDQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMULUDQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMULHRSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMADDUBSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMADDWD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VMPSADBW_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPBLENDW_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSADBW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSHUFB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSRLW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSRLD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSRLQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSLLW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSLLD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSLLQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSRAW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSRAD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSRLW_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSRLD_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSRLQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSLLW_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSLLD_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSLLQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSRAW_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSRAD_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSRLDQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSLLDQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPALIGNR_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE VPMOVSXBW256_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMOVSXBD256_VdqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMOVSXBQ256_VdqWdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMOVSXWD256_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMOVSXWQ256_VdqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMOVSXDQ256_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMOVZXBW256_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMOVZXBD256_VdqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMOVZXBQ256_VdqWdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMOVZXWD256_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMOVZXWQ256_VdqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMOVZXDQ256_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE VPERMD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPERMQ_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSRAVD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSLLVD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSLLVQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSRLVD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSRLVQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE VPBROADCASTB_VdqWb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPBROADCASTW_VdqWw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPBROADCASTD_VdqWd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPBROADCASTQ_VdqWq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE VGATHERDPS_VpsHps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VGATHERQPS_VpsHps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VGATHERDPD_VpdHpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VGATHERQPD_VpdHpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* AVX2 */

  /* AVX2 FMA */
  BX_INSF_TYPE VFMADDPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFMADDPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFMADDSD_VpdHsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFMADDSS_VpsHssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFMADDSUBPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFMADDSUBPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFMSUBADDPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFMSUBADDPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFMSUBPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFMSUBPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFMSUBSD_VpdHsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFMSUBSS_VpsHssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFNMADDPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFNMADDPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFNMADDSD_VpdHsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFNMADDSS_VpsHssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFNMSUBPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFNMSUBPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFNMSUBSD_VpdHsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFNMSUBSS_VpsHssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* AVX2 FMA */

  /* BMI */
  BX_INSF_TYPE ANDN_GdBdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MULX_GdBdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLSI_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLSMSK_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLSR_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RORX_GdEdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHLX_GdEdBdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHRX_GdEdBdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SARX_GdEdBdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BEXTR_GdEdBdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BZHI_GdEdBdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PEXT_GdEdBdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PDEP_GdEdBdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ANDN_GqBqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MULX_GqBqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLSI_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLSMSK_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLSR_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RORX_GqEqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHLX_GqEqBqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SHRX_GqEqBqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SARX_GqEqBqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BEXTR_GqEqBqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BZHI_GqEqBqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PEXT_GqEqBqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PDEP_GqEqBqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* BMI */

  /* FMA4 specific handlers (AMD) */
  BX_INSF_TYPE VFMADDSS_VssHssWssVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFMADDSD_VsdHsdWsdVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFMSUBSS_VssHssWssVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFMSUBSD_VsdHsdWsdVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFNMADDSS_VssHssWssVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFNMADDSD_VsdHsdWsdVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFNMSUBSS_VssHssWssVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFNMSUBSD_VsdHsdWsdVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* FMA4 specific handlers (AMD) */

  /* XOP (AMD) */
  BX_INSF_TYPE VPCMOV_VdqHdqWdqVIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPPERM_VdqHdqWdqVIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSHAB_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSHAW_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSHAD_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSHAQ_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPROTB_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPROTW_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPROTD_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPROTQ_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSHLB_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSHLW_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSHLD_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPSHLQ_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMACSSWW_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMACSSWD_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMACSSDQL_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMACSSDD_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMACSSDQH_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMACSWW_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMACSWD_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMACSDQL_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMACSDD_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMACSDQH_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMADCSSWD_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPMADCSWD_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPROTB_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPROTW_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPROTD_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPROTQ_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPCOMB_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPCOMW_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPCOMD_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPCOMQ_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPCOMUB_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPCOMUW_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPCOMUD_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPCOMUQ_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFRCZPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFRCZPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFRCZSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VFRCZSD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHADDBW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHADDBD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHADDBQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHADDWD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHADDWQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHADDDQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHADDUBW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHADDUBD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHADDUBQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHADDUWD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHADDUWQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHADDUDQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHSUBBW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHSUBWD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPHSUBDQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPERMIL2PS_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE VPERMIL2PD_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* XOP (AMD) */

  /* TBM (AMD) */
  BX_INSF_TYPE BEXTR_GdEdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLCFILL_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLCI_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLCIC_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLCMSK_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLCS_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLSFILL_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLSIC_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE T1MSKC_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE TZMSK_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE BEXTR_GqEqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLCFILL_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLCI_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLCIC_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLCMSK_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLCS_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLSFILL_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BLSIC_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE T1MSKC_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE TZMSK_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* TBM (AMD) */
#endif

  BX_INSF_TYPE LZCNT_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE LZCNT_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  /* BMI - TZCNT */
  BX_INSF_TYPE TZCNT_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE TZCNT_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* BMI - TZCNT */

  /* SSE4A */
  BX_INSF_TYPE EXTRQ_UdqIbIb(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE EXTRQ_VdqUq(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE INSERTQ_VdqUqIbIb(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE INSERTQ_VdqUdq(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  /* SSE4A */

  BX_INSF_TYPE CMPXCHG8B(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RETnear32_Iw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RETnear32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RETnear16_Iw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RETnear16(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RETfar32_Iw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RETfar16_Iw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE XADD_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XADD_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XADD_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE XADD_EbGbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XADD_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XADD_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE CMOVO_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVNO_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVB_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVNB_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVZ_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVNZ_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVBE_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVNBE_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVS_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVNS_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVP_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVNP_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVL_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVNL_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVLE_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVNLE_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE CMOVO_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVNO_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVB_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVNB_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVZ_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVNZ_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVBE_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVNBE_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVS_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVNS_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVP_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVNP_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVL_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVNL_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVLE_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMOVNLE_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE CWDE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CDQ(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE CMPXCHG_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMPXCHG_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMPXCHG_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE CMPXCHG_EbGbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMPXCHG_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE CMPXCHG_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE MUL_AXEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE IMUL_AXEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE DIV_AXEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE IDIV_AXEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE IMUL_GwEwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE IMUL_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE NOP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PAUSE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_RLIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_RHIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_RXIw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MOV_ERXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE INC_RX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE DEC_RX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE INC_ERX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE DEC_ERX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XCHG_RXAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE XCHG_ERXEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE PUSH_RX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUSH_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUSH_ERX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE PUSH_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE POP_RX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE POP_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE POP_ERX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE POP_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE POPCNT_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE POPCNT_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE ADCX_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE ADOX_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE RDTSCP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE INVLPG(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RSM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE WRMSR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RDTSC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RDPMC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE RDMSR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SYSENTER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE SYSEXIT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE MONITOR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE MWAIT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_INSF_TYPE UndefinedOpcode(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BxError(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS
  BX_INSF_TYPE BxEndTrace(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
#if BX_CPU_LEVEL >= 6
  BX_INSF_TYPE BxNoSSE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_INSF_TYPE BxNoAVX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

  bx_address BxResolve16BaseIndex(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  bx_address BxResolve32Base(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  bx_address BxResolve32BaseIndex(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_AVX
  bx_address BxResolveGatherD(bxInstruction_c *, unsigned) BX_CPP_AttrRegparmN(2);
  bx_address BxResolveGatherQ(bxInstruction_c *, unsigned) BX_CPP_AttrRegparmN(2);
#endif
// <TAG-CLASS-CPU-END>

#if BX_DEBUGGER || BX_DISASM || BX_INSTRUMENTATION || BX_GDBSTUB
  bx_bool  dbg_xlate_linear2phy(bx_address linear, bx_phy_address *phy, bx_bool verbose = 0);
#if BX_SUPPORT_VMX >= 2
  bx_bool dbg_translate_guest_physical(bx_phy_address guest_paddr, bx_phy_address *phy, bx_bool verbose = 0);
#endif
#endif
#if BX_LARGE_RAMFILE
  bx_bool check_addr_in_tlb_buffers(const Bit8u *addr, const Bit8u *end);
#endif
  void atexit(void);

  // now for some ancillary functions...
  void cpu_loop(void);
  bx_bool handleAsyncEvent(void);
  bx_bool handleWaitForEvent(void);
  bx_bool interrupts_enabled(void);
  void InterruptAcknowledge(void);

  int fetchDecode32(const Bit8u *fetchPtr, bxInstruction_c *i, unsigned remainingInPage) BX_CPP_AttrRegparmN(3);

  void boundaryFetch(const Bit8u *fetchPtr, unsigned remainingInPage, bxInstruction_c *);
  bxICacheEntry_c *serveICacheMiss(bxICacheEntry_c *entry, Bit32u eipBiased, bx_phy_address pAddr);
  bxICacheEntry_c* getICacheEntry(void);
  bx_bool mergeTraces(bxICacheEntry_c *entry, bxInstruction_c *i, bx_phy_address pAddr);
#if BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS
  BX_INSF_TYPE linkTrace(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
#endif
  void prefetch(void);
  void updateFetchModeMask(void);
  BX_CPP_INLINE void invalidate_prefetch_q(void)
  {
    bx_cpu.eipPageWindowSize = 0;
  }

  BX_CPP_INLINE void invalidate_stack_cache(void)
  {
    bx_cpu.espPageWindowSize = 0;
  }

  bx_bool write_virtual_checks(bx_segment_reg_t *seg, Bit32u offset, unsigned len) BX_CPP_AttrRegparmN(3);
  bx_bool read_virtual_checks(bx_segment_reg_t *seg, Bit32u offset, unsigned len) BX_CPP_AttrRegparmN(3);
  bx_bool execute_virtual_checks(bx_segment_reg_t *seg, Bit32u offset, unsigned len) BX_CPP_AttrRegparmN(3);

  Bit8u read_virtual_byte_32(unsigned s, Bit32u offset) BX_CPP_AttrRegparmN(2);
  Bit16u read_virtual_word_32(unsigned s, Bit32u offset) BX_CPP_AttrRegparmN(2);
  Bit32u read_virtual_dword_32(unsigned s, Bit32u offset) BX_CPP_AttrRegparmN(2);
  Bit64u read_virtual_qword_32(unsigned s, Bit32u offset) BX_CPP_AttrRegparmN(2);

#if BX_CPU_LEVEL >= 6
  void read_virtual_dqword_32(unsigned seg, Bit32u off, BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
  void read_virtual_dqword_aligned_32(unsigned seg, Bit32u off, BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
#if BX_SUPPORT_AVX
  void read_virtual_dword_vector_32(unsigned seg, Bit32u off, unsigned elements, BxPackedAvxRegister *data);
  void read_virtual_dword_vector_aligned_32(unsigned seg, Bit32u off, unsigned elements, BxPackedAvxRegister *data);
#endif
#endif

#if BX_CPU_LEVEL >= 6
  void write_virtual_dqword_32(unsigned seg, Bit32u offset, const BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
  void write_virtual_dqword_aligned_32(unsigned seg, Bit32u offset, const BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
#if BX_SUPPORT_AVX
  void write_virtual_dword_vector_32(unsigned seg, Bit32u off, unsigned elements, const BxPackedAvxRegister *data);
  void write_virtual_dword_vector_aligned_32(unsigned seg, Bit32u off, unsigned elements, const BxPackedAvxRegister *data);
#endif
#endif

  void write_virtual_byte_32(unsigned s, Bit32u offset, Bit8u data) BX_CPP_AttrRegparmN(3);
  void write_virtual_word_32(unsigned s, Bit32u offset, Bit16u data) BX_CPP_AttrRegparmN(3);
  void write_virtual_dword_32(unsigned s, Bit32u offset, Bit32u data) BX_CPP_AttrRegparmN(3);
  void write_virtual_qword_32(unsigned s, Bit32u offset, Bit64u data) BX_CPP_AttrRegparmN(3);

  Bit8u read_RMW_virtual_byte_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);
  Bit16u read_RMW_virtual_word_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);
  Bit32u read_RMW_virtual_dword_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);
  Bit64u read_RMW_virtual_qword_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);

  void write_RMW_virtual_byte(Bit8u val8) BX_CPP_AttrRegparmN(1);
  void write_RMW_virtual_word(Bit16u val16) BX_CPP_AttrRegparmN(1);
  void write_RMW_virtual_dword(Bit32u val32) BX_CPP_AttrRegparmN(1);
  void write_RMW_virtual_qword(Bit64u val64) BX_CPP_AttrRegparmN(1);

  // write of word/dword to new stack could happen only in legacy mode
  void write_new_stack_word_32(bx_segment_reg_t *seg, Bit32u offset, unsigned curr_pl, Bit16u data);
  void write_new_stack_dword_32(bx_segment_reg_t *seg, Bit32u offset, unsigned curr_pl, Bit32u data);
  void write_new_stack_qword_32(bx_segment_reg_t *seg, Bit32u offset, unsigned curr_pl, Bit64u data);

// write
#define write_virtual_byte(seg, offset, data)    				write_virtual_byte_32(seg, offset, data)
#define write_virtual_word(seg, offset, data)    				write_virtual_word_32(seg, offset, data)
#define write_virtual_dword(seg, offset, data)   				write_virtual_dword_32(seg, offset, data)
#define write_virtual_qword(seg, offset, data)   				write_virtual_qword_32(seg, offset, data)
#define write_virtual_dqword(seg, offset, data)  				write_virtual_dqword_32(seg, offset, (const BxPackedXmmRegister*)(data))
#define write_virtual_dqword_aligned(seg, offset, data) write_virtual_dqword_aligned_32(seg, offset, (const BxPackedXmmRegister*)(data))

#if BX_SUPPORT_AVX

#define write_virtual_dword_vector(seg, offset, elements, data) \
  write_virtual_dword_vector_32(seg, offset, elements, (const BxPackedAvxRegister*)(data))
#define write_virtual_dword_vector_aligned(seg, offset, elements, data) \
  write_virtual_dword_vector_aligned_32(seg, offset, elements, (const BxPackedAvxRegister*)(data))

#endif

// read
#define read_virtual_byte(seg, offset)  \
  read_virtual_byte_32(seg, offset)
#define read_virtual_word(seg, offset)  \
  read_virtual_word_32(seg, offset)
#define read_virtual_dword(seg, offset) \
  read_virtual_dword_32(seg, offset)
#define read_virtual_qword(seg, offset) \
  read_virtual_qword_32(seg, offset)
#define read_virtual_dqword(seg, offset, data) \
  read_virtual_dqword_32(seg, offset, (BxPackedXmmRegister*)(data))
#define read_virtual_dqword_aligned(seg, offset, data) \
  read_virtual_dqword_aligned_32(seg, offset, (BxPackedXmmRegister*)(data))

#if BX_SUPPORT_AVX

#define read_virtual_dword_vector(seg, offset, elements, data) \
  read_virtual_dword_vector_32(seg, offset, elements, (BxPackedAvxRegister*)(data))
#define read_virtual_dword_vector_aligned(seg, offset, elements, data) \
  read_virtual_dword_vector_aligned_32(seg, offset, elements, (BxPackedAvxRegister*)(data))

#endif

// RMW
#define read_RMW_virtual_byte(seg, offset)    read_RMW_virtual_byte_32(seg, offset)
#define read_RMW_virtual_word(seg, offset)    read_RMW_virtual_word_32(seg, offset)
#define read_RMW_virtual_dword(seg, offset)   read_RMW_virtual_dword_32(seg, offset)
#define read_RMW_virtual_qword(seg, offset)   read_RMW_virtual_qword_32(seg, offset)


  void stack_write_byte(bx_address offset, Bit8u data) BX_CPP_AttrRegparmN(2);
  void stack_write_word(bx_address offset, Bit16u data) BX_CPP_AttrRegparmN(2);
  void stack_write_dword(bx_address offset, Bit32u data) BX_CPP_AttrRegparmN(2);
  void stack_write_qword(bx_address offset, Bit64u data) BX_CPP_AttrRegparmN(2);

  Bit8u stack_read_byte(bx_address offset) BX_CPP_AttrRegparmN(1);
  Bit16u stack_read_word(bx_address offset) BX_CPP_AttrRegparmN(1);
  Bit32u stack_read_dword(bx_address offset) BX_CPP_AttrRegparmN(1);
  Bit64u stack_read_qword(bx_address offset) BX_CPP_AttrRegparmN(1);

  void stackPrefetch(bx_address offset, unsigned len) BX_CPP_AttrRegparmN(2);

  Bit8u  system_read_byte(bx_address laddr) BX_CPP_AttrRegparmN(1);
  Bit16u system_read_word(bx_address laddr) BX_CPP_AttrRegparmN(1);
  Bit32u system_read_dword(bx_address laddr) BX_CPP_AttrRegparmN(1);
  Bit64u system_read_qword(bx_address laddr) BX_CPP_AttrRegparmN(1);

  void system_write_byte(bx_address laddr, Bit8u data) BX_CPP_AttrRegparmN(2);
  void system_write_word(bx_address laddr, Bit16u data) BX_CPP_AttrRegparmN(2);
  void system_write_dword(bx_address laddr, Bit32u data) BX_CPP_AttrRegparmN(2);

  Bit8u* v2h_read_byte(bx_address laddr, bx_bool user) BX_CPP_AttrRegparmN(2);
  Bit8u* v2h_write_byte(bx_address laddr, bx_bool user) BX_CPP_AttrRegparmN(2);

  void branch_near16(Bit16u new_IP) BX_CPP_AttrRegparmN(1);
  void branch_near32(Bit32u new_EIP) BX_CPP_AttrRegparmN(1);

  void branch_far32(bx_selector_t *selector,
       bx_descriptor_t *descriptor, Bit32u eip, Bit8u cpl);
  void branch_far64(bx_selector_t *selector,
       bx_descriptor_t *descriptor, bx_address rip, Bit8u cpl);

#if BX_SUPPORT_REPEAT_SPEEDUPS
  Bit32u FastRepMOVSB(bxInstruction_c *i, unsigned srcSeg, bx_address srcOff,
       unsigned dstSeg, bx_address dstOff, Bit32u  byteCount);
  Bit32u FastRepMOVSW(bxInstruction_c *i, unsigned srcSeg, bx_address srcOff,
       unsigned dstSeg, bx_address dstOff, Bit32u  wordCount);
  Bit32u FastRepMOVSD(bxInstruction_c *i, unsigned srcSeg, bx_address srcOff,
       unsigned dstSeg, bx_address dstOff, Bit32u dwordCount);

  Bit32u FastRepSTOSB(bxInstruction_c *i, unsigned dstSeg, bx_address dstOff,
       Bit8u  val, Bit32u  byteCount);
  Bit32u FastRepSTOSW(bxInstruction_c *i, unsigned dstSeg, bx_address dstOff,
       Bit16u val, Bit32u  wordCount);
  Bit32u FastRepSTOSD(bxInstruction_c *i, unsigned dstSeg, bx_address dstOff,
       Bit32u val, Bit32u dwordCount);

  Bit32u FastRepINSW(bxInstruction_c *i, bx_address dstOff,
       Bit16u port, Bit32u wordCount);
  Bit32u FastRepOUTSW(bxInstruction_c *i, unsigned srcSeg, bx_address srcOff,
       Bit16u port, Bit32u wordCount);
#endif

  void repeat(bxInstruction_c *i, BxRepIterationPtr_tR execute) BX_CPP_AttrRegparmN(2);
  void repeat_ZF(bxInstruction_c *i, BxRepIterationPtr_tR execute) BX_CPP_AttrRegparmN(2);

  // linear address for access_linear expected to be canonical !
  void access_read_linear(bx_address laddr, unsigned len, unsigned curr_pl, unsigned rw, void *data);
  void access_write_linear(bx_address laddr, unsigned len, unsigned curr_pl, void *data);
  void page_fault(unsigned fault, bx_address laddr, unsigned user, unsigned rw);

  void access_read_physical(bx_phy_address paddr, unsigned len, void *data);
  void access_write_physical(bx_phy_address paddr, unsigned len, void *data);

  bx_hostpageaddr_t getHostMemAddr(bx_phy_address addr, unsigned rw);

  // linear address for translate_linear expected to be canonical !
  bx_phy_address translate_linear(bx_address laddr, unsigned user, unsigned rw);
  bx_phy_address translate_linear_legacy(bx_address laddr, Bit32u &lpf_mask, Bit32u &combined_access, unsigned user, unsigned rw);
  void update_access_dirty(bx_phy_address *entry_addr, Bit32u *entry, unsigned leaf, unsigned write);
#if BX_CPU_LEVEL >= 6
  bx_phy_address translate_linear_load_PDPTR(bx_address laddr, unsigned user, unsigned rw);
  bx_phy_address translate_linear_PAE(bx_address laddr, Bit32u &lpf_mask, Bit32u &combined_access, unsigned user, unsigned rw);
  int check_entry_PAE(const char *s, Bit64u entry, Bit64u reserved, unsigned rw, bx_bool *nx_fault);
  void update_access_dirty_PAE(bx_phy_address *entry_addr, Bit64u *entry, unsigned max_level, unsigned leaf, unsigned write);
#endif

#if BX_SUPPORT_VMX >= 2
  bx_phy_address translate_guest_physical(bx_phy_address guest_paddr, bx_address guest_laddr, bx_bool guest_laddr_valid, bx_bool is_page_walk, unsigned rw);
  void update_ept_access_dirty(bx_phy_address *entry_addr, Bit64u *entry, unsigned leaf, unsigned write);
  bx_bool is_eptptr_valid(Bit64u eptptr);
#endif
#if BX_SUPPORT_SVM
  void nested_page_fault(unsigned fault, bx_phy_address guest_paddr, unsigned rw, unsigned is_page_walk);
  bx_phy_address nested_walk_long_mode(bx_phy_address guest_paddr, unsigned rw, bx_bool is_page_walk);
  bx_phy_address nested_walk_PAE(bx_phy_address guest_paddr, unsigned rw, bx_bool is_page_walk);
  bx_phy_address nested_walk_legacy(bx_phy_address guest_paddr, unsigned rw, bx_bool is_page_walk);
  bx_phy_address nested_walk(bx_phy_address guest_paddr, unsigned rw, bx_bool is_page_walk);
#endif

#if BX_CPU_LEVEL >= 6
  void TLB_flushNonGlobal(void);
#endif
  void TLB_flush(void);
  void TLB_invlpg(bx_address laddr);
  void set_INTR(bx_bool value);
  void inhibit_interrupts(unsigned mask);
  bx_bool interrupts_inhibited(unsigned mask);
  const char *strseg(bx_segment_reg_t *seg);
  void interrupt(Bit8u vector, unsigned type, bx_bool push_error,
                 Bit16u error_code);
  void real_mode_int(Bit8u vector, bx_bool push_error, Bit16u error_code);
  void protected_mode_int(Bit8u vector, unsigned soft_int, bx_bool push_error,
                 Bit16u error_code);

  void exception(unsigned vector, Bit16u error_code)
                  BX_CPP_AttrNoReturn();
  void init_SMRAM(void);
  int  int_number(unsigned s);
  bx_bool SetCR0(bx_address val) BX_CPP_AttrRegparmN(1);
  bx_bool check_CR0(bx_address val) BX_CPP_AttrRegparmN(1);
  bx_bool SetCR3(bx_address val) BX_CPP_AttrRegparmN(1);
#if BX_CPU_LEVEL >= 5
  bx_bool SetCR4(bx_address val) BX_CPP_AttrRegparmN(1);
  bx_bool check_CR4(bx_address val) BX_CPP_AttrRegparmN(1);
  Bit32u get_cr4_allow_mask(void);
#endif
#if BX_CPU_LEVEL >= 6
  bx_bool CheckPDPTR(bx_phy_address cr3_val) BX_CPP_AttrRegparmN(1);
#endif
#if BX_SUPPORT_VMX >= 2
  bx_bool CheckPDPTR(Bit64u *pdptr) BX_CPP_AttrRegparmN(1);
#endif
#if BX_CPU_LEVEL >= 5
  bx_bool SetEFER(bx_address val) BX_CPP_AttrRegparmN(1);
#endif

  void reset(unsigned source);
  void shutdown(void);
  void handleCpuModeChange(void);
  void handleCpuContextChange(void);
#if BX_CPU_LEVEL >= 4
  void handleAlignmentCheck(void);
#endif
#if BX_CPU_LEVEL >= 6
  void handleSseModeChange(void);
  void handleAvxModeChange(void);
#endif

#if BX_CPU_LEVEL >= 5
  bx_bool rdmsr(Bit32u index, Bit64u *val_64) BX_CPP_AttrRegparmN(2);
  bx_bool handle_unknown_rdmsr(Bit32u index, Bit64u *val_64) BX_CPP_AttrRegparmN(2);
  bx_bool wrmsr(Bit32u index, Bit64u  val_64) BX_CPP_AttrRegparmN(2);
  bx_bool handle_unknown_wrmsr(Bit32u index, Bit64u  val_64) BX_CPP_AttrRegparmN(2);
#endif


  void task_gate(bxInstruction_c *i, bx_selector_t *selector, bx_descriptor_t *gate_descriptor, unsigned source);
  void jump_protected(bxInstruction_c *i, Bit16u cs, bx_address disp) BX_CPP_AttrRegparmN(3);
  void jmp_call_gate(bx_selector_t *selector, bx_descriptor_t *gate_descriptor) BX_CPP_AttrRegparmN(2);
  void call_gate(bx_descriptor_t *gate_descriptor) BX_CPP_AttrRegparmN(1);

  void call_protected(bxInstruction_c *i, Bit16u cs, bx_address disp) BX_CPP_AttrRegparmN(3);

  void return_protected(bxInstruction_c *i, Bit16u pop_bytes) BX_CPP_AttrRegparmN(2);
  void iret_protected(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  void validate_seg_reg(unsigned seg);
  void validate_seg_regs(void);
  void stack_return_to_v86(Bit32u new_eip, Bit32u raw_cs_selector, Bit32u flags32);
  void iret16_stack_return_from_v86(bxInstruction_c *);
  void iret32_stack_return_from_v86(bxInstruction_c *);
  int  v86_redirect_interrupt(Bit8u vector);
  void init_v8086_mode(void);
  void task_switch_load_selector(bx_segment_reg_t *seg,
                 bx_selector_t *selector, Bit16u raw_selector, Bit8u cs_rpl);
  void task_switch(bxInstruction_c *i, bx_selector_t *selector, bx_descriptor_t *descriptor,
                 unsigned source, Bit32u dword1, Bit32u dword2, bx_bool push_error = 0, Bit32u error_code = 0);
  void get_SS_ESP_from_TSS(unsigned pl, Bit16u *ss, Bit32u *esp);

  void write_flags(Bit16u flags, bx_bool change_IOPL, bx_bool change_IF) BX_CPP_AttrRegparmN(3);
  void writeEFlags(Bit32u eflags, Bit32u changeMask) BX_CPP_AttrRegparmN(2); // Newer variant.
  void write_eflags_fpu_compare(int float_relation);
  Bit32u force_flags(void);
  Bit32u read_eflags(void) { return bx_cpu.force_flags(); }

  bx_bool allow_io(bxInstruction_c *i, Bit16u addr, unsigned len) BX_CPP_AttrRegparmN(3);
  Bit32u  get_descriptor_l(const bx_descriptor_t *) BX_CPP_AttrRegparmN(1);
  Bit32u  get_descriptor_h(const bx_descriptor_t *) BX_CPP_AttrRegparmN(1);
  bx_bool set_segment_ar_data(bx_segment_reg_t *seg, bx_bool valid, Bit16u raw_selector,
                         bx_address base, Bit32u limit_scaled, Bit16u ar_data);
  void    check_cs(bx_descriptor_t *descriptor, Bit16u cs_raw, Bit8u check_rpl, Bit8u check_cpl);
  // the basic assumption of the code that load_cs and load_ss cannot fail !
  void    load_cs(bx_selector_t *selector, bx_descriptor_t *descriptor, Bit8u cpl) BX_CPP_AttrRegparmN(3);
  void    load_ss(bx_selector_t *selector, bx_descriptor_t *descriptor, Bit8u cpl) BX_CPP_AttrRegparmN(3);
  void    touch_segment(bx_selector_t *selector, bx_descriptor_t *descriptor) BX_CPP_AttrRegparmN(2);
  void    fetch_raw_descriptor(const bx_selector_t *selector,
                         Bit32u *dword1, Bit32u *dword2, unsigned exception_no);
  bx_bool fetch_raw_descriptor2(const bx_selector_t *selector,
                         Bit32u *dword1, Bit32u *dword2) BX_CPP_AttrRegparmN(3);
  void    load_seg_reg(bx_segment_reg_t *seg, Bit16u new_value) BX_CPP_AttrRegparmN(2);
  void    load_null_selector(bx_segment_reg_t *seg, unsigned value) BX_CPP_AttrRegparmN(2);

//  void    push_16(Bit16u value16) BX_CPP_AttrRegparmN(1);
//  void    push_32(Bit32u value32) BX_CPP_AttrRegparmN(1);
//  Bit16u  pop_16(void);
//  Bit32u  pop_32(void);

  void    sanity_checks(void);
  void    assert_checks(void);

  void    enter_system_management_mode(void);
  bx_bool resume_from_system_management_mode(BX_SMM_State *smm_state);
  void    smram_save_state(Bit32u *smm_saved_state);
  bx_bool smram_restore_state(const Bit32u *smm_saved_state);

  void    deliver_INIT(void);
  void    deliver_NMI(void);
  void    deliver_SMI(void);
  void    deliver_SIPI(unsigned vector);
  void    debug(bx_address offset);
#if BX_DISASM
  void    debug_disasm_instruction(bx_address offset);
#endif

#if BX_X86_DEBUGGER
  // x86 hardware debug support
  bx_bool hwbreakpoint_check(bx_address laddr, unsigned opa, unsigned opb);
#if BX_CPU_LEVEL >= 5
  void    iobreakpoint_match(unsigned port, unsigned len);
#endif
  Bit32u  code_breakpoint_match(bx_address laddr);
  void    hwbreakpoint_match(bx_address laddr, unsigned len, unsigned rw);
  Bit32u  hwdebug_compare(bx_address laddr, unsigned len, unsigned opa, unsigned opb);
#endif

  void init_FetchDecodeTables(void);

  BX_CPP_INLINE int bx_cpuid_support_x86_64(void);
  BX_CPP_INLINE int bx_cpuid_support_debug_extensions(void);
  BX_CPP_INLINE int bx_cpuid_support_1g_paging(void);
  BX_CPP_INLINE int bx_cpuid_support_vme(void);
  BX_CPP_INLINE int bx_cpuid_support_tsc(void);
  BX_CPP_INLINE int bx_cpuid_support_pae(void);
  BX_CPP_INLINE int bx_cpuid_support_pge(void);
  BX_CPP_INLINE int bx_cpuid_support_pse(void);
  BX_CPP_INLINE int bx_cpuid_support_mmx(void);
  BX_CPP_INLINE int bx_cpuid_support_sse(void);
  BX_CPP_INLINE int bx_cpuid_support_sep(void);
  BX_CPP_INLINE int bx_cpuid_support_fxsave_fxrstor(void);
  BX_CPP_INLINE int bx_cpuid_support_pcid(void);
  BX_CPP_INLINE int bx_cpuid_support_xsave(void);
  BX_CPP_INLINE int bx_cpuid_support_fsgsbase(void);
  BX_CPP_INLINE int bx_cpuid_support_smep(void);
  BX_CPP_INLINE int bx_cpuid_support_smx(void);
  BX_CPP_INLINE int bx_cpuid_support_vmx(void);
  BX_CPP_INLINE int bx_cpuid_support_svm(void);
  BX_CPP_INLINE int bx_cpuid_support_rdtscp(void);
  BX_CPP_INLINE int bx_cpuid_support_tsc_deadline(void);

  BX_CPP_INLINE unsigned which_cpu(void) { return bx_cpu.bx_cpuid; }
  BX_CPP_INLINE Bit64u get_icount(void) { return bx_cpu.icount; }
  BX_CPP_INLINE Bit64u get_icount_last_sync(void) { return bx_cpu.icount_last_sync; }
  BX_CPP_INLINE const bx_gen_reg_t *get_gen_regfile() { return bx_cpu.gen_reg; }

  BX_CPP_INLINE bx_address get_instruction_pointer(void);

  BX_CPP_INLINE Bit32u get_eip(void) { return (bx_cpu.gen_reg[BX_32BIT_REG_EIP].dword.erx); }
  BX_CPP_INLINE Bit16u get_ip (void) { return (bx_cpu.gen_reg[BX_16BIT_REG_IP].word.rx); }


  BX_CPP_INLINE Bit8u get_reg8l(unsigned reg);
  BX_CPP_INLINE Bit8u get_reg8h(unsigned reg);
  BX_CPP_INLINE void  set_reg8l(unsigned reg, Bit8u val);
  BX_CPP_INLINE void  set_reg8h(unsigned reg, Bit8u val);

  BX_CPP_INLINE Bit16u get_reg16(unsigned reg);
  BX_CPP_INLINE void   set_reg16(unsigned reg, Bit16u val);
  BX_CPP_INLINE Bit32u get_reg32(unsigned reg);
  BX_CPP_INLINE void   set_reg32(unsigned reg, Bit32u val);


#if BX_CPU_LEVEL >= 6
  BX_CPP_INLINE unsigned get_cr8();
#endif

  bx_address get_segment_base(unsigned seg);

  // The linear address must be truncated to the 32-bit when CPU is not
  // executing in long64 mode.  The function  must  be used  to compute
  // linear address everywhere when a code is shared between long64 and
  // legacy mode. For legacy mode only  just use Bit32u to store linear 
  // address value.
  bx_address get_laddr(unsigned seg, bx_address offset);

  Bit32u get_laddr32(unsigned seg, Bit32u offset);


  DECLARE_EFLAG_ACCESSOR   (ID,  21)
  DECLARE_EFLAG_ACCESSOR   (VIP, 20)
  DECLARE_EFLAG_ACCESSOR   (VIF, 19)
  DECLARE_EFLAG_ACCESSOR   (AC,  18)
  DECLARE_EFLAG_ACCESSOR   (VM,  17)
  DECLARE_EFLAG_ACCESSOR   (RF,  16)
  DECLARE_EFLAG_ACCESSOR   (NT,  14)
  DECLARE_EFLAG_ACCESSOR_IOPL(   12)
  DECLARE_EFLAG_ACCESSOR   (DF,  10)
  DECLARE_EFLAG_ACCESSOR   (IF,   9)
  DECLARE_EFLAG_ACCESSOR   (TF,   8)

  BX_CPP_INLINE bx_bool real_mode(void);
  BX_CPP_INLINE bx_bool smm_mode(void);
  BX_CPP_INLINE bx_bool protected_mode(void);
  BX_CPP_INLINE bx_bool v8086_mode(void);
  BX_CPP_INLINE bx_bool long_mode(void);
  BX_CPP_INLINE bx_bool long64_mode(void);
  BX_CPP_INLINE unsigned get_cpu_mode(void);

#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  BX_CPP_INLINE bx_bool alignment_check(void);
#endif

#if BX_CPU_LEVEL >= 5
  Bit64u get_TSC();
  void   set_TSC(Bit64u tsc);
#endif

#if BX_SUPPORT_FPU
  void print_state_FPU(void);
  void prepareFPU(bxInstruction_c *i, bx_bool = 1);
  void FPU_check_pending_exceptions(void);
  void FPU_update_last_instruction(bxInstruction_c *i);
  void FPU_stack_underflow(int stnr, int pop_stack = 0);
  void FPU_stack_overflow(void);
  unsigned FPU_exception(unsigned exception, bx_bool = 0);
  bx_address fpu_save_environment(bxInstruction_c *i);
  bx_address fpu_load_environment(bxInstruction_c *i);
  Bit8u pack_FPU_TW(Bit16u tag_word);
  Bit16u unpack_FPU_TW(Bit16u tag_byte);
#endif

#if BX_CPU_LEVEL >= 5
  void prepareMMX(void);
  void prepareFPU2MMX(void); /* cause transition from FPU to MMX technology state */
  void print_state_MMX(void);
#endif

#if BX_CPU_LEVEL >= 6
  void check_exceptionsSSE(int);
  void print_state_SSE(void);

  void prepareXSAVE(void);
  void print_state_AVX(void);
#endif

#if BX_SUPPORT_MONITOR_MWAIT
  bx_bool    is_monitor(bx_phy_address addr, unsigned len);
  void    check_monitor(bx_phy_address addr, unsigned len);
#endif

  bx_address read_CR0(void);
#if BX_CPU_LEVEL >= 5
  bx_address read_CR4(void);
#endif
#if BX_CPU_LEVEL >= 6
  Bit32u ReadCR8(bxInstruction_c *i);
  void WriteCR8(bxInstruction_c *i, bx_address val);
#endif
#if BX_SUPPORT_VMX
  Bit16u VMread16(unsigned encoding) BX_CPP_AttrRegparmN(1);
  Bit32u VMread32(unsigned encoding) BX_CPP_AttrRegparmN(1);
  Bit64u VMread64(unsigned encoding) BX_CPP_AttrRegparmN(1);
  bx_address VMread_natural(unsigned encoding) BX_CPP_AttrRegparmN(1);
  void VMwrite16(unsigned encoding, Bit16u val_16) BX_CPP_AttrRegparmN(2);
  void VMwrite32(unsigned encoding, Bit32u val_32) BX_CPP_AttrRegparmN(2);
  void VMwrite64(unsigned encoding, Bit64u val_64) BX_CPP_AttrRegparmN(2);
  void VMwrite_natural(unsigned encoding, bx_address val) BX_CPP_AttrRegparmN(2);
  void VMsucceed(void);
  void VMfailInvalid(void);
  void VMfail(Bit32u error_code);
  void VMabort(VMX_vmabort_code error_code);
  Bit32u LoadMSRs(Bit32u msr_cnt, bx_phy_address pAddr);
  Bit32u StoreMSRs(Bit32u msr_cnt, bx_phy_address pAddr);
  unsigned VMXReadRevisionID(bx_phy_address pAddr);
  VMX_error_code VMenterLoadCheckVmControls(void);
  VMX_error_code VMenterLoadCheckHostState(void);
  Bit32u VMenterLoadCheckGuestState(Bit64u *qualification);
  void VMenterInjectEvents(void);
  void VMexit(Bit32u reason, Bit64u qualification);
  void VMexitSaveGuestState(void);
  void VMexitSaveGuestMSRs(void);
  void VMexitLoadHostState(void);
  void set_VMCSPTR(Bit64u vmxptr);
  void init_vmx_capabilities();
  void init_VMCS(void);
  bx_bool vmcs_field_supported(Bit32u encoding);
  void register_vmx_state(bx_param_c *parent);
#if BX_SUPPORT_VMX >= 2
  Bit16u VMX_Get_Current_VPID(void);
#endif
  // vmexit reasons
  void VMexit_Instruction(bxInstruction_c *i, Bit32u reason, bx_bool rw = BX_READ) BX_CPP_AttrRegparmN(3);
  void VMexit_Event(unsigned type, unsigned vector,
       Bit16u errcode, bx_bool errcode_valid, Bit64u qualification = 0);
  void VMexit_TripleFault(void);
  void VMexit_ExtInterrupt(void);
  void VMexit_TaskSwitch(Bit16u tss_selector, unsigned source) BX_CPP_AttrRegparmN(2);
  void VMexit_PAUSE(void);
#if BX_SUPPORT_VMX >= 2
  void VMexit_PreemptionTimerExpired(void);
#endif
  bx_bool VMexit_CLTS(void);
  void VMexit_MSR(unsigned op, Bit32u msr) BX_CPP_AttrRegparmN(2);
  void VMexit_IO(bxInstruction_c *i, unsigned port, unsigned len) BX_CPP_AttrRegparmN(3);
  Bit32u VMexit_LMSW(bxInstruction_c *i, Bit32u msw) BX_CPP_AttrRegparmN(2);
  bx_address VMexit_CR0_Write(bxInstruction_c *i, bx_address) BX_CPP_AttrRegparmN(2);
  void VMexit_CR3_Read(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  void VMexit_CR3_Write(bxInstruction_c *i, bx_address) BX_CPP_AttrRegparmN(2);
  bx_address VMexit_CR4_Write(bxInstruction_c *i, bx_address) BX_CPP_AttrRegparmN(2);
  void VMexit_CR8_Read(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  void VMexit_CR8_Write(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  void VMexit_DR_Access(unsigned read, unsigned dr, unsigned reg);
#if BX_SUPPORT_VMX >= 2
  void vmfunc_eptp_switching(void);
#endif
#endif

#if BX_SUPPORT_SVM
  void SvmEnterSaveHostState(SVM_HOST_STATE *host);
  bx_bool SvmEnterLoadCheckControls(SVM_CONTROLS *ctrls);
  bx_bool SvmEnterLoadCheckGuestState(void);
  bx_bool SvmInjectEvents(void);
  void Svm_Vmexit(int reason, Bit64u exitinfo1 = 0, Bit64u exitinfo2 = 0);
  void SvmExitSaveGuestState(void);
  void SvmExitLoadHostState(SVM_HOST_STATE *host);
  Bit8u vmcb_read8(unsigned offset);
  Bit16u vmcb_read16(unsigned offset);
  Bit32u vmcb_read32(unsigned offset);
  Bit64u vmcb_read64(unsigned offset);
  void vmcb_write8(unsigned offset, Bit8u val_8);
  void vmcb_write16(unsigned offset, Bit16u val_16);
  void vmcb_write32(unsigned offset, Bit32u val_32);
  void vmcb_write64(unsigned offset, Bit64u val_64);
  void svm_segment_read(bx_segment_reg_t *seg, unsigned offset);
  void svm_segment_write(bx_segment_reg_t *seg, unsigned offset);
  void SvmInterceptException(unsigned type, unsigned vector,
       Bit16u errcode, bx_bool errcode_valid, Bit64u qualification = 0);
  void SvmInterceptIO(bxInstruction_c *i, unsigned port, unsigned len);
  void SvmInterceptMSR(unsigned op, Bit32u msr);
  void SvmInterceptTaskSwitch(Bit16u tss_selector, unsigned source, bx_bool push_error, Bit32u error_code);
  void VirtualInterruptAcknowledge(void);
  void register_svm_state(bx_param_c *parent);
#endif

#if BX_CONFIGURE_MSRS
  int load_MSRs(const char *file);
#endif
};

#if BX_CPU_LEVEL >= 5
BX_CPP_INLINE void BX_CPU_C::prepareMMX(void)
{
  if(bx_cpu.cr0.get_EM())
    exception(BX_UD_EXCEPTION, 0);

  if(bx_cpu.cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0);

  /* check floating point status word for a pending FPU exceptions */
  FPU_check_pending_exceptions();
}

BX_CPP_INLINE void BX_CPU_C::prepareFPU2MMX(void)
{
  bx_cpu.the_i387.twd = 0;
  bx_cpu.the_i387.tos = 0; /* reset FPU Top-Of-Stack */
}
#endif

#if BX_CPU_LEVEL >= 6
BX_CPP_INLINE void BX_CPU_C::prepareXSAVE(void)
{
  if(! bx_cpu.cr4.get_OSXSAVE())
    exception(BX_UD_EXCEPTION, 0);

  if(bx_cpu.cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0);
}
#endif

// Can be used as LHS or RHS.
#define RMAddr(i)  (bx_cpu.address_xlation.rm_addr)

#if defined(NEED_CPU_REG_SHORTCUTS)

#include "stack.h"

#define RSP_SPECULATIVE {              \
  bx_cpu.speculative_rsp = 1; \
  bx_cpu.prev_rsp = RSP;      \
}

#define RSP_COMMIT {                   \
  bx_cpu.speculative_rsp = 0; \
}

#endif // defined(NEED_CPU_REG_SHORTCUTS)

//
// bit 0 - CS.D_B
// bit 1 - long64 mode (CS.L)
// bit 2 - SSE_OK
// bit 3 - AVX_OK
//
// updateFetchModeMask - has to be called everytime 
//   CS.L / CS.D_B / CR0.PE, CR0.TS or CR0.EM / CR4.OSFXSR / CR4.OSXSAVE changes
//
BX_CPP_INLINE void BX_CPU_C::updateFetchModeMask(void)
{
  bx_cpu.fetchModeMask =
#if BX_CPU_LEVEL >= 6
#if BX_SUPPORT_AVX
     (bx_cpu.avx_ok << 3) |
#endif
     (bx_cpu.sse_ok << 2) |
#endif

     (bx_cpu.sregs[BX_SEG_REG_CS].cache.u.segment.d_b);

  bx_cpu.user_pl = // CPL == 3
     (bx_cpu.sregs[BX_SEG_REG_CS].selector.rpl == 3);
}

BX_CPP_INLINE bx_address BX_CPU_C::get_segment_base(unsigned seg)
{
  return bx_cpu.sregs[seg].cache.u.segment.base;
}

BX_CPP_INLINE Bit32u BX_CPU_C::get_laddr32(unsigned seg, Bit32u offset)
{
  return (Bit32u) bx_cpu.sregs[seg].cache.u.segment.base + offset;
}

BX_CPP_INLINE bx_address BX_CPU_C::get_laddr(unsigned seg, bx_address offset)
{
  return get_laddr32(seg, (Bit32u) offset);
}

BX_CPP_INLINE Bit8u BX_CPU_C::get_reg8l(unsigned reg)
{
  assert(reg < BX_GENERAL_REGISTERS);
  return (bx_cpu.gen_reg[reg].word.byte.rl);
}

BX_CPP_INLINE void BX_CPU_C::set_reg8l(unsigned reg, Bit8u val)
{
  assert(reg < BX_GENERAL_REGISTERS);
  bx_cpu.gen_reg[reg].word.byte.rl = val;
}

BX_CPP_INLINE Bit8u BX_CPU_C::get_reg8h(unsigned reg)
{
  assert(reg < BX_GENERAL_REGISTERS);
  return (bx_cpu.gen_reg[reg].word.byte.rh);
}

BX_CPP_INLINE void BX_CPU_C::set_reg8h(unsigned reg, Bit8u val)
{
  assert(reg < BX_GENERAL_REGISTERS);
  bx_cpu.gen_reg[reg].word.byte.rh = val;
}

BX_CPP_INLINE bx_address BX_CPU_C::get_instruction_pointer(void)
{
  return bx_cpu.get_eip();
}

BX_CPP_INLINE Bit16u BX_CPU_C::get_reg16(unsigned reg)
{
  assert(reg < BX_GENERAL_REGISTERS);
  return (bx_cpu.gen_reg[reg].word.rx);
}

BX_CPP_INLINE void BX_CPU_C::set_reg16(unsigned reg, Bit16u val)
{
  assert(reg < BX_GENERAL_REGISTERS);
  bx_cpu.gen_reg[reg].word.rx = val;
}

BX_CPP_INLINE Bit32u BX_CPU_C::get_reg32(unsigned reg)
{
  assert(reg < BX_GENERAL_REGISTERS);
  return (bx_cpu.gen_reg[reg].dword.erx);
}

BX_CPP_INLINE void BX_CPU_C::set_reg32(unsigned reg, Bit32u val)
{
   assert(reg < BX_GENERAL_REGISTERS);
   bx_cpu.gen_reg[reg].dword.erx = val;
}

#if BX_CPU_LEVEL >= 6
// CR8 is aliased to APIC->TASK PRIORITY register
//   APIC.TPR[7:4] = CR8[3:0]
//   APIC.TPR[3:0] = 0
// Reads of CR8 return zero extended APIC.TPR[7:4]
BX_CPP_INLINE unsigned BX_CPU_C::get_cr8(void)
{
   printf("CPU should not reach this point!!!!\n");
   exit(0);
}
#endif

BX_CPP_INLINE bx_bool BX_CPU_C::real_mode(void)
{
  return (bx_cpu.cpu_mode == BX_MODE_IA32_REAL);
}

BX_CPP_INLINE bx_bool BX_CPU_C::smm_mode(void)
{
  return (bx_cpu.in_smm);
}

BX_CPP_INLINE bx_bool BX_CPU_C::v8086_mode(void)
{
  return (bx_cpu.cpu_mode == BX_MODE_IA32_V8086);
}

BX_CPP_INLINE bx_bool BX_CPU_C::protected_mode(void)
{
  return (bx_cpu.cpu_mode >= BX_MODE_IA32_PROTECTED);
}

BX_CPP_INLINE bx_bool BX_CPU_C::long_mode(void)
{
  return 0;
}

BX_CPP_INLINE bx_bool BX_CPU_C::long64_mode(void)
{
  return 0;
}

BX_CPP_INLINE unsigned BX_CPU_C::get_cpu_mode(void)
{
  return (bx_cpu.cpu_mode);
}

#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
BX_CPP_INLINE bx_bool BX_CPU_C::alignment_check(void)
{
  return bx_cpu.alignment_check_mask;
}
#endif

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_svm(void)
{
  return (bx_cpu.isa_extensions_bitmask & BX_ISA_SVM) != 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_smx(void)
{
  return (bx_cpu.isa_extensions_bitmask & BX_ISA_SMX) != 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_vmx(void)
{
  return (bx_cpu.isa_extensions_bitmask & BX_ISA_VMX) != 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_xsave(void)
{
  return (bx_cpu.isa_extensions_bitmask & BX_ISA_XSAVE) != 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_pcid(void)
{
  return 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_fsgsbase(void)
{
  return 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_smep(void)
{
  return (bx_cpu.cpu_extensions_bitmask & BX_CPU_SMEP) != 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_vme(void)
{
  return (bx_cpu.cpu_extensions_bitmask & BX_CPU_VME) != 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_tsc(void)
{
  return (bx_cpu.isa_extensions_bitmask & BX_ISA_PENTIUM) != 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_debug_extensions(void)
{
  return (bx_cpu.cpu_extensions_bitmask & BX_CPU_DEBUG_EXTENSIONS) != 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_pse(void)
{
  return (bx_cpu.cpu_extensions_bitmask & BX_CPU_PSE) != 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_pae(void)
{
  return (bx_cpu.cpu_extensions_bitmask & BX_CPU_PAE) != 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_pge(void)
{
  return (bx_cpu.cpu_extensions_bitmask & BX_CPU_PGE) != 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_mmx(void)
{
  return (bx_cpu.isa_extensions_bitmask & BX_ISA_MMX) != 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_sse(void)
{
  return (bx_cpu.isa_extensions_bitmask & BX_ISA_SSE) != 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_sep(void)
{
  return (bx_cpu.isa_extensions_bitmask & BX_ISA_SYSENTER_SYSEXIT) != 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_fxsave_fxrstor(void)
{
  return (bx_cpu.isa_extensions_bitmask & BX_ISA_SSE) != 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_x86_64(void)
{
  return 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_1g_paging(void)
{
  return 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_rdtscp(void)
{
  return 0;
}

BX_CPP_INLINE int BX_CPU_C::bx_cpuid_support_tsc_deadline(void)
{
  return (bx_cpu.cpu_extensions_bitmask & BX_CPU_TSC_DEADLINE) != 0;
}

IMPLEMENT_EFLAG_ACCESSOR   (ID,  21)
IMPLEMENT_EFLAG_ACCESSOR   (VIP, 20)
IMPLEMENT_EFLAG_ACCESSOR   (VIF, 19)
IMPLEMENT_EFLAG_ACCESSOR   (AC,  18)
IMPLEMENT_EFLAG_ACCESSOR   (VM,  17)
IMPLEMENT_EFLAG_ACCESSOR   (RF,  16)
IMPLEMENT_EFLAG_ACCESSOR   (NT,  14)
IMPLEMENT_EFLAG_ACCESSOR_IOPL(   12)
IMPLEMENT_EFLAG_ACCESSOR   (DF,  10)
IMPLEMENT_EFLAG_ACCESSOR   (IF,   9)
IMPLEMENT_EFLAG_ACCESSOR   (TF,   8)

IMPLEMENT_EFLAG_SET_ACCESSOR      (ID,  21)
IMPLEMENT_EFLAG_SET_ACCESSOR      (VIP, 20)
IMPLEMENT_EFLAG_SET_ACCESSOR      (VIF, 19)
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
IMPLEMENT_EFLAG_SET_ACCESSOR_AC   (     18)
#else
IMPLEMENT_EFLAG_SET_ACCESSOR      (AC,  18)
#endif
IMPLEMENT_EFLAG_SET_ACCESSOR_VM   (     17)
IMPLEMENT_EFLAG_SET_ACCESSOR_RF   (RF,  16)
IMPLEMENT_EFLAG_SET_ACCESSOR      (NT,  14)
IMPLEMENT_EFLAG_SET_ACCESSOR      (DF,  10)
IMPLEMENT_EFLAG_SET_ACCESSOR_IF_TF(IF,   9)
IMPLEMENT_EFLAG_SET_ACCESSOR_IF_TF(TF,   8)
                               
#define BX_TASK_FROM_CALL       0
#define BX_TASK_FROM_IRET       1
#define BX_TASK_FROM_JUMP       2
#define BX_TASK_FROM_INT        3

// exception types for interrupt method
enum {
  BX_EXTERNAL_INTERRUPT = 0,
  BX_NMI = 2,
  BX_HARDWARE_EXCEPTION = 3,  // all exceptions except #BP and #OF
  BX_SOFTWARE_INTERRUPT = 4,
  BX_PRIVILEGED_SOFTWARE_INTERRUPT = 5,
  BX_SOFTWARE_EXCEPTION = 6
};

#if BX_CPU_LEVEL >= 6
enum {
  BX_INVPCID_INDIVIDUAL_ADDRESS_NON_GLOBAL_INVALIDATION,
  BX_INVPCID_SINGLE_CONTEXT_NON_GLOBAL_INVALIDATION,
  BX_INVPCID_ALL_CONTEXT_INVALIDATION,
  BX_INVPCID_ALL_CONTEXT_NON_GLOBAL_INVALIDATION
};
#endif

// <TAG-DEFINES-DECODE-START>

//
// For decoding...
//

// If the BxImmediate mask is set, the lowest 4 bits of the attribute
// specify which kinds of immediate data required by instruction.

#define BxImmediate         0x000f // bits 3..0: any immediate
#define BxImmediate_I1      0x0001 // imm8 = 1
#define BxImmediate_Ib      0x0002 // 8 bit
#define BxImmediate_Ib_SE   0x0003 // sign extend to OS size
#define BxImmediate_Iw      0x0004 // 16 bit
#define BxImmediate_Id      0x0005 // 32 bit
#define BxImmediate_O       0x0006 // MOV_ALOd, mov_OdAL, mov_eAXOv, mov_OveAX
#define BxImmediate_BrOff8  0x0008 // Relative branch offset byte

#define BxImmediate_Ib4     BxImmediate_Ib // Register encoded in Ib[7:4]
#define BxImmediate_Ib5     BxImmediate_Ib

#define BxImmediate_BrOff16 BxImmediate_Iw // Relative branch offset word, not encodable in 64-bit mode
#define BxImmediate_BrOff32 BxImmediate_Id // Relative branch offset dword

// Lookup for opcode and attributes in another opcode tables
// Totally 15 opcode groups supported
#define BxGroupX            0x00f0 // bits 7..4: opcode groups definition
#define BxPrefixSSE66       0x0010 // Group encoding: 0001, SSE_PREFIX_66
#define BxPrefixSSEF3       0x0020 // Group encoding: 0010, SSE_PREFIX_F3
#define BxPrefixSSEF2       0x0030 // Group encoding: 0011, SSE_PREFIX_F2
#define BxPrefixSSE         0x0040 // Group encoding: 0100
#define BxGroupN            0x0050 // Group encoding: 0101
#define BxSplitGroupN       0x0060 // Group encoding: 0110
#define BxFPEscape          0x0070 // Group encoding: 0111
#define Bx3ByteOp           0x0080 // Group encoding: 1000
#define BxOSizeGrp          0x0090 // Group encoding: 1001
#define BxPrefixVEX         0x00A0 // Group encoding: 1010
#define BxSplitVexW         0x00B0 // Group encoding: 1011
#define BxSplitVexW64       0x00C0 // Group encoding: 1100 - VexW ignored in 32-bit mode
#define BxSplitMod11B       0x00D0 // Group encoding: 1101

// The BxImmediate2 mask specifies kind of second immediate data
// required by instruction.
#define BxImmediate2        0x0300 // bits 8.9: any immediate
#define BxImmediate_Ib2     0x0100
#define BxImmediate_Iw2     0x0200
#define BxImmediate_Id2     0x0300

#define BxLockable          0x0400 // bit 10
#define BxRepeatable        0x0800 // bit 11
#define BxVexW0             0x1000 // bit 12
#define BxVexW1             0x2000 // bit 13

#define BxTraceEnd          0x8000 // bit 15


#ifdef BX_TRACE_CACHE_NO_SPECULATIVE_TRACING
  #define BxTraceJCC      BxTraceEnd
#else
  #define BxTraceJCC      0
#endif

#define BxGroup1          BxGroupN
#define BxGroup1A         BxGroupN
#define BxGroup2          BxGroupN
#define BxGroup3          BxGroupN
#define BxGroup4          BxGroupN
#define BxGroup5          BxGroupN
#define BxGroup6          BxGroupN
#define BxGroup7          BxFPEscape
#define BxGroup8          BxGroupN
#define BxGroup9          BxGroupN

#define BxGroup11         BxGroupN
#define BxGroup12         BxGroupN
#define BxGroup13         BxGroupN
#define BxGroup14         BxGroupN
#define BxGroup15         BxSplitGroupN
#define BxGroup16         BxGroupN
#define BxGroup17         BxGroupN
#define BxGroup17A        BxGroupN

#define BxGroupFP         BxSplitGroupN

// <TAG-DEFINES-DECODE-END>

#endif  // #ifndef BX_CPU_H
