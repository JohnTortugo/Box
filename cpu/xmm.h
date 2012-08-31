/////////////////////////////////////////////////////////////////////////
// $Id: xmm.h 10737 2011-10-19 20:54:04Z sshwarts $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003-2011 Stanislav Shwartsman
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

#ifndef BX_SSE_EXTENSIONS_H
#define BX_SSE_EXTENSIONS_H

/* XMM REGISTER */

typedef union bx_xmm_reg_t {
   Bit8s   xmm_sbyte[16];
   Bit16s  xmm_s16[8];
   Bit32s  xmm_s32[4];
   Bit64s  xmm_s64[2];
   Bit8u   xmm_ubyte[16];
   Bit16u  xmm_u16[8];
   Bit32u  xmm_u32[4];
   Bit64u  xmm_u64[2];
} BxPackedXmmRegister;

#ifdef BX_BIG_ENDIAN
#define xmm64s(i)   xmm_s64[1 - (i)]
#define xmm32s(i)   xmm_s32[3 - (i)]
#define xmm16s(i)   xmm_s16[7 - (i)]
#define xmmsbyte(i) xmm_sbyte[15 - (i)]
#define xmmubyte(i) xmm_ubyte[15 - (i)]
#define xmm16u(i)   xmm_u16[7 - (i)]
#define xmm32u(i)   xmm_u32[3 - (i)]
#define xmm64u(i)   xmm_u64[1 - (i)]
#else
#define xmm64s(i)   xmm_s64[(i)]
#define xmm32s(i)   xmm_s32[(i)]
#define xmm16s(i)   xmm_s16[(i)]
#define xmmsbyte(i) xmm_sbyte[(i)]
#define xmmubyte(i) xmm_ubyte[(i)]
#define xmm16u(i)   xmm_u16[(i)]
#define xmm32u(i)   xmm_u32[(i)]
#define xmm64u(i)   xmm_u64[(i)]
#endif

/* AVX REGISTER */

enum bx_avx_vector_length {
  BX_NO_VL,
  BX_VL128,
  BX_VL256
};

#if BX_SUPPORT_AVX
#define BX_VLMAX BX_VL256
#else
#define BX_VLMAX BX_VL128
#endif

#if BX_SUPPORT_AVX

typedef union bx_avx_reg_t {
   Bit8s   avx_sbyte[32];
   Bit16s  avx_s16[16];
   Bit32s  avx_s32[8];
   Bit64s  avx_s64[4];
   Bit8u   avx_ubyte[32];
   Bit16u  avx_u16[16];
   Bit32u  avx_u32[8];
   Bit64u  avx_u64[4];
   BxPackedXmmRegister avx_v128[2];
} BxPackedAvxRegister;

#ifdef BX_BIG_ENDIAN
#define avx64s(i)   avx_s64[3 - (i)]
#define avx32s(i)   avx_s32[7 - (i)]
#define avx16s(i)   avx_s16[15 - (i)]
#define avxsbyte(i) avx_sbyte[31 - (i)]
#define avxubyte(i) avx_ubyte[31 - (i)]
#define avx16u(i)   avx_u16[15 - (i)]
#define avx32u(i)   avx_u32[7 - (i)]
#define avx64u(i)   avx_u64[3 - (i)]
#define avx128(i)   avx_v128[1 - (i)]
#else
#define avx64s(i)   avx_s64[(i)]
#define avx32s(i)   avx_s32[(i)]
#define avx16s(i)   avx_s16[(i)]
#define avxsbyte(i) avx_sbyte[(i)]
#define avxubyte(i) avx_ubyte[(i)]
#define avx16u(i)   avx_u16[(i)]
#define avx32u(i)   avx_u32[(i)]
#define avx64u(i)   avx_u64[(i)]
#define avx128(i)   avx_v128[(i)]
#endif

#endif

#if BX_SUPPORT_X86_64
#  define BX_XMM_REGISTERS 16
#else
#  define BX_XMM_REGISTERS 8
#endif

#if BX_SUPPORT_AVX

/* read XMM register */
#define BX_READ_XMM_REG(index) ((BX_CPU_THIS_PTR vmm[index]).avx128(0))

#else

/* read XMM register */
#define BX_READ_XMM_REG(index) ((BX_CPU_THIS_PTR vmm[index]))

#endif

/* read only high 64 bit of the register */
#define BX_READ_XMM_REG_HI_QWORD(index) \
    ((BX_READ_XMM_REG(index)).xmm64u(1))

/* read only low 64 bit of the register */
#define BX_READ_XMM_REG_LO_QWORD(index) \
    ((BX_READ_XMM_REG(index)).xmm64u(0))

/* read only low 32 bit of the register */
#define BX_READ_XMM_REG_LO_DWORD(index) \
    ((BX_READ_XMM_REG(index)).xmm32u(0))

/* read only low 16 bit of the register */
#define BX_READ_XMM_REG_LO_WORD(index) \
    ((BX_READ_XMM_REG(index)).xmm16u(0))

/* read only low 8 bit of the register */
#define BX_READ_XMM_REG_LO_BYTE(index) \
    ((BX_READ_XMM_REG(index)).xmmubyte(0))

/* short names for above macroses */
#define BX_XMM_REG_HI_QWORD BX_READ_XMM_REG_HI_QWORD
#define BX_XMM_REG_LO_QWORD BX_READ_XMM_REG_LO_QWORD
#define BX_XMM_REG_LO_DWORD BX_READ_XMM_REG_LO_DWORD

#define BX_XMM_REG BX_READ_XMM_REG

/* store only high 64 bit of the register, rest of the register unchanged */
#define BX_WRITE_XMM_REG_HI_QWORD(index, reg64) \
    { (BX_XMM_REG(index)).xmm64u(1) = (reg64); }

/* store only low 64 bit of the register, rest of the register unchanged */
#define BX_WRITE_XMM_REG_LO_QWORD(index, reg64) \
    { (BX_XMM_REG(index)).xmm64u(0) = (reg64); }

/* store only low 32 bit of the register, rest of the register unchanged */
#define BX_WRITE_XMM_REG_LO_DWORD(index, reg32) \
    { (BX_XMM_REG(index)).xmm32u(0) = (reg32); }

/* store only low 16 bit of the register, rest of the register unchanged */
#define BX_WRITE_XMM_REG_LO_WORD(index, reg16) \
    { (BX_XMM_REG(index)).xmm16u(0) = (reg16); }

/* store only low 8 bit of the register, rest of the register unchanged */
#define BX_WRITE_XMM_REG_LO_BYTE(index, reg8) \
    { (BX_XMM_REG(index)).xmmubyte(0) = (reg8); }

/* store XMM register */
#define BX_WRITE_XMM_REG(index, reg) \
    { (BX_XMM_REG(index)) = (reg); }

#if BX_SUPPORT_AVX

/* read AVX register */
#define BX_READ_AVX_REG(index) (BX_CPU_THIS_PTR vmm[index])

#define BX_AVX_REG BX_READ_AVX_REG

/* read AVX register */
#define BX_READ_AVX_REG_LINE(index, line) \
     ((BX_READ_AVX_REG(index)).avx128(line))

/* write AVX register */
#define BX_WRITE_AVX_REG(index, reg) \
    { (BX_READ_AVX_REG(index)) = (reg); }

/* clear upper part of AVX register */
#define BX_CLEAR_AVX_HIGH(index) \
    { (BX_READ_AVX_REG(index)).avx64u(2) = (BX_READ_AVX_REG(index)).avx64u(3) = 0; }

/* write AVX register and potentialy clear upper part of the register */
#define BX_WRITE_AVX_REGZ(index, reg, vlen) \
    { BX_CPU_THIS_PTR vmm[index] = (reg);   \
      if (vlen == BX_VL128) BX_CLEAR_AVX_HIGH(index); }

/* write XMM register while clearing upper part of AVX register */
#define BX_WRITE_XMM_REG_CLEAR_HIGH(index, reg) \
    { BX_XMM_REG(index) = (reg); BX_CLEAR_AVX_HIGH(index); }

/* write XMM register while clearing upper part of AVX register */
#define BX_WRITE_XMM_REGZ(index, reg, vlen) \
    { (BX_XMM_REG(index)) = (reg);   \
      if (vlen == BX_VL128) BX_CLEAR_AVX_HIGH(index); }

#else

/* write XMM register while clearing upper part of AVX register */
#define BX_WRITE_XMM_REG_CLEAR_HIGH(index, reg) \
    BX_WRITE_XMM_REG(index, reg)

/* write XMM register while clearing upper part of AVX register */
#define BX_WRITE_XMM_REGZ(index, reg, vlen) \
    BX_WRITE_XMM_REG(index, reg)

#endif

/* MXCSR REGISTER */

/* 31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16
 * ==|==|=====|==|==|==|==|==|==|==|==|==|==|==|==  (reserved)
 *  0| 0| 0| 0| 0| 0| 0| 0| 0| 0| 0| 0| 0| 0|MM| 0
 *
 * 15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0
 * ==|==|=====|==|==|==|==|==|==|==|==|==|==|==|==
 * FZ| R C |PM|UM|OM|ZM|DM|IM|DZ|PE|UE|OE|ZE|DE|IE
 */

/* MXCSR REGISTER FIELDS DESCRIPTION */

/*
 * IE  0    Invalid-Operation Exception             0
 * DE  1    Denormalized-Operand Exception          0
 * ZE  2    Zero-Divide Exception                   0
 * OE  3    Overflow Exception                      0
 * UE  4    Underflow Exception                     0
 * PE  5    Precision Exception                     0
 * DZ  6    Denormals are Zeros                     0
 * IM  7    Invalid-Operation Exception Mask        1
 * DM  8    Denormalized-Operand Exception Mask     1
 * ZM  9    Zero-Divide Exception Mask              1
 * OM 10    Overflow Exception Mask                 1
 * UM 11    Underflow Exception Mask                1
 * PM 12    Precision Exception Mask                1
 * RC 13-14 Floating-Point Rounding Control         00
 * FZ 15    Flush-to-Zero for Masked Underflow      0
 * RZ 16    Reserved                                0
 * MM 17    Misaligned Exception Mask               0
 */

#define MXCSR_EXCEPTIONS                 0x0000003F
#define MXCSR_DAZ                        0x00000040
#define MXCSR_MASKED_EXCEPTIONS          0x00001F80
#define MXCSR_ROUNDING_CONTROL           0x00006000
#define MXCSR_FLUSH_MASKED_UNDERFLOW     0x00008000
#define MXCSR_MISALIGNED_EXCEPTION_MASK  0x00020000

#define MXCSR_IE 0x00000001
#define MXCSR_DE 0x00000002
#define MXCSR_ZE 0x00000004
#define MXCSR_OE 0x00000008
#define MXCSR_UE 0x00000010
#define MXCSR_PE 0x00000020

#define MXCSR_IM 0x00000080
#define MXCSR_DM 0x00000100
#define MXCSR_ZM 0x00000200
#define MXCSR_OM 0x00000400
#define MXCSR_UM 0x00000800
#define MXCSR_PM 0x00001000

#define MXCSR_RESET 0x00001F80  /* reset value of the MXCSR register */

struct BOCHSAPI bx_mxcsr_t
{
  Bit32u mxcsr;

  bx_mxcsr_t (Bit32u val = MXCSR_RESET)
	: mxcsr(val) {}

#define IMPLEMENT_MXCSR_ACCESSOR(name, bitmask, bitnum)        \
  int get_##name () const {                                    \
    return (mxcsr & (bitmask)) >> (bitnum);                    \
  }

  IMPLEMENT_MXCSR_ACCESSOR(exceptions_masks, MXCSR_MASKED_EXCEPTIONS, 7);
  IMPLEMENT_MXCSR_ACCESSOR(DAZ, MXCSR_DAZ, 6);
  IMPLEMENT_MXCSR_ACCESSOR(rounding_mode, MXCSR_ROUNDING_CONTROL, 13);
  IMPLEMENT_MXCSR_ACCESSOR(flush_masked_underflow, MXCSR_FLUSH_MASKED_UNDERFLOW, 15);
  IMPLEMENT_MXCSR_ACCESSOR(MM, MXCSR_MISALIGNED_EXCEPTION_MASK, 17);

  IMPLEMENT_MXCSR_ACCESSOR(IE, MXCSR_IE, 0);
  IMPLEMENT_MXCSR_ACCESSOR(DE, MXCSR_DE, 1);
  IMPLEMENT_MXCSR_ACCESSOR(ZE, MXCSR_ZE, 2);
  IMPLEMENT_MXCSR_ACCESSOR(OE, MXCSR_OE, 3);
  IMPLEMENT_MXCSR_ACCESSOR(UE, MXCSR_UE, 4);
  IMPLEMENT_MXCSR_ACCESSOR(PE, MXCSR_PE, 5);

  IMPLEMENT_MXCSR_ACCESSOR(IM, MXCSR_IM, 7);
  IMPLEMENT_MXCSR_ACCESSOR(DM, MXCSR_DM, 8);
  IMPLEMENT_MXCSR_ACCESSOR(ZM, MXCSR_ZM, 9);
  IMPLEMENT_MXCSR_ACCESSOR(OM, MXCSR_OM, 10);
  IMPLEMENT_MXCSR_ACCESSOR(UM, MXCSR_UM, 11);
  IMPLEMENT_MXCSR_ACCESSOR(PM, MXCSR_PM, 12);

  void set_exceptions(int status) {
    mxcsr |= (status & MXCSR_EXCEPTIONS);
  }

};

#if defined(NEED_CPU_REG_SHORTCUTS)
  #define MXCSR             (BX_CPU_THIS_PTR mxcsr)
  #define BX_MXCSR_REGISTER (BX_CPU_THIS_PTR mxcsr.mxcsr)
  #define MXCSR_MASK        (BX_CPU_THIS_PTR mxcsr_mask)
#endif

/* INTEGER SATURATION */

/*
 * SaturateWordSToByteS  converts  a  signed  16-bit  value  to a signed
 * 8-bit value.  If  the signed 16-bit value is less than -128, it is 
 * represented by the saturated value  -128  (0x80).  If it is  greater 
 * than 127, it is represented by the saturated value 127 (0x7F).
*/
BX_CPP_INLINE Bit8s BX_CPP_AttrRegparmN(1) SaturateWordSToByteS(Bit16s value)
{
  if(value < -128) return -128;
  if(value >  127) return  127;
  return (Bit8s) value;
}

/*
 * SaturateDwordSToWordS  converts  a  signed  32-bit  value  to a signed
 * 16-bit  value.  If  the signed 32-bit value is less than -32768, it is
 * represented  by  the saturated value -32768 (0x8000). If it is greater
 * than 32767, it is represented by the saturated value 32767 (0x7FFF).
*/
BX_CPP_INLINE Bit16s BX_CPP_AttrRegparmN(1) SaturateDwordSToWordS(Bit32s value)
{
  if(value < -32768) return -32768;
  if(value >  32767) return  32767;
  return (Bit16s) value;
}

/*
 * SaturateQwordSToDwordS  converts  a  signed  64-bit  value  to a signed
 * 32-bit value. If the signed 64-bit value is less than -2147483648, it
 * is represented by the saturated value -2147483648 (0x80000000). If it
 * is greater than 2147483647, it is represented by the saturated value
 * 2147483647 (0x7FFFFFFF).
*/
BX_CPP_INLINE Bit32s BX_CPP_AttrRegparmN(1) SaturateQwordSToDwordS(Bit64s value)
{
  if(value < BX_CONST64(-2147483648)) return BX_CONST64(-2147483648);
  if(value > 2147483647) return 2147483647;
  return (Bit32s) value;
}

/*
 * SaturateWordSToByteU  converts  a  signed  16-bit value to an unsigned
 * 8-bit  value.  If  the  signed  16-bit  value  is less than zero it is
 * represented  by  the saturated value zero (0x00).If it is greater than
 * 255 it is represented by the saturated value 255 (0xFF).
*/
BX_CPP_INLINE Bit8u BX_CPP_AttrRegparmN(1) SaturateWordSToByteU(Bit16s value)
{
  if(value < 0) return 0;
  if(value > 255) return 255;
  return (Bit8u) value;
}

/*
 * SaturateDwordSToWordU  converts  a  signed 32-bit value to an unsigned
 * 16-bit  value.  If  the  signed  32-bit value is less than zero, it is
 * represented  by  the  saturated  value zero (0x0000). If it is greater
 * than  65535, it is represented by the saturated value 65535 (0xFFFF).
*/
BX_CPP_INLINE Bit16u BX_CPP_AttrRegparmN(1) SaturateDwordSToWordU(Bit32s value)
{
  if(value < 0) return 0;
  if(value > 65535) return 65535;
  return (Bit16u) value;
}

/*
 * SaturateQwordSToDwordU  converts  a  signed 64-bit value to an unsigned
 * 32-bit  value.  If  the  signed  64-bit value is less than  zero, it is
 * represented by the saturated value zero (0x00000000). If it is  greater
 * than  4294967295, it is represented  by the  saturated value 4294967295
 * (0xFFFFFFFF).
*/
BX_CPP_INLINE Bit32u BX_CPP_AttrRegparmN(1) SaturateDwordSToWordU(Bit64s value)
{
  if(value < 0) return 0;
  if(value > BX_CONST64(4294967295)) return BX_CONST64(4294967295);
  return (Bit32u) value;
}

#endif
