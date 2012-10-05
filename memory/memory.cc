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

#include <string.h>

#define ADDR_VIRTUAL_TO_REAL(x) ((Bit32u) ((x-virtualBase)+memory))
#define ADDR_REAL_TO_VIRTUAL(x)  ((virtualBase + x)-((Bit32u) memory))

BX_MEM_C::BX_MEM_C() : virtualBase(0), size(0), memory(NULL) , lastAddress(0){ }

BX_MEM_C::~BX_MEM_C() { free(memory); }

void BX_MEM_C::allocate(Bit64u size) {
  memory = (Bit8u *) malloc(size);
  this->size = size;
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
   if ( (addr - virtualBase) > this->size )
      BX_PANIC(("Attempt to read beyond memory size limit. Address: %08lx\n",addr));
   lastAddress = ADDR_VIRTUAL_TO_REAL(addr);
   return *((Bit8u *) lastAddress);
}

Bit16u BX_MEM_C::read_word( Bit32u addr)
{
   if ( (addr - virtualBase)> this->size )
	      BX_PANIC(("Attempt to read beyond memory size limit. Address: %08lx\n",addr));
   lastAddress = ADDR_VIRTUAL_TO_REAL(addr);
   return *((Bit16u *) lastAddress);
}

Bit32u BX_MEM_C::read_dword( Bit32u addr)
{
   if ( (addr - virtualBase) > this->size )
	      BX_PANIC(("Attempt to read beyond memory size limit. Address: %08lx\n",addr));
   lastAddress = ADDR_VIRTUAL_TO_REAL(addr);
   return *((Bit32u *) lastAddress);
}

Bit64u BX_MEM_C::read_qword( Bit32u addr)
{
   if ( (addr - virtualBase) > this->size )
	      BX_PANIC(("Attempt to read beyond memory size limit. Address: %08lx\n",addr));
   lastAddress = ADDR_VIRTUAL_TO_REAL(addr);
   return *((Bit64u *) lastAddress);
}

void BX_MEM_C::write_byte( Bit32u addr, Bit8u data)
{
   Bit8u *ptr = (Bit8u *) ADDR_VIRTUAL_TO_REAL(addr);

   if ( (addr - virtualBase) > this->size )
	      BX_PANIC(("Attempt to read beyond memory size limit. Address: %08lx\n",addr));
   *ptr = data;
}

void BX_MEM_C::write_word( Bit32u addr, Bit16u data)
{
   Bit16u *ptr = (Bit16u *) ADDR_VIRTUAL_TO_REAL(addr);

   if ( (addr - virtualBase) > this->size )
	      BX_PANIC(("Attempt to read beyond memory size limit. Address: %08lx\n",addr));
   *ptr = data;
}

void BX_MEM_C::write_dword( Bit32u addr, Bit32u data)
{
  Bit32u *ptr = (Bit32u *) ADDR_VIRTUAL_TO_REAL(addr);
  if ( (addr - virtualBase)> this->size )
	      BX_PANIC(("Attempt to read beyond memory size limit. Address: %08lx\n",addr));
   *ptr = data;
}

void BX_MEM_C::write_qword( Bit32u addr, Bit64u data)
{
   Bit64u *ptr = (Bit64u *) ADDR_VIRTUAL_TO_REAL(addr);
   if ( (addr - virtualBase) > this->size )
	      BX_PANIC(("Attempt to read beyond memory size limit. Address: %08lx\n",addr));
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

int BX_MEM_C::loadFile(char * fname, Bit32u addr)
{
//  struct stat stat_buf;
//  int fd, ret;
//  unsigned long size, offset;
//
//  if ( addr > this->size )
//     BX_PANIC(("Attempt to write beyond memory size limit. Address: %lx\n",addr));
//
//  // read file
//  fd = open(fname, O_RDONLY);
//
//  if (fd < 0)
//     BX_PANIC(("loadFile: couldn't open image file '%s'.", fname));
//
//  ret = fstat(fd, &stat_buf);
//
//  if (ret)
//     BX_PANIC(("loadFile: couldn't stat image file '%s'.", fname));
//
//
//  size = (unsigned long) stat_buf.st_size;
//
//  if ((addr + size) > this->size)
//     BX_PANIC(("loadFile: address range > physical memsize!"));
//
//  offset = 0;
//
//  while (size > 0) {
//     ret = read(fd, (bx_ptr_t) (this->memory+offset), size);
//
//     if (ret <= 0)
//        BX_PANIC(("loadFile: read failed on image"));
//
//     size -= ret;
//     offset += ret;
//  }
//
//  close(fd);
//
//  BX_INFO(("loadFile: '%s', size=%u read into memory at %08x",
//           fname, (unsigned) stat_buf.st_size, (unsigned) addr));
//
//  return stat_buf.st_size;
}

int BX_MEM_C::loadData(void * ptr, Bit32u size, Bit32u addr)
{
//  if ((addr + size) > this->size)
//     BX_PANIC(("loadData: address range > physical memsize!"));
//
//  memcpy((void *) memory+addr, ptr, size);
//
	return 0;
}

/*!
 * Read len bytes from the memory starting at offset and put them
 * in the memory pointed by content.
 */
void BX_MEM_C::read(Bit8u *content, Bit32u offset, Bit32u len) {
	// read len bytes from memory
	memcpy((void *) content,(void *) (memory+offset),len);
/*
	for (int i=0; i<len; i++) {
		content[i] = this->memory[offset++];
	}
*/
}

/*!
 * Write len bytes to the memory starting at offset reading them
 * from the memory pointed by content.
 */
void BX_MEM_C::write(Bit8u *content, Bit32u offset, Bit32u len) {
	// write len bytes to memory
	memcpy((void *)(memory+offset), (void *)content, len);
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
