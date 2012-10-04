#include <stdlib.h>
#include <stdio.h>

#include "bochs.h"
#include "config.h"
#include "debug.h"
#include "cpu/cpu.h"
#include "syscall/syscall.h"
#include "elf/ElfLoader.h"

BX_CPU_C bx_cpu;
BX_MEM_C bx_mem;
BX_SYSCALL bx_sys;


void verifyParams(int argc, char **argv);
void printBanner();
void initialize(Bit32u entry);
int run();
void bx_load_null_kernel_hack();

int main(int argc, char **argv) {
    // verifica parametros
	verifyParams(argc, argv);

	// print banner
	printBanner();

  // aloca memória (100MB)
	BX_INFO(("Allocating Memory."));
	bx_mem.allocate(100*1024*1024);

  // carrega o elf na memória
	BX_INFO(("Loading ELF."));
  ElfLoader loader(argc, argv, getenv("LD_LIBRARY_PATH"), bx_mem.memory, bx_mem.size);

  initialize(loader.getEntryAddress());

  //TODO:  execute init stubs

  // salta para o interpretador
  BX_INFO(("Running Interpreter."));
  run();

  //TODO:  execute fini stubs

  return 0;
}

void initialize(Bit32u entry)
{
    bx_cpu.initialize();
    bx_cpu.sanity_checks();
    bx_cpu.register_state();

    BX_INSTR_INITIALIZE(0);

    bx_load_null_kernel_hack();

    bx_cpu.gen_reg[BX_32BIT_REG_EIP].dword.erx = (intptr_t) entry;
    bx_cpu.gen_reg[BX_32BIT_REG_EBP].dword.erx =
    bx_cpu.gen_reg[BX_32BIT_REG_ESP].dword.erx = (intptr_t) bx_mem.positionToVirtualAddress(0x63FFFFF);
}

int run() {

    bx_cpu.cpu_loop();

    return 0;
}

void bx_load_null_kernel_hack(void)
{
  // The RESET function will have been called first.
  // Set CPU and memory features which are assumed at this point.

  // CS deltas
  BX_CPU(0)->sregs[BX_SEG_REG_CS].cache.valid = 1; 	// Valid segment cache
  BX_CPU(0)->sregs[BX_SEG_REG_CS].cache.p = 1; 	   	   // Segment present
  BX_CPU(0)->sregs[BX_SEG_REG_CS].cache.dpl = 3; 	   // Ring 3
  BX_CPU(0)->sregs[BX_SEG_REG_CS].cache.segment = 1; 	   // Data/Code segment
  BX_CPU(0)->sregs[BX_SEG_REG_CS].cache.u.segment.base = 0x00000000;
  BX_CPU(0)->sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xFFFFFFFF;
  BX_CPU(0)->sregs[BX_SEG_REG_CS].cache.u.segment.g   = 1; // page granularity
  BX_CPU(0)->sregs[BX_SEG_REG_CS].cache.u.segment.d_b = 1; // 32bit
  BX_CPU(0)->sregs[BX_SEG_REG_CS].selector.index = 1; 	   // First segment
  BX_CPU(0)->sregs[BX_SEG_REG_CS].selector.ti = 0; 	   // GDT
  BX_CPU(0)->sregs[BX_SEG_REG_CS].selector.rpl = 3; 	   // Ring 3 privilege

  // DS deltas
  BX_CPU(0)->sregs[BX_SEG_REG_DS].cache.valid = 1; 	// Valid segment cache
  BX_CPU(0)->sregs[BX_SEG_REG_DS].cache.p = 1; 	   	   // Segment present
  BX_CPU(0)->sregs[BX_SEG_REG_DS].cache.dpl = 3; 	   // Ring 3
  BX_CPU(0)->sregs[BX_SEG_REG_DS].cache.type = 3; 	   // Type read/write
  BX_CPU(0)->sregs[BX_SEG_REG_DS].cache.segment = 1; 	   // Data/Code segment
  BX_CPU(0)->sregs[BX_SEG_REG_DS].cache.u.segment.base = 0x00000000;
  BX_CPU(0)->sregs[BX_SEG_REG_DS].cache.u.segment.limit_scaled = 0xFFFFFFFF;
  BX_CPU(0)->sregs[BX_SEG_REG_DS].cache.u.segment.g   = 1; // page granularity
  BX_CPU(0)->sregs[BX_SEG_REG_DS].cache.u.segment.d_b = 1; // 32bit
  BX_CPU(0)->sregs[BX_SEG_REG_DS].selector.index = 2; 	   // Second segment
  BX_CPU(0)->sregs[BX_SEG_REG_DS].selector.ti = 0; 	   // GDT
  BX_CPU(0)->sregs[BX_SEG_REG_DS].selector.rpl = 3; 	   // Ring 3 privilege

  // SS deltas
  BX_CPU(0)->sregs[BX_SEG_REG_SS].cache.valid = 1; 	// Valid segment cache
  BX_CPU(0)->sregs[BX_SEG_REG_SS].cache.p = 1;				// Segment present
  BX_CPU(0)->sregs[BX_SEG_REG_SS].cache.dpl = 3; 	           // Ring 3
  //BX_CPU(0)->sregs[BX_SEG_REG_SS].cache.type = 3; 	   // Type read/write
  BX_CPU(0)->sregs[BX_SEG_REG_SS].cache.segment = 1; 	   // Data/Code segment
  BX_CPU(0)->sregs[BX_SEG_REG_SS].cache.u.segment.base = 0x00000000;
  BX_CPU(0)->sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled = 0xFFFFFFFF;
  BX_CPU(0)->sregs[BX_SEG_REG_SS].cache.u.segment.g   = 1; // page granularity
  BX_CPU(0)->sregs[BX_SEG_REG_SS].cache.u.segment.d_b = 1; // 32bit
  BX_CPU(0)->sregs[BX_SEG_REG_DS].selector.index = 3; 	   // Third segment
  BX_CPU(0)->sregs[BX_SEG_REG_DS].selector.ti = 0; 	   // GDT
  BX_CPU(0)->sregs[BX_SEG_REG_DS].selector.rpl = 3; 	   // Ring 3 privilege

  // CR0 deltas
  BX_CPU(0)->cr0.set_PG(0); // paging disabled
  BX_CPU(0)->cr0.set_PE(1); // protected mode

  BX_CPU(0)->handleCpuModeChange();
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
