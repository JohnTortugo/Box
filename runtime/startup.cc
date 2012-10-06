#include<stdlib.h>
#include <string.h>
#include "bochs.h"
#include "config.h"
#include "runtime.h"

extern char **environ;

void setup_start_environment(int argc, char *argv[], ElfLoader * loader)
{
	int i=0,size;
	Bit32u stackaddr=bx_mem.RealToVirtualAddress((Bit32u)  bx_mem.memory + bx_mem.size -1);
	vector<Bit32u> envs;
    vector<Bit32u> args;
    vector<Bit32u>::iterator it;

	stackaddr &= 0xFFFFFFF0; //Stack align;

    bx_cpu.gen_reg[BX_32BIT_REG_EIP].dword.erx = (intptr_t) loader->getEntryAddress();
    bx_cpu.gen_reg[BX_32BIT_REG_EBP].dword.erx = (intptr_t) stackaddr;

    //Copy environment variables to stack
    while (environ[i] != '\0' ) {
    	size= strlen(environ[i])+1;
    	printf("Env[%d] Addr: %08lx Size: %d: %s\n",i , stackaddr, size, environ[i]);
    	envs.push_back(stackaddr);
    	bx_mem.write((Bit8u *) environ[i++],bx_mem.virtualAddressToPosition(stackaddr), size);
    	stackaddr -= size;
    }

    //Copy command line arguments to stack
    for(i=1;i<argc;i++) {
    	size= strlen(argv[i])+1;
    	printf("Arg[%d] Addr: %08lx Size: %d: %s\n",i-1, stackaddr, size, argv[i]);
    	args.push_back(stackaddr);
    	bx_mem.write((Bit8u *) argv[i],bx_mem.virtualAddressToPosition(stackaddr), size);
    	stackaddr -= size;
    }

	stackaddr &= 0xFFFFFFF0; //Stack align;
    bx_cpu.gen_reg[BX_32BIT_REG_ESP].dword.erx = (intptr_t) stackaddr+4;

    // Auxiliary vector NULL entry (2 dwords)
    bx_cpu.push_32(0);
    bx_cpu.push_32(0);

    // Null delimiter
    bx_cpu.push_32(0);

    //Environment variables pointers
    for ( it=envs.begin() ; it < envs.end(); it++ )
          bx_cpu.push_32(*it);

    // Null delimiter
    bx_cpu.push_32(0);

    //Command line arguments
    for ( it=args.begin() ; it < args.end(); it++ )
          bx_cpu.push_32(*it);

    //Argument count
    bx_cpu.push_32(argc-1);

    // Set exit function pointer (_fini ?)
    bx_cpu.gen_reg[BX_32BIT_REG_EDX].dword.erx = 0; // loader->getFiniAddress();
}

void switchTo32bitsMode(void)
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
  BX_CPU(0)->sregs[BX_SEG_REG_SS].cache.type = 3; 	   // Type read/write
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
