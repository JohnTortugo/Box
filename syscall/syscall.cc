#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu/cpu.h"
#include "syscall/syscall.h"
#include "debug.h"

#include <stdio.h>

void BX_SYSCALL::handle() {
	BX_DEBUG(("System Call: \n"));

	switch (EAX) {
		case __NR_restart_syscall:
			printf("restart_syscall.\n");
			break;
		case __NR_exit:
			printf("exit.\n");
			break;
		case __NR_fork:
			printf("fork.\n");
			break;
		case __NR_read:
			printf("read.\n");
			break;
		case __NR_write:
			printf("write.\n");
			break;
		case __NR_open:
			printf("open.\n");
			break;
		case __NR_close:
			printf("close.\n");
			break;
		default:
			break;
	}
}
