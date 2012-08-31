#include <stdlib.h>
#include <stdio.h>

#include "bochs.h"
#include "config.h"
#include "elfloader/ElfLoader.h"

void cpu(void);

int main(int argc, char **argv) {
	printf("Hello World!\n");

	ElfLoader loader(argv[0], getenv("LD_LIBRARY_PATH"));

	cpu();

	return 0;
}
