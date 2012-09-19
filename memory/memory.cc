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
	Bit32u addr = bx_cpu.get_laddr(s, offset);
	if ( addr > this->size )
        BX_PANIC(("Atempt to read beyond memory size limit. Address: %lx\n",addr));
	return *(memory+addr);
}

Bit16u BX_MEM_C::read_word(unsigned s, Bit32u offset) {
	Bit32u addr = bx_cpu.get_laddr(s, offset);
	if ( addr > this->size )
        BX_PANIC(("Atempt to read beyond memory size limit. Address: %lx\n",addr));
	return *((Bit16u *)(memory+addr));
}

Bit32u BX_MEM_C::read_dword(unsigned s, Bit32u offset) {
	Bit32u addr = bx_cpu.get_laddr(s, offset);
	if ( addr > this->size )
        BX_PANIC(("Atempt to read beyond memory size limit. Address: %lx\n",addr));
	return *((Bit32u *)(memory+addr));
}

Bit64u BX_MEM_C::read_qword(unsigned s, Bit32u offset) {
	Bit32u addr = bx_cpu.get_laddr(s, offset);
	if ( addr > this->size )
        BX_PANIC(("Atempt to read beyond memory size limit. Address: %lx\n",addr));
	return *((Bit64u *)(memory+addr));
}

void BX_MEM_C::write_byte(unsigned s, Bit32u offset, Bit8u data) {
	Bit32u addr = bx_cpu.get_laddr(s, offset);
	if ( addr > this->size )
        BX_PANIC(("Atempt to write beyond memory size limit. Address: %lx\n",addr));
	*(memory+addr) = data;
}

void BX_MEM_C::write_word(unsigned s, Bit32u offset, Bit16u data) {
	Bit32u addr = bx_cpu.get_laddr(s, offset);
	if ( addr > this->size )
        BX_PANIC(("Atempt to write beyond memory size limit. Address: %lx\n",addr));
	*((Bit16u *) (memory+addr)) = data;
}

void BX_MEM_C::write_dword(unsigned s, Bit32u offset, Bit32u data) {
	Bit32u addr = bx_cpu.get_laddr(s, offset);
	if ( addr > this->size )
        BX_PANIC(("Atempt to write beyond memory size limit. Address: %lx\n",addr));
	*((Bit32u *) (memory+addr)) = data;
}

void BX_MEM_C::write_qword(unsigned s, Bit32u offset, Bit64u data) {
	Bit32u addr = bx_cpu.get_laddr(s, offset);
	if ( addr > this->size )
        BX_PANIC(("Atempt to write beyond memory size limit. Address: %lx\n",addr));
	*((Bit64u *) (memory+addr)) = data;
}
