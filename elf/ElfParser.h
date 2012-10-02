#ifndef __ELF_PARSER__
#define __ELF_PARSER__

#include <vector>
#include <string>
#include <elf.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

class ElfParser {
// private fields
private:
	string fileName;
	FILE *felf;

	// .shstrtab
	unsigned char *sec_hdr_table;
	unsigned int sec_hdr_table_size;

	// .dynstr
	unsigned char *dyn_str_table;
	unsigned int dyn_str_table_size;

	// .strtab
	unsigned char *str_table;
	unsigned int str_table_size;

	Elf32_Ehdr hdr;
	vector<Elf32_Phdr> phEntries;
	vector<Elf32_Shdr> shEntries;
	vector<Elf32_Dyn> dynSecEntries;

	// library shared object name (soname)
	string soname;

	// content of .dynamic sections
	vector<string> needed_libraries;

	// onde procurar bibliotecas compartilhadas
	string rPath;

	// for relocations with implicit addend
	Elf32_Addr rel;				// relocation table address
	Elf32_Word relsz;			// relocation table size
	Elf32_Word relent;		// relocation entry size

	// for relocations with explicit addend
	Elf32_Addr rela;
	Elf32_Word relasz;
	Elf32_Word relaent;

	Elf32_Word pltrel;		// indicates REL or RELA for plt relocation entries
	Elf32_Addr jmprel;		// address of relocation entries associated with PLT
	Elf32_Word pltrelsz;	// total size of relocations associated with plt entries

	Elf32_Addr hash;			// address of hash symbol table
	Elf32_Word dynsym;		// address of dynamic symbol table
	Elf32_Word syment;		// size of each symbol table entry
	Elf32_Addr strtab;		// string table address
	Elf32_Word strsz;			// size of string table

	Elf32_Addr init;			// address of initialization stub
	Elf32_Addr fini;			// address of finalization stub

	Elf32_Addr pltgot;		// address of Global Offset Table

// public methods
public:
	ElfParser(string elfPath);

	// print the section string table
	void printSectionsHdrTable();

	// extract the section header string table content
	void extractSectionsHdrTable();

	void parseDynamicEntries();

	void printDynamicSection();

	// extract each entry of .dynamic section and store in dynSecEntries
	void extractDynamicSection(Elf32_Shdr dSec);

	// extract all string tables content
	void extractStrTabSection(Elf32_Shdr stSec);

	// iterate over Section Header entries and extract the content of each section
	// extract the content of all sections
	void extractSections();

	// extract sections header table
	void extractSecHdrTable();

	// print sections header table
	void printSecHdrTable();

	// parse program header table
	void extractProgHdrTable();

	// return the list of program header entries
	vector<Elf32_Phdr> getProgHdrTable() { this->extractProgHdrTable(); return this->phEntries; }

	// print program header table
	void printProgHdrTable();

	char * segmentTypeName(Elf32_Word type);

	// extract elf header file
	void extractElfHeader();

	// print elf header to console
	void printElfHeader();

	// accessor methods
	vector<string> getNeededLibraries();

	string getRpath();

	void elfClose() { fclose(this->felf); };

	void read(Bit8u *content, Elf32_Off offset, Elf32_Word len);

	Elf32_Addr getEntryAddress() const {
		return hdr.e_entry;
	}

	Elf32_Word getDynsym() const {
		return dynsym;
	}

	Elf32_Addr getFini() const {
		return fini;
	}

	Elf32_Addr getHash() const {
		return hash;
	}

	Elf32_Addr getInit() const {
		return init;
	}

	Elf32_Addr getJmprel() const {
		return jmprel;
	}

	Elf32_Addr getPltgot() const {
		return pltgot;
	}

	Elf32_Word getPltrel() const {
		return pltrel;
	}

	Elf32_Word getPltrelsz() const {
		return pltrelsz;
	}

	Elf32_Addr getRel() const {
		return rel;
	}

	Elf32_Addr getRela() const {
		return rela;
	}

	Elf32_Word getRelaent() const {
		return relaent;
	}

	Elf32_Word getRelasz() const {
		return relasz;
	}

	Elf32_Word getRelent() const {
		return relent;
	}

	Elf32_Word getRelsz() const {
		return relsz;
	}

	Elf32_Word getStrsz() const {
		return strsz;
	}

	Elf32_Addr getStrtab() const {
		return strtab;
	}

	Elf32_Word getSyment() const {
		return syment;
	}

	string getFileName() const {
		return fileName;
	}

	string getSoname() const {
		if (soname == "") BX_ERROR(("SONAME is empty for %s\n", getFileName().c_str()));
		return soname;
	}

	Elf32_Ehdr getHdr() const {
		return hdr;
	}
};

#endif
