/////////////////////////////////////////////////////////////////////////
// $Id: instr.h 11356 2012-08-21 19:58:41Z sshwarts $
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

#ifndef BX_INSTR_H
#define BX_INSTR_H

class bxInstruction_c;

typedef void BX_INSF_TYPE;

#if BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS

#define BX_SYNC_TIME_IF_SINGLE_PROCESSOR(allowed_delta) {                     \
  if (BX_SMP_PROCESSORS == 1) {                                               \
    Bit32u delta = BX_CPU_THIS_PTR icount - BX_CPU_THIS_PTR icount_last_sync; \
    if (delta >= allowed_delta) {                                             \
      BX_CPU_THIS_PTR icount_last_sync = BX_CPU_THIS_PTR icount;              \
      BX_TICKN(delta);                                                        \
    }                                                                         \
  }                                                                           \
}

#define BX_COMMIT_INSTRUCTION(i) {                     \
  BX_CPU_THIS_PTR prev_rip = RIP; /* commit new RIP */ \
  BX_INSTR_AFTER_EXECUTION(BX_CPU_ID, (i));            \
  BX_CPU_THIS_PTR icount++;                            \
}

#define BX_EXECUTE_INSTRUCTION(i) {                    \
  BX_INSTR_BEFORE_EXECUTION(BX_CPU_ID, (i));           \
  RIP += (i)->ilen();                                  \
  return BX_CPU_CALL_METHOD(i->execute, (i));          \
}

#define BX_NEXT_TRACE(i) {                             \
  BX_COMMIT_INSTRUCTION(i);                            \
  return;                                              \
}

#define BX_LINK_TRACE(i) {                             \
  BX_COMMIT_INSTRUCTION(i);                            \
  linkTrace(i);                                        \
  return;                                              \
}

#define BX_NEXT_INSTR(i) {                             \
  BX_COMMIT_INSTRUCTION(i);                            \
  if (BX_CPU_THIS_PTR async_event) return;             \
  ++i;                                                 \
  BX_EXECUTE_INSTRUCTION(i);                           \
}

#else // BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS

#define BX_NEXT_TRACE(i) { return; }
#define BX_NEXT_INSTR(i) { return; }
#define BX_LINK_TRACE(i) { return; }

#define BX_SYNC_TIME_IF_SINGLE_PROCESSOR(allowed_delta) \
  if (BX_SMP_PROCESSORS == 1) BX_TICK1()

#endif

// <TAG-TYPE-EXECUTEPTR-START>
typedef BX_INSF_TYPE (BX_CPU_C::*BxExecutePtr_tR)(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
typedef bx_address (BX_CPU_C::*BxResolvePtr_tR)(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
typedef void (BX_CPU_C::*BxRepIterationPtr_tR)(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
// <TAG-TYPE-EXECUTEPTR-END>

extern bx_address bx_asize_mask[];

const char *get_bx_opcode_name(Bit16u ia_opcode);

// <TAG-CLASS-INSTRUCTION-START>
class bxInstruction_c {
public:
  // Function pointers; a function to resolve the modRM address
  // given the current state of the CPU and the instruction data,
  // and a function to execute the instruction after resolving
  // the memory address (if any).
  BxExecutePtr_tR execute;

  union {
    BxExecutePtr_tR execute2;
    bxInstruction_c *next;
  } handlers;

  BxResolvePtr_tR ResolveModrm;

  struct {
    // 15...0 opcode
    Bit16u ia_opcode;

    //  7...4 (unused)
    //  3...0 ilen (0..15)
    Bit8u ilen;

    //  7...6 VEX Vector Length (0=no VL, 1=128 bit, 2=256 bit)
    //        repUsed (0=none, 2=0xF2, 3=0xF3)
    //  5...5 extend8bit
    //  4...4 mod==c0 (modrm)
    //  3...3 os64
    //  2...2 os32
    //  1...1 as64
    //  0...0 as32
    Bit8u metaInfo1;
  } metaInfo;

#define BX_INSTR_METADATA_DST   0
#define BX_INSTR_METADATA_SRC1  1
#define BX_INSTR_METADATA_SRC2  2
#define BX_INSTR_METADATA_SRC3  3
#define BX_INSTR_METADATA_SEG   4
#define BX_INSTR_METADATA_BASE  5
#define BX_INSTR_METADATA_INDEX 6
#define BX_INSTR_METADATA_SCALE 7

  // using 5-bit field for registers (16 regs in 64-bit, RIP, NIL)
  Bit8u metaData[8];

  union {
    // Form (longest case): [opcode+modrm+sib/displacement32/immediate32]
    struct {
      union {
        Bit32u Id;
        Bit16u Iw;
        Bit8u  Ib;
      };
      union {
        Bit16u displ16u; // for 16-bit modrm forms
        Bit32u displ32u; // for 32-bit modrm forms

        Bit32u Id2;
        Bit16u Iw2;
        Bit8u  Ib2;
      };
    } modRMForm;

#if BX_SUPPORT_X86_64
    struct {
      Bit64u   Iq;  // for MOV Rx,imm64
    } IqForm;
#endif
  };

#ifdef BX_INSTR_STORE_OPCODE_BYTES
  Bit8u opcode_bytes[16];

  BX_CPP_INLINE const Bit8u* get_opcode_bytes(void) const {
    return opcode_bytes;
  }

  BX_CPP_INLINE void set_opcode_bytes(const Bit8u *opcode) {
    memcpy(opcode_bytes, opcode, ilen());
  }
#endif

  BX_CPP_INLINE BxExecutePtr_tR execute2(void) const {
    return handlers.execute2;
  }

  BX_CPP_INLINE unsigned seg(void) const {
    return metaData[BX_INSTR_METADATA_SEG];
  }
  BX_CPP_INLINE void setSeg(unsigned val) {
    metaData[BX_INSTR_METADATA_SEG] = val;
  }

  BX_CPP_INLINE void setFoo(unsigned foo) {
    // none of x87 instructions has immediate
    modRMForm.Iw = foo;
  }
  BX_CPP_INLINE unsigned foo() const {
    return modRMForm.Iw;
  }
  BX_CPP_INLINE unsigned b1() const {
    return modRMForm.Iw >> 8;
  }

  BX_CPP_INLINE void setSibScale(unsigned scale) {
    metaData[BX_INSTR_METADATA_SCALE] = scale;
  }
  BX_CPP_INLINE unsigned sibScale() const {
    return metaData[BX_INSTR_METADATA_SCALE];
  }
  BX_CPP_INLINE void setSibIndex(unsigned index) {
    metaData[BX_INSTR_METADATA_INDEX] = index;
  }
  BX_CPP_INLINE unsigned sibIndex() const {
    return metaData[BX_INSTR_METADATA_INDEX];
  }
  BX_CPP_INLINE void setSibBase(unsigned base) {
    metaData[BX_INSTR_METADATA_BASE] = base;
  }
  BX_CPP_INLINE unsigned sibBase() const {
    return metaData[BX_INSTR_METADATA_BASE];
  }
  BX_CPP_INLINE Bit32s displ32s() const { return (Bit32s) modRMForm.displ32u; }
  BX_CPP_INLINE Bit16s displ16s() const { return (Bit16s) modRMForm.displ16u; }
  BX_CPP_INLINE Bit32u Id() const  { return modRMForm.Id; }
  BX_CPP_INLINE Bit16u Iw() const  { return modRMForm.Iw; }
  BX_CPP_INLINE Bit8u  Ib() const  { return modRMForm.Ib; }
  BX_CPP_INLINE Bit16u Iw2() const { return modRMForm.Iw2; }
  BX_CPP_INLINE Bit8u  Ib2() const { return modRMForm.Ib2; }
#if BX_SUPPORT_X86_64
  BX_CPP_INLINE Bit64u Iq() const  { return IqForm.Iq; }
#endif

  // Info in the metaInfo field.
  // Note: the 'L' at the end of certain flags, means the value returned
  // is for Logical comparisons, eg if (i->os32L() && i->as32L()).  If you
  // want a bx_bool value, use os32B() etc.  This makes for smaller
  // code, when a strict 0 or 1 is not necessary.
  BX_CPP_INLINE void init(unsigned os32, unsigned as32, unsigned os64, unsigned as64)
  {
    metaInfo.metaInfo1 = (os32<<2) | (os64<<3) | (as32<<0) | (as64<<1); // VL = 0
  }

  BX_CPP_INLINE unsigned os32L(void) const {
    return metaInfo.metaInfo1 & (1<<2);
  }
  BX_CPP_INLINE void setOs32B(unsigned bit) {
    metaInfo.metaInfo1 = (metaInfo.metaInfo1 & ~(1<<2)) | (bit<<2);
  }
  BX_CPP_INLINE void assertOs32(void) {
    metaInfo.metaInfo1 |= (1<<2);
  }

#if BX_SUPPORT_X86_64
  BX_CPP_INLINE unsigned os64L(void) const {
    return metaInfo.metaInfo1 & (1<<3);
  }
  BX_CPP_INLINE void assertOs64(void) {
    metaInfo.metaInfo1 |= (1<<3);
  }
#else
  BX_CPP_INLINE unsigned os64L(void) const { return 0; }
#endif


  BX_CPP_INLINE unsigned as32L(void) const {
    return metaInfo.metaInfo1 & 0x1;
  }
  BX_CPP_INLINE void setAs32B(unsigned bit) {
    metaInfo.metaInfo1 = (metaInfo.metaInfo1 & ~0x1) | (bit);
  }

#if BX_SUPPORT_X86_64
  BX_CPP_INLINE unsigned as64L(void) const {
    return metaInfo.metaInfo1 & (1<<1);
  }
  BX_CPP_INLINE void clearAs64(void) {
    metaInfo.metaInfo1 &= ~(1<<1);
  }
#else
  BX_CPP_INLINE unsigned as64L(void) const { return 0; }
#endif

  BX_CPP_INLINE unsigned asize(void) const {
    return metaInfo.metaInfo1 & 0x3;
  }
  BX_CPP_INLINE bx_address asize_mask(void) const {
    return bx_asize_mask[asize()];
  }

#if BX_SUPPORT_X86_64
  BX_CPP_INLINE unsigned extend8bitL(void) const {
    return metaInfo.metaInfo1 & (1<<5);
  }
  BX_CPP_INLINE void assertExtend8bit(void) {
    metaInfo.metaInfo1 |= (1<<5);
  }
#endif

  BX_CPP_INLINE unsigned ilen(void) const {
    return metaInfo.ilen;
  }
  BX_CPP_INLINE void setILen(unsigned ilen) {
    metaInfo.ilen = ilen;
  }

  BX_CPP_INLINE unsigned getIaOpcode(void) const {
    return metaInfo.ia_opcode;
  }
  BX_CPP_INLINE void setIaOpcode(Bit16u op) {
    metaInfo.ia_opcode = op;
  }
  BX_CPP_INLINE const char* getIaOpcodeName(void) const {
    return get_bx_opcode_name(getIaOpcode());
  }

  BX_CPP_INLINE unsigned repUsedL(void) const {
    return metaInfo.metaInfo1 >> 6;
  }
  BX_CPP_INLINE unsigned repUsedValue(void) const {
    return metaInfo.metaInfo1 >> 6;
  }
  BX_CPP_INLINE void setRepUsed(unsigned value) {
    metaInfo.metaInfo1 = (metaInfo.metaInfo1 & 0x3f) | (value << 6);
  }

  BX_CPP_INLINE unsigned getVL(void) const {
#if BX_SUPPORT_AVX
    return metaInfo.metaInfo1 >> 6;
#else
    return 0;
#endif
  }
  BX_CPP_INLINE void setVL(unsigned value) {
    metaInfo.metaInfo1 = (metaInfo.metaInfo1 & 0x3f) | (value << 6);
  }

  BX_CPP_INLINE void setSrcReg(unsigned src, unsigned reg) {
    metaData[src] = reg;
  }

  BX_CPP_INLINE unsigned dst() const {
    return metaData[BX_INSTR_METADATA_DST];
  }
 
  BX_CPP_INLINE unsigned src1() const {
    return metaData[BX_INSTR_METADATA_SRC1];
  }
  BX_CPP_INLINE unsigned src2() const {
    return metaData[BX_INSTR_METADATA_SRC2];
  }
  BX_CPP_INLINE unsigned src3() const {
    return metaData[BX_INSTR_METADATA_SRC3];
  }

  BX_CPP_INLINE unsigned src() const { return src1(); }

  BX_CPP_INLINE unsigned modC0() const
  {
    // This is a cheaper way to test for modRM instructions where
    // the mod field is 0xc0.  FetchDecode flags this condition since
    // it is quite common to be tested for.
    return metaInfo.metaInfo1 & (1<<4);
  }
  BX_CPP_INLINE void assertModC0()
  {
    metaInfo.metaInfo1 |= (1<<4);
  }

#if BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS
  BX_CPP_INLINE bxInstruction_c* getNextTrace() const {
    return handlers.next;
  }
  BX_CPP_INLINE void setNextTrace(bxInstruction_c* iptr) {
    handlers.next = iptr;
  }
#endif

};
// <TAG-CLASS-INSTRUCTION-END>

enum {
#define bx_define_opcode(a, b, c, d, s1, s2, s3, s4, e) a,
#include "ia_opcodes.h"
   BX_IA_LAST
};
#undef  bx_define_opcode

#endif
