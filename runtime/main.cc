#include <stdlib.h>
#include <stdio.h>
#include "bochs.h"
#include "config.h"
#include "runtime.h"
#include "syscall/syscall.h"

BX_CPU_C bx_cpu;
BX_MEM_C bx_mem;
BX_SYSCALL bx_sys;
BX_RUNTIME bx_rnt;

int main(int argc, char **argv) {
	Bit32u memsize = 100*1024*1024;

    // verifica parametros
	verifyParams(argc, argv);

	// print banner
	// printBanner();

	// aloca memória (100MB)
	BX_INFO(("Allocating Memory."));
	bx_mem.allocate(memsize);

	// carrega o elf na memória
	BX_INFO(("Loading ELF."));
	ElfLoader loader(argc, argv, getenv("LD_LIBRARY_PATH"),
		                    bx_mem.memory,memsize);

	// Initialize Virtual CPU
	initialize();

	// Setup start environment (stack and registers)
	setup_environment(argc, argv, &loader);

	//TODO:  execute init stubs

	// salta para o interpretador
	BX_INFO(("Running Interpreter."));
	run();

	//TODO:  execute fini stubs

	return 0;
}

Bit32u BX_RUNTIME::newMappedIOSpace(int i, Bit32u offset, Bit32u length) {
    if (i >= 0) {
        this->fileDescriptors[i].mMapped    = true;
        this->fileDescriptors[i].fileOffset = offset;
        this->fileDescriptors[i].memLength  = length;
        this->fileDescriptors[i].memOffset  = bx_mem.afterLibrary;

        bx_mem.afterLibrary = bx_mem.afterLibrary + this->fileDescriptors[i].memLength + 1;

        return this->fileDescriptors[i].memOffset;
    }
    else {
        BX_FD newFD;

        newFD.fileBacked = false;
        newFD.mMapped    = true;
        newFD.freed      = false;
        newFD.fileOffset = offset;
        newFD.memLength  = length;
        newFD.memOffset  = bx_mem.afterLibrary;

        this->fileDescriptors.push_back(newFD);

        bx_mem.afterLibrary = bx_mem.afterLibrary + newFD.memLength + 1;

        return newFD.memOffset;
    }
}

void initialize()
{
    bx_cpu.initialize();
    bx_cpu.sanity_checks();
    bx_cpu.register_state();
#if BX_DICACHE == 1
    bx_cpu.initializeDiCache();
#endif

    BX_INSTR_INITIALIZE(0);

    switchTo32bitsMode();
}

int run() {

    bx_cpu.cpu_loop();

    return 0;
}



void verifyParams(int argc, char **argv) {
	if (argc < 2) {
		printf("Usage: %s <executable> [arguments]\n", argv[0]);
		exit(1);
	}
}

void printBanner() {
	printf("Box x86 32-Bits Process Version %s\n", VER_STRING);
	printf("---------------------------------------\n");
}
