#include "ElfLoader.h"

#include <iostream>
#include <queue>
#include <set>
#include <string>

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

	BX_INFO(("Main Executable Loaded."));

	// parse the content of ld.so.cache
	BX_INFO(("Parsing the content of ld.so.cache"));
	parseLdCache();

	// start loading shared libs
	BX_INFO(("Loading Shared Libraries."));
	hasDependencies.push(mainExecutable);
	loadSharedLibs();
	BX_INFO(("%d Libraries Loaded.", sharedLibs.size()));

	// create the process address space
	BX_INFO(("Mounting Process Address Space."));
	createAddressSpace();
	dumpAddressSpaceInfo();
	BX_INFO(("Address Space Mounted."));

	// Doing relocations
	BX_INFO(("Doing relocations."));
	doRelocations();
	BX_INFO(("Relocations done."));
}

/*!
 * Carrega todas as bibliotecas necessarias a execucao de mainExecutable
 */
void ElfLoader::loadSharedLibs() {

	// while still have an ELF object with dependencies not satisfied
	while ( !hasDependencies.empty() ) {
		ElfParser elf = hasDependencies.front(); hasDependencies.pop();

		// get list of dynamically needed libraries
		vector<string> libs = elf.getNeededLibraries();

		// iterate over all libs
		for (int i=0; i<libs.size(); i++) {
			string libName = libs[i];

			// check if the lib isn't already loaded
			if (alreadyLoadedLibs.find(libName) != alreadyLoadedLibs.end()) {
				continue;
			}

			// find the library
			string libPath = findLibrary(libName, elf.getRpath());

			if (libPath != "") {
				ElfParser lib(libPath);

				// new shared library to insert in address space
				sharedLibs.push_back(lib);

				// annotate to verify the dependencies
				hasDependencies.push(lib);

				// mark as already checked library
				alreadyLoadedLibs.insert(libName);
			}
			else {
				BX_ERROR(("Library not found: %s", libName.c_str()));
			}
		}
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

	// load shared libraries
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

	// allocating space for heap (10MB) and stack (remainder)
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

	// stack is initialized at memory_size and is decremented until reaching heap start
	segDesc.fileName 		= "Stack";
	segDesc.segmentIndex 	= 0;
	segDesc.hdr.p_align 	= segDesc.hdr.p_filesz = segDesc.hdr.p_flags = 0;
	segDesc.hdr.p_memsz		= segDesc.hdr.p_offset = segDesc.hdr.p_paddr = 0;
	segDesc.hdr.p_type		= segDesc.hdr.p_vaddr  = 0;
	segDesc.loadedPos 		= this->memory_size;

	this->loadedSegments.push_back(segDesc);
}

void ElfLoader::dumpAddressSpaceInfo() {
	printf("%25s %10s %12s %s %11s %12s %12s %12s %12s %12s %12s %12s\n", "File", "SegIndx", "RealPos", "|", "Type","Offset","VirtAddr","PhysAddr","FileSiz","MemSiz","Flag","Align");

	for (int i=0; i<this->loadedSegments.size(); i++) {
		LoadedSegment segDesc = this->loadedSegments[i];

		printf("%25s ", segDesc.fileName.c_str());
		printf("%10d ", segDesc.segmentIndex);
		printf("0x%010x ", segDesc.loadedPos);
		printf("|");

		printf("0x%010x ", segDesc.hdr.p_type);
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

void ElfLoader::doRelocations() {
	// pointer to relocation table (with implicit addends)
	Elf32_Addr rel 		= mainExecutable.getRel();
	Elf32_Addr rela 	= mainExecutable.getRela();

	if (rel == 0 && rela == 0) BX_DEBUG(("REL and RELA equals zero for mainExecutable."));

	// check if there are any relocation (implicit) for the
	// main executable
	if (rel > 0) {
		// bytes already read
		Bit32u aRead = 0;

		rel = bx_mem.virtualAddressToPosition(rel);

		printf("Relocation Table Entries (REL) [%x] (%s)\n", rel, mainExecutable.getFileName().c_str());
		printf("------------------------------\n");
		printf("%10s %10s %10s %10s\n", "Offset","Info","Symb. Ind.","Rel. Type");
		while (aRead < mainExecutable.getRelsz()) {
			Elf32_Rel reloc;

			mainExecutable.read((Bit8u *)&reloc, rel, mainExecutable.getRelent());

			printf("0x%08x 0x%08x 0x%08x 0x%08x\n", reloc.r_offset, reloc.r_info, ELF32_R_SYM(reloc.r_info), ELF32_R_TYPE(reloc.r_info));

			aRead 	+= mainExecutable.getRelent();
			rel		+= mainExecutable.getRelent();
		}
	}

	// check each shared library for relocations that
	// need to be solved
	for (int slIndex = 0; slIndex<sharedLibs.size(); slIndex++) {
		rel 	= sharedLibs[slIndex].getRel();
		rela 	= sharedLibs[slIndex].getRela();

		if (rel == 0 && rela == 0) BX_DEBUG(("REL and RELA equals zero for SharedLib[%d].", slIndex));

		// check if there are any relocation (implicit) for the
		// shared library
		if (rel > 0) {
			// bytes already read
			Bit32u aRead = 0;

			// rel indicates an offset inside the shared library code
			rel = loadedSegments[slIndex].loadedPos + rel;

			printf("Relocation Table Entries (REL) [%x] (%s)\n", rel, sharedLibs[slIndex].getFileName().c_str());
			printf("------------------------------\n");
			printf("%10s %10s %10s %10s\n", "Offset","Info","Symb. Ind.","Rel. Type");
			while (aRead < sharedLibs[slIndex].getRelsz()) {
				Elf32_Rel reloc;

				sharedLibs[slIndex].read((Bit8u *)&reloc, rel, sharedLibs[slIndex].getRelent());

				printf("0x%08x 0x%08x 0x%08x 0x%08x\n", reloc.r_offset, reloc.r_info, ELF32_R_SYM(reloc.r_info), ELF32_R_TYPE(reloc.r_info));

				aRead 	+= sharedLibs[slIndex].getRelent();
				rel		+= sharedLibs[slIndex].getRelent();
			}
		}
	}
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
