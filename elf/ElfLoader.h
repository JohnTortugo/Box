#include <elf.h>

#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <vector>

#include "config.h"
#include "debug.h"
#include "ElfParser.h"

using namespace std;

class LoadedSegment {
	public:
		string fileName;
		Bit8u segmentIndex;
		Elf32_Phdr hdr;
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
		set<string> alreadyLoadedLibs;

		// description list of loaded segments
		vector<LoadedSegment> loadedSegments;

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
};
