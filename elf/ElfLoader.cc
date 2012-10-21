#include "ElfLoader.h"

#include <iostream>
#include <queue>
#include <set>
#include <map>
#include <string>
#include <elf.h>

#include "config.h"
#include "debug.h"
#include "ElfParser.h"
#include "memory/memory.h"

extern BX_MEM_C bx_mem;

ElfLoader::ElfLoader(int p_argc, char **p_argv, char *ldLibPath, Bit8u *p_memory, Bit32u mem_size) : mainExecutable(p_argv[1]) {
	this->executablePath 	= string(p_argv[1]);
	this->ldLibraryPath 	= ldLibPath;
	this->memory 					= p_memory;
	this->memory_indx			= 0;
	this->memory_size			= mem_size;
	this->argc						= p_argc;
	this->argv						= p_argv;

	// check if the argument was indeed an ELF executable
	// file.
	if (mainExecutable.getHdr().e_type != ET_EXEC) {
		BX_PANIC(("%s isn't an ELF executable file.", p_argv[1]));
	}

	BX_INFO(("Main executable loaded."));

	// parse the content of ld.so.cache
	BX_INFO(("Parsing the content of ld.so.cache"));
	parseLdCache();
	BX_INFO(("ld.so.cache parsed read %d definitions.", ldCache.size()));

	// start loading shared libs
	BX_INFO(("Loading shared libraries."));
	loadSharedLibs();
	BX_INFO(("Expanding symbol lookup scope."));
	expandSymbolScopeMap();
	//dumpSymbolScopeMap();
	BX_INFO(("%d libraries loaded.", sharedLibs.size()));

	// create the process address space
	BX_INFO(("Mounting process address space."));
	createAddressSpace();
	dumpAddressSpaceInfo();
	BX_INFO(("Address space mounted."));

	// Doing relocations
	BX_INFO(("Doing relocations."));
	doRelocations();
	BX_INFO(("Relocations done."));
}

/*!
 * Load all dependencies of mainExecutable and the dependencies
 * of its dependencies and so on..
 * Fill the first level of a DFS scope index for symbol resolution.
 */
void ElfLoader::loadSharedLibs() {
	// first of all we will solve the dependencies for the
	// main executable.
	// get list of dynamically needed libraries
	vector<string> libs = mainExecutable.getNeededLibraries();

	// auxiliary variable
	vector<Bit8s> scope;

	// the first item symbol scope list is the mainExecutable
	// for libraries is the mainExecutable plus the library
	scope.push_back(-1);

	// iterate over all LIBs
	for (int i=0; i<libs.size(); i++) {
		string libName = libs[i];

		// check if the LIB isn't already loaded
		// if it is take its loaded number and insert into the symbol scope map
		// because when doing symbol lookup we need to consider this library
		if (alreadyLoadedLibs.find(libName) != alreadyLoadedLibs.end()) {
			scope.push_back(alreadyLoadedLibs[libName]);
			continue;
		}

		// find the library
		string libPath = findLibrary(libName, mainExecutable.getRpath());

		if (libPath != "") {
			ElfParser lib(libPath);

			// new shared library to insert in address space
			Bit32u libIndex = sharedLibs.size();
			sharedLibs.push_back(lib);

			// annotate to verify the dependencies
			hasDependencies.push(lib);

			// mark as already checked library
			alreadyLoadedLibs[libName] = libIndex;

			// insert into symbol scope..
			scope.push_back(libIndex);
		}
		else {
			BX_ERROR(("Library not found: %s", libName.c_str()));
		}
	}

	// save mainExecutable scope
	symbolScopeMap.push_back(scope);
	scope.clear();


	// Now re solve all dependencies between shared libraries
	// while still have an ELF object with dependencies not satisfied
	while (!hasDependencies.empty()) {
		ElfParser elf = hasDependencies.front(); hasDependencies.pop();

		// the first item symbol scope list is the mainExecutable
		// for libraries is the mainExecutable plus the library
		scope.push_back(-1); 									// mainExecutable
		scope.push_back(alreadyLoadedLibs[elf.getSoname()]);	// the library itself

		// get list of dynamically needed libraries
		vector<string> libs = elf.getNeededLibraries();

		// iterate over all libs
		for (int i=0; i<libs.size(); i++) {
			string libName = libs[i];

			// check if the lib isn't already loaded
			if (alreadyLoadedLibs.find(libName) != alreadyLoadedLibs.end()) {
				scope.push_back(alreadyLoadedLibs[libName]);
				continue;
			}

			// find the library
			string libPath = findLibrary(libName, elf.getRpath());

			if (libPath != "") {
				ElfParser lib(libPath);

				// new shared library to insert in address space
				Bit32u libIndex = sharedLibs.size();
				sharedLibs.push_back(lib);

				// annotate to verify the dependencies
				hasDependencies.push(lib);

				// mark as already checked library
				alreadyLoadedLibs[libName] = libIndex;

				// update library symbol scope
				scope.push_back(libIndex);
			}
			else {
				BX_ERROR(("Library not found: %s", libName.c_str()));
			}
		}

		// save mainExecutable scope
		symbolScopeMap.push_back(scope);
		scope.clear();
	}
}

/*!
 * This function expand the symbol scope resolution across
 * the levels of a breadth-first-search. That is, loadSharedLibs
 * already filled scope with the first level dependencies, this
 * method expand the scope with entries of further levels.
 */
void ElfLoader::expandSymbolScopeMap() {
	// for each shared library and the mainExecutable
	for (int sI=0; sI<symbolScopeMap.size(); sI++) {
		vector<Bit8s> scope = symbolScopeMap[sI];

		// start on the third item in scope list, because the first
		// is mainExecutable and it is above all nodes, also skip
		// the node it self (second) because it already has its
		// first level entries expanded (they are the next entries)
		for (int scopeI=2; scopeI<scope.size(); scopeI++) {
			Bit8s dep = scope[scopeI];

			// + 1 because the indexes are in relation to sharedLibs
			// entries but in symbolScope there are +1 entry for the
			// mainExecutable
			vector<Bit8s> scopeTarget = symbolScopeMap[dep+1];

			// iterate over all dependencies of the target and
			// add those that haven't already been added
			// start in 2 because first is mainExecutable and
			// second entry is DEP (above)
			for (int sTarIndex=2; sTarIndex<scopeTarget.size(); sTarIndex++) {
				Bit8s depTar = scopeTarget[sTarIndex];
				bool found	 = false;

				// search on the library scope of 'dep' to see if any of
				// its libraries isn't in 'sI' scope
				for (int scopeIPrev=0; scopeIPrev<scope.size(); scopeIPrev++) {
					if (scope[scopeIPrev] == depTar) {
						found = true;
						break;
					}
				}

				// if this library wasn't already addeded in the scope of sI
				// add it now
				if (!found) {
					scope.push_back(depTar);
				}
			}
		}

		// we need update the 'sI' entry in symbolScopeMap
		symbolScopeMap[sI] = scope;
	}
}

void ElfLoader::dumpSymbolScopeMap() {
	for (int sI=0; sI<symbolScopeMap.size(); sI++) {
		if (sI == 0)
			printf("[%15s] ", "Main");
		else
			printf("[%15s] ", this->sharedLibs[sI-1].getSoname().c_str());

		vector<Bit8s> scope = symbolScopeMap[sI];

		for (int scopeI=0; scopeI<scope.size(); scopeI++) {
			if (scope[scopeI] == -1)
				printf("%15s", "Main");
			else
				printf("%15s ", this->sharedLibs[scope[scopeI]].getSoname().c_str());
		}

		printf("\n");
	}
}

/*!
 * procura pela biblioteca com SONAME libName.
 *
 * Search order:
 * DT_RPATH, LD_LIBRARY_PATH, DT_RUNPATH, ld.so.cache, /lib e /usr/lib
 *
 * DT_RPATH and DT_RUNPATH are not generally used, here we
 * omit support for DT_RUNPATH as it doesn't appear in ELF
 * specification.
 */
string ElfLoader::findLibrary(string libName, string Rpath) {
	// search for the library in DT_RPATH
	if (Rpath != "") {
		vector<string> paths = parseLibPath(Rpath);

		// verify the existence of the library in each path
		for (int i=0; i<paths.size(); i++) {
			string path = paths[i] + "/" + libName;

			FILE *fp = fopen(path.c_str(), "r");
			if (fp) {
				BX_DEBUG(("Lib %s found at %s", libName.c_str(), path.c_str()));
				fclose(fp);
				return path;
			}
		}
	}

	// Check if the library is present in LD_LIBRARY_PATH
	if (ldLibraryPath != NULL) {
		vector<string> paths = parseLibPath(ldLibraryPath);

		// verify the existence of the library in each path
		for (int i=0; i<paths.size(); i++) {
			string path = paths[i] + "/" + libName;

			FILE *fp = fopen(path.c_str(), "r");
			if (fp) {
				BX_DEBUG(("Lib %s found at %s", libName.c_str(), path.c_str()));
				fclose(fp);
				return path;
			}
		}
	}

	// Check if the library is in DT_RUNPATH
	// omited


	// Check if the library is in ld.so.cache
	for (int cIndex=0; cIndex<this->ldCache.size(); cIndex++) {
		if (this->ldCache[cIndex].first == libName) {
			BX_DEBUG(("Lib %s found at %s", libName.c_str(), this->ldCache[cIndex].second.c_str()));
			return this->ldCache[cIndex].second;
		}
	}

	// Finally, check if the library is present in /lib or /usr/lib
	string pathUsr = "/usr/lib/" + libName;
	FILE *fpUsr = fopen(pathUsr.c_str(), "r");
	if (fpUsr) {
		BX_DEBUG(("Lib %s found at %s", libName.c_str(), pathUsr.c_str()));
		fclose(fpUsr);
		return pathUsr;
	}

	string pathLib = "/lib/" + libName;
	FILE *fpLib = fopen(pathLib.c_str(), "r");
	if (fpLib) {
		BX_DEBUG(("Lib %s found at %s", libName.c_str(), pathLib.c_str()));
		fclose(fpLib);
		return pathLib;
	}

	// Library not found
	return "";
}


vector<string> ElfLoader::parseLibPath(string str) {
	return parseLibPath(str.c_str());
}


vector<string> ElfLoader::parseLibPath(char *str) {
	vector<string> paths;
	char *aux = strdup(str);
	char *p = strtok(aux, ":");

	while (p != NULL) {
		paths.push_back(p);
		p = strtok(NULL, ":");
	}

	return paths;
}

/*!
 * Load the main executable and shared object dependencies in the main memory ptr.
 */
void ElfLoader::createAddressSpace() {
	// list of all segments in mainExecutable
	vector<Elf32_Phdr> segments = mainExecutable.getProgHdrTable();

	// just to count the number of loadable segments
	int numLoadable = 0;

	// Position where the previous segment preferred to be loaded
	Bit32u prevBase = 0;

	// find loadable segments
	for (int segIndex=0; segIndex<segments.size(); segIndex++) {
		if (segments[segIndex].p_type == PT_LOAD) {
			numLoadable++;
			if (numLoadable > 2) {
				BX_PANIC(("Num. of Loadable segments is greater than 2 in %s.",
						         mainExecutable.getFileName().c_str()));
			}

			// the first loadable segment goes in the first by of memory
			// subsequent segments goes in (p_vaddr - prevBase) bytes from
			// the begining of the previous loaded segment
			if (segments[segIndex].p_offset == 0) {
				this->memory_indx	= 0;
				prevBase 			= segments[segIndex].p_vaddr;

				// register base virtual address in memory module
				bx_mem.setVirtualMemBase(segments[segIndex].p_vaddr);
			}
			else {
				this->memory_indx	= (segments[segIndex].p_vaddr - prevBase);
				prevBase 			= segments[segIndex].p_vaddr;
			}

			// segment description
			LoadedSegment segDesc;
			segDesc.fileName 			= mainExecutable.getFileName();
			segDesc.segmentIndex 	= segIndex;
			segDesc.hdr 					= segments[segIndex];
			segDesc.loadedPos 		= this->memory_indx;

			this->loadedSegments.push_back(segDesc);

			// read p_filesz bytes from file
			mainExecutable.read(&this->memory[this->memory_indx], segments[segIndex].p_offset, segments[segIndex].p_filesz);

			// fills the remainder p_memsz - p_filesz bytes with zero
			for (int j=segments[segIndex].p_filesz; j<segments[segIndex].p_memsz; j++)
				this->memory[this->memory_indx + j] = 0;

			this->memory_indx += segments[segIndex].p_memsz;
		}
	}
	// ok main executable loaded


	/*
	 * Program break points to the first byte after .bss of main executable.
	 */
	bx_mem.setOrigProgramBreak(this->memory_indx);
	bx_mem.setProgramBreak(this->memory_indx);

	/*
	 * Now allocate space for the heap just after the DATA (.data .bss) segment
	 * of the main executable.
	 */
	LoadedSegment segDesc;
	segDesc.fileName 		= "Heap";
	segDesc.segmentIndex 	= 0;
	segDesc.hdr.p_align 	= segDesc.hdr.p_filesz = segDesc.hdr.p_flags = 0;
	segDesc.hdr.p_memsz		= segDesc.hdr.p_offset = segDesc.hdr.p_paddr = 0;
	segDesc.hdr.p_type		= segDesc.hdr.p_vaddr  = 0;
	segDesc.loadedPos 		= this->memory_indx;

	Bit32u ind = this->memory_indx;
	for (this->memory_indx; this->memory_indx<ind+(10*1024*1024); this->memory_indx++) {
		this->memory[this->memory_indx] = 0;
	}

	this->loadedSegments.push_back(segDesc);


	/*
	 * Library start is the point where the first library was loaded.
	 */
	bx_mem.setLibraryStart(this->memory_indx);


	/*
	 * Load shared libraries segments.
	 */
	for (int sIndex = 0; sIndex<sharedLibs.size(); sIndex++) {
		// list of all segments in the shared library
		segments 	= sharedLibs[sIndex].getProgHdrTable();
		numLoadable = 0;

		// find loadable segments
		for (int i=0; i<segments.size(); i++) {
			if (segments[i].p_type == PT_LOAD) {
				numLoadable++;
				if (numLoadable > 2) {
					BX_PANIC(("Num. of Loadable segments is greater than 2 in %s.",
							         sharedLibs[sIndex].getFileName().c_str()));
				}

				if (segments[i].p_vaddr == 0) {
					prevBase 			= this->memory_indx;
				}
				else {
					// start of previous segment + offset
					this->memory_indx	= prevBase + (segments[i].p_vaddr - 0);
					prevBase 			= this->memory_indx;
				}

				// segment description
				LoadedSegment segDesc;
				segDesc.fileName 		= sharedLibs[sIndex].getFileName();
				segDesc.segmentIndex 	= i;
				segDesc.hdr 			= segments[i];
				segDesc.loadedPos 		= this->memory_indx;

				this->loadedSegments.push_back(segDesc);

				// read p_filesz bytes from file
				sharedLibs[sIndex].read(&this->memory[this->memory_indx], segments[i].p_offset, segments[i].p_filesz);

				// fills the remainder p_memsz - p_filesz bytes with zero
				for (int j=segments[i].p_filesz; j<segments[i].p_memsz; j++)
					this->memory[this->memory_indx + j] = 0;

				this->memory_indx += segments[i].p_memsz;
			}
		}
	}
	// ok loading shared libs segments






	/*
	 * stack is initialized at memory_size
	 */
	segDesc.fileName 		= "Stack";
	segDesc.segmentIndex 	= 0;
	segDesc.hdr.p_align 	= segDesc.hdr.p_filesz = segDesc.hdr.p_flags = 0;
	segDesc.hdr.p_memsz		= segDesc.hdr.p_offset = segDesc.hdr.p_paddr = 0;
	segDesc.hdr.p_type		= segDesc.hdr.p_vaddr  = 0;
	segDesc.loadedPos 		= this->memory_size;

	this->loadedSegments.push_back(segDesc);
}


void ElfLoader::dumpAddressSpaceInfo() {
	printf("%30s %10s %12s %s %5s %12s %12s %12s %12s %12s %12s %12s\n", "File", "SegIndx", "RealPos", "|", "Type","Offset","VirtAddr","PhysAddr","FileSiz","MemSiz","Flag","Align");

	for (int i=0; i<this->loadedSegments.size(); i++) {
		LoadedSegment segDesc = this->loadedSegments[i];

		printf("%30s ", segDesc.fileName.c_str());
		printf("%10d ", segDesc.segmentIndex);
		printf("0x%010x ", segDesc.loadedPos);
		printf("|");

		printf("0x%04x ", segDesc.hdr.p_type);
		printf("0x%010x ", segDesc.hdr.p_offset);
		printf("0x%010x ", segDesc.hdr.p_vaddr);
		printf("0x%010x ", segDesc.hdr.p_paddr);
		printf("0x%010x ", segDesc.hdr.p_filesz);
		printf("0x%010x ", segDesc.hdr.p_memsz);
		printf("0x%010x ", segDesc.hdr.p_flags);
		printf("0x%010x ", segDesc.hdr.p_align);

		printf("\n");
	}
}

// almost there
// falta tratar enderecos qdo o st_value eh do mainExecutable ou shared lib
// verificar tipos dos objetos
void ElfLoader::doRelocations() {
	// pointer to relocation tables
	// REL and RELA are relocations for data and
	// JMPREL is the relocation table for PLT
	Elf32_Addr rel 		= mainExecutable.getRel();
	Elf32_Addr jmprel	= mainExecutable.getJmprel();
	Elf32_Addr rela 	= mainExecutable.getRela();

	if (rel == 0 && jmprel == 0  && rela == 0) BX_DEBUG(("REL and RELA equals zero for mainExecutable."));

	// Data Relocations
	// check if there are any relocation (implicit) for the
	// main executable
	if (rel > 0) {
		// bytes already read
		Bit32u aRead = 0;

		rel = bx_mem.virtualAddressToPosition(rel);

//		fprintf(stderr, "Data Relocation Table Entries (REL) [%x] (%s)\n", rel, mainExecutable.getFileName().c_str());
//		fprintf(stderr, "------------------------------\n");
//		fprintf(stderr, "%10s %10s %10s %10s\n", "Offset","Info","Symb. Ind.","Rel. Type");
		while (aRead < mainExecutable.getRelsz()) {
			Elf32_Rel reloc;

			bx_mem.read((Bit8u *)&reloc, rel, mainExecutable.getRelent());

//			printf("0x%08x 0x%08x 0x%08x 0x%08x\n", reloc.r_offset, reloc.r_info, ELF32_R_SYM(reloc.r_info), ELF32_R_TYPE(reloc.r_info));

			// scopeMap index for mainExecutable is zero
			solveRelocation(reloc, 0);

			aRead 	+= mainExecutable.getRelent();
			rel		+= mainExecutable.getRelent();
		}
	}

	// PLT Relocations
	if (jmprel > 0) {
		// bytes already read
		Bit32u aRead = 0;

		jmprel = bx_mem.virtualAddressToPosition(jmprel);

//		fprintf(stderr, "PLT Relocation Table Entries (JMPREL) [%x] (%s)\n", jmprel, mainExecutable.getFileName().c_str());
//		fprintf(stderr, "------------------------------\n");
//		fprintf(stderr, "%10s %10s %10s %10s\n", "Offset","Info","Symb. Ind.","Rel. Type");
		while (aRead < mainExecutable.getPltrelsz()) {
			Elf32_Rel reloc;

			bx_mem.read((Bit8u *)&reloc, jmprel, mainExecutable.getRelent());

//			printf("0x%08x 0x%08x 0x%08x 0x%08x\n", reloc.r_offset, reloc.r_info, ELF32_R_SYM(reloc.r_info), ELF32_R_TYPE(reloc.r_info));

			// scopeMap index for mainExecutable is zero
			solveRelocation(reloc, 0);

			aRead 	+= mainExecutable.getRelent();
			jmprel	+= mainExecutable.getRelent();
		}
	}

	// check each shared library for relocations that
	// need to be solved
	//
	// these 2*(slIndex + 1) calculations are for making correspondence
	// between the sharedLib index and the loaded segments. That is,
	// for each shared library there were loaded two segments (TEXT and
	// DATA) and we need increment one because the mainExecutable has
	// also two segments loaded but it isn't in sharedLibs, so we need
	// to skip its index.
	for (int slIndex = 0; slIndex<sharedLibs.size(); slIndex++) {
		rel 	= sharedLibs[slIndex].getRel();
		jmprel	= sharedLibs[slIndex].getJmprel();
		rela 	= sharedLibs[slIndex].getRela();

		if (rel == 0  && jmprel == 0 && rela == 0) BX_DEBUG(("REL and RELA equals zero for SharedLib[%d].", slIndex));

		// Data Relocations
		// check if there are any relocation (implicit) for the
		// shared library
		if (rel > 0) {
			// bytes already read
			Bit32u aRead = 0;

			// rel indicates an offset inside the shared library code
			rel = loadedSegments[2*slIndex + 3].loadedPos + rel;

//			fprintf(stderr, "Data Relocation Table Entries (REL) (%x) [%x] (%s)\n", rel - loadedSegments[2*slIndex + 3].loadedPos, rel, sharedLibs[slIndex].getFileName().c_str());
//			fprintf(stderr, "------------------------------\n");
//			fprintf(stderr, "%10s %10s %10s %10s\n", "Offset","Info","Symb. Ind.","Rel. Type");
			while (aRead < sharedLibs[slIndex].getRelsz()) {
				Elf32_Rel reloc;

				bx_mem.read((Bit8u *)&reloc, rel, sharedLibs[slIndex].getRelent());

//				printf("0x%08x 0x%08x 0x%08x 0x%08x\n", reloc.r_offset, reloc.r_info, ELF32_R_SYM(reloc.r_info), ELF32_R_TYPE(reloc.r_info));

				// scopeMap index for sharedLibrary is +1 because in
				// scopeMap there is one entry for mainExecutable
				solveRelocation(reloc, slIndex+1);

				aRead 	+= sharedLibs[slIndex].getRelent();
				rel		+= sharedLibs[slIndex].getRelent();
			}
		}

		// PLT Relocations
		if (jmprel > 0) {
			// bytes already read
			Bit32u aRead = 0;

			// rel indicates an offset inside the shared library code
			jmprel = loadedSegments[2*slIndex + 3].loadedPos + jmprel;

//			fprintf(stderr, "PLT Relocation Table Entries (JMPREL) (%x) [%x] (%s)\n", jmprel - loadedSegments[2*slIndex + 3].loadedPos, jmprel, sharedLibs[slIndex].getFileName().c_str());
//			fprintf(stderr, "------------------------------\n");
//			fprintf(stderr, "%10s %10s %10s %10s\n", "Offset","Info","Symb. Ind.","Rel. Type");
			while (aRead < sharedLibs[slIndex].getPltrelsz()) {
				Elf32_Rel reloc;

				bx_mem.read((Bit8u *)&reloc, jmprel, sharedLibs[slIndex].getRelent());

//				printf("0x%08x 0x%08x 0x%08x 0x%08x\n", reloc.r_offset, reloc.r_info, ELF32_R_SYM(reloc.r_info), ELF32_R_TYPE(reloc.r_info));
				// scopeMap index for sharedLibrary is +1 because in
				// scopeMap there is one entry for mainExecutable
				solveRelocation(reloc, slIndex+1);

				aRead 	+= sharedLibs[slIndex].getRelent();
				jmprel	+= sharedLibs[slIndex].getRelent();
			}
		}
	}
}

/*!
 * This method is responsible to manage the symbol lookup scope
 * search, identify what kind of relocation is need and apply it.
 */
void ElfLoader::solveRelocation(Elf32_Rel reloc, Bit8u scopeIndex) {
	// offset where relocation apply
	Elf32_Addr relOff 	= reloc.r_offset;

	Elf32_Word addend 	= 0;
	Elf32_Word finValue	= 0;
	Elf32_Addr libBase	= 0;
	Elf32_Addr gotAddr	= 0;

	if (scopeIndex > 0) {
		Bit32u slIndex = scopeIndex-1;

		relOff 	= bx_mem.positionToVirtualAddress(loadedSegments[2*slIndex + 3].loadedPos + relOff);
		libBase = bx_mem.positionToVirtualAddress(loadedSegments[2*slIndex + 3].loadedPos);
		gotAddr	= bx_mem.positionToVirtualAddress(loadedSegments[2*slIndex + 3].loadedPos + sharedLibs[slIndex].getGotAddr());
	}
	else {
		libBase = bx_mem.positionToVirtualAddress(loadedSegments[0].loadedPos);
		gotAddr	= mainExecutable.getGotAddr();
	}

	// for what symbol is the relocation
	Bit32u symbIndex 	= ELF32_R_SYM(reloc.r_info);

	// what type of relocation is to be made
	Bit32u relType 		= ELF32_R_TYPE(reloc.r_info);

	// the symbol being relocated
	Elf32_Sym symbol;

	// if the relocation need a symbol solve it address now
	if (relType == 1 || relType == 2 || relType == 6 || relType == 7 || relType == 9) {
		bool symbFound = symbolLookup(symbIndex, scopeIndex, &symbol);

		if (!symbFound) {
		    BX_ERROR(("Symbol not found!"));
		}
	}

	bx_mem.read((Bit8u *)&addend, bx_mem.virtualAddressToPosition(relOff), sizeof(Elf32_Word));

	#define G 	(symbol.st_value)
	#define A 	(addend)
	#define S 	(symbol.st_value)
	#define P 	(relOff)
	#define L 	(symbol.st_value)
	#define B 	(libBase)
	#define GOT (gotAddr)

	// identify and solve the relocation
	switch (relType) {
		case R_386_NONE:
			break;
		case R_386_32:
			//printf("R_386_32: S + A = %x + %x\n", S, A);
			finValue = S + A;
			break;
		case R_386_PC32:
			//printf("R_386_PC32: S + A - P = %x + %x - %x = %x\n", S, A, P, S + A - P);
			finValue = S + A - P;
			break;
		case R_386_GOT32:
			//printf("R_386_GOT32: G + A = %x + %x\n", G, A);
			finValue = G + A;
			break;
		case R_386_PLT32:
			//printf("R_386_PLT32: L + A - P = %x + %x - %x\n", L, A, P);
			finValue = L + A - P;
			break;
		case R_386_COPY:
			//printf("R_386_COPY\n");
			bx_mem.copy(bx_mem.virtualAddressToPosition(symbol.st_value), bx_mem.virtualAddressToPosition(relOff), symbol.st_size);
			return ;
		case R_386_GLOB_DAT:
			//printf("R_386_GLOB_DAT: S = %x\n", S);
			finValue = S;
			break;
		case R_386_JMP_SLOT:
			//printf("R_386_JMP_SLOT: S = %x\n", S);
			finValue = S;
			break;
		case R_386_RELATIVE:
			//printf("R_386_RELATIVE: B + A = %x + %x\n", B, A);
			finValue = B + A;
			break;
		case R_386_GOTOFF:
			//printf("R_386_GOTOFF: S + A - GOT = %x + %x - %x\n", S, A, GOT);
			finValue = S + A - GOT;
			break;
		case R_386_GOTPC:
			//printf("R_386_GOTPC: GOT + A - P = %x + %x - %x\n", GOT, A, P);
			finValue = GOT + A - P;
			break;
		case R_386_32PLT:
		    /* not supported */
		case R_386_TLS_TPOFF:
		case R_386_TLS_IE:
        case R_386_TLS_GOTIE:
        case R_386_TLS_LE:
        case R_386_TLS_GD:
        case R_386_TLS_LDM:
        case R_386_16:
        case R_386_PC16:
        case R_386_8:
        case R_386_PC8:
        case R_386_TLS_GD_32:
        case R_386_TLS_GD_PUSH:
        case R_386_TLS_GD_CALL:
        case R_386_TLS_GD_POP:
        case R_386_TLS_LDM_32:
        case R_386_TLS_LDM_PUSH:
        case R_386_TLS_LDM_CALL:
        case R_386_TLS_LDM_POP:
        case R_386_TLS_LDO_32:
        case R_386_TLS_IE_32:
        case R_386_TLS_LE_32:
        case R_386_TLS_DTPMOD32:
        case R_386_TLS_DTPOFF32:
        case R_386_TLS_TPOFF32:
        case R_386_TLS_GOTDESC:
        case R_386_TLS_DESC_CALL:
        case R_386_TLS_DESC:
        case R_386_IRELATIVE:
        case R_386_NUM:
            break;
		default:
			BX_ERROR(("Relocation type don't identified: %x", relType));
			break;
	}

	bx_mem.write((Bit8u *)&finValue, bx_mem.virtualAddressToPosition(relOff), sizeof(Elf32_Word));
}

bool ElfLoader::symbolLookup(Bit32u symbIndex, Bit8u scopeIndex, Elf32_Sym *symbol) {
	Elf32_Addr hashAddr = 0, gnuHashAddr = 0;

	if (scopeIndex == 0) { // main executable
		hashAddr 	= mainExecutable.getHash();
		gnuHashAddr = mainExecutable.getGnuHash();
	}
	else {	// shared library (-1)
		Bit32u slIndex = scopeIndex-1;

		hashAddr 	= sharedLibs[slIndex].getHash();
		gnuHashAddr = sharedLibs[slIndex].getGnuHash();
	}

	// identify the best type of hash to use
	if (gnuHashAddr != 0) {
		return symbolLookupGnuHash(symbIndex, scopeIndex, symbol);
	}
	else if (hashAddr != 0) {
		return symbolLookupElfHash(symbIndex, scopeIndex, symbol);
	}
	else {
		BX_ERROR(("Object doesn't have an HASH or GNU_HASH section."));
		return false;
	}
}

/*!
 * This method is responsible for searching an symbol definition
 * across the symbolScopeMap. Return the virtual address where
 * the symbol where loaded.
 */
bool ElfLoader::symbolLookupElfHash(Bit32u symbIndex, Bit8u scopeIndex, Elf32_Sym *symbol) {
	// Check if the symbol index is valid
	if (symbIndex == 0) {
		BX_ERROR(("Symbol with index zero passed to lookup."));
		return 0;
	}

	// the scope which we should search the symbol
	vector<Bit8s> scope = symbolScopeMap[scopeIndex];

	// contains the symbol name
	Bit8u *symName = NULL;

	// contains the local definition of the symbol we are searching
	Elf32_Sym symLoc;

	// we first get the symbol name in the symbol table/str table
	// of the ELF that has the relocation
	if (scopeIndex == 0) { // main executable
		symLoc = symbolFromSymbIndex(mainExecutable, symbIndex, -loadedSegments[0].hdr.p_vaddr);
		symName = symbolNameFromSymbol(mainExecutable, symLoc, -loadedSegments[0].hdr.p_vaddr);
	}
	else {	// shared library (-1)
		Bit32u slIndex = scopeIndex-1;

		symLoc = symbolFromSymbIndex(sharedLibs[slIndex], symbIndex, loadedSegments[2*slIndex + 3].loadedPos);
		symName = symbolNameFromSymbol(sharedLibs[slIndex], symLoc, loadedSegments[2*slIndex + 3].loadedPos);
	}

//	printf("Searching symbol: %s\n", symName);
//	printf("Scope size: %d\n", scope.size());

	// for each entry in scope
	for (int sI=0; sI<scope.size(); sI++) {
		Bit8s scEntry 	= scope[sI];

//		printf("In sI = %d, scEntry = %d\n", sI, scEntry);

		// these first two case cope with the case where we need
		// to search the symbol directly in the dynsym table, the
		// last case threat the case where we need a hash of the
		// symbol name to find an entry for it in the dynsym table.
		if (scopeIndex == 0 && sI == 0) {	// symbol search by index
			// symbol table address
			Elf32_Addr symTableAddr = mainExecutable.getDynsym();

			// physical address
			Elf32_Addr symEntry = bx_mem.virtualAddressToPosition(symTableAddr + symbIndex*sizeof(Elf32_Sym));

			// read the symbol definition
			bx_mem.read((Bit8u *)symbol, symEntry, sizeof(Elf32_Sym));

			// verifica se o symbol em symbEntry eh o procurado
			if (symbol->st_shndx != SHN_UNDEF) {
				Bit8u *actSymbName = symbolNameFromSymbol(mainExecutable, *symbol, -loadedSegments[0].hdr.p_vaddr);
//				printf("\tTesting symbol %s\n", actSymbName);
				if (strcmp((char *)actSymbName, (char *)symName) == 0) {
					return true;
				}
			}
		}
		else if (scopeIndex > 0 && sI == 1) {	// symbol search by index
			Bit32u slIndex = scopeIndex-1;

			// symbol table address
			Elf32_Addr symTableAddr = sharedLibs[slIndex].getDynsym();

			// physical address
			Elf32_Addr symEntry = loadedSegments[2*slIndex + 3].loadedPos + symTableAddr + symbIndex*sizeof(Elf32_Sym);

			// read the symbol definition
			bx_mem.read((Bit8u *)symbol, symEntry, sizeof(Elf32_Sym));

			// verifica se o symbol em symbEntry eh o procurado
			if (symbol->st_shndx != SHN_UNDEF) {
				Bit8u *actSymbName = symbolNameFromSymbol(sharedLibs[slIndex], *symbol, loadedSegments[2*slIndex + 3].loadedPos);
//				printf("\tTesting symbol %s\n", actSymbName);
				if (strcmp((char *)actSymbName, (char *)symName) == 0) {
				    symbol->st_value = bx_mem.positionToVirtualAddress(loadedSegments[2*slIndex + 3].loadedPos + symbol->st_value);
					return true;
				}
			}
		}
		else {	// symbol search by name hash
			Elf32_Addr hashAddr = 0, bucksAddr = 0, chainsAddr = 0, symTableAddr = 0;
			Elf32_Word nbucket  = 0, nchain    = 0;
			Bit32s loadedPos = 0;
			ElfParser *elf;

			// the current object scope is the mainExecutable or is a shared lib?
			if (scEntry == -1) {
				elf = &mainExecutable;

				hashAddr = mainExecutable.getHash();
				hashAddr = bx_mem.virtualAddressToPosition(hashAddr);

				symTableAddr = mainExecutable.getDynsym();
				symTableAddr = bx_mem.virtualAddressToPosition(symTableAddr);

				loadedPos = -loadedSegments[0].hdr.p_vaddr;
			}
			else {
				elf = &sharedLibs[scEntry];

				hashAddr = sharedLibs[scEntry].getHash();
				hashAddr = loadedSegments[2*scEntry + 3].loadedPos + hashAddr;

				symTableAddr = sharedLibs[scEntry].getDynsym();
				symTableAddr = loadedSegments[2*scEntry + 3].loadedPos + symTableAddr;

				loadedPos = loadedSegments[2*scEntry + 3].loadedPos;
			}

			// read the number of buckets and the number of chains
			//fprintf(stderr, "Name: %s\n", elf->getFileName().c_str());
			//fprintf(stderr, "hashAddr=%lx\n" ,hashAddr);
			bx_mem.read((Bit8u *)&nbucket, hashAddr, sizeof(nbucket));
			bx_mem.read((Bit8u *)&nchain, hashAddr + sizeof(nbucket), sizeof(nchain));

			//printf("Name: %s\n", elf->getFileName().c_str());
			//printf("Nbucket %d\n", nbucket);
			//printf("Nchain %d\n", nchain);

			// bucks start  8 bytes after hash address
			// chains start 8 bytes + all buckets after hash address
			bucksAddr 	= hashAddr + 2*sizeof(nbucket);
			chainsAddr 	= bucksAddr + nbucket*sizeof(nbucket);

			// hash to access nbuckets
			Elf32_Word x = elfHash(symName);
			Elf32_Word h = x % nbucket;

			// the symbol content
			Elf32_Word y;

			// index buckets by y and get symbol table entry
			bx_mem.read((Bit8u *)&y, bucksAddr + h * sizeof(nbucket), sizeof(y));

			while (y != 0) {
				// symbol table entry pointed by nbucket
				Elf32_Addr symEntry = symTableAddr + y*sizeof(Elf32_Sym);

				// read the symbol definition
				bx_mem.read((Bit8u *)symbol, symEntry, sizeof(Elf32_Sym));

				// verifica se o symbol em symbEntry eh o procurado
				if (symbol->st_shndx != SHN_UNDEF &&
					(ELF32_ST_BIND(symbol->st_info) == STB_GLOBAL ||
					ELF32_ST_BIND(symbol->st_info) == STB_WEAK)
				   ) {
					Bit8u *actSymbName = symbolNameFromSymbol(*elf, *symbol, loadedPos);
//					printf("\tTesting symbol %s %d\n", actSymbName, ELF32_ST_BIND(symbol.st_info));
					if (strcmp((char *)actSymbName, (char *)symName) == 0) {
					    if (scEntry != -1)
					        symbol->st_value = bx_mem.positionToVirtualAddress(loadedPos + symbol->st_value);

						return true;
					}
				}

				// already calculate the next position to search
				bx_mem.read((Bit8u *)&y, chainsAddr + y * sizeof(nbucket), sizeof(y));
			}
		}
	}

	// check if we found or not the symbol
	// symbols that are WEAK doesn't need necessarily have an definition
	Bit32u symLocType = ELF32_ST_TYPE(symLoc.st_info);
	Bit32u symLocBind = ELF32_ST_BIND(symLoc.st_info);

	// reset symbol to where we point
	symbol->st_info = 0;
	symbol->st_name = 0;
	symbol->st_value = 0;
	symbol->st_size = 0;
	symbol->st_shndx = 0;

	if (symLocBind == STB_WEAK) {
		return true;
	}

	BX_ERROR(("Symbol not found: %s %d %d\n", symName, symLocBind, symLocType));

	return false;
}


/*!
 * This method is responsible for searching an symbol definition
 * across the symbolScopeMap. Return the virtual address where
 * the symbol where loaded.
 */
bool ElfLoader::symbolLookupGnuHash(Bit32u symbIndex, Bit8u scopeIndex, Elf32_Sym *symbol) {
	// Check if the symbol index is valid
	if (symbIndex == 0) {
		BX_ERROR(("Symbol with index zero passed to lookup."));
		return 0;
	}

	// the scope which we should search the symbol
	vector<Bit8s> scope = symbolScopeMap[scopeIndex];

	// contains the symbol name
	Bit8u *symName = NULL;

	// contains the local definition of the symbol we are searching
	Elf32_Sym symLoc;

	// we first get the symbol name in the symbol table/str table
	// of the ELF that has the relocation
	if (scopeIndex == 0) { // main executable
		symLoc = symbolFromSymbIndex(mainExecutable, symbIndex, -loadedSegments[0].hdr.p_vaddr);
		symName = symbolNameFromSymbol(mainExecutable, symLoc, -loadedSegments[0].hdr.p_vaddr);
	}
	else {	// shared library (-1)
		Bit32u slIndex = scopeIndex-1;

		symLoc = symbolFromSymbIndex(sharedLibs[slIndex], symbIndex, loadedSegments[2*slIndex + 3].loadedPos);
		symName = symbolNameFromSymbol(sharedLibs[slIndex], symLoc, loadedSegments[2*slIndex + 3].loadedPos);
	}
//
//	printf("Searching symbol: %s\n", symName);
//	printf("Scope size: %d\n", scope.size());

	// for each entry in scope
	for (int sI=0; sI<scope.size(); sI++) {
		Bit8s scEntry 	= scope[sI];

//		printf("In scopeIndex = %d, sI = %d, scEntry = %d\n", scopeIndex, sI, scEntry);

		// these first two case cope with the case where we need
		// to search the symbol directly in the dynsym table, the
		// last case threat the case where we need a hash of the
		// symbol name to find an entry for it in the dynsym table.
		if (scopeIndex == 0 && sI == 0) {	// symbol search by index
			// symbol table address
			Elf32_Addr symTableAddr = mainExecutable.getDynsym();

			// physical address
			Elf32_Addr symEntry = bx_mem.virtualAddressToPosition(symTableAddr + symbIndex*sizeof(Elf32_Sym));

			// read the symbol definition
			bx_mem.read((Bit8u *)symbol, symEntry, sizeof(Elf32_Sym));

			// verifica se o symbol em symbEntry eh o procurado
			if (symbol->st_shndx != SHN_UNDEF) {
				Bit8u *actSymbName = symbolNameFromSymbol(mainExecutable, *symbol, -loadedSegments[0].hdr.p_vaddr);
//				printf("\tTesting symbol %s\n", actSymbName);
				if (strcmp((char *)actSymbName, (char *)symName) == 0) {
					return bx_mem.positionToVirtualAddress(symEntry);
				}
			}
		}
		else if (scopeIndex > 0 && sI == 1) {	// symbol search by index
			Bit32u slIndex = scopeIndex-1;

			// symbol table address
			Elf32_Addr symTableAddr = sharedLibs[slIndex].getDynsym();

			// physical address
			Elf32_Addr symEntry = loadedSegments[2*slIndex + 3].loadedPos + symTableAddr + symbIndex*sizeof(Elf32_Sym);

			// read the symbol definition
			bx_mem.read((Bit8u *)symbol, symEntry, sizeof(Elf32_Sym));

			// verifica se o symbol em symbEntry eh o procurado
			if (symbol->st_shndx != SHN_UNDEF) {
				Bit8u *actSymbName = symbolNameFromSymbol(sharedLibs[slIndex], *symbol, loadedSegments[2*slIndex + 3].loadedPos);
//				printf("\tTesting symbol %s\n", actSymbName);
				if (strcmp((char *)actSymbName, (char *)symName) == 0) {
				    // adjust the symbol value virtual address
				    symbol->st_value = bx_mem.positionToVirtualAddress(loadedSegments[2*slIndex + 3].loadedPos + symbol->st_value);
					return bx_mem.positionToVirtualAddress(symEntry);
				}
			}
		}
		else {	// symbol search by name hash
			// the current object scope is the mainExecutable or is a shared lib?
			if (scEntry == -1) {
				GnuHashInfo ghi = createGnuHashInfo(&mainExecutable, -bx_mem.virtualBase);

				bool fnd = findSymbolGnuHash(ghi, (const char *)symName, symbol);

				if (fnd) {
				    return true;
				}
			}
			else {
				GnuHashInfo ghi = createGnuHashInfo(&sharedLibs[scEntry], loadedSegments[2*scEntry + 3].loadedPos);

				bool fnd = findSymbolGnuHash(ghi, (const char *)symName, symbol);

				if (fnd) {
				    symbol->st_value = bx_mem.positionToVirtualAddress(loadedSegments[2*scEntry + 3].loadedPos + symbol->st_value);
				    return true;
				}
			}
		}
	}

	// check if we found or not the symbol
	// symbols that are WEAK doesn't need necessarily have an definition
	Bit32u symLocType = ELF32_ST_TYPE(symLoc.st_info);
	Bit32u symLocBind = ELF32_ST_BIND(symLoc.st_info);

	// reset symbol to where we point
	symbol->st_info = 0;
	symbol->st_name = 0;
	symbol->st_value = 0;
	symbol->st_size = 0;
	symbol->st_shndx = 0;

	if (symLocBind == STB_WEAK) {
		BX_DEBUG(("Weak definition: %s", symName));
		return true;
	}

	BX_ERROR(("Symbol not found: %s %d %d\n", symName, symLocBind, symLocType));

	return false;
}

bool ElfLoader::findSymbolGnuHash(GnuHashInfo os, const char *symname, Elf32_Sym *symbol) {
	Elf32_Word  c;
	Elf32_Word  h1, h2;
	Elf32_Word  n;
	Elf32_Word	bitmask;
	Elf32_Word  hashval;
	Elf32_Word  bloom, bucket;

	/*
	 * Hash the name, generate the "second" hash
	 * from it for the Bloom filter.
	 */
	h1 = gnuSymHash(symname);
	h2 = h1 >> os.os_shift2;

	/* Test against the Bloom filter */
	c = sizeof (Elf32_Word) * 8;
	n = (h1 / c) & os.os_maskwords_bm;
	bitmask = (1 << (h1 % c)) | (1 << (h2 % c));
	bx_mem.read((Bit8u *)&bloom, os.os_bloom + n*sizeof(Elf32_Word), sizeof(Elf32_Word)); // os->os_bloom[n];
	if ((bloom & bitmask) != bitmask)
		return false;

	/* Locate the hash chain, and corresponding hash value element */
	bx_mem.read((Bit8u *)&bucket, os.os_buckets + (h1 % os.os_nbuckets)*sizeof(Elf32_Word), sizeof(Elf32_Word));
	n = bucket; //os.os_buckets[h1 % os.os_nbuckets];
	if (n == 0)    /* Empty hash chain, symbol not present */
		return false;

	Elf32_Word idx_sym = n;
	Elf32_Word idx_has = n - os.os_symndx;

	bx_mem.read((Bit8u *)symbol, 	os.os_dynsym  + idx_sym*sizeof(Elf32_Sym), sizeof(Elf32_Sym)); // symbol 	= &os.os_dynsym[n];
	bx_mem.read((Bit8u *)&hashval, 	os.os_hashval + idx_has*sizeof(Elf32_Word), sizeof(Elf32_Word)); // hashval = &os.os_hashval[n - os.os_symndx];

	/*
	 * Walk the chain until the symbol is found or
	 * the chain is exhausted.
	 */
	//for (h1 &= ~1; 1; symbol++) {
	h1 &= ~1;
	while (true) {
		h2 = hashval;

		// this is hashvall++ originally
		idx_has++;
		bx_mem.read((Bit8u *)&hashval, os.os_hashval + idx_has*sizeof(Elf32_Word), sizeof(Elf32_Word));

		/*
		 * Compare the strings to verify match. Note that
		 * a given hash chain can contain different hash
		 * values. We'd get the right result by comparing every
		 * string, but comparing the hash values first lets us
		 * screen obvious mismatches at very low cost and avoid
		 * the relatively expensive string compare.
		 *
		 * We are intentionally glossing over some things here:
		 *
		 *    -  We could test sym->st_name for 0, which indicates
		 *	 a NULL string, and avoid a strcmp() in that case.
		 *
		 *    - The real runtime linker must also take symbol
		 * 	versioning into account. This is an orthogonal
		 *	issue to hashing, and is left out of this
		 *	example for simplicity.
		 *
		 * A real implementation might test (h1 == (h2 & ~1), and then
		 * call a (possibly inline) function to validate the rest.
		 */
//		printf("Testing symbol %s\n", (char *)bx_mem.str(os.os_dynstr + symbol->st_name) );
		if ((h1 == (h2 & ~1)) &&
			!strcmp((char *)symname, (char *)bx_mem.str(os.os_dynstr + symbol->st_name)))
				return true;

		/* Done if at end of chain */
		if (h2 & 1)
			break;

		idx_sym++;
		bx_mem.read((Bit8u *)symbol, os.os_dynsym + idx_sym*sizeof(Elf32_Sym), sizeof(Elf32_Sym));
	}

	/* This object does not have the desired symbol */
	return false;
}


/*!
 * This method return the name of the symb that have index in symbIndex
 * as tell the symbol table and dynamic string table of ELF file elf.
 * Care must be taken to pass the correct address in loadedPos
 * (negative offset for mainExecutable).
 */
Elf32_Sym ElfLoader::symbolFromSymbIndex(ElfParser elf, Bit32u symbIndex, Bit32s loadedPos) {
	// the symbol content
	Elf32_Sym symbol;

	// symbol table address
	Elf32_Addr symTableAddr = elf.getDynsym();

	// physical address
	Elf32_Addr symEntry = loadedPos + symTableAddr + symbIndex*sizeof(symbol);

	// read the symbol definition
	bx_mem.read((Bit8u *)&symbol, symEntry, sizeof(symbol));

	return symbol;
}

/*!
 * This method return the name of the symb that have index in symbIndex
 * as tell the symbol table and dynamic string table of ELF file elf.
 * Care must be taken to pass the correct address in loadedPos
 * (negative offset for mainExecutable).
 */
Bit8u * ElfLoader::symbolNameFromSymbIndex(ElfParser elf, Bit32u symbIndex, Bit32s loadedPos) {
	// the symbol content
	Elf32_Sym symbol;

	// symbol table address
	Elf32_Addr symTableAddr = elf.getDynsym();

	// physical address
	Elf32_Addr symEntry = loadedPos + symTableAddr + symbIndex*sizeof(symbol);

	// read the symbol definition
	bx_mem.read((Bit8u *)&symbol, symEntry, sizeof(symbol));

	// string table address
	Elf32_Addr strTableAddr = elf.getStrtab();

	// physical address
	Elf32_Addr strEntry = loadedPos + strTableAddr + symbol.st_name;

	// read the symbol name
	return bx_mem.memStrdup(strEntry);
}

/*!
 * This method return the name of the symbol passed.
 * Care must be taken to pass the correct address in loadedPos
 * (negative offset for mainExecutable).
 */
Bit8u * ElfLoader::symbolNameFromSymbol(ElfParser elf, Elf32_Sym symbol, Bit32s loadedPos) {
	// string table address
	Elf32_Addr strTableAddr = elf.getStrtab();

	// physical address
	Elf32_Addr strEntry = loadedPos + strTableAddr + symbol.st_name;

	// read the symbol name
	return bx_mem.memStrdup(strEntry);
}

/*!
 * This method parses the content of /etc/ld.so.cache and
 * update the vector ldCache with the cross-references found.
 * the ldCache contain a pairs [lib-soname -- library-path]
 */
void ElfLoader::parseLdCache() {
	FILE* pipe = popen("/sbin/ldconfig -p", "r");

	if (!pipe) { BX_INFO(("Can't execute /sbin/ldconfig")); }

	if (feof(pipe)) { BX_INFO(("Output of /sbin/ldconfig -p is empty.")); }

	Bit32u numEntries;
	char buffer[500], soname[500], path[500];

	// reset ldCache
	ldCache.clear();

	// to check later if we read all entries correctly
	fgets(buffer, 500, pipe);
	sscanf(buffer, "%d%*[^\n]\n", &numEntries);

	// parse all pairs and insert them into ldCache
	while(!feof(pipe)) {
		if(fgets(buffer, 500, pipe) != NULL) {
			sscanf(buffer, "%s (%*[^)]) => %[^\n]\n", soname, path);
			ldCache.push_back(make_pair(string(soname), string(path)));
		}
	}

	// check if we read all entries correctly
	if (ldCache.size() != numEntries) {
		BX_PANIC(("ldCache.size() (%d) and numEntries (%d) didn't match!", ldCache.size(), numEntries));
	}

	pclose(pipe);
}

GnuHashInfo ElfLoader::createGnuHashInfo(ElfParser *elf, Bit32s loadedPos) {
    GnuHashInfo ghi;

//    fprintf(stderr, "GNU HASH INFO\n");
//    fprintf(stderr, "gnu hash addr: %x\n", elf->getGnuHash());
//    fprintf(stderr, "lib base addr: %x\n", loadedPos);
//    fprintf(stderr, "gnu hash addr adjusted: %x\n", loadedPos + elf->getGnuHash());

    // address of dynamic string table
    ghi.os_dynstr = loadedPos + elf->getStrtab();

    // address of dynsym
    ghi.os_dynsym = loadedPos + elf->getDynsym();

    // read nbuckets
    //fprintf(stderr, "reading buckets from: %x\n", loadedPos + elf->getGnuHash());
    bx_mem.read((Bit8u *)&ghi.os_nbuckets, loadedPos + elf->getGnuHash(), sizeof(Elf32_Word));
    //fprintf(stderr, "num buckets: %x\n", ghi.os_nbuckets);

    // read symndx
    bx_mem.read((Bit8u *)&ghi.os_symndx, loadedPos + elf->getGnuHash() + 1*sizeof(Elf32_Word), sizeof(Elf32_Word));

    // read maskwords
    bx_mem.read((Bit8u *)&ghi.os_maskwords_bm, loadedPos + elf->getGnuHash() + 2*sizeof(Elf32_Word), sizeof(Elf32_Word));

    // read shift2
    bx_mem.read((Bit8u *)&ghi.os_shift2, loadedPos + elf->getGnuHash() + 3*sizeof(Elf32_Word), sizeof(Elf32_Word));

    // start 4*(32) bits after the gnuHash
    ghi.os_bloom        = loadedPos + elf->getGnuHash() + 4*sizeof(Elf32_Word);

    // start after the header and bloom filter
    ghi.os_buckets      = loadedPos + elf->getGnuHash() + (4 + ghi.os_maskwords_bm)*sizeof(Elf32_Word);

    // start after the header, bloom filter and buckets
    ghi.os_hashval      = loadedPos + elf->getGnuHash() + (4 + ghi.os_maskwords_bm + ghi.os_nbuckets)*sizeof(Elf32_Word);

    // this is the number of entries in the bloom filter minus one
    ghi.os_maskwords_bm = ghi.os_maskwords_bm - 1;

    return ghi;
}
