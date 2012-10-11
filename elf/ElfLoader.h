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

/*!
 * This class describe information needed to lookup a symbol in a
 * GNU_HASH section.
 */
struct GnuHashInfo {
    Elf32_Addr  os_dynstr;       /* Dynamic string table            */
    Elf32_Addr  os_dynsym;       /* Dynamic symbol table            */
    Elf32_Word  os_nbuckets;     /* # hash buckets                  */
    Elf32_Word  os_symndx;       /* Index of 1st dynsym in hash     */
    Elf32_Word  os_maskwords_bm; /* # Bloom filter words, minus 1   */
    Elf32_Word  os_shift2;       /* Bloom filter hash shift         */
    Elf32_Addr  os_bloom;        /* Bloom filter words              */
    Elf32_Addr  os_buckets;      /* Hash buckets                    */
    Elf32_Addr  os_hashval;      /* Hash value array                */
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

	void solveRelocation(Elf32_Rel reloc, Bit8u scopeIndex);

	bool symbolLookup(Bit32u symbIndex, Bit8u scopeIndex, Elf32_Sym *symbol);

	bool symbolLookupElfHash(Bit32u symbIndex, Bit8u scopeIndex, Elf32_Sym *symbol);

	bool symbolLookupGnuHash(Bit32u symbIndex, Bit8u scopeIndex, Elf32_Sym *symbol);
	bool findSymbolGnuHash(GnuHashInfo os, const char *symname, Elf32_Sym *symbol);

	bool versionedSymbolLookupGnuHash(Bit32u symbIndex, Bit8u scopeIndex, Elf32_Sym *symbol);

	Bit8u * symbolNameFromSymbIndex(ElfParser elf, Bit32u symbIndex, Bit32s loadedPos);

	Elf32_Sym symbolFromSymbIndex(ElfParser elf, Bit32u symbIndex, Bit32s loadedPos);

	Bit8u * symbolNameFromSymbol(ElfParser elf, Elf32_Sym symbol, Bit32s loadedPos);

	GnuHashInfo createGnuHashInfo(ElfParser *elf, Bit32s loadedPos);

	Bit32u getEntryAddress() const { return mainExecutable.getEntryAddress(); }

	Bit32u getFiniAddress() const { return mainExecutable.getFini(); }

	ElfParser * getMainExecutable() const { return &mainExecutable; }

	vector<LoadedSegment> getLoadedSegments() const { return loadedSegments; }

	vector<ElfParser> getSharedLibs() const { return sharedLibs; }

	Bit32u elfHash(const unsigned char *name) {
		Bit32u h = 0, g;
		while (*name) {
			h = (h << 4) + *name++;

			if (g = (h & 0xf0000000)) h ^= g >> 24;

			h &= ~g;
		}

		return h;
	}

	uint_fast32_t gnuSymHash(const char *s) {
		uint_fast32_t h = 5381;

		for (unsigned char c = *s; c != '\0'; c = *++s)
				h = h * 33 + c;

		return h & 0xffffffff;
	}

};
