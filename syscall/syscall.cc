#include "syscall.h"

extern BX_MEM_C bx_mem;

void BX_SYSCALL::handle()
{
	switch (EAX) {
		case __NR_access:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(EBX);
				EAX = access((const char *)ptr, ECX);
			}
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
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(EBX);
				EAX = chmod((const char *)ptr, ECX);
			}
			printf("chmod\n");
			break;

		case __NR_close:
			EAX = close(EBX);
			printf("close.\n");
			break;

		case __NR_creat:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(EBX);
				EAX = creat((const char *)ptr, ECX);
			}
			printf("creat\n");
			break;

		case __NR_dup:
			EAX = dup(EBX);
			printf("dup.\n");
			break;

		case __NR_dup2:
			EAX = dup2(EBX, ECX);
			printf("dup2.\n");
			break;

		case __NR_execve:
			{
				Bit32u filename, argv, envp;
				filename = bx_mem.VirtualToRealAddress(EBX);
				argv = bx_mem.VirtualToRealAddress(ECX);
				envp = bx_mem.VirtualToRealAddress(EDX);
				EAX = execve((const char *)filename, (char *const *) argv, (char *const *) envp);
			}
			printf("execve.\n");
			break;

		case __NR_exit:
			_exit(EBX);
			break;

		case __NR_fchmod:
			EAX = fchmod(EBX, ECX);
			printf("fchmod.\n");
			break;

		case __NR_fchown:
			EAX = fchown(EBX, ECX, EDX);
			printf("fchown.\n");
			break;

		case __NR_fcntl:
			printf("fnctl.\n");
			break;

		case __NR_fcntl64:
			printf("fnctl64.\n");
			break;

		case __NR_flock:
			EAX = flock(EBX, ECX);
			printf("flock\n");
			break;

		case __NR_fork:
			printf("fork.\n");
			break;

		case __NR_fstat64:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(ECX);
				EAX = fstat(EBX, (struct stat *)ptr);
			}
			printf("fstat64.\n");
			break;

		case __NR_fsync:
			EAX = fsync(EBX);
			printf("fsync\n");
			break;

		case __NR_ftruncate:
			EAX = ftruncate(EBX, ECX);
			printf("ftruncate\n");
			break;

		case __NR_getcwd:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(EBX);
				EAX = bx_mem.RealToVirtualAddress((Bit32u)getcwd((char *)ptr, ECX));
			}
			printf("getcwd.\n");
			break;

		case __NR_getdents:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(ECX);
				//EAX = getdents(EBX, (struct linux_dirent *)ptr, EDX);
			}
			printf("getdents\n");
			break;

		case __NR_getegid:
			EAX = getegid();
			printf("getegid.\n");
			break;

		case __NR_getgid:
			EAX = getgid();
			printf("getgid.\n");
			break;

		case __NR_getpid:
			EAX = getpid();
			printf("getpid.\n");
			break;

		case __NR_gettimeofday:
			{
			  Bit32u tv, tz;
				tv = bx_mem.VirtualToRealAddress(EBX);
				tz = bx_mem.VirtualToRealAddress(ECX);
				EAX = gettimeofday((struct timeval *)tv, (struct timezone *)tz);
			}
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
			BX_PANIC(("Unimplemented system call: EAX: 0x%08x",EAX));
	}
}
