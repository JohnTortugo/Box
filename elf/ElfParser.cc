#include "config.h"
#include "debug.h"
#include "ElfParser.h"
#include <vector>
#include <string>
#include <elf.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

ElfParser::ElfParser(string elfPath) {
	this->rel 			= 0;
	this->rela 			= 0;
	this->rel 			= 0;
	this->relsz 		= 0;
	this->relent 		= 0;
	this->rela 			= 0;
	this->relasz 		= 0;
	this->relaent 		= 0;
	this->pltrel 		= 0;
	this->jmprel 		= 0;
	this->pltrelsz 		= 0;
	this->hash 			= 0;
	this->dynsym 		= 0;
	this->syment 		= 0;
	this->strtab 		= 0;
	this->strsz 		= 0;
	this->init 			= 0;
	this->fini 			= 0;
	this->pltgot 		= 0;
	this->rPath			= "";
	this->fileName 	= elfPath;

	// open elf file
	felf = fopen(elfPath.c_str(), "rb");

	if (!felf) {
		BX_ERROR(("unable to open elf file: %s\n", elfPath.c_str()));
		return ;
	}

	extractElfHeader();
	//printElfHeader();

	extractProgHdrTable();
	//printProgHdrTable();

	extractSecHdrTable();
	//printSecHdrTable();

	extractSectionsHdrTable();

	extractSections();

	parseDynamicEntries();

	//printSectionsHdrTable();
}

// print the section string table
void ElfParser::printSectionsHdrTable() {
	int ind = 0;
	printf("[%02d] ", ind);
	for (int i=0; i<sec_hdr_table_size; i++) {
		if (sec_hdr_table[i] == '\0') {
			printf("\n");
			ind++;
			if (i != sec_hdr_table_size-1) {
				printf("[%02d] ", ind);
			}
		}
		else {
			printf("%c", sec_hdr_table[i]);
		}
	}
}

// extract the section header string table content
void ElfParser::extractSectionsHdrTable() {
	if (hdr.e_shstrndx == 0) {
		printf("No index for section header string table.\n");
		return ;
	}

	Elf32_Shdr hdrt = shEntries[hdr.e_shstrndx];

	if (hdrt.sh_type != SHT_STRTAB) {
		printf("SHSTRNDX does not point to an string table.\n");
		return ;
	}

	sec_hdr_table = (unsigned char *) malloc(hdrt.sh_size);
	sec_hdr_table_size = hdrt.sh_size;
	fseek(felf, hdrt.sh_offset, SEEK_SET);
	fread(sec_hdr_table, hdrt.sh_size, 1, felf);
}

void ElfParser::parseDynamicEntries() {
	for (int i=0; i<dynSecEntries.size(); i++) {
		Elf32_Dyn dyn = dynSecEntries[i];

		switch (dyn.d_tag) {
			case DT_NULL:
				/* nothing needed */
				break;
			case DT_NEEDED:
				needed_libraries.push_back((char *)&dyn_str_table[dyn.d_un.d_val]);
				break;
			case DT_PLTGOT:
				this->pltgot = dyn.d_un.d_ptr;
				break;
			case DT_HASH:
				this->hash = dyn.d_un.d_ptr;
				break;
			case DT_GNU_HASH:
				this->gnuHash = dyn.d_un.d_ptr;
//				fprintf(stderr,"hash = %x\n", this->gnuHash);
				break;
			case DT_SYMTAB:
				this->dynsym = dyn.d_un.d_ptr;
				break;
			case DT_RELA:
				this->rela = dyn.d_un.d_ptr;
				break;
			case DT_RELASZ:
				this->relasz = dyn.d_un.d_val;
				break;
			case DT_RELAENT:
				this->relaent = dyn.d_un.d_val;
				break;
			case DT_SYMENT:
				this->syment = dyn.d_un.d_val;
				break;
			case DT_INIT:
				this->init = dyn.d_un.d_ptr;
				break;
			case DT_FINI:
				this->fini = dyn.d_un.d_ptr;
				break;
			case DT_RPATH:
				rPath = string((char *) &dyn_str_table[dyn.d_un.d_val]);
				break;
			case DT_REL:
				this->rel = dyn.d_un.d_ptr;
				break;
			case DT_RELSZ:
				this->relsz = dyn.d_un.d_val;
				break;
			case DT_RELENT:
				this->relent = dyn.d_un.d_val;
				break;
			case DT_PLTREL:
				this->pltrel = dyn.d_un.d_val;
				break;
			case DT_PLTRELSZ:
				this->pltrelsz = dyn.d_un.d_val;
				break;
			case DT_JMPREL:
				this->jmprel = dyn.d_un.d_ptr;
				break;
			case DT_STRTAB:
				this->strtab = dyn.d_un.d_ptr;
				break;
			case DT_STRSZ:
				this->strsz = dyn.d_un.d_val;
				break;
			case DT_SONAME:
				this->soname = string((char *) &dyn_str_table[dyn.d_un.d_val]);
				break;
			case DT_DEBUG:
				break;
			case DT_TEXTREL:
				break;
			case DT_SYMBOLIC:
				break;
			case DT_LOPROC:
				break;
			case DT_HIPROC:
				break;
		}
	}
}

void ElfParser::printDynamicSection() {
	// iterate over all entries in .dynamic section
	for (int i=0; i<dynSecEntries.size(); i++) {
		Elf32_Dyn dyn = dynSecEntries[i];

		switch (dyn.d_tag) {
			case DT_NULL:
				/* always ignored */
				break;
			case DT_NEEDED:
				printf("Shared library: [%s]\n", &dyn_str_table[dyn.d_un.d_val]);
				break;
			case DT_PLTRELSZ:
				break;
			case DT_PLTGOT:
				break;
			case DT_HASH:
				break;
			case DT_STRTAB:
				break;
			case DT_SYMTAB:
				break;
			case DT_RELA:
				break;
			case DT_RELASZ:
				break;
			case DT_RELAENT:
				break;
			case DT_STRSZ:
				break;
			case DT_SYMENT:
				break;
			case DT_INIT:
				break;
			case DT_FINI:
				break;
			case DT_SONAME:
				break;
			case DT_RPATH:
				break;
			case DT_SYMBOLIC:
				break;
			case DT_REL:
				break;
			case DT_RELSZ:
				break;
			case DT_RELENT:
				break;
			case DT_PLTREL:
				break;
			case DT_DEBUG:
				break;
			case DT_TEXTREL:
				break;
			case DT_JMPREL:
				break;
			case DT_LOPROC:
				break;
			case DT_HIPROC:
				break;
		}
	}
}

// extract each entry of .dynamic section and store in dynSecEntries
void ElfParser::extractDynamicSection(Elf32_Shdr dSec) {
	fseek(felf, dSec.sh_offset, SEEK_SET);

	while (true) {
		Elf32_Dyn dyn;

		fread(&dyn, sizeof(Elf32_Dyn), 1, felf);

		dynSecEntries.push_back(dyn);

		if (dyn.d_tag == DT_NULL) break;
	}
}

// extract all string tables content
void ElfParser::extractStrTabSection(Elf32_Shdr stSec) {
	if (strcmp((char *)&sec_hdr_table[stSec.sh_name], ".dynstr") == 0) {
		dyn_str_table 		= (unsigned char *) malloc(stSec.sh_size);
		dyn_str_table_size 	= stSec.sh_size;
		fseek(felf, stSec.sh_offset, SEEK_SET);
		fread(dyn_str_table, stSec.sh_size, 1, felf);
	}
	else if (strcmp((char *)&sec_hdr_table[stSec.sh_name], ".strtab") == 0) {
		str_table 		= (unsigned char *) malloc(stSec.sh_size);
		str_table_size 	= stSec.sh_size;
		fseek(felf, stSec.sh_offset, SEEK_SET);
		fread(str_table, stSec.sh_size, 1, felf);
	}
	else if (strcmp((char *)&sec_hdr_table[stSec.sh_name], ".shstrtab") == 0) {
		// already parsed
	}
	else {
		printf("String table not identified: %s\n", &sec_hdr_table[stSec.sh_name]);
	}
}

// iterate over Section Header entries and extract the content of each section
// extract the content of all sections
void ElfParser::extractSections() {
	for (int i=0; i<shEntries.size(); i++) {
		switch (shEntries[i].sh_type) {
			case SHT_DYNAMIC:
				extractDynamicSection(shEntries[i]);
				break;
			case SHT_STRTAB:
				extractStrTabSection(shEntries[i]);
				break;
		}
	}
}

// extract sections header table
void ElfParser::extractSecHdrTable() {
	if (hdr.e_shoff == 0 || hdr.e_shnum == 0) {
		printf("file does not have a sht.\n");
		return ;
	}

	fseek(felf, hdr.e_shoff, SEEK_SET);
	for (int i=0; i<hdr.e_shnum; i++) {
		Elf32_Shdr she;
		fread(&she, sizeof(Elf32_Shdr), 1, felf);
		shEntries.push_back(she);
	}
}

// print sections header table
void ElfParser::printSecHdrTable() {
	printf("\nSECTION HEADER TABLE\n");
	printf("-------------------------\n");

	for (int i=0; i<shEntries.size(); i++) {
		printf("sh_name:      0x%04x\n", shEntries[i].sh_name);
		printf("sh_type:      0x%04x\n", shEntries[i].sh_type);
		printf("sh_flags:     0x%04x\n", shEntries[i].sh_flags);
		printf("sh_addr:      0x%04x\n", shEntries[i].sh_addr);
		printf("sh_offset:    0x%04x\n", shEntries[i].sh_offset);
		printf("sh_size:      0x%04x\n", shEntries[i].sh_size);
		printf("sh_link:      0x%04x\n", shEntries[i].sh_link);
		printf("sh_info:      0x%04x\n", shEntries[i].sh_info);
		printf("sh_addralign: 0x%04x\n", shEntries[i].sh_addralign);
		printf("sh_entsize:   0x%04x\n", shEntries[i].sh_entsize);
		printf("\n");
	}

}

// parse program header table
void ElfParser::extractProgHdrTable() {
	phEntries.clear();

	if (hdr.e_phoff == 0 || hdr.e_phnum == 0) {
		printf("file does not have a pht.\n");
		return ;
	}

	fseek(felf, hdr.e_phoff, SEEK_SET);
	for (int i=0; i<hdr.e_phnum; i++) {
		Elf32_Phdr phe;
		fread(&phe, sizeof(Elf32_Phdr), 1, felf);
		phEntries.push_back(phe);
	}
}

// print program header table
void ElfParser::printProgHdrTable() {
	printf("\nPROGRAM HEADER TABLE\n");
	printf("-------------------------\n");

	printf("%10s %12s %12s %12s %12s %12s %12s %12s\n", "Type","Offset","VirtAddr","PhysAddr","FileSiz","MemSiz","Flag","Align");

	for (int i=0; i<phEntries.size(); i++) {
		printf("%10s ", segmentTypeName(phEntries[i].p_type));
		printf("0x%010x ", phEntries[i].p_offset);
		printf("0x%010x ", phEntries[i].p_vaddr);
		printf("0x%010x ", phEntries[i].p_paddr);
		printf("0x%010x ", phEntries[i].p_filesz);
		printf("0x%010x ", phEntries[i].p_memsz);
		printf("0x%010x ", phEntries[i].p_flags);
		printf("0x%010x ", phEntries[i].p_align);
		printf("\n");
	}
}

char * ElfParser::segmentTypeName(Elf32_Word type) {
	switch (type) {
		case PT_NULL:
			return "Null";
			break;
		case PT_LOAD:
			return "Load";
			break;
		case PT_DYNAMIC:
			return "Dynamic";
			break;
		case PT_INTERP:
			return "Interp";
			break;
		case PT_NOTE:
			return "Note";
			break;
		case PT_SHLIB:
			return "Shlib";
			break;
		case PT_PHDR:
			return "Phdr";
			break;
		case PT_LOPROC:
			return "Loproc";
			break;
		case PT_HIPROC:
			return "Hiproc";
			break;
		default:
			return "Undefined";
	}
}

// extract elf header file
void ElfParser::extractElfHeader() {
	fread(&hdr, sizeof(Elf32_Ehdr), 1, felf);

}

// print elf header to console
void ElfParser::printElfHeader() {
	printf("\nELF HEADER\n");
	printf("-------------------------\n");

	printf("e_ident:     [");
	for (int i=0; i<EI_NIDENT; i++) printf("0x%02x ", hdr.e_ident[i]);
	printf("]\n");
	printf("e_type:      0x%04x\n", hdr.e_type);
	printf("e_machine:   0x%04x\n", hdr.e_machine);
	printf("e_version:   0x%08x\n", hdr.e_version);
	printf("e_entry:     0x%08x\n", hdr.e_entry);
	printf("e_phoff:     0x%08x\n", hdr.e_phoff);
	printf("e_shoff:     0x%08x\n", hdr.e_shoff);
	printf("e_flags:     0x%08x\n", hdr.e_flags);
	printf("e_ehsize:    0x%04x\n", hdr.e_ehsize);
	printf("e_phentsize: 0x%04x\n", hdr.e_phentsize);
	printf("e_phnum:     0x%04x\n", hdr.e_phnum);
	printf("e_shentsize: 0x%04x\n", hdr.e_shentsize);
	printf("e_shnum:     0x%04x\n", hdr.e_shnum);
	printf("e_shstrndx:  0x%04x\n", hdr.e_shstrndx);
}


// accessor methods
vector<string> ElfParser::getNeededLibraries() {
	return this->needed_libraries;
}

string ElfParser::getRpath() {
	return this->rPath;
}

void ElfParser::read(Bit8u *content, Elf32_Off offset, Elf32_Word len) {
	// Position the FILE cursor at OFFSET from the BEGINING of FILE
	fseek(felf, offset, SEEK_SET);

	// read len bytes from file
	Elf32_Word len2 = fread(content, 1, len, felf);

	if (len2 != len) BX_PANIC(("%d bytes read, %d was expected.", len2, len));
}

// print the section string table
Elf32_Addr ElfParser::getGotAddr() {
	for (int i=0; i<shEntries.size(); i++) {
		if (strcmp((char *)&sec_hdr_table[ shEntries[i].sh_name ], ".got") == 0) {
			return shEntries[i].sh_addr;
		}
	}

	return 0;
}
