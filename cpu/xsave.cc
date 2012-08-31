/////////////////////////////////////////////////////////////////////////
// $Id: xsave.cc 11301 2012-07-27 08:13:39Z sshwarts $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2008-2012 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
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

/* 0F AE /4 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XSAVE(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  unsigned index;
  BxPackedXmmRegister xmm;

  BX_CPU_THIS_PTR prepareXSAVE();

  BX_DEBUG(("XSAVE: save processor state XCR0=0x%08x", BX_CPU_THIS_PTR xcr0.get32()));

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  bx_address laddr = get_laddr(i->seg(), eaddr);

  if (laddr & 0x3f) {
    BX_ERROR(("XSAVE: access not aligned to 64-byte"));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_address asize_mask = i->asize_mask();

  //
  // We will go feature-by-feature and not run over all XCR0 bits
  //

  Bit64u header1 = read_virtual_qword(i->seg(), (eaddr + 512) & asize_mask);

  Bit32u features_save_enable_mask = BX_CPU_THIS_PTR xcr0.get32() & EAX;

  /////////////////////////////////////////////////////////////////////////////
  if ((features_save_enable_mask & BX_XCR0_FPU_MASK) != 0)
  {
    xmm.xmm16u(0) = BX_CPU_THIS_PTR the_i387.get_control_word();
    xmm.xmm16u(1) = BX_CPU_THIS_PTR the_i387.get_status_word();
    xmm.xmm16u(2) = pack_FPU_TW(BX_CPU_THIS_PTR the_i387.get_tag_word());

    /* x87 FPU Opcode (16 bits) */
    /* The lower 11 bits contain the FPU opcode, upper 5 bits are reserved */
    xmm.xmm16u(3) = BX_CPU_THIS_PTR the_i387.foo;

    /*
     * x87 FPU IP Offset (32/64 bits)
     * The contents of this field differ depending on the current
     * addressing mode (16/32/64 bit) when the FXSAVE instruction was executed:
     *   + 64-bit mode - 64-bit IP offset
     *   + 32-bit mode - 32-bit IP offset
     *   + 16-bit mode - low 16 bits are IP offset; high 16 bits are reserved.
     * x87 CS FPU IP Selector
     *   + 16 bit, in 16/32 bit mode only
     */
#if BX_SUPPORT_X86_64
    if (i->os64L()) {
      xmm.xmm64u(1) = (BX_CPU_THIS_PTR the_i387.fip);
    }
    else
#endif
    {
      xmm.xmm32u(2) = (Bit32u)(BX_CPU_THIS_PTR the_i387.fip);
      xmm.xmm32u(3) =         (BX_CPU_THIS_PTR the_i387.fcs);
    }

    write_virtual_dqword(i->seg(), eaddr, (Bit8u *) &xmm);

    /*
     * x87 FPU Instruction Operand (Data) Pointer Offset (32/64 bits)
     * The contents of this field differ depending on the current
     * addressing mode (16/32 bit) when the FXSAVE instruction was executed:
     *   + 64-bit mode - 64-bit offset
     *   + 32-bit mode - 32-bit offset
     *   + 16-bit mode - low 16 bits are offset; high 16 bits are reserved.
     * x87 DS FPU Instruction Operand (Data) Pointer Selector
     *   + 16 bit, in 16/32 bit mode only
     */
#if BX_SUPPORT_X86_64
    if (i->os64L()) {
      write_virtual_qword(i->seg(), (eaddr + 16) & asize_mask, BX_CPU_THIS_PTR the_i387.fdp);
    }
    else
#endif
    {
      write_virtual_dword(i->seg(), (eaddr + 16) & asize_mask, (Bit32u) BX_CPU_THIS_PTR the_i387.fdp);
      write_virtual_dword(i->seg(), (eaddr + 20) & asize_mask, (Bit32u) BX_CPU_THIS_PTR the_i387.fds);
    }
    /* do not touch MXCSR state */

    /* store i387 register file */
    for(index=0; index < 8; index++)
    {
      const floatx80 &fp = BX_READ_FPU_REG(index);

      xmm.xmm64u(0) = fp.fraction;
      xmm.xmm64u(1) = 0;
      xmm.xmm16u(4) = fp.exp;

      write_virtual_dqword(i->seg(), (eaddr+index*16+32) & asize_mask, (Bit8u *) &xmm);
    }

    header1 |= BX_XCR0_FPU_MASK;
  }

  /////////////////////////////////////////////////////////////////////////////
  if ((features_save_enable_mask & (BX_XCR0_SSE_MASK | BX_XCR0_AVX_MASK)) != 0)
  {
    // store MXCSR
    write_virtual_dword(i->seg(), (eaddr + 24) & asize_mask, BX_MXCSR_REGISTER);
    write_virtual_dword(i->seg(), (eaddr + 28) & asize_mask, MXCSR_MASK);
  }

  /////////////////////////////////////////////////////////////////////////////
  if ((features_save_enable_mask & BX_XCR0_SSE_MASK) != 0)
  {
    /* store XMM register file */
    for(index=0; index < BX_XMM_REGISTERS; index++)
    {
      // save XMM8-XMM15 only in 64-bit mode
      if (index < 8 || long64_mode()) {
        write_virtual_dqword(i->seg(),
           (eaddr+index*16+160) & asize_mask, (Bit8u *)(&BX_READ_XMM_REG(index)));
      }
    }

    header1 |= BX_XCR0_SSE_MASK;
  }

#if BX_SUPPORT_AVX
  /////////////////////////////////////////////////////////////////////////////
  if ((features_save_enable_mask & BX_XCR0_AVX_MASK) != 0)
  {
    /* store AVX state */
    for(index=0; index < BX_XMM_REGISTERS; index++)
    {
      // save YMM8-YMM15 only in 64-bit mode
      if (index < 8 || long64_mode()) {
        write_virtual_dqword(i->seg(),
           (eaddr+index*16+576) & asize_mask, (Bit8u *)(&BX_READ_AVX_REG_LINE(index, 1)));
      }
    }

    header1 |= BX_XCR0_AVX_MASK;
  }
#endif

  // always update header to 'dirty' state
  write_virtual_qword(i->seg(), (eaddr + 512) & asize_mask, header1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F AE /5 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XRSTOR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  unsigned index;
  BxPackedXmmRegister xmm;

  BX_CPU_THIS_PTR prepareXSAVE();

  BX_DEBUG(("XRSTOR: restore processor state XCR0=0x%08x", BX_CPU_THIS_PTR xcr0.get32()));

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  bx_address laddr = get_laddr(i->seg(), eaddr);

  if (laddr & 0x3f) {
    BX_ERROR(("XRSTOR: access not aligned to 64-byte"));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_address asize_mask = i->asize_mask();

  Bit64u header1 = read_virtual_qword(i->seg(), (eaddr + 512) & asize_mask);
  Bit64u header2 = read_virtual_qword(i->seg(), (eaddr + 520) & asize_mask);
  Bit64u header3 = read_virtual_qword(i->seg(), (eaddr + 528) & asize_mask);

  if ((~BX_CPU_THIS_PTR xcr0.get32() & header1) != 0) {
    BX_ERROR(("XRSTOR: Broken header state"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (header2 != 0 || header3 != 0) {
    BX_ERROR(("XRSTOR: Reserved header state is not '0"));
    exception(BX_GP_EXCEPTION, 0);
  }

  //
  // We will go feature-by-feature and not run over all XCR0 bits
  //

  Bit32u features_load_enable_mask = BX_CPU_THIS_PTR xcr0.get32() & EAX;

  /////////////////////////////////////////////////////////////////////////////
  if ((features_load_enable_mask & BX_XCR0_FPU_MASK) != 0)
  {
    if (header1 & BX_XCR0_FPU_MASK) {
      // load FPU state from XSAVE area
      read_virtual_dqword(i->seg(), eaddr, (Bit8u *) &xmm);

      BX_CPU_THIS_PTR the_i387.cwd =  xmm.xmm16u(0);
      BX_CPU_THIS_PTR the_i387.swd =  xmm.xmm16u(1);
      BX_CPU_THIS_PTR the_i387.tos = (xmm.xmm16u(1) >> 11) & 0x07;

      /* always set bit 6 as '1 */
      BX_CPU_THIS_PTR the_i387.cwd =
         (BX_CPU_THIS_PTR the_i387.cwd & ~FPU_CW_Reserved_Bits) | 0x0040;

      /* Restore x87 FPU Opcode */
      /* The lower 11 bits contain the FPU opcode, upper 5 bits are reserved */
      BX_CPU_THIS_PTR the_i387.foo = xmm.xmm16u(3) & 0x7FF;

      /* Restore x87 FPU IP */
#if BX_SUPPORT_X86_64
      if (i->os64L()) {
        BX_CPU_THIS_PTR the_i387.fip = xmm.xmm64u(1);
        BX_CPU_THIS_PTR the_i387.fcs = 0;
      }
      else
#endif
      {
        BX_CPU_THIS_PTR the_i387.fip = xmm.xmm32u(2);
        BX_CPU_THIS_PTR the_i387.fcs = xmm.xmm16u(6);
      }

      Bit32u tag_byte = xmm.xmmubyte(4);

      /* Restore x87 FPU DP */
      read_virtual_dqword(i->seg(), (eaddr + 16) & asize_mask, (Bit8u *) &xmm);

#if BX_SUPPORT_X86_64
      if (i->os64L()) {
        BX_CPU_THIS_PTR the_i387.fdp = xmm.xmm64u(0);
        BX_CPU_THIS_PTR the_i387.fds = 0;
      }
      else
#endif
      {
        BX_CPU_THIS_PTR the_i387.fdp = xmm.xmm32u(0);
        BX_CPU_THIS_PTR the_i387.fds = xmm.xmm16u(2);
      }

      /* load i387 register file */
      for(index=0; index < 8; index++)
      {
        floatx80 reg;
        reg.fraction = read_virtual_qword(i->seg(), (eaddr+index*16+32) & asize_mask);
        reg.exp      = read_virtual_word (i->seg(), (eaddr+index*16+40) & asize_mask);

        // update tag only if it is not empty
        BX_WRITE_FPU_REGISTER_AND_TAG(reg,
              IS_TAG_EMPTY(index) ? FPU_Tag_Empty : FPU_tagof(reg), index);
      }

      /* Restore floating point tag word - see desription for FXRSTOR instruction */
      BX_CPU_THIS_PTR the_i387.twd = unpack_FPU_TW(tag_byte);

      /* check for unmasked exceptions */
      if (FPU_PARTIAL_STATUS & ~FPU_CONTROL_WORD & FPU_CW_Exceptions_Mask) {
        /* set the B and ES bits in the status-word */
        FPU_PARTIAL_STATUS |= FPU_SW_Summary | FPU_SW_Backward;
      }
      else {
        /* clear the B and ES bits in the status-word */
        FPU_PARTIAL_STATUS &= ~(FPU_SW_Summary | FPU_SW_Backward);
      }
    }
    else {
      // initialize FPU with reset values
      BX_CPU_THIS_PTR the_i387.init();

      for (index=0;index<8;index++) {
        static floatx80 reg = { 0, 0 };
        BX_FPU_REG(index) = reg;
      }
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  if ((features_load_enable_mask & (BX_XCR0_SSE_MASK | BX_XCR0_AVX_MASK)) != 0)
  {
    Bit32u new_mxcsr = read_virtual_dword(i->seg(), (eaddr + 24) & asize_mask);
    if(new_mxcsr & ~MXCSR_MASK)
       exception(BX_GP_EXCEPTION, 0);
    BX_MXCSR_REGISTER = new_mxcsr;
  }

  static BxPackedXmmRegister xmmnil; /* compiler will clear the variable */

  /////////////////////////////////////////////////////////////////////////////
  if ((features_load_enable_mask & BX_XCR0_SSE_MASK) != 0)
  {
    if (header1 & BX_XCR0_SSE_MASK) {
      // load SSE state from XSAVE area
      for(index=0; index < BX_XMM_REGISTERS; index++)
      {
         // restore XMM8-XMM15 only in 64-bit mode
         if (index < 8 || long64_mode()) {
           read_virtual_dqword(i->seg(),
               (eaddr+index*16+160) & asize_mask, (Bit8u *)(&BX_READ_XMM_REG(index)));
         }
      }
    }
    else {
       // initialize SSE with reset values
       for(index=0; index < BX_XMM_REGISTERS; index++) {
         // set XMM8-XMM15 only in 64-bit mode
         if (index < 8 || long64_mode())
           BX_WRITE_XMM_REG(index, xmmnil);
       }
    }
  }

#if BX_SUPPORT_AVX
  /////////////////////////////////////////////////////////////////////////////
  if ((features_load_enable_mask & BX_XCR0_AVX_MASK) != 0)
  {
    if (header1 & BX_XCR0_AVX_MASK) {
      // load AVX state from XSAVE area
      for(index=0; index < BX_XMM_REGISTERS; index++)
      {
         // restore YMM8-YMM15 only in 64-bit mode
         if (index < 8 || long64_mode()) {
           read_virtual_dqword(i->seg(),
               (eaddr+index*16+576) & asize_mask, (Bit8u *)(&BX_READ_AVX_REG_LINE(index, 1)));
         }
      }
    }
    else {
       // initialize upper part of AVX registers with reset values
       for(index=0; index < BX_XMM_REGISTERS; index++) {
         // set YMM8-YMM15 only in 64-bit mode
         if (index < 8 || long64_mode()) {
           for (int j=2;j < BX_VLMAX*2;j++)
             BX_CPU_THIS_PTR vmm[index].avx64u(j) = 0;
         }
       }
    }
  }
#endif
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 01 D0 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XGETBV(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if(! BX_CPU_THIS_PTR cr4.get_OSXSAVE()) {
    BX_ERROR(("XGETBV: OSXSAVE feature is not enabled in CR4!"));
    exception(BX_UD_EXCEPTION, 0);
  }

  // For now hardcoded handle only XCR0 register, it should take a few
  // years until extension will be required
  if (ECX != 0) {
    BX_ERROR(("XGETBV: Invalid XCR register %d", ECX));
    exception(BX_GP_EXCEPTION, 0);
  }

  RDX = 0;
  RAX = BX_CPU_THIS_PTR xcr0.get32();
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 01 D1 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XSETBV(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if(! BX_CPU_THIS_PTR cr4.get_OSXSAVE()) {
    BX_ERROR(("XSETBV: OSXSAVE feature is not enabled in CR4!"));
    exception(BX_UD_EXCEPTION, 0);
  }

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    BX_ERROR(("VMEXIT: XSETBV in VMX non-root operation"));
    VMexit(VMX_VMEXIT_XSETBV, 0);
  }
#endif

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT1_XSETBV)) Svm_Vmexit(SVM_VMEXIT_XSETBV);
  }
#endif

  // CPL is always 3 in vm8086 mode
  if (/* v8086_mode() || */ CPL != 0) {
    BX_ERROR(("XSETBV: The current priveledge level is not 0"));
    exception(BX_GP_EXCEPTION, 0);
  }

  // For now hardcoded handle only XCR0 register, it should take a few
  // years until extension will be required
  if (ECX != 0) {
    BX_ERROR(("XSETBV: Invalid XCR register %d", ECX));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (EDX != 0 || (EAX & ~BX_CPU_THIS_PTR xcr0_suppmask) != 0 || (EAX & 1) == 0) {
    BX_ERROR(("XSETBV: Attempting to change reserved bits!"));
    exception(BX_GP_EXCEPTION, 0);
  }

#if BX_SUPPORT_AVX
  if ((EAX & (BX_XCR0_AVX_BIT | BX_XCR0_SSE_BIT)) == BX_XCR0_AVX_BIT) {
    BX_ERROR(("XSETBV: Attempting to set AVX without SSE!"));
    exception(BX_GP_EXCEPTION, 0);
  }
#endif

  BX_CPU_THIS_PTR xcr0.set32(EAX);

#if BX_SUPPORT_AVX
  handleAvxModeChange();
#endif

#endif // BX_CPU_LEVEL >= 6

  BX_NEXT_TRACE(i);
}
