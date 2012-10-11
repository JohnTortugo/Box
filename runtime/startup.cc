#include<stdlib.h>
#include <string.h>
#include <elf.h>
#include "bochs.h"
#include "config.h"
#include "runtime.h"

extern char **environ;

char init_prog[] = {
		0xe8, 0x28, 0x00, 0x00, 0x00,	// call   2d <get_pos>
		0x83, 0xc3, 0x2c,						// add    $0x2c,%ebx
		0x8b, 0x03,								// mov    (%ebx),%eax
		0x85, 0xc0,								// test   %eax,%eax
		0x74, 0x18,								// je     26 <end>
		0x53,										// push   %ebx
		0x68, 0xff, 0x00, 0xff, 0x00,		// push   $0xff00ff       ; argc
		0x68, 0x00, 0xff, 0x00, 0xff,		// push   $0xff00ff00   ; argv (ptr)
		0x68, 0xff, 0x00, 0xff, 0x00,		// push   $0xff00ff       ; env (ptr)
		0xff, 0xd0,								// call   *%eax
		0x5b,										// pop    %ebx
		0x83, 0xc3, 0x04,						// add    $0x4,%ebx
		0xeb, 0xe2,								// jmp    8 <test>
		0x83, 0xc3, 0x04,						// add    $0x4,%ebx
		0x8b, 0x03,								// mov    (%ebx),%eax
		0xff, 0xe0,								// jmp    *%eax
		0x8b, 0x1c, 0x24,						// mov	 (%esp), %ebx
		0xc3											// ret
};

#define GS_SEG_SIZE 4096 //4K page

Bit32u build_vdso_page(Bit32u end);
Bit32u build_init_table(ElfLoader *loader);
void save_auxiliary_vectors(Bit32u execfn, Bit32u vsyscall, ElfLoader * loader);
void setInitProgArgs(Bit32u initAddr, int argc, Bit32u argv, Bit32u env);

void setup_start_environment(int argc, char *argv[], ElfLoader * loader)
{
	int i=0,size;
	Bit32u inits, vsyscall, stackaddr, addrArgv, addrEnv;
	Bit32u av_EXECFN;
	vector<Bit32u> envs;
    vector<Bit32u> args;
    vector<Bit32u>::iterator it;

	inits = build_init_table(loader);
	BX_INFO(("init table address: 0x%08lx", inits));

	stackaddr = (inits - 2) & 0xFFFFFFF0;

	vsyscall = build_vdso_page(stackaddr);
	BX_INFO(("vsyscall page address: 0x%08lx", vsyscall));

	stackaddr= (vsyscall - 2) & 0xFFFFFFF0;
	BX_INFO(("stack start address: 0x%08lx", stackaddr));


    bx_cpu.gen_reg[BX_32BIT_REG_EBP].dword.erx = (intptr_t) stackaddr;

    //Copy environment variables to stack
    while (environ[i] != '\0' ) {
    	size= strlen(environ[i])+1;
//    	printf("Env[%d] Addr: %08lx Size: %d: %s\n",i , stackaddr, size, environ[i]);
    	envs.push_back(stackaddr-size+1);
    	bx_mem.write((Bit8u *) environ[i++],bx_mem.virtualAddressToPosition(stackaddr-size+1), size);
    	stackaddr -= size;
    }

    //Copy command line arguments to stack
    for(i=1;i<argc;i++) {
    	size= strlen(argv[i])+1;
//    	printf("Arg[%d] Addr: %08lx Size: %d: %s\n",i-1, stackaddr, size, argv[i]);
    	args.push_back(stackaddr-size+1);
    	bx_mem.write((Bit8u *) argv[i],bx_mem.virtualAddressToPosition(stackaddr-size+1), size);
    	stackaddr -= size;
    }

    //Auxiliary vector data (AT_EXECFN)
	size= strlen(argv[1])+1;
	args.push_back(stackaddr-size+1);
	bx_mem.write((Bit8u *) argv[1],bx_mem.virtualAddressToPosition(stackaddr-size+1), size);
	stackaddr -= size;

	stackaddr &= 0xFFFFFFF0; //Stack align;

    bx_cpu.gen_reg[BX_32BIT_REG_ESP].dword.erx = (intptr_t) stackaddr+4;

    // Auxiliary vector NULL entry (2 dwords)
    bx_cpu.push_32(0);
    bx_cpu.push_32(0);

    save_auxiliary_vectors(av_EXECFN, vsyscall, loader);

    // Null delimiter
    bx_cpu.push_32(0);

    //Environment variables pointers
    for ( it=envs.begin() ; it < envs.end(); it++ )
          bx_cpu.push_32(*it);

    addrEnv = bx_cpu.gen_reg[BX_32BIT_REG_ESP].dword.erx;

    // Null delimiter
    bx_cpu.push_32(0);

    //Command line arguments
    for ( it=args.begin() ; it < args.end(); it++ )
          bx_cpu.push_32(*it);

    addrArgv = bx_cpu.gen_reg[BX_32BIT_REG_ESP].dword.erx;

    //Argument count
    bx_cpu.push_32(argc-1);

    // Set exit function pointer (_fini ?)
    bx_cpu.gen_reg[BX_32BIT_REG_EDX].dword.erx = 0; // loader->getFiniAddress();

    // Patch init routine
    setInitProgArgs(inits,argc-1,addrArgv, addrEnv);

    // EIP points to libraries initialization routine pointed by inits
    bx_cpu.gen_reg[BX_32BIT_REG_EIP].dword.erx = (intptr_t) inits;
}

void switchTo32bitsMode()
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

  // GS deltas - sysenter
  BX_CPU(0)->sregs[BX_SEG_REG_GS].cache.valid = 1; 	// Valid segment cache
  BX_CPU(0)->sregs[BX_SEG_REG_GS].cache.p = 1; 	   	   // Segment present
  BX_CPU(0)->sregs[BX_SEG_REG_GS].cache.dpl = 3; 	   // Ring 3
  BX_CPU(0)->sregs[BX_SEG_REG_GS].cache.type = 3; 	   // Type read/write
  BX_CPU(0)->sregs[BX_SEG_REG_GS].cache.segment = 1; 	   // Data/Code segment
  BX_CPU(0)->sregs[BX_SEG_REG_GS].cache.u.segment.base = bx_mem.positionToVirtualAddress(bx_mem.size - GS_SEG_SIZE -1);
  BX_CPU(0)->sregs[BX_SEG_REG_GS].cache.u.segment.limit_scaled =
		              BX_CPU(0)->sregs[BX_SEG_REG_GS].cache.u.segment.base + GS_SEG_SIZE;
  BX_CPU(0)->sregs[BX_SEG_REG_GS].cache.u.segment.g   = 0; // page granularity
  BX_CPU(0)->sregs[BX_SEG_REG_GS].cache.u.segment.d_b = 1; // 32bit
  BX_CPU(0)->sregs[BX_SEG_REG_GS].selector.index = 3; 	   // Second segment
  BX_CPU(0)->sregs[BX_SEG_REG_GS].selector.ti = 1; 	   // GDT
  BX_CPU(0)->sregs[BX_SEG_REG_GS].selector.rpl = 3; 	   // Ring 3 privilege

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
  BX_CPU(0)->sregs[BX_SEG_REG_DS].selector.index = 4; 	   // Third segment
  BX_CPU(0)->sregs[BX_SEG_REG_DS].selector.ti = 0; 	   // GDT
  BX_CPU(0)->sregs[BX_SEG_REG_DS].selector.rpl = 3; 	   // Ring 3 privilege


  // CR0 deltas
  BX_CPU(0)->cr0.set_PG(0); // paging disabled
  BX_CPU(0)->cr0.set_PE(1); // protected mode

  BX_CPU(0)->handleCpuModeChange();
}

Bit32u build_vdso_page(Bit32u end)
{
  char instr[] = {
		   0xcd, 0x80,  // int 80h
		   0xc3            // ret
  };

  Bit32u start = end - GS_SEG_SIZE;
  printf("vsyscall range 0x%08lx - 0x%08lx\n",start,end);

  bx_mem.write((Bit8u *) instr, bx_mem.virtualAddressToPosition(start) + 0x10, sizeof(instr));

  return start;
}


Bit32u build_init_table(ElfLoader *loader)
{
    Bit32u offset = bx_mem.size - 4;
    Bit8u * mem = (Bit8u *) bx_mem.memory;
    vector<LoadedSegment> loadedSegments = loader->getLoadedSegments();
	vector<ElfParser> libs = loader->getSharedLibs();
    vector<ElfParser>::iterator it;
    Elf32_Addr addr;
    int i;

	*(Bit32u *) (mem+offset)  = 0; // Delimiter

    offset -= 4;

	*(Bit32u *) (mem+offset)  = loader->getEntryAddress();

    offset -= 4;

    *(Bit32u *) (mem+offset)  = 0; // Delimiter

    for ( it=libs.begin() ; it < libs.end(); it++ ){

      if ( (addr = it->getInit()) != 0 )  {
    	 offset -= 4;

    	 // Search for the segment load position
    	 for(i=0;i< loadedSegments.size();i++ ) {
//           printf("Lib %s Flag %d Position %08lx\n",
//        		       loadedSegments[i].fileName.c_str(),
//        		       loadedSegments[i].hdr.p_flags & PF_X,
//        		       loadedSegments[i].loadedPos);
           if ( loadedSegments[i].fileName == it->getFileName() &&
       		   (loadedSegments[i].hdr.p_flags & PF_X) ) break;
    	 }
    	 addr += loadedSegments[i].loadedPos;

    	 *(Bit32u *) (mem+offset)  = (Bit32u) bx_mem.positionToVirtualAddress(addr);
      }
    }

    offset-= sizeof(init_prog);

    bx_mem.write((Bit8u *) init_prog,offset,sizeof(init_prog));

    return bx_mem.positionToVirtualAddress(offset);
}

void save_auxiliary_vectors(Bit32u execfn, Bit32u vsyscall, ElfLoader * loader)
{
	Elf32_Ehdr hdr = loader->getMainExecutable()->getHdr();

	bx_cpu.push_32(AT_PAGESZ);
	bx_cpu.push_32(4096);

	bx_cpu.push_32(AT_EXECFN);
	bx_cpu.push_32(execfn);

	bx_cpu.push_32(AT_CLKTCK);
	bx_cpu.push_32(100);

	bx_cpu.push_32(AT_ENTRY);
	bx_cpu.push_32(loader->getEntryAddress());

	bx_cpu.push_32(AT_SYSINFO_EHDR);
	bx_cpu.push_32(vsyscall);

	bx_cpu.push_32(AT_PHDR);
	bx_cpu.push_32(hdr.e_phoff);

	bx_cpu.push_32(AT_PHENT);
	bx_cpu.push_32(hdr.e_phentsize);

	bx_cpu.push_32(AT_PHNUM);
	bx_cpu.push_32(hdr.e_phnum);


}

void setInitProgArgs(Bit32u initAddr, int argc, Bit32u argv, Bit32u env)
{
  Bit8u * mem = (Bit8u *) (bx_mem.memory +bx_mem.virtualAddressToPosition(initAddr));

  * ((Bit32u *)(mem+16)) = argc;
  * ((Bit32u *)(mem+21)) = argv;
  * ((Bit32u *)(mem+26)) = env;
}
