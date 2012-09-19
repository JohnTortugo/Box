/////////////////////////////////////////////////////////////////////////
// $Id: memory.h 10994 2012-01-19 18:32:11Z vruppert $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2009  The Bochs Project
//
//  I/O memory handlers API Copyright (C) 2003 by Frank Cornelis
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

#ifndef BX_MEM_H
#  define BX_MEM_H 1

#include "config.h"

class BX_MEM_C {
private:
	Bit8u		*memory;

public:
	BX_MEM_C() {
		memory = (Bit8u *) malloc(sizeof(Bit8u) * 1024 * 1024); // 1MB
	}

	Bit8u read_byte(unsigned s, Bit32u offset);
	Bit16u read_word(unsigned s, Bit32u offset);
	Bit32u read_dword(unsigned s, Bit32u offset);
	Bit64u read_qword(unsigned s, Bit32u offset);

	void write_byte(Bit8u val8);
	void write_word(Bit16u val16);
	void write_dword(Bit32u val32);
	void write_qword(Bit64u val64);

	void write_byte(unsigned seg, Bit32u offset, Bit8u data);
	void write_word(unsigned seg, Bit32u offset, Bit16u data);
	void write_dword(unsigned seg, Bit32u offset, Bit32u data);
	void write_qword(unsigned seg, Bit32u offset, Bit64u data);

	~BX_MEM_C() {
		free(memory);
	}
};

#endif
