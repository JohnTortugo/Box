/////////////////////////////////////////////////////////////////////////
// $Id: segment_ctrl.cc 11313 2012-08-05 13:52:40Z sshwarts $
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
#include "debug.h"
#define LOG_THIS BX_CPU_THIS_PTR

// LES/LDS can't be called from long64 mode
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LES_GwMp(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  Bit32u eaddr = (Bit32u) BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u reg_16 = bx_mem.read_word(i->seg(), eaddr);
  Bit16u es     = bx_mem.read_word(i->seg(), (eaddr + 2) & i->asize_mask());

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES], es);

  BX_WRITE_16BIT_REG(i->dst(), reg_16);

  BX_NEXT_INSTR(i);
}

// LES/LDS can't be called from long64 mode
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LES_GdMp(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  Bit32u eaddr = (Bit32u) BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u es = bx_mem.read_word(i->seg(), (eaddr + 4) & i->asize_mask());
  Bit32u reg_32 = bx_mem.read_dword(i->seg(), eaddr);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES], es);

  BX_WRITE_32BIT_REGZ(i->dst(), reg_32);

  BX_NEXT_INSTR(i);
}

// LES/LDS can't be called from long64 mode
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LDS_GwMp(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  Bit32u eaddr = (Bit32u) BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u reg_16 = bx_mem.read_word(i->seg(), eaddr);
  Bit16u ds     = bx_mem.read_word(i->seg(), (eaddr + 2) & i->asize_mask());

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS], ds);

  BX_WRITE_16BIT_REG(i->dst(), reg_16);

  BX_NEXT_INSTR(i);
}

// LES/LDS can't be called from long64 mode
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LDS_GdMp(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  Bit32u eaddr = (Bit32u) BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u ds = bx_mem.read_word(i->seg(), (eaddr + 4) & i->asize_mask());
  Bit32u reg_32 = bx_mem.read_dword(i->seg(), eaddr);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS], ds);

  BX_WRITE_32BIT_REGZ(i->dst(), reg_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LFS_GwMp(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u reg_16 = bx_mem.read_word(i->seg(), eaddr);
  Bit16u fs     = bx_mem.read_word(i->seg(), (eaddr + 2) & i->asize_mask());

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], fs);

  BX_WRITE_16BIT_REG(i->dst(), reg_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LFS_GdMp(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u fs = bx_mem.read_word(i->seg(), (eaddr + 4) & i->asize_mask());
  Bit32u reg_32 = bx_mem.read_dword(i->seg(), eaddr);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], fs);

  BX_WRITE_32BIT_REGZ(i->dst(), reg_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LGS_GwMp(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u reg_16 = bx_mem.read_word(i->seg(), eaddr);
  Bit16u gs     = bx_mem.read_word(i->seg(), (eaddr + 2) & i->asize_mask());

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], gs);

  BX_WRITE_16BIT_REG(i->dst(), reg_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LGS_GdMp(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u gs = bx_mem.read_word(i->seg(), (eaddr + 4) & i->asize_mask());
  Bit32u reg_32 = bx_mem.read_dword(i->seg(), eaddr);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], gs);

  BX_WRITE_32BIT_REGZ(i->dst(), reg_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LSS_GwMp(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u reg_16 = bx_mem.read_word(i->seg(), eaddr);
  Bit16u ss     = bx_mem.read_word(i->seg(), (eaddr + 2) & i->asize_mask());

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], ss);

  BX_WRITE_16BIT_REG(i->dst(), reg_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LSS_GdMp(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u ss = bx_mem.read_word(i->seg(), (eaddr + 4) & i->asize_mask());
  Bit32u reg_32 = bx_mem.read_dword(i->seg(), eaddr);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], ss);

  BX_WRITE_32BIT_REGZ(i->dst(), reg_32);

  BX_NEXT_INSTR(i);
}

