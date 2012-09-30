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

  // execute init stubs

  // salta para o interpretador
  BX_INFO(("Running Interpreter."));
  //run();

  // execute fini stubs

  return 0;
}

int run() {
	int CacheSize;

	char instr[] =  {
                        0xb8,0x01,0x00,0x00,0x00,      	        // mov    $0x1,%eax
                        0xbb,0x02,0x00,0x00,0x00,      	        // mov    $0x2,%ebx
                        0x89,0xc6,				// mov    %eax,%esi
                        0x89,0xda,				// mov    %ebx,%edx
                        0x01,0xf2,				// add    %esi,%edx
                        0x89,0xd1,				// mov    %edx,%ecx
                        0x89,0xc6,				// mov    %eax,%esi
                        0x89,0xca,				// mov    %ecx,%edx
                        0xb9,0x00,0x01,0x00,0x00,	      	// mov    $0x100,%ecx
                        0x89,0xc8,	                	// mov    %ecx,%eax
                        0xc7,0x00,0x0a,0x00,0x00,0x00,   	// movl   $0xa,(%eax)
                        0x89,0xf7,				// mov    %esi,%edi
                        0x29,0xd7,				// sub    %edx,%edi
                        0x89,0xfa,				// mov    %edi,%edx
                        0x89,0xd3,				// mov    %edx,%ebx
                        0x55,					// push   %ebp
                        0x89, 0xe5,				// mov    %esp,%ebp
                        0x53,					// push   %ebx
                        0x83, 0xec, 0x04,			// sub    $0x4,%esp
                        0x5b,					// pop    %ebx
                        0x81, 0xc3, 0xfc, 0x6c, 0x0a, 0x00,	// add    $0xa6cfc,%ebx
                        0x8b, 0x93, 0xfc, 0xff, 0xff, 0xff,     // mov    -0x4(%ebx),%edx
                        0x85, 0xd2,				// test   %edx,%edx
                        0x74, 0x05,				// je     806743a
                        0xe8, 0xd1, 0x03, 0x00, 0x00,		// call   8067810
                        0x58,					// pop    %eax
                        0x5b,					// pop    %ebx
                        0xc9,					// leave
                        0xc3					// ret
                    };

		CacheSize = sizeof(instr);
		bx_mem.loadData( (void *) instr, CacheSize,0);

    bx_cpu.initialize();
    bx_cpu.sanity_checks();
    bx_cpu.register_state();

    BX_INSTR_INITIALIZE(0);

    BX_DEBUG(("CPU mode: %s", cpu_mode_string(bx_cpu.get_cpu_mode())));

    bx_load_null_kernel_hack();

    BX_DEBUG(("CPU mode: %s", cpu_mode_string(bx_cpu.get_cpu_mode())));

    bx_cpu.gen_reg[BX_32BIT_REG_EIP].dword.erx = (intptr_t) 0;

    bx_cpu.cpu_loop();

    return 0;
}

void bx_load_null_kernel_hack(void)
{
  // The RESET function will have been called first.
  // Set CPU and memory features which are assumed at this point.
  //
  //bx_load_kernel_image(SIM->get_param_string(BXPN_LOAD32BITOS_PATH)->getptr(), 0x100000);

  // EIP deltas
  BX_CPU(0)->prev_rip = BX_CPU(0)->gen_reg[BX_32BIT_REG_EIP].dword.erx = 0x100000;


  // CS deltas
  BX_CPU(0)->sregs[BX_SEG_REG_CS].cache.p = 1; 	   	   // Segment present
  BX_CPU(0)->sregs[BX_SEG_REG_CS].cache.dpl = 3; 	   // Ring 3
  BX_CPU(0)->sregs[BX_SEG_REG_CS].cache.segment = 1; 	   // Data/Code segment
  BX_CPU(0)->sregs[BX_SEG_REG_CS].cache.u.segment.base = 0x00000000;
  BX_CPU(0)->sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0x000FFFFF;
  BX_CPU(0)->sregs[BX_SEG_REG_CS].cache.u.segment.g   = 0; // page granularity
  BX_CPU(0)->sregs[BX_SEG_REG_CS].cache.u.segment.d_b = 1; // 32bit
  BX_CPU(0)->sregs[BX_SEG_REG_CS].selector.index = 1; 	   // First segment
  BX_CPU(0)->sregs[BX_SEG_REG_CS].selector.ti = 0; 	   // GDT
  BX_CPU(0)->sregs[BX_SEG_REG_CS].selector.rpl = 3; 	   // Ring 3 privilege

  // DS deltas
  BX_CPU(0)->sregs[BX_SEG_REG_DS].cache.p = 1; 	   	   // Segment present
  BX_CPU(0)->sregs[BX_SEG_REG_DS].cache.dpl = 3; 	   // Ring 3
  BX_CPU(0)->sregs[BX_SEG_REG_DS].cache.segment = 1; 	   // Data/Code segment
  BX_CPU(0)->sregs[BX_SEG_REG_DS].cache.u.segment.base = 0x00100000;
  BX_CPU(0)->sregs[BX_SEG_REG_DS].cache.u.segment.limit_scaled = 0x0007FFFF;
  BX_CPU(0)->sregs[BX_SEG_REG_DS].cache.u.segment.g   = 0; // page granularity
  BX_CPU(0)->sregs[BX_SEG_REG_DS].cache.u.segment.d_b = 1; // 32bit
  BX_CPU(0)->sregs[BX_SEG_REG_DS].selector.index = 2; 	   // Second segment
  BX_CPU(0)->sregs[BX_SEG_REG_DS].selector.ti = 0; 	   // GDT
  BX_CPU(0)->sregs[BX_SEG_REG_DS].selector.rpl = 3; 	   // Ring 3 privilege

  // SS deltas
  BX_CPU(0)->sregs[BX_SEG_REG_SS].cache.p = 1; 	   	   // Segment present
  BX_CPU(0)->sregs[BX_SEG_REG_SS].cache.dpl = 3; 	   // Ring 3
  BX_CPU(0)->sregs[BX_SEG_REG_SS].cache.segment = 1; 	   // Data/Code segment
  BX_CPU(0)->sregs[BX_SEG_REG_SS].cache.u.segment.base = 0x00180000;
  BX_CPU(0)->sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled = 0x0007FFFF;
  BX_CPU(0)->sregs[BX_SEG_REG_SS].cache.u.segment.g   = 0; // page granularity
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
