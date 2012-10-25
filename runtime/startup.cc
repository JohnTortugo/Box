#include<stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <elf.h>
#include <algorithm>
#include "bochs.h"
#include "config.h"
#include "runtime.h"

extern char **environ;

#include "init_stub.h"

#define GS_SEG_SIZE 4096 //4K page

Bit32u build_vdso_page(Bit32u end);
Bit32u build_init_table(ElfLoader *loader);
void save_auxiliary_vectors(Bit32u execfn, Bit32u random, Bit32u vsyscall, ElfLoader * loader);
void setInitProgArgs(Bit32u initAddr, int argc, Bit32u argv, Bit32u env);

void setup_environment(int argc, char *argv[], ElfLoader * loader)
{
    int i,size;
    Bit32u inits, vsyscall, stackaddr, addrArgv, addrEnv;
    Bit32u av_EXECFN, av_RANDOM;
    vector<Bit32u> envs;
    vector<Bit32u> args;
    vector<Bit32u>::reverse_iterator it;
    char random[16];

	inits = build_init_table(loader);
//	BX_INFO(("init table address: 0x%08lx", inits));

	stackaddr = (inits) & 0xFFFFFFF0;

	vsyscall = build_vdso_page(stackaddr);
//	BX_INFO(("vsyscall page address: 0x%08lx", vsyscall));

	stackaddr = (vsyscall) & 0xFFFFFFF0;
//	BX_INFO(("stack start address: 0x%08lx", stackaddr));

    bx_cpu.gen_reg[BX_32BIT_REG_EBP].dword.erx = (intptr_t) stackaddr;
    
    //Auxiliary vector data (AT_EXECFN)
    size= strlen(argv[1])+1;
    av_EXECFN = stackaddr-size+1;
    bx_mem.write((Bit8u *) argv[1],bx_mem.virtualAddressToPosition(av_EXECFN), size);
    stackaddr -= size;

    //Auxiliary vector data (AT_RANDOM)
    srand(time(NULL));
    for(i=0;i<16;i++)
    	random[i]= rand() & 0xFF;
    size= 16;
    av_RANDOM = stackaddr-size+1;
    bx_mem.write((Bit8u *) random,bx_mem.virtualAddressToPosition(av_RANDOM), size);
    stackaddr -= size;

    i=0;

    //Copy environment variables to stack
    while (environ[i] != '\0' ) {
    	if ( strncmp("_=", environ[i],2) == 0 ) {
    		char aux[150];
    		size = snprintf(aux,150,"_=%s", argv[1])+1;
    		stackaddr -= size;
    		envs.push_back(stackaddr);
        	bx_mem.write((Bit8u *) aux,bx_mem.virtualAddressToPosition(stackaddr), size);
        	i++;
    	} else {
        	size= strlen(environ[i])+1;
        	stackaddr -= size;
        	envs.push_back(stackaddr);
        	bx_mem.write((Bit8u *) environ[i++],bx_mem.virtualAddressToPosition(stackaddr), size);
    	}
//    	stackaddr --;
    }

    //Copy command line arguments to stack
    for(i=1;i<argc;i++) {
    	size= strlen(argv[i])+1;
    	stackaddr -= size;
    	args.push_back(stackaddr);
    	bx_mem.write((Bit8u *) argv[i],bx_mem.virtualAddressToPosition(stackaddr), size);
//    	stackaddr --;
    }

    stackaddr &= 0xFFFFFFF0; //Stack align;

    bx_cpu.gen_reg[BX_32BIT_REG_ESP].dword.erx = (intptr_t) stackaddr+4;

    // Auxiliary vector entries
    save_auxiliary_vectors(av_EXECFN, av_RANDOM, vsyscall, loader);

    // Null delimiter
    bx_cpu.push_32(0);

    //Environment variables pointers
    for ( it=envs.rbegin() ; it < envs.rend(); it++ )
          bx_cpu.push_32(*it);

    addrEnv = bx_cpu.gen_reg[BX_32BIT_REG_ESP].dword.erx;

    // Null delimiter
    bx_cpu.push_32(0);

    //Command line arguments
    for ( it=args.rbegin() ; it < args.rend(); it++ )
          bx_cpu.push_32(*it);

    addrArgv = bx_cpu.gen_reg[BX_32BIT_REG_ESP].dword.erx;

    //Argument count
    bx_cpu.push_32(argc-1);

    // Patch init routine
    //setInitProgArgs(inits,argc-1,addrArgv, addrEnv);

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

  // ES and SS points to the same segment of DS
  BX_CPU(0)->sregs[BX_SEG_REG_SS] =  BX_CPU(0)->sregs[BX_SEG_REG_ES] =  BX_CPU(0)->sregs[BX_SEG_REG_DS];

  // GS deltas - sysenter
  /*
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
  */

  // CR0 deltas
  BX_CPU(0)->cr0.set_PG(0); // paging disabled
  BX_CPU(0)->cr0.set_PE(1); // protected mode

  BX_CPU(0)->handleCpuModeChange();
}

Bit32u build_vdso_page(Bit32u end)
{
  Bit32u start = (end - GS_SEG_SIZE) & 0xFFFFFFF0;

  memset(bx_mem.memory + bx_mem.virtualAddressToPosition(start), 255,GS_SEG_SIZE);

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

    printf("MainExecutable Init at: %x\n", loader->getEntryAddress());
	*(Bit32u *) (mem+offset)  = loader->getEntryAddress();

    offset -= 4;

    *(Bit32u *) (mem+offset)  = 0; // Delimiter

    for ( it=libs.begin() ; it < libs.end(); it++ ){

      if ( (addr = it->getInit()) != 0 )  {
    	 offset -= 4;

    	 // Search for the segment load position
    	 for(i=0;i< loadedSegments.size();i++ ) {
          if ( loadedSegments[i].fileName == it->getFileName() &&
       		   (loadedSegments[i].hdr.p_flags & PF_X) ) break;
    	 }
    	 addr += loadedSegments[i].loadedPos;

    	 *(Bit32u *) (mem+offset)  = (Bit32u) bx_mem.positionToVirtualAddress(addr);
    	 printf("Lib %s Init at: %x\n", it->getFileName().c_str(), bx_mem.positionToVirtualAddress(addr));
      }
    }

    offset-= sizeof(init_stub);

    bx_mem.write((Bit8u *) init_stub, offset, sizeof(init_stub));

    return bx_mem.positionToVirtualAddress(offset);
}

void save_auxiliary_vectors(Bit32u execfn, Bit32u random, Bit32u vsyscall, ElfLoader * loader)
{
	Elf32_Ehdr hdr = loader->getMainExecutable()->getHdr();

	bx_cpu.push_32(0);
	bx_cpu.push_32(AT_NULL);

	bx_cpu.push_32(4096);
	bx_cpu.push_32(AT_PAGESZ);

	bx_cpu.push_32(execfn);
	bx_cpu.push_32(AT_EXECFN);

	bx_cpu.push_32(100);
	bx_cpu.push_32(AT_CLKTCK);

	bx_cpu.push_32(loader->getEntryAddress());
	bx_cpu.push_32(AT_ENTRY);

	bx_cpu.push_32(vsyscall);
	bx_cpu.push_32(AT_SYSINFO_EHDR);

	bx_cpu.push_32(hdr.e_phoff + bx_mem.virtualBase);
	bx_cpu.push_32(AT_PHDR);

	bx_cpu.push_32(hdr.e_phentsize);
	bx_cpu.push_32(AT_PHENT);

	bx_cpu.push_32(hdr.e_phnum);
	bx_cpu.push_32(AT_PHNUM);

	bx_cpu.push_32(getuid());
	bx_cpu.push_32(AT_UID);

	bx_cpu.push_32(geteuid());
	bx_cpu.push_32(AT_EUID);

	bx_cpu.push_32(getgid());
	bx_cpu.push_32(AT_GID);

	bx_cpu.push_32(getegid());
	bx_cpu.push_32(AT_EGID);

	bx_cpu.push_32(0);
	bx_cpu.push_32(AT_SECURE);

	bx_cpu.push_32(random);
	bx_cpu.push_32(AT_RANDOM);
}

void setInitProgArgs(Bit32u initAddr, int argc, Bit32u argv, Bit32u env)
{
  const char *sARGC="ARGC";
  const char *sARGV="ARGV";
  const char *sENVP="ENVP";
  Bit8u * mem = (Bit8u *) (bx_mem.memory +bx_mem.virtualAddressToPosition(initAddr));
  Bit8u * pos;

  pos = std::search(mem, mem + sizeof(init_stub), sARGC, sARGC + strlen(sARGC));
  if ( pos == NULL )
	 BX_PANIC(("Tag ARGC not fount in init_stub"));
  *((Bit32u *) pos) = argc;

  pos = std::search(mem, mem + sizeof(init_stub), sARGV, sARGV + strlen(sARGV));
  if ( pos == NULL )
	 BX_PANIC(("Tag ARGV not fount in init_stub"));
  *((Bit32u *) pos) = argv;

  pos = std::search(mem, mem + sizeof(init_stub), sENVP, sENVP + strlen(sENVP));
  if ( pos == NULL )
	 BX_PANIC(("Tag ENVP not fount in init_stub"));
  *((Bit32u *) pos) = env;
/*
  * ((Bit32u *)(mem+16)) = env;
  * ((Bit32u *)(mem+21)) = argv;
  * ((Bit32u *)(mem+26)) = argc;
  */

}
