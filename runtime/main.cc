#include <stdlib.h>
#include <stdio.h>
#include "bochs.h"
#include "config.h"
#include "runtime.h"
#include "syscall/syscall.h"
#include "elf/ElfLoader.h"

BX_CPU_C bx_cpu;
BX_MEM_C bx_mem;
BX_SYSCALL bx_sys;

int main(int argc, char **argv) {
	Bit32u memsize = 100*1024*1024;

    // verifica parametros
	verifyParams(argc, argv);

	// print banner
	printBanner();

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
  setup_start_environment(argc, argv, memsize,
		                                  loader.getEntryAddress());

  //TODO:  execute init stubs

  // salta para o interpretador
  BX_INFO(("Running Interpreter."));
  run();

  //TODO:  execute fini stubs

  return 0;
}

void initialize()
{
    bx_cpu.initialize();
    bx_cpu.sanity_checks();
    bx_cpu.register_state();

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
