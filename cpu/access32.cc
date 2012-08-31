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
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
      Bit32u lpf = LPFOf(laddr);
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us write access
        // from this CPL.
        if (! (tlbEntry->accessBits & (0x2 | USER_PL))) {
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 1, CPL, BX_WRITE, (Bit8u*) &data);
          Bit8u *hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
          pageWriteStampTable.decWriteStamp(pAddr, 1);
          *hostAddr = data;
          return;
        }
      }
      access_write_linear(laddr, 1, CPL, (void *) &data);
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
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 1);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
      Bit32u lpf = AlignedAccessLPFOf(laddr, (1 & BX_CPU_THIS_PTR alignment_check_mask));
#else
      Bit32u lpf = LPFOf(laddr);
#endif    
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us write access
        // from this CPL.
        if (! (tlbEntry->accessBits & (0x2 | USER_PL))) {
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 2, CPL, BX_WRITE, (Bit8u*) &data);
          Bit16u *hostAddr = (Bit16u*) (hostPageAddr | pageOffset);
          pageWriteStampTable.decWriteStamp(pAddr, 2);
          WriteHostWordToLittleEndian(hostAddr, data);
          return;
        }
      }

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check()) {
        if (laddr & 1) {
          BX_ERROR(("write_virtual_word_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      access_write_linear(laddr, 2, CPL, (void *) &data);
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
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 3);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
      Bit32u lpf = AlignedAccessLPFOf(laddr, (3 & BX_CPU_THIS_PTR alignment_check_mask));
#else
      Bit32u lpf = LPFOf(laddr);
#endif    
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us write access
        // from this CPL.
        if (! (tlbEntry->accessBits & (0x2 | USER_PL))) {
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 4, CPL, BX_WRITE, (Bit8u*) &data);
          Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
          pageWriteStampTable.decWriteStamp(pAddr, 4);
          WriteHostDWordToLittleEndian(hostAddr, data);
          return;
        }
      }

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check()) {
        if (laddr & 3) {
          BX_ERROR(("write_virtual_dword_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      access_write_linear(laddr, 4, CPL, (void *) &data);
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
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 7);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
      Bit32u lpf = AlignedAccessLPFOf(laddr, (7 & BX_CPU_THIS_PTR alignment_check_mask));
#else
      Bit32u lpf = LPFOf(laddr);
#endif    
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us write access
        // from this CPL.
        if (! (tlbEntry->accessBits & (0x2 | USER_PL))) {
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 8, CPL, BX_WRITE, (Bit8u*) &data);
          Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
          pageWriteStampTable.decWriteStamp(pAddr, 8);
          WriteHostQWordToLittleEndian(hostAddr, data);
          return;
        }
      }

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check()) {
        if (laddr & 7) {
          BX_ERROR(("write_virtual_qword_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      access_write_linear(laddr, 8, CPL, (void *) &data);
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

#if BX_CPU_LEVEL >= 6

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_dqword_32(unsigned s, Bit32u offset, const BxPackedXmmRegister *data)
{
  Bit32u laddr;
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessWOK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-15)) {
accessOK:
      laddr = get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 15);
      Bit32u lpf = LPFOf(laddr);
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us write access
        // from this CPL.
        if (! (tlbEntry->accessBits & (0x2 | USER_PL))) {
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 16, CPL, BX_WRITE, (Bit8u*) data);
          Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
          pageWriteStampTable.decWriteStamp(pAddr, 16);
          WriteHostQWordToLittleEndian(hostAddr,   data->xmm64u(0));
          WriteHostQWordToLittleEndian(hostAddr+1, data->xmm64u(1));
          return;
        }
      }

      access_write_linear(laddr, 16, CPL, (void *) data);
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
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
      Bit32u lpf = LPFOf(laddr);
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us write access
        // from this CPL.
        if (! (tlbEntry->accessBits & (0x2 | USER_PL))) {
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 16, CPL, BX_WRITE, (Bit8u*) data);
          Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
          pageWriteStampTable.decWriteStamp(pAddr, 16);
          WriteHostQWordToLittleEndian(hostAddr,   data->xmm64u(0));
          WriteHostQWordToLittleEndian(hostAddr+1, data->xmm64u(1));
          return;
        }
      }
      access_write_linear(laddr, 16, CPL, (void *) data);
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
  BX_ASSERT(elements > 0);

  Bit32u laddr;
  unsigned len = elements << 2;
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessWOK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-len+1)) {
accessOK:
      laddr = get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, len-1);
      Bit32u lpf = LPFOf(laddr);
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us write access
        // from this CPL.
        if (! (tlbEntry->accessBits & (0x2 | USER_PL))) {
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, len, CPL, BX_WRITE, (Bit8u*) data);
          Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
          pageWriteStampTable.decWriteStamp(pAddr, len);
          for (unsigned n=0; n < elements; n++) {
            WriteHostDWordToLittleEndian(hostAddr, data->avx32u(n));
            hostAddr++;
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
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
      Bit32u lpf = LPFOf(laddr);
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us write access
        // from this CPL.
        if (! (tlbEntry->accessBits & (0x2 | USER_PL))) {
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, len, CPL, BX_WRITE, (Bit8u*) data);
          Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
          pageWriteStampTable.decWriteStamp(pAddr, len);
          for (unsigned n=0; n < elements; n++) {
            WriteHostDWordToLittleEndian(hostAddr, data->avx32u(n));
            hostAddr++;
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

#endif

  Bit8u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_virtual_byte_32(unsigned s, Bit32u offset)
{
  Bit32u laddr;
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];
  Bit8u data;

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessROK) {
    if (offset <= seg->cache.u.segment.limit_scaled) {
accessOK:
      laddr = get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
      Bit32u lpf = LPFOf(laddr);
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us read access
        // from this CPL.
        if (! (tlbEntry->accessBits & USER_PL)) { // Read this pl OK.
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          Bit8u *hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
          data = *hostAddr;
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 1, CPL, BX_READ, (Bit8u*) &data);

          return data;
        }
      }
      access_read_linear(laddr, 1, CPL, BX_READ, (void *) &data);
      return data;
    }
    else {
      BX_ERROR(("read_virtual_byte_32(): segment limit violation"));
      exception(int_number(s), 0);
    }
  }

  if (!read_virtual_checks(seg, offset, 1))
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
      laddr = get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 1);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
      Bit32u lpf = AlignedAccessLPFOf(laddr, (1 & BX_CPU_THIS_PTR alignment_check_mask));
#else
      Bit32u lpf = LPFOf(laddr);
#endif    
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us read access
        // from this CPL.
        if (! (tlbEntry->accessBits & USER_PL)) { // Read this pl OK.
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          Bit16u *hostAddr = (Bit16u*) (hostPageAddr | pageOffset);
          ReadHostWordFromLittleEndian(hostAddr, data);
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 2, CPL, BX_READ, (Bit8u*) &data);
          return data;
        }
      }

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check()) {
        if (laddr & 1) {
          BX_ERROR(("read_virtual_word_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      access_read_linear(laddr, 2, CPL, BX_READ, (void *) &data);
      return data;
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
  Bit32u laddr;
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];
  Bit32u data;

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessROK) {
    if (offset < (seg->cache.u.segment.limit_scaled-2)) {
accessOK:
      laddr = get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 3);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
      Bit32u lpf = AlignedAccessLPFOf(laddr, (3 & BX_CPU_THIS_PTR alignment_check_mask));
#else
      Bit32u lpf = LPFOf(laddr);
#endif    
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us read access
        // from this CPL.
        if (! (tlbEntry->accessBits & USER_PL)) { // Read this pl OK.
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
          ReadHostDWordFromLittleEndian(hostAddr, data);
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 4, CPL, BX_READ, (Bit8u*) &data);
          return data;
        }
      }

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check()) {
        if (laddr & 3) {
          BX_ERROR(("read_virtual_dword_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      access_read_linear(laddr, 4, CPL, BX_READ, (void *) &data);
      return data;
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
  Bit32u laddr;
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];
  Bit64u data;

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessROK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-7)) {
accessOK:
      laddr = get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 7);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
      Bit32u lpf = AlignedAccessLPFOf(laddr, (7 & BX_CPU_THIS_PTR alignment_check_mask));
#else
      Bit32u lpf = LPFOf(laddr);
#endif    
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us read access
        // from this CPL.
        if (! (tlbEntry->accessBits & USER_PL)) { // Read this pl OK.
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
          ReadHostQWordFromLittleEndian(hostAddr, data);
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 8, CPL, BX_READ, (Bit8u*) &data);
          return data;
        }
      }

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check()) {
        if (laddr & 7) {
          BX_ERROR(("read_virtual_qword_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      access_read_linear(laddr, 8, CPL, BX_READ, (void *) &data);
      return data;
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

#if BX_CPU_LEVEL >= 6

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_virtual_dqword_32(unsigned s, Bit32u offset, BxPackedXmmRegister *data)
{
  Bit32u laddr;
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessROK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-15)) {
accessOK:
      laddr = get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 15);
      Bit32u lpf = LPFOf(laddr);
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us read access
        // from this CPL.
        if (! (tlbEntry->accessBits & USER_PL)) { // Read this pl OK.
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
          ReadHostQWordFromLittleEndian(hostAddr,   data->xmm64u(0));
          ReadHostQWordFromLittleEndian(hostAddr+1, data->xmm64u(1));
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 16, CPL, BX_READ, (Bit8u*) data);
          return;
        }
      }
      access_read_linear(laddr, 16, CPL, BX_READ, (void *) data);
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
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
      Bit32u lpf = LPFOf(laddr);
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us read access
        // from this CPL.
        if (! (tlbEntry->accessBits & USER_PL)) { // Read this pl OK.
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
          ReadHostQWordFromLittleEndian(hostAddr,   data->xmm64u(0));
          ReadHostQWordFromLittleEndian(hostAddr+1, data->xmm64u(1));
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 16, CPL, BX_READ, (Bit8u*) data);
          return;
        }
      }
      access_read_linear(laddr, 16, CPL, BX_READ, (void *) data);
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
  BX_ASSERT(elements > 0);

  Bit32u laddr;
  unsigned len = elements << 2;
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessROK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-len+1)) {
accessOK:
      laddr = get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, len-1);
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

#endif

//////////////////////////////////////////////////////////////
// special Read-Modify-Write operations                     //
// address translation info is kept across read/write calls //
//////////////////////////////////////////////////////////////

  Bit8u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_byte_32(unsigned s, Bit32u offset)
{
  Bit32u laddr;
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];
  Bit8u data;

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessWOK) {
    if (offset <= seg->cache.u.segment.limit_scaled) {
accessOK:
      laddr = get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
      Bit32u lpf = LPFOf(laddr);
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us write access
        // from this CPL.
        if (! (tlbEntry->accessBits & (0x2 | USER_PL))) {
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
          Bit8u *hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
          pageWriteStampTable.decWriteStamp(pAddr, 1);
          data = *hostAddr;
          BX_CPU_THIS_PTR address_xlation.pages = (bx_ptr_equiv_t) hostAddr;
          BX_CPU_THIS_PTR address_xlation.paddress1 = pAddr;
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 1, CPL, BX_READ, (Bit8u*) &data);
          return data;
        }
      }
      access_read_linear(laddr, 1, CPL, BX_RW, (void *) &data);
      return data;
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
  Bit16u data;

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessWOK) {
    if (offset < seg->cache.u.segment.limit_scaled) {
accessOK:
      laddr = get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 1);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
      Bit32u lpf = AlignedAccessLPFOf(laddr, (1 & BX_CPU_THIS_PTR alignment_check_mask));
#else
      Bit32u lpf = LPFOf(laddr);
#endif    
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us write access
        // from this CPL.
        if (! (tlbEntry->accessBits & (0x2 | USER_PL))) {
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
          Bit16u *hostAddr = (Bit16u*) (hostPageAddr | pageOffset);
          pageWriteStampTable.decWriteStamp(pAddr, 2);
          ReadHostWordFromLittleEndian(hostAddr, data);
          BX_CPU_THIS_PTR address_xlation.pages = (bx_ptr_equiv_t) hostAddr;
          BX_CPU_THIS_PTR address_xlation.paddress1 = pAddr;
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 2, CPL, BX_READ, (Bit8u*) &data);
          return data;
        }
      }

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check()) {
        if (laddr & 1) {
          BX_ERROR(("read_RMW_virtual_word_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      access_read_linear(laddr, 2, CPL, BX_RW, (void *) &data);
      return data;
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
  Bit32u data;

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessWOK) {
    if (offset < (seg->cache.u.segment.limit_scaled-2)) {
accessOK:
      laddr = get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 3);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
      Bit32u lpf = AlignedAccessLPFOf(laddr, (3 & BX_CPU_THIS_PTR alignment_check_mask));
#else
      Bit32u lpf = LPFOf(laddr);
#endif    
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us write access
        // from this CPL.
        if (! (tlbEntry->accessBits & (0x2 | USER_PL))) {
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
          Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
          pageWriteStampTable.decWriteStamp(pAddr, 4);
          ReadHostDWordFromLittleEndian(hostAddr, data);
          BX_CPU_THIS_PTR address_xlation.pages = (bx_ptr_equiv_t) hostAddr;
          BX_CPU_THIS_PTR address_xlation.paddress1 = pAddr;
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 4, CPL, BX_READ, (Bit8u*) &data);
          return data;
        }
      }

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check()) {
        if (laddr & 3) {
          BX_ERROR(("read_RMW_virtual_dword_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      access_read_linear(laddr, 4, CPL, BX_RW, (void *) &data);
      return data;
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
  Bit64u data;

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessWOK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-7)) {
accessOK:
      laddr = get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 7);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
      Bit32u lpf = AlignedAccessLPFOf(laddr, (7 & BX_CPU_THIS_PTR alignment_check_mask));
#else
      Bit32u lpf = LPFOf(laddr);
#endif    
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us write access
        // from this CPL.
        if (! (tlbEntry->accessBits & (0x2 | USER_PL))) {
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
          Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
          pageWriteStampTable.decWriteStamp(pAddr, 8);
          ReadHostQWordFromLittleEndian(hostAddr, data);
          BX_CPU_THIS_PTR address_xlation.pages = (bx_ptr_equiv_t) hostAddr;
          BX_CPU_THIS_PTR address_xlation.paddress1 = pAddr;
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 8, CPL, BX_READ, (Bit8u*) &data);
          return data;
        }
      }

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check()) {
        if (laddr & 7) {
          BX_ERROR(("read_RMW_virtual_qword_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      access_read_linear(laddr, 8, CPL, BX_RW, (void *) &data);
      return data;
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
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
    BX_CPU_THIS_PTR address_xlation.paddress1, 1, BX_WRITE, 0, (Bit8u*) &val8);

  if (BX_CPU_THIS_PTR address_xlation.pages > 2) {
    // Pages > 2 means it stores a host address for direct access.
    Bit8u *hostAddr = (Bit8u *) BX_CPU_THIS_PTR address_xlation.pages;
    *hostAddr = val8;
  }
  else {
    // address_xlation.pages must be 1
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 1, &val8);
  }
}

  void BX_CPP_AttrRegparmN(1)
BX_CPU_C::write_RMW_virtual_word(Bit16u val16)
{
  if (BX_CPU_THIS_PTR address_xlation.pages > 2) {
    // Pages > 2 means it stores a host address for direct access.
    Bit16u *hostAddr = (Bit16u *) BX_CPU_THIS_PTR address_xlation.pages;
    WriteHostWordToLittleEndian(hostAddr, val16);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1, 2, BX_WRITE, 0, (Bit8u*) &val16);
  }
  else if (BX_CPU_THIS_PTR address_xlation.pages == 1) {
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 2, &val16);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1, 2, BX_WRITE, 0, (Bit8u*) &val16);
  }
  else {
#ifdef BX_LITTLE_ENDIAN
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 1, &val16);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1, 1, BX_WRITE, 0,  (Bit8u*) &val16);
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress2, 1, ((Bit8u *) &val16) + 1);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress2, 1, BX_WRITE, 0, ((Bit8u*) &val16)+1);
#else
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 1, ((Bit8u *) &val16) + 1);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1, 1, BX_WRITE, 0, ((Bit8u*) &val16)+1);
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress2, 1, &val16);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress2, 1, BX_WRITE, 0,  (Bit8u*) &val16);
#endif
  }
}

  void BX_CPP_AttrRegparmN(1)
BX_CPU_C::write_RMW_virtual_dword(Bit32u val32)
{
  if (BX_CPU_THIS_PTR address_xlation.pages > 2) {
    // Pages > 2 means it stores a host address for direct access.
    Bit32u *hostAddr = (Bit32u *) BX_CPU_THIS_PTR address_xlation.pages;
    WriteHostDWordToLittleEndian(hostAddr, val32);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1, 4, BX_WRITE, 0, (Bit8u*) &val32);
  }
  else if (BX_CPU_THIS_PTR address_xlation.pages == 1) {
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 4, &val32);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1, 4, BX_WRITE, 0, (Bit8u*) &val32);
  }
  else {
#ifdef BX_LITTLE_ENDIAN
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1, &val32);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1, BX_WRITE, 0, (Bit8u*) &val32);
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2,
        ((Bit8u *) &val32) + BX_CPU_THIS_PTR address_xlation.len1);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2, BX_WRITE, 0,
        ((Bit8u *) &val32) + BX_CPU_THIS_PTR address_xlation.len1);
#else
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1,
        ((Bit8u *) &val32) + (4 - BX_CPU_THIS_PTR address_xlation.len1));
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1, BX_WRITE, 0,
        ((Bit8u *) &val32) + (4 - BX_CPU_THIS_PTR address_xlation.len1));
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2, &val32);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2, BX_WRITE, 0, (Bit8u*) &val32);
#endif
  }
}

  void BX_CPP_AttrRegparmN(1)
BX_CPU_C::write_RMW_virtual_qword(Bit64u val64)
{
  if (BX_CPU_THIS_PTR address_xlation.pages > 2) {
    // Pages > 2 means it stores a host address for direct access.
    Bit64u *hostAddr = (Bit64u *) BX_CPU_THIS_PTR address_xlation.pages;
    WriteHostQWordToLittleEndian(hostAddr, val64);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1, 8, BX_WRITE, 0, (Bit8u*) &val64);
  }
  else if (BX_CPU_THIS_PTR address_xlation.pages == 1) {
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 8, &val64);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1, 8, BX_WRITE, 0, (Bit8u*) &val64);
  }
  else {
#ifdef BX_LITTLE_ENDIAN
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1, &val64);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1, BX_WRITE, 0, (Bit8u*) &val64);
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2,
        ((Bit8u *) &val64) + BX_CPU_THIS_PTR address_xlation.len1);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2, BX_WRITE, 0,
        ((Bit8u *) &val64) + BX_CPU_THIS_PTR address_xlation.len1);
#else
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1,
        ((Bit8u *) &val64) + (8 - BX_CPU_THIS_PTR address_xlation.len1));
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1, BX_WRITE, 0,
        ((Bit8u *) &val64) + (8 - BX_CPU_THIS_PTR address_xlation.len1));
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2, &val64);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2, BX_WRITE, 0, (Bit8u*) &val64);
#endif
  }
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
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 1);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
      Bit32u lpf = AlignedAccessLPFOf(laddr, (1 & BX_CPU_THIS_PTR alignment_check_mask));
#else
      Bit32u lpf = LPFOf(laddr);
#endif    
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us write access
        // from this CPL.
        if (! (tlbEntry->accessBits & (0x2 | user))) {
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 2, curr_pl, BX_WRITE, (Bit8u*) &data);
          Bit16u *hostAddr = (Bit16u*) (hostPageAddr | pageOffset);
          pageWriteStampTable.decWriteStamp(pAddr, 2);
          WriteHostWordToLittleEndian(hostAddr, data);
          return;
        }
      }

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check() && user) {
        if (laddr & 1) {
          BX_ERROR(("write_new_stack_word_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      access_write_linear(laddr, 2, curr_pl, (void *) &data);
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
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 3);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
      Bit32u lpf = AlignedAccessLPFOf(laddr, (3 & BX_CPU_THIS_PTR alignment_check_mask));
#else
      Bit32u lpf = LPFOf(laddr);
#endif    
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us write access
        // from this CPL.
        if (! (tlbEntry->accessBits & (0x2 | user))) {
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 4, curr_pl, BX_WRITE, (Bit8u*) &data);
          Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
          pageWriteStampTable.decWriteStamp(pAddr, 4);
          WriteHostDWordToLittleEndian(hostAddr, data);
          return;
        }
      }

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check() && user) {
        if (laddr & 3) {
          BX_ERROR(("write_new_stack_dword_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      access_write_linear(laddr, 4, curr_pl, (void *) &data);
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
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 7);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
      Bit32u lpf = AlignedAccessLPFOf(laddr, (7 & BX_CPU_THIS_PTR alignment_check_mask));
#else
      Bit32u lpf = LPFOf(laddr);
#endif    
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf) {
        // See if the TLB entry privilege level allows us write access
        // from this CPL.
        if (! (tlbEntry->accessBits & (0x2 | user))) {
          bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
          Bit32u pageOffset = PAGE_OFFSET(laddr);
          bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
          BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 8, curr_pl, BX_WRITE, (Bit8u*) &data);
          Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
          pageWriteStampTable.decWriteStamp(pAddr, 8);
          WriteHostQWordToLittleEndian(hostAddr, data);
          return;
        }
      }

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
      if (BX_CPU_THIS_PTR alignment_check() && user) {
        if (laddr & 7) {
          BX_ERROR(("write_new_stack_qword_32(): #AC misaligned access"));
          exception(BX_AC_EXCEPTION, 0);
        }
      }
#endif

      access_write_linear(laddr, 8, curr_pl, (void *) &data);
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
