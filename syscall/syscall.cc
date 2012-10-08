#include "syscall.h"

extern BX_MEM_C bx_mem;


void BX_SYSCALL::handle()
{
	switch (EAX) {
		case __NR_access:
			printf("access.\n");
			break;

		case __NR_brk:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(EBX);
				EAX = brk((void *)ptr);
			}
		  break;

		case __NR_chmod:
			printf("chmod\n");
			break;

		case __NR_close:
			EAX = close(EBX);
			printf("close.\n");
			break;

		case __NR_creat:
			printf("creat\n");
			break;

		case __NR_dup:
			printf("dup.\n");
			break;

		case __NR_dup2:
			printf("dup2.\n");
			break;

		case __NR_execve:
			printf("execve.\n");
			break;

		case __NR_exit:
			_exit(EBX);
			break;

		case __NR_fchmod:
			printf("fchmod.\n");
			break;

		case __NR_fchown:
			printf("fchown.\n");
			break;

		case __NR_fcntl:
			printf("fnctl.\n");
			break;

		case __NR_fcntl64:
			printf("fnctl64.\n");
			break;

		case __NR_flock:
			printf("flock\n");
			break;

		case __NR_fork:
			printf("fork.\n");
			break;

		case __NR_fstat64:
			printf("fstat64.\n");
			break;

		case __NR_fsync:
			printf("fsync\n");
			break;

		case __NR_ftruncate:
			printf("ftruncate\n");
			break;

		case __NR_getcwd:
			printf("getcwd.\n");
			break;

		case __NR_getdents:
			printf("getdents\n");
			break;

		case __NR_getegid:
			printf("getegid.\n");
			break;

		case __NR_getgid:
			printf("getgid.\n");
			break;

		case __NR_getpid:
			printf("getpid.\n");
			break;

		case __NR_gettimeofday:
			printf("gettimeofday.\n");
			break;

		case __NR_getuid:
			printf("getuid.\n");
			break;

		case __NR_getrlimit:
			printf("getrlimit.\n");
			break;

		case __NR_ioctl:
			printf("ioctl.\n");
			break;

		case __NR_link:
			printf("link.\n");
			break;

		case __NR_kill:
			printf("kill.\n");
			break;

		case __NR_lseek:
			printf("lseek.\n");
			break;

		case __NR_lstat64:
			printf("lstat64.\n");
			break;

		case __NR_madvise:
			printf("madvise.\n");
			break;

		case __NR_mmap:
			printf("mmap.\n");
			break;

		case __NR_mmap2:
			printf("mmap2.\n");
			break;

		case __NR_mkdir:
			printf("mkdir.\n");
			break;

		case __NR_modify_ldt:
			printf("modify ldt.\n");
			break;

		case __NR_mprotect:
			printf("mprotect.\n");
			break;

		case __NR_mremap:
			printf("mremap.\n");
			break;

		case __NR_munmap:
			printf("munmap.\n");
			break;

		case __NR_nanosleep:
			printf("nanosleep.\n");
			break;

		case __NR_open:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(EBX);
				EAX = open((const char *)ptr, ECX, EDX);
			}
			printf("open.\n");
			break;

		case __NR_pipe:
			printf("pipe.\n");
			break;

		case __NR_read:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(ECX);
				EAX = read(EBX, (void *)ptr, EDX);
				printf("read.\n");
			}
			break;

		case __NR_readlink:
			printf("read link.\n");
			break;

		case __NR_rename:
			printf("rename.\n");
			break;

		case __NR_restart_syscall:
			//EAX = sys_restart_syscall();
			printf("restart_syscall.\n");
			break;

		case __NR_rmdir:
			printf("rmdir.\n");
			break;

		case __NR_rt_sigaction:
			printf("rtsigaction.\n");
			break;

		case __NR_rt_sigprocmask:
			printf("rt_sigprocmask.\n");
			break;

		case __NR_rt_sigreturn:
			printf("rt_sigreturn.\n");
			break;

		case __NR_sigreturn:
			printf("sigreturn.\n");
			break;

		case __NR_socketcall:
			printf("socketcall.\n");
			break;

		case __NR_statfs:
			printf("statfs.\n");
			break;

		case __NR_stat64:
			printf("stat64.\n");
			break;

		case __NR_time:
			printf("time.\n");
			break;

		case __NR_times:
			printf("times.\n");
			break;

		case __NR_umask:
			printf("umask.\n");
			break;

		case __NR_unlink:
			printf("unlink.\n");
			break;

		case __NR_utime:
			printf("utime.\n");
			break;

		case __NR_waitpid:
			printf("waitpid.\n");
			break;

		case __NR_wait4:
			printf("wait4.\n");
			break;

		case __NR_write:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(ECX);
				EAX = write(EBX, (const void *) ptr , EDX);
			}
			break;

		case __NR_writev:
			printf("writev.\n");
			break;

		default:
			BX_PANIC(("Unknow system call: EAX: 0x%08x",EAX));
	}
}
