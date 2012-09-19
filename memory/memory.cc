/////////////////////////////////////////////////////////////////////////
// $Id: memory.cc 11221 2012-06-18 11:41:26Z sshwarts $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2009  The Bochs Project
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
//
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "cpu/cpu.h"
#include "debug.h"


Bit8u BX_MEM_C::read_byte(unsigned s, Bit32u offset) {
	printf("Reading from seg: %x (%s), base %x, offset %x, laddr: %x\n", s, bx_cpu.strseg(&bx_cpu.sregs[s]), bx_cpu.get_segment_base(s), offset, bx_cpu.get_laddr(s, offset));

	return 0;
}

Bit16u BX_MEM_C::read_word(unsigned s, Bit32u offset) {
	printf("Reading from seg: %x (%s), base %x, offset %x, laddr: %x\n", s, bx_cpu.strseg(&bx_cpu.sregs[s]), bx_cpu.get_segment_base(s), offset, bx_cpu.get_laddr(s, offset));
	return 0;
}

Bit32u BX_MEM_C::read_dword(unsigned s, Bit32u offset) {
	printf("Reading from seg: %x (%s), base %x, offset %x, laddr: %x\n", s, bx_cpu.strseg(&bx_cpu.sregs[s]), bx_cpu.get_segment_base(s), offset, bx_cpu.get_laddr(s, offset));
	return 0;
}

Bit64u BX_MEM_C::read_qword(unsigned s, Bit32u offset) {
	printf("Reading from seg: %x (%s), base %x, offset %x, laddr: %x\n", s, bx_cpu.strseg(&bx_cpu.sregs[s]), bx_cpu.get_segment_base(s), offset, bx_cpu.get_laddr(s, offset));
	return 0;
}

void BX_MEM_C::write_byte(Bit8u val8) {
	printf("writing to %x\n", val8);
}

void BX_MEM_C::write_word(Bit16u val16) {
	printf("writing to %x\n", val16);
}

void BX_MEM_C::write_dword(Bit32u val32) {
	printf("writing to %x\n", val32);
}

void BX_MEM_C::write_qword(Bit64u val64) {
	printf("writing to %x\n", val64);
}

void BX_MEM_C::write_byte(unsigned s, Bit32u offset, Bit8u data) {
	printf("Writing from seg: %x (%s), base %x, offset %x, laddr: %x\n", s, bx_cpu.strseg(&bx_cpu.sregs[s]), bx_cpu.get_segment_base(s), offset, bx_cpu.get_laddr(s, offset));
}

void BX_MEM_C::write_word(unsigned s, Bit32u offset, Bit16u data) {
	printf("Writing from seg: %x (%s), base %x, offset %x, laddr: %x\n", s, bx_cpu.strseg(&bx_cpu.sregs[s]), bx_cpu.get_segment_base(s), offset, bx_cpu.get_laddr(s, offset));
}

void BX_MEM_C::write_dword(unsigned s, Bit32u offset, Bit32u data) {
	printf("Writing from seg: %x (%s), base %x, offset %x, laddr: %x\n", s, bx_cpu.strseg(&bx_cpu.sregs[s]), bx_cpu.get_segment_base(s), offset, bx_cpu.get_laddr(s, offset));
}

void BX_MEM_C::write_qword(unsigned s, Bit32u offset, Bit64u data) {
	printf("Writing from seg: %x (%s), base %x, offset %x, laddr: %x\n", s, bx_cpu.strseg(&bx_cpu.sregs[s]), bx_cpu.get_segment_base(s), offset, bx_cpu.get_laddr(s, offset));
}
