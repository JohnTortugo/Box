#include <elf.h>

#include <iostream>
#include <queue>
#include <set>
#include <map>
#include <string>
#include <vector>

#include "config.h"
#include "debug.h"
#include "ElfParser.h"

using namespace std;

/*!
 * This class describe an ELF loadable segment loaded in BX_MEM_C.memory
 */
class LoadedSegment {
	public:
		// the name of the elf file where this segment came from
		string fileName;

		// the segment index inside the elf file program segments
		Bit8u segmentIndex;

		// the elf header describing this segment
		Elf32_Phdr hdr;

		// position inside bx_mem.memory where the segment was loaded
		Bit32u loadedPos;
};

class ElfLoader {
	private:
		string executablePath;
		char *ldLibraryPath;

		// Pointer to simulated guest memory
		Bit8u *memory;

		// index of next free byte in "memory"
		Bit32u memory_indx;

		// size of allocated memory for guest data
		Bit32u memory_size;

		int argc;
		char **argv;

		// main executable object
		ElfParser mainExecutable;

		// shared library objects
		vector<ElfParser> sharedLibs;

		// ELFObjects that have dependency
		queue<ElfParser> hasDependencies;

		// Name of shared libraries that have already been loaded
		map<string, Bit32u> alreadyLoadedLibs;

		// description list of loaded segments
		vector<LoadedSegment> loadedSegments;

		// map the content of ld.so.cache, that is
		// library-soname to library-path
		vector< pair<string, string> > ldCache;

		vector< vector<Bit8s> > symbolScopeMap;

	public:
		ElfLoader(int p_argc, char **p_argv, char *ldLibPath, Bit8u *p_memory, Bit32u mem_size);

		~ElfLoader() {
			mainExecutable.elfClose();

			for (int i=0; i<sharedLibs.size(); i++) sharedLibs[i].elfClose();
		}

		void loadSharedLibs();

		string findLibrary(string libName, string Rpath);

		vector<string> parseLibPath(string str);

		vector<string> parseLibPath(char *str);

		void createAddressSpace();

		void dumpAddressSpaceInfo();

		void doRelocations();

		void parseLdCache();

		void dumpSymbolScopeMap();

		void expandSymbolScopeMap();
};
