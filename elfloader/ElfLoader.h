#include <elf.h>

#include <iostream>
#include <string>

#include "../elfparser/ElfParser.h"

using namespace std;

class ElfLoader {

	public:
		ElfLoader(string execPath) {
			ElfParser elf(execPath);
		}
};
