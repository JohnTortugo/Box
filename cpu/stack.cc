/////////////////////////////////////////////////////////////////////////
// $Id: stack.cc 11117 2012-03-28 21:11:19Z sshwarts $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2012 Stanislav Shwartsman
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

void BX_CPU_C::stack_write_byte(bx_address offset, Bit8u data) {
	bx_mem.write_byte(BX_SEG_REG_SS, offset, data);
}

void BX_CPU_C::stack_write_word(bx_address offset, Bit16u data) {
	bx_mem.write_word(BX_SEG_REG_SS, offset, data);
}

void BX_CPU_C::stack_write_dword(bx_address offset, Bit32u data) {
	bx_mem.write_dword(BX_SEG_REG_SS, offset, data);
}

void BX_CPU_C::stack_write_qword(bx_address offset, Bit64u data) {
	bx_mem.write_qword(BX_SEG_REG_SS, offset, data);
}

Bit8u BX_CPU_C::stack_read_byte(bx_address offset) {
	return bx_mem.read_byte(BX_SEG_REG_SS, offset);
}

Bit16u BX_CPU_C::stack_read_word(bx_address offset) {
	return bx_mem.read_word(BX_SEG_REG_SS, offset);
}

Bit32u BX_CPU_C::stack_read_dword(bx_address offset) {
	return bx_mem.read_dword(BX_SEG_REG_SS, offset);
}

Bit64u BX_CPU_C::stack_read_qword(bx_address offset) {
	return bx_mem.read_qword(BX_SEG_REG_SS, offset);
}

void BX_CPU_C::push_16(Bit16u value16) {
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) { /* StackAddrSize = 32 */
    stack_write_word((Bit32u) (ESP-2), value16);
    ESP -= 2;
  }
  else /* StackAddrSize = 16 */
  {
    stack_write_word((Bit16u) (SP-2), value16);
    SP -= 2;
  }
}

void BX_CPU_C::push_32(Bit32u value32) {
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) { /* StackAddrSize = 32 */
    stack_write_dword((Bit32u) (ESP-4), value32);
    ESP -= 4;
  }
  else /* StackAddrSize = 16 */
  {
    stack_write_dword((Bit16u) (SP-4), value32);
    SP -= 4;
  }
}

/* pop 16 bit operand from the stack */
Bit16u BX_CPU_C::pop_16(void)
{
  Bit16u value16;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) { /* StackAddrSize = 32 */
    value16 = stack_read_word(ESP);
    ESP += 2;
  }
  else { /* StackAddrSize = 16 */
    value16 = stack_read_word(SP);
    SP += 2;
  }

  return value16;
}

/* pop 32 bit operand from the stack */
Bit32u BX_CPU_C::pop_32(void)
{
  Bit32u value32;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) { /* StackAddrSize = 32 */
    value32 = stack_read_dword(ESP);
    ESP += 4;
  }
  else { /* StackAddrSize = 16 */
    value32 = stack_read_dword(SP);
    SP += 4;
  }

  return value32;
}
