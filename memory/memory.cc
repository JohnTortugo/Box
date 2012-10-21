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

#include <unistd.h>

#include <string.h>

#define ADDR_VIRTUAL_TO_REAL(x) ((Bit32u) ((x-virtualBase)+memory))
#define ADDR_REAL_TO_VIRTUAL(x)  ((virtualBase + x)-((Bit32u) memory))

BX_MEM_C::BX_MEM_C() : virtualBase(0), size(0), memory(NULL) , lastAddress(0){ }

BX_MEM_C::~BX_MEM_C() { free(memory); }

void BX_MEM_C::allocate(Bit64u size) {
  memory = (Bit8u *) malloc(size);
  this->size = size;
}

BX_CPP_INLINE void  BX_MEM_C::check_memory_limit(Bit32u addr)
{
   if ( addr < virtualBase || (addr - virtualBase > this->size-1) )
	   BX_PANIC(("Attempt to access a memory position out of memory space.\n\t"
			          " Requested address: 0x%08lx \n\t"
			          " Memory space: 0x%08lx - 0x%08lx ", addr, virtualBase, (virtualBase + size -1)));
}

/*!
 * Set the (emulated) virtual base address which the main executable
 * was loaded.
 */
void BX_MEM_C::setVirtualMemBase(Bit32u vBase) {
	this->virtualBase = vBase;
}

/*!
 * Translate virtual address to the correspondent index inside
 * the memory vector.
 */
Bit32u BX_MEM_C::virtualAddressToPosition(Bit32u vir) {
	return vir - this->virtualBase;
}

/*!
 * Translate an index into the memory vector to an virtual
 * address.
 */
Bit32u BX_MEM_C::positionToVirtualAddress(Bit32u pos) {
	return this->virtualBase + pos;
}

Bit8u BX_MEM_C::read_byte( Bit32u addr)
{
   check_memory_limit(addr);
   lastAddress = ADDR_VIRTUAL_TO_REAL(addr);
   return *((Bit8u *) lastAddress);
}

Bit16u BX_MEM_C::read_word(Bit32u addr)
{
   check_memory_limit(addr);
   lastAddress = ADDR_VIRTUAL_TO_REAL(addr);
   return *((Bit16u *) lastAddress);
}

Bit32u BX_MEM_C::read_dword(Bit32u addr)
{
   check_memory_limit(addr);
   lastAddress = ADDR_VIRTUAL_TO_REAL(addr);

   if ( addr == 0xE446328 ) {
  	  BX_INFO(("**Reading %08lx from 0xE4463E8",*((Bit32u *) lastAddress)));
   }

   return *((Bit32u *) lastAddress);
}

Bit64u BX_MEM_C::read_qword( Bit32u addr)
{
   check_memory_limit(addr);
   lastAddress = ADDR_VIRTUAL_TO_REAL(addr);
   return *((Bit64u *) lastAddress);
}

void BX_MEM_C::write_byte( Bit32u addr, Bit8u data)
{
   Bit8u *ptr = (Bit8u *) ADDR_VIRTUAL_TO_REAL(addr);

   check_memory_limit(addr);
   *ptr = data;
}

void BX_MEM_C::write_word( Bit32u addr, Bit16u data)
{
   Bit16u *ptr = (Bit16u *) ADDR_VIRTUAL_TO_REAL(addr);

   check_memory_limit(addr);
   *ptr = data;
}

void BX_MEM_C::write_dword( Bit32u addr, Bit32u data)
{
  Bit32u *ptr = (Bit32u *) ADDR_VIRTUAL_TO_REAL(addr);

  if ( addr == 0xE446328 ) {
	  printf("[BX_INFO] **Writing %08lx to %08lx\n",data,addr);
  }
  check_memory_limit(addr);
  *ptr = data;
}

void BX_MEM_C::write_qword( Bit32u addr, Bit64u data)
{
   Bit64u *ptr = (Bit64u *) ADDR_VIRTUAL_TO_REAL(addr);
   check_memory_limit(addr);
   *ptr = data;
}

 Bit32u  BX_MEM_C::VirtualToRealAddress(Bit32u address)
{
  return ADDR_VIRTUAL_TO_REAL(address);
};

 Bit32u BX_MEM_C::RealToVirtualAddress(Bit32u address)
{
  return ADDR_REAL_TO_VIRTUAL(address);
}

/*!
 * Read len bytes from the memory starting at offset and put them
 * in the memory pointed by content.
 */
void BX_MEM_C::read(Bit8u *content, Bit32u offset, Bit32u len) {
	memcpy((void *) content,(void *) (memory+offset),len);
}

/*!
 * Write len bytes to the memory starting at offset reading them
 * from the memory pointed by content.
 */
void BX_MEM_C::write(Bit8u *content, Bit32u offset, Bit32u len) {
    memcpy((void *)(memory+offset), (void *)content, len);
}

/*!
 * Make a copy from two different regions from the memory.
 * Copies len bytes starting at offsetFrom to the location starting
 * at offsetTo
 */
void BX_MEM_C::copy(Bit32u offsetFrom, Bit32u offsetTo, Bit32u len) {
    memcpy((void *)(memory+offsetTo), (void *)(memory+offsetFrom), len);
}



/*!
 * Read the content starting at offset until reach an '\0'.
 * Return a pointer to the content just read.
 */
Bit8u * BX_MEM_C::memStrdup(Bit32u offset) {
	Bit8u *ptr = (Bit8u *) strdup((const char *) (memory+offset));

	if (ptr == NULL) BX_ERROR(("memStrdup returning NULL."));

	return ptr;
}

void BX_MEM_C::dump(char *filename)
{
  FILE *fd;
  Bit32u offset;

  fd = fopen(filename,"wb" );

  offset = size;

  fwrite(memory,offset,1,fd);

  fclose(fd);
}

Bit32u BX_MEM_C::getProgramBreak() {
	return this->programBreak;
}

void BX_MEM_C::setProgramBreak(Bit32u pb) {
	this->programBreak = pb;
}

Bit32u BX_MEM_C::getOrigProgramBreak() {
	return this->origProgramBreak;
}

void BX_MEM_C::setOrigProgramBreak(Bit32u pb) {
	this->origProgramBreak = pb;
}

Bit32u BX_MEM_C::getLibraryStart() {
	return this->libraryStart;
}

void BX_MEM_C::setLibraryStart(Bit32u ls) {
	this->libraryStart = ls;
}
