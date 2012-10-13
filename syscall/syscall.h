#ifndef __BX_SYSCALL__
#define __BX_SYSCALL__ 1

#define NEED_CPU_REG_SHORTCUTS 1
#include "asm/unistd.h"
#include <stdio.h>
#include <unistd.h>
#include "bochs.h"
#include "debug.h"

class BX_SYSCALL {
public:
	void handle();

};

#include "cpu/cpu.h"
#include <sys/file.h>
#include "sys/utsname.h"
#include <sys/resource.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <utime.h>
#include <sys/times.h>
#include <sys/vfs.h>
#include <sys/wait.h>

#endif
