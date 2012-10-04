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

BX_MEM_C::BX_MEM_C() : virtualBase(0), size(0), memory(NULL)
{
}

BX_MEM_C::~BX_MEM_C()
{
   free(memory);
}

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

Bit8u BX_MEM_C::read_byte(unsigned s, Bit32u offset) 
{
   Bit32u addr = bx_cpu.get_laddr(s, offset);
   if ( (addr - virtualBase) > this->size )
      BX_PANIC(("Attempt to read beyond memory size limit. Address: %lx\n",addr));
   return *(memory+addr-virtualBase);
}

Bit16u BX_MEM_C::read_word(unsigned s, Bit32u offset) 
{
   Bit32u addr = bx_cpu.get_laddr(s, offset);
   if ( (addr - virtualBase)> this->size )
      BX_PANIC(("Attempt to read beyond memory size limit. Address: %lx\n",addr));
   return *((Bit16u *)(memory+addr-virtualBase));
}

Bit32u BX_MEM_C::read_dword(unsigned s, Bit32u offset) 
{
   Bit32u addr = bx_cpu.get_laddr(s, offset);
   if ( (addr - virtualBase) > this->size )
      BX_PANIC(("Attempt to read beyond memory size limit. Address: %lx\n",addr));
   return *((Bit32u *)(memory+addr-virtualBase));
}

Bit64u BX_MEM_C::read_qword(unsigned s, Bit32u offset) 
{
   Bit32u addr = bx_cpu.get_laddr(s, offset);
   if ( (addr - virtualBase) > this->size )
      BX_PANIC(("Attempt to read beyond memory size limit. Address: %lx\n",addr));
   return *((Bit64u *)(memory+addr-virtualBase));
}

void BX_MEM_C::write_byte(unsigned s, Bit32u offset, Bit8u data) 
{
   Bit32u addr = bx_cpu.get_laddr(s, offset);
   if ( (addr - virtualBase) > this->size )
      BX_PANIC(("Attempt to write beyond memory size limit. Address: %lx\n",addr));
   *(memory+addr-virtualBase) = data;
}

void BX_MEM_C::write_word(unsigned s, Bit32u offset, Bit16u data) 
{
   Bit32u addr = bx_cpu.get_laddr(s, offset);
   if ( (addr - virtualBase) > this->size )
      BX_PANIC(("Attempt to write beyond memory size limit. Address: %lx\n",addr));
   *((Bit16u *) (memory+addr-virtualBase)) = data;
}

void BX_MEM_C::write_dword(unsigned s, Bit32u offset, Bit32u data) 
{
   Bit32u addr = bx_cpu.get_laddr(s, offset);
   if ( (addr - virtualBase)> this->size )
      BX_PANIC(("Attempt to write beyond memory size limit. Address: %lx\n",addr));
   *((Bit32u *) (memory+ virtualBase - addr)) = data;
}

void BX_MEM_C::write_qword(unsigned s, Bit32u offset, Bit64u data) 
{
   Bit32u addr = bx_cpu.get_laddr(s, offset);
   if ( (addr - virtualBase) > this->size )
      BX_PANIC(("Attempt to write beyond memory size limit. Address: %lx\n",addr));
   *((Bit64u *) (memory+addr-virtualBase)) = data;
}

Bit32u BX_MEM_C::VirtualToRealAddress(Bit32u address)
{
  return (Bit32u) (address-virtualBase+memory);
};

Bit32u BX_MEM_C::RealToVirtualAddress(Bit32u address)
{
  return virtualBase + address-((Bit32u) memory);
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
