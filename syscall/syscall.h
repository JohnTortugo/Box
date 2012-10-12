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


#endif
