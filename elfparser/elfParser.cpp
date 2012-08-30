#include <iostream>

#include <elf.h>

#include "ElfFile.h"
using namespace std;

int main(int argc, char **argv) {

	ElfFile elf(argv[1]);

	return 0;
}
