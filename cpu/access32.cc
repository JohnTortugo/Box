/////////////////////////////////////////////////////////////////////////
// $Id: access32.cc 11329 2012-08-08 20:43:07Z sshwarts $
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

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_byte_32(unsigned s, Bit32u offset, Bit8u data)
{
  Bit32u laddr;
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessWOK) {
    if (offset <= seg->cache.u.segment.limit_scaled) {
accessOK:
      laddr = get_laddr32(s, offset);

      bx_mem.write_byte(laddr,data);
      return;
    }
    else {
      BX_ERROR(("write_virtual_byte_32(): segment limit violation"));
      exception(int_number(s), 0);
    }
  }

  if (!write_virtual_checks(seg, offset, 1))
    exception(int_number(s), 0);
  goto accessOK;
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_word_32(unsigned s, Bit32u offset, Bit16u data)
{
  Bit32u laddr;
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessWOK) {
    if (offset < seg->cache.u.segment.limit_scaled) {
accessOK:
      laddr = get_laddr32(s, offset);

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check()) {
        if (laddr & 1) {
          BX_ERROR(("write_virtual_word_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      bx_mem.write_word(laddr, data);
      return;
    }
    else {
      BX_ERROR(("write_virtual_word_32(): segment limit violation"));
      exception(int_number(s), 0);
    }
  }

  if (!write_virtual_checks(seg, offset, 2))
    exception(int_number(s), 0);
  goto accessOK;
}

void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_dword_32(unsigned s, Bit32u offset, Bit32u data)
{
  Bit32u laddr;
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessWOK) {
    if (offset < (seg->cache.u.segment.limit_scaled-2)) {
accessOK:
      laddr = get_laddr32(s, offset);

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check()) {
        if (laddr & 3) {
          BX_ERROR(("write_virtual_dword_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      bx_mem.write_dword(laddr,data);
      return;
    }
    else {
      BX_ERROR(("write_virtual_dword_32(): segment limit violation"));
      exception(int_number(s), 0);
    }
  }

  if (!write_virtual_checks(seg, offset, 4))
    exception(int_number(s), 0);
  goto accessOK;
}

void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_qword_32(unsigned s, Bit32u offset, Bit64u data)
{
  Bit32u laddr;
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessWOK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-7)) {
accessOK:
      laddr = get_laddr32(s, offset);

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check()) {
        if (laddr & 7) {
          BX_ERROR(("write_virtual_qword_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      bx_mem.write_qword(laddr,data);
      return;
    }
    else {
      BX_ERROR(("write_virtual_qword_32(): segment limit violation"));
      exception(int_number(s), 0);
    }
  }

  if (!write_virtual_checks(seg, offset, 8))
    exception(int_number(s), 0);
  goto accessOK;
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_dqword_32(unsigned s, Bit32u offset, const BxPackedXmmRegister *data)
{
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessWOK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-15)) {
accessOK:
          Bit32u laddr = get_laddr32(s, offset);
          bx_mem.write_qword(laddr,   data->xmm64u(0));
          bx_mem.write_qword(laddr+8,   data->xmm64u(0));
      return;
    }
    else {
      BX_ERROR(("write_virtual_dqword_32(): segment limit violation"));
      exception(int_number(s), 0);
    }
  }

  if (!write_virtual_checks(seg, offset, 16))
    exception(int_number(s), 0);
  goto accessOK;
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_dqword_aligned_32(unsigned s, Bit32u offset, const BxPackedXmmRegister *data)
{
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  Bit32u laddr = get_laddr32(s, offset);
  // must check alignment here because #GP on misaligned access is higher
  // priority than other segment related faults
  if (laddr & 15) {
    BX_ERROR(("write_virtual_dqword_aligned_32(): #GP misaligned access"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (seg->cache.valid & SegAccessWOK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-15)) {
accessOK:
      bx_mem.write_qword(laddr,   data->xmm64u(0));
      bx_mem.write_qword(laddr+8,   data->xmm64u(0));
      return;
    }
    else {
      BX_ERROR(("write_virtual_dqword_aligned_32(): segment limit violation"));
      exception(int_number(s), 0);
    }
  }

  if (!write_virtual_checks(seg, offset, 16))
    exception(int_number(s), 0);
  goto accessOK;
}

#if BX_SUPPORT_AVX

void BX_CPU_C::write_virtual_dword_vector_32(unsigned s, Bit32u offset, unsigned elements, const BxPackedAvxRegister *data)
{
  BX_PANIC((" write_virtual_dword_vector_32: not implemented"));
  BX_ASSERT(elements > 0);

  Bit32u laddr;
  unsigned len = elements << 2;
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessWOK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-len+1)) {
accessOK:
          for (unsigned n=0; n < elements; n++) {
            bx_mem.write_dword(s, offset,   data->avx32u(n));
            offset+=4;
          }
          return;
        }
      }

      access_write_linear(laddr, len, CPL, (void *) data);
      return;
    }
    else {
      BX_ERROR(("write_virtual_dword_vector_32(): segment limit violation"));
      exception(int_number(s), 0);
    }
  }

  if (!write_virtual_checks(seg, offset, len))
    exception(int_number(s), 0);
  goto accessOK;
}

void BX_CPU_C::write_virtual_dword_vector_aligned_32(unsigned s, Bit32u offset, unsigned elements, const BxPackedAvxRegister *data)
{
  BX_PANIC((" write_virtual_dword_vector_aligned_32: not implemented"));
  BX_ASSERT(elements > 0);

  unsigned len = elements << 2;
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  Bit32u laddr = get_laddr32(s, offset);
  // must check alignment here because #GP on misaligned access is higher
  // priority than other segment related faults
  if (laddr & (len-1)) {
    BX_ERROR(("write_virtual_dword_vector_aligned_32(): #GP misaligned access"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (seg->cache.valid & SegAccessWOK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-len+1)) {
accessOK:
         for (unsigned n=0; n < elements; n++) {
           bx_mem.write_dword(s, offset,   data->avx32u(n));
           offset+=4;
         }
         return;
        }
      }
      access_write_linear(laddr, len, CPL, (void *) data);
      return;
    }
    else {
      BX_ERROR(("write_virtual_dword_vector_aligned_32(): segment limit violation"));
      exception(int_number(s), 0);
    }
  }

  if (!write_virtual_checks(seg, offset, len))
    exception(int_number(s), 0);
  goto accessOK;
}

#endif // BX_SUPPORT_AVX

Bit8u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_virtual_byte_32(unsigned s, Bit32u offset)
{
Bit32u laddr;
bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];
Bit16u data;

BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

if (seg->cache.valid & SegAccessROK) {
  if (offset < seg->cache.u.segment.limit_scaled) {
accessOK:
    Bit32u laddr = get_laddr32(s, offset);

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
    if (BX_CPU_THIS_PTR alignment_check()) {
      if (laddr & 1) {
        BX_ERROR(("read_virtual_word_32(): #AC misaligned access"));
        exception(BX_AC_EXCEPTION, 0);
      }
    }
#endif

    return bx_mem.read_byte(laddr);
  }
  else {
    BX_ERROR(("read_virtual_word_32(): segment limit violation"));
    exception(int_number(s), 0);
  }
}

if (!read_virtual_checks(seg, offset, 2))
  exception(int_number(s), 0);
goto accessOK;
}

Bit16u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_virtual_word_32(unsigned s, Bit32u offset)
{
Bit32u laddr;
bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];
Bit16u data;

BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

if (seg->cache.valid & SegAccessROK) {
  if (offset < seg->cache.u.segment.limit_scaled) {
accessOK:
    Bit32u laddr = get_laddr32(s, offset);

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
    if (BX_CPU_THIS_PTR alignment_check()) {
      if (laddr & 1) {
        BX_ERROR(("read_virtual_word_32(): #AC misaligned access"));
        exception(BX_AC_EXCEPTION, 0);
      }
    }
#endif

    return bx_mem.read_word(laddr);
  }
  else {
    BX_ERROR(("read_virtual_word_32(): segment limit violation"));
    exception(int_number(s), 0);
  }
}

if (!read_virtual_checks(seg, offset, 2))
  exception(int_number(s), 0);
goto accessOK;
}

Bit32u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_virtual_dword_32(unsigned s, Bit32u offset)
{
bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];
Bit32u data;

BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

if (seg->cache.valid & SegAccessROK) {
  if (offset < (seg->cache.u.segment.limit_scaled-2)) {
accessOK:
    Bit32u laddr = get_laddr32(s, offset);

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
    if (BX_CPU_THIS_PTR alignment_check()) {
      if (laddr & 3) {
        BX_ERROR(("read_virtual_dword_32(): #AC misaligned access"));
        exception(BX_AC_EXCEPTION, 0);
      }
    }
#endif
    return bx_mem.read_dword(laddr);
  }
  else {
    BX_ERROR(("read_virtual_dword_32(): segment limit violation"));
    exception(int_number(s), 0);
  }
}

if (!read_virtual_checks(seg, offset, 4))
  exception(int_number(s), 0);
goto accessOK;
}

Bit64u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_virtual_qword_32(unsigned s, Bit32u offset)
{
bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

if (seg->cache.valid & SegAccessROK) {
  if (offset <= (seg->cache.u.segment.limit_scaled-7)) {
accessOK:
    Bit32u laddr = get_laddr32(s, offset);

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
    if (BX_CPU_THIS_PTR alignment_check()) {
      if (laddr & 7) {
        BX_ERROR(("read_virtual_qword_32(): #AC misaligned access"));
        exception(BX_AC_EXCEPTION, 0);
      }
    }
#endif

    return bx_mem.read_qword(laddr);
  }
  else {
    BX_ERROR(("read_virtual_qword_32(): segment limit violation"));
    exception(int_number(s), 0);
  }
}

if (!read_virtual_checks(seg, offset, 8))
  exception(int_number(s), 0);
goto accessOK;
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_virtual_dqword_32(unsigned s, Bit32u offset, BxPackedXmmRegister *data)
{
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessROK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-15)) {
accessOK:
      Bit32u laddr = get_laddr32(s,offset);
      data->xmm_u64[0] = bx_mem.read_qword(laddr);
      data->xmm_u64[1] = bx_mem.read_qword(laddr+8);
      return;
    }
    else {
      BX_ERROR(("read_virtual_dqword_32(): segment limit violation"));
      exception(int_number(s), 0);
    }
  }

  if (!read_virtual_checks(seg, offset, 16))
    exception(int_number(s), 0);
  goto accessOK;
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_virtual_dqword_aligned_32(unsigned s, Bit32u offset, BxPackedXmmRegister *data)
{
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  Bit32u laddr = get_laddr32(s, offset);
  // must check alignment here because #GP on misaligned access is higher
  // priority than other segment related faults
  if (laddr & 15) {
    BX_ERROR(("read_virtual_dqword_aligned_32(): #GP misaligned access"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (seg->cache.valid & SegAccessROK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-15)) {
accessOK:
      data->xmm_u64[0] = bx_mem.read_qword(laddr);
      data->xmm_u64[1] = bx_mem.read_qword(laddr+8);
      return;
    }
    else {
      BX_ERROR(("read_virtual_dqword_aligned_32(): segment limit violation"));
      exception(int_number(s), 0);
    }
  }

  if (!read_virtual_checks(seg, offset, 16))
    exception(int_number(s), 0);
  goto accessOK;
}

#if BX_SUPPORT_AVX

void BX_CPU_C::read_virtual_dword_vector_32(unsigned s, Bit32u offset, unsigned elements, BxPackedAvxRegister *data)
{
  BX_PANIC((" read_virtual_dword_vector_32: not implemented"));
  BX_ASSERT(elements > 0);

  Bit32u laddr;
  unsigned len = elements << 2;
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessROK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-len+1)) {
accessOK:
      for (unsigned n=0; n < elements; n++) {
        data-> bx_mem.read_dword(s,offset);
    	  ReadHostDWordFromLittleEndian(hostAddr, data->avx32u(n));
        offset+=4;
      }
      return;
    }
    else {
      BX_ERROR(("read_virtual_dword_vector_32(len=%d): segment limit violation", len));
      exception(int_number(s), 0);
    }
  }

  if (!read_virtual_checks(seg, offset, len))
    exception(int_number(s), 0);
  goto accessOK;
}

void BX_CPU_C::read_virtual_dword_vector_aligned_32(unsigned s, Bit32u offset, unsigned elements, BxPackedAvxRegister *data)
{
  BX_PANIC((" read_virtual_dword_vector_aligned_32: not implemented"));
  BX_ASSERT(elements > 0);

  unsigned len = elements << 2;
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  Bit32u laddr = get_laddr32(s, offset);
  // must check alignment here because #GP on misaligned access is higher
  // priority than other segment related faults
  if (laddr & (len-1)) {
    BX_ERROR(("read_virtual_dword_vector_aligned_32(): #GP misaligned access"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (seg->cache.valid & SegAccessROK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-len+1)) {
accessOK:
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
      Bit32u lpf = LPFOf(laddr);
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us read access
        // from this CPL.
        if (! (tlbEntry->accessBits & USER_PL)) { // Read this pl OK.
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
          for (unsigned n=0; n < elements; n++) {
            ReadHostDWordFromLittleEndian(hostAddr, data->avx32u(n));
            hostAddr++;
          }
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), len, CPL, BX_READ, (Bit8u*) data);
          return;
        }
      }
      access_read_linear(laddr, len, CPL, BX_READ, (void *) data);
      return;
    }
    else {
      BX_ERROR(("read_virtual_dword_vector_aligned_32(): segment limit violation"));
      exception(int_number(s), 0);
    }
  }

  if (!read_virtual_checks(seg, offset, len))
    exception(int_number(s), 0);
  goto accessOK;
}

#endif

//////////////////////////////////////////////////////////////
// special Read-Modify-Write operations                     //
// address translation info is kept across read/write calls //
//////////////////////////////////////////////////////////////

  Bit8u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_byte_32(unsigned s, Bit32u offset)
{
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessWOK) {
    if (offset <= seg->cache.u.segment.limit_scaled) {
accessOK:
      Bit32u laddr = get_laddr32(s,offset);
      return bx_mem.read_byte(laddr);
    }
    else {
      BX_ERROR(("read_RMW_virtual_byte_32(): segment limit violation"));
      exception(int_number(s), 0);
    }
  }

  if (!write_virtual_checks(seg, offset, 1))
    exception(int_number(s), 0);
  goto accessOK;
}

  Bit16u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_word_32(unsigned s, Bit32u offset)
{
  Bit32u laddr;
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessWOK) {
    if (offset < seg->cache.u.segment.limit_scaled) {
accessOK:
      laddr = get_laddr32(s, offset);

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check()) {
        if (laddr & 1) {
          BX_ERROR(("read_RMW_virtual_word_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      return bx_mem.read_word(laddr);
    }
    else {
      BX_ERROR(("read_RMW_virtual_word_32(): segment limit violation"));
      exception(int_number(s), 0);
    }
  }

  if (!write_virtual_checks(seg, offset, 2))
    exception(int_number(s), 0);
  goto accessOK;
}

  Bit32u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_dword_32(unsigned s, Bit32u offset)
{
  Bit32u laddr;
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessWOK) {
    if (offset < (seg->cache.u.segment.limit_scaled-2)) {
accessOK:
      laddr = get_laddr32(s, offset);

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check()) {
        if (laddr & 3) {
          BX_ERROR(("read_RMW_virtual_dword_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      return bx_mem.read_dword(laddr);
    }
    else {
      BX_ERROR(("read_RMW_virtual_dword_32(): segment limit violation"));
      exception(int_number(s), 0);
    }
  }

  if (!write_virtual_checks(seg, offset, 4))
    exception(int_number(s), 0);
  goto accessOK;
}

  Bit64u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_qword_32(unsigned s, Bit32u offset)
{
  Bit32u laddr;
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessWOK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-7)) {
accessOK:
      laddr = get_laddr32(s, offset);

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check()) {
        if (laddr & 7) {
          BX_ERROR(("read_RMW_virtual_qword_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      return bx_mem.read_qword(laddr);
    }
    else {
      BX_ERROR(("read_RMW_virtual_qword_32(): segment limit violation"));
      exception(int_number(s), 0);
    }
  }

  if (!write_virtual_checks(seg, offset, 8))
    exception(int_number(s), 0);
  goto accessOK;
}

  void BX_CPP_AttrRegparmN(1)
BX_CPU_C::write_RMW_virtual_byte(Bit8u val8)
{
   bx_mem.write_byte(val8);
}

  void BX_CPP_AttrRegparmN(1)
BX_CPU_C::write_RMW_virtual_word(Bit16u val16)
{
  bx_mem.write_word(val16);
}

  void BX_CPP_AttrRegparmN(1)
BX_CPU_C::write_RMW_virtual_dword(Bit32u val32)
{
  bx_mem.write_dword(val32);
}

  void BX_CPP_AttrRegparmN(1)
BX_CPU_C::write_RMW_virtual_qword(Bit64u val64)
{
  bx_mem.write_qword(val64);
}

//
// Write data to new stack, these methods are required for emulation
// correctness but not performance critical.
//

// assuming the write happens in legacy mode
void BX_CPU_C::write_new_stack_word_32(bx_segment_reg_t *seg, Bit32u offset, unsigned curr_pl, Bit16u data)
{
  Bit32u laddr;

  if (seg->cache.valid & SegAccessWOK) {
    if (offset < seg->cache.u.segment.limit_scaled) {
accessOK:
      laddr = (Bit32u)(seg->cache.u.segment.base) + offset;
      bx_bool user = (curr_pl == 3);

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check() && user) {
        if (laddr & 1) {
          BX_ERROR(("write_new_stack_word_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      bx_mem.write_word(laddr,data);
      return;
    }
    else {
      BX_ERROR(("write_new_stack_word_32(): segment limit violation"));
      exception(BX_SS_EXCEPTION, 
         seg->selector.rpl != CPL ? (seg->selector.value & 0xfffc) : 0);
    }
  }

  // add error code when segment violation occurs when pushing into new stack
  if (!write_virtual_checks(seg, offset, 2))
    exception(BX_SS_EXCEPTION, 
         seg->selector.rpl != CPL ? (seg->selector.value & 0xfffc) : 0);
  goto accessOK;
}

// assuming the write happens in legacy mode
void BX_CPU_C::write_new_stack_dword_32(bx_segment_reg_t *seg, Bit32u offset, unsigned curr_pl, Bit32u data)
{
  Bit32u laddr;

  if (seg->cache.valid & SegAccessWOK) {
    if (offset < (seg->cache.u.segment.limit_scaled-2)) {
accessOK:
      laddr = (Bit32u)(seg->cache.u.segment.base) + offset;
      bx_bool user = (curr_pl == 3);

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check() && user) {
        if (laddr & 3) {
          BX_ERROR(("write_new_stack_dword_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      bx_mem.write_dword(laddr,data);
      return;
    }
    else {
      BX_ERROR(("write_new_stack_dword_32(): segment limit violation"));
      exception(BX_SS_EXCEPTION, 
         seg->selector.rpl != CPL ? (seg->selector.value & 0xfffc) : 0);
    }
  }

  // add error code when segment violation occurs when pushing into new stack
  if (!write_virtual_checks(seg, offset, 4))
    exception(BX_SS_EXCEPTION, 
         seg->selector.rpl != CPL ? (seg->selector.value & 0xfffc) : 0);
  goto accessOK;
}

// assuming the write happens in legacy mode
void BX_CPU_C::write_new_stack_qword_32(bx_segment_reg_t *seg, Bit32u offset, unsigned curr_pl, Bit64u data)
{
  Bit32u laddr;

  if (seg->cache.valid & SegAccessWOK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-7)) {
accessOK:
      laddr = (Bit32u)(seg->cache.u.segment.base) + offset;
      bx_bool user = (curr_pl == 3);

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check() && user) {
        if (laddr & 7) {
          BX_ERROR(("write_new_stack_qword_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      bx_mem.write_qword(laddr,data);
      return;
    }
    else {
      BX_ERROR(("write_new_stack_qword_32(): segment limit violation"));
      exception(BX_SS_EXCEPTION, 
         seg->selector.rpl != CPL ? (seg->selector.value & 0xfffc) : 0);
    }
  }

  // add error code when segment violation occurs when pushing into new stack
  if (!write_virtual_checks(seg, offset, 8))
    exception(BX_SS_EXCEPTION, 
        seg->selector.rpl != CPL ? (seg->selector.value & 0xfffc) : 0);
  goto accessOK;
}
