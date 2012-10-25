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
	BX_CPP_INLINE void  check_memory_limit(Bit32u addr);

public:
	Bit64u		 size;
	Bit8u		*memory;
	Bit32u		virtualBase;
	Bit32u      lastAddress;

	// first byte after mainExecutable .bss or address set by syscall brk
	Bit32u		programBreak;

	// original program break, this is to keep where is the end of DATA
	// segment for the main executable
	Bit32u		origProgramBreak;

	// first byte where an shared library was loaded
	Bit32u		libraryStart;

	// mark the first free byte after the libraries or when the process use mmap
	// this point to the next free byte after all memory mapped io regions
	Bit32u      afterLibrary;

	BX_MEM_C();
	~BX_MEM_C();

	void allocate(Bit64u size);

	void setVirtualMemBase(Bit32u vBase);

	Bit32u getProgramBreak();
	void setProgramBreak(Bit32u pb);

	Bit32u getLibraryStart();
	void setLibraryStart(Bit32u ls);

	Bit32u getOrigProgramBreak();
	void setOrigProgramBreak(Bit32u ls);

	Bit32u readUINT32(Bit32u offset);
	Bit32s readSINT32(Bit32u offset);

	void read(Bit8u *content, Bit32u offset, Bit32u len);
	void write(Bit8u *content, Bit32u offset, Bit32u len);
	void copy(Bit32u offsetFrom, Bit32u offsetTo, Bit32u len);
	Bit8u * memStrdup(Bit32u offset);
	Bit8u * str(Bit32u offset) { return &memory[offset]; };

	Bit8u read_byte(Bit32u addr);
	Bit16u read_word(Bit32u addr);
	Bit32u read_dword( Bit32u addr);
	Bit64u read_qword(Bit32u addr);

	void write_byte(Bit32u addr, Bit8u data);
	void write_word(Bit32u addr, Bit16u data);
	void write_dword(Bit32u addr, Bit32u data);
	void write_qword(Bit32u addr, Bit64u data);

	// Write after read operations
	void write_byte(Bit8u data) { *((Bit8u *) lastAddress) = data; }
	void write_word(Bit16u data) {  *((Bit16u *) lastAddress) = data; }
	void write_dword(Bit32u data){ *((Bit32u *) lastAddress) = data; }
	void write_qword(Bit64u data){ *((Bit64u *) lastAddress) = data; }

    // Memory offset <-> Virtual Address conversion
	Bit32u virtualAddressToPosition(Bit32u vir);
	Bit32u positionToVirtualAddress(Bit32u pos);

	// Real Address <-> Virtual Address conversion
	Bit32u VirtualToRealAddress(Bit32u address);
    Bit32u RealToVirtualAddress(Bit32u address);

    void dump(char *filename);
};

#endif
