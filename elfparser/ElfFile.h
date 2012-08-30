#ifndef __ELF_FILE__
#define __ELF_FILE__

#include <vector>
#include <string>
#include <elf.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

class ElfFile {
	// private fields
	private:
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

		// content of .dynamic sections
		vector<string> needed_libraries;

	// private methods

	// public methods
	public:
		ElfFile(string elfPath) {
			// open elf file
			felf = fopen(elfPath.c_str(), "rb");

			if (!felf) {
				printf("unable to open elf file: %s\n", elfPath.c_str());
				return ;
			}
		
			extractElfHeader();
			//printElfHeader();

			extractProgHdrTable();
			//printProgHdrTable();

			extractSecHdrTable();
			//printSecHdrTable();

			extractSectionsHdrTable();
			//printSectionsHdrTable();

			extractSections();

			printDynamicSection();

			// close elf file
			fclose(felf);
		}

		// print the section string table
		void printSectionsHdrTable() {
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
		void extractSectionsHdrTable() {
			if (hdr.e_shstrndx == 0) {
				printf("No index for section header string table.\n");
				return ;
			}

			Elf32_Shdr hdrt = shEntries[hdr.e_shstrndx];

			if (hdrt.sh_type != SHT_STRTAB) {
				printf("SHSTRNDX does not point to an stringtable.\n");
				return ;
			}

			sec_hdr_table = (unsigned char *) malloc(hdrt.sh_size);
			sec_hdr_table_size = hdrt.sh_size;
			fseek(felf, hdrt.sh_offset, SEEK_SET);
			fread(sec_hdr_table, hdrt.sh_size, 1, felf);
		}

		void parseDynamicSections() {
			for (int i=0; i<dynSecEntries.size(); i++) {
				Elf32_Dyn dyn = dynSecEntries[i];

				switch (dyn.d_tag) {
					case DT_NULL:
						/* nothing needed */
						break;
					case DT_NEEDED:
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

		void printDynamicSection() {
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

		// extract the content of dynamic section
		void extractDynamicSection(Elf32_Shdr dSec) {
			fseek(felf, dSec.sh_offset, SEEK_SET);

			while (true) {
				Elf32_Dyn dyn;

				fread(&dyn, sizeof(Elf32_Dyn), 1, felf);

				dynSecEntries.push_back(dyn);

				if (dyn.d_tag == DT_NULL) break;
			}
		}

		// extract all string tables content
		void extractStrTabSection(Elf32_Shdr stSec) {
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

		// extract the content of all sections
		void extractSections() {
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
		void extractSecHdrTable() {
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
		void printSecHdrTable() {
			printf("\nSECTION HEADER TABLE\n");
			printf("-------------------------\n");

			for (int i=0; i<shEntries.size(); i++) {
				printf("sh_name: 		0x%x\n", shEntries[i].sh_name);
				printf("sh_type: 		0x%x\n", shEntries[i].sh_type);
				printf("sh_flags: 		0x%x\n", shEntries[i].sh_flags);
				printf("sh_addr: 		0x%x\n", shEntries[i].sh_addr);
				printf("sh_offset: 		0x%x\n", shEntries[i].sh_offset);
				printf("sh_size: 		0x%x\n", shEntries[i].sh_size);
				printf("sh_link: 		0x%x\n", shEntries[i].sh_link);
				printf("sh_info: 		0x%x\n", shEntries[i].sh_info);
				printf("sh_addralign: 	0x%x\n", shEntries[i].sh_addralign);
				printf("sh_entsize: 	0x%x\n", shEntries[i].sh_entsize);
				printf("\n");
			}

		}

		// parse program header table 
		void extractProgHdrTable() {
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
		void printProgHdrTable() {
			printf("\nPROGRAM HEADER TABLE\n");
			printf("-------------------------\n");

			for (int i=0; i<phEntries.size(); i++) {
				printf("p_type: 	0x%x\n", phEntries[i].p_type);
				printf("p_offset: 	0x%x\n", phEntries[i].p_offset);
				printf("p_vaddr: 	0x%x\n", phEntries[i].p_vaddr);
				printf("p_paddr: 	0x%x\n", phEntries[i].p_paddr);
				printf("p_filesz: 	0x%x\n", phEntries[i].p_filesz);
				printf("p_memsz: 	0x%x\n", phEntries[i].p_memsz);
				printf("p_flags: 	0x%x\n", phEntries[i].p_flags);
				printf("p_align: 	0x%x\n", phEntries[i].p_align);
				printf("\n");
			}
		}

		// extract elf header file
		void extractElfHeader() {
			fread(&hdr, sizeof(Elf32_Ehdr), 1, felf);
		
		}

		// print elf header to console
		void printElfHeader() {
			printf("\nELF HEADER\n");
			printf("-------------------------\n");

			printf("e_ident: 		[");
			for (int i=0; i<EI_NIDENT; i++) printf("%x ", hdr.e_ident[i]);
			printf("]\n");
			printf("e_type: 		0x%x\n", hdr.e_type);
			printf("e_machine: 		0x%x\n", hdr.e_machine);
			printf("e_version: 		0x%x\n", hdr.e_version);
			printf("e_entry: 		0x%x\n", hdr.e_entry);
			printf("e_phoff: 		0x%x\n", hdr.e_phoff);
			printf("e_shoff: 		0x%x\n", hdr.e_shoff);
			printf("e_flags: 		0x%x\n", hdr.e_flags);
			printf("e_ehsize: 		0x%x\n", hdr.e_ehsize);
			printf("e_phentsize: 	0x%x\n", hdr.e_phentsize);
			printf("e_phnum: 		0x%x\n", hdr.e_phnum);
			printf("e_shentsize: 	0x%x\n", hdr.e_shentsize);
			printf("e_shnum: 		0x%x\n", hdr.e_shnum);
			printf("e_shstrndx: 	0x%x\n", hdr.e_shstrndx);
		}
};

#endif
