#define NEED_CPU_REG_SHORTCUTS 1
#include <stdio.h>
#include <unistd.h>
#include "bochs.h"
#include "cpu/cpu.h"
#include "syscall.h"
#include "debug.h"

extern BX_MEM_C bx_mem;


void BX_SYSCALL::handle()
{
	switch (EAX) {
		case __NR_restart_syscall:
			printf("restart_syscall.\n");
			break;
		case __NR_exit:
			_exit(EBX);
			break;
		case __NR_fork:
			printf("fork.\n");
			break;
		case __NR_read:
			printf("read.\n");
			break;
		case __NR_write:
		   {
			  Bit32u ptr;
			  ptr = bx_mem.VirtualToRealAddress(ECX);
			  EAX = write(EBX, (const void *) ptr , EDX);
			}
			break;
		case __NR_open:
			printf("open.\n");
			break;
		case __NR_close:
			printf("close.\n");
			break;
		default:
	        BX_PANIC(("Unknow system call: EAX: %x	",EAX));
	}
}
