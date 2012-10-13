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
			EAX = getuid();
			printf("getuid.\n");
			break;

		case __NR_getrlimit:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(ECX);
				EAX = getrlimit(EBX, (struct rlimit *) ptr);
			}

			printf("getrlimit.\n");
			break;

		case __NR_ioctl:
			//EAX = ioctl(int d, int request, ...);
			printf("ioctl.\n");
			break;

		case __NR_link:
			{
				Bit32u oldpath, newpath;
				oldpath = bx_mem.VirtualToRealAddress(EBX);
				newpath = bx_mem.VirtualToRealAddress(ECX);
				EAX = link((const char *)oldpath, (const char *)newpath);
			}
			printf("link.\n");
			break;

		case __NR_kill:
			EAX = kill(EBX, ECX);
			printf("kill.\n");
			break;

		case __NR_lseek:
			EAX = lseek(EBX, ECX, EDX);
			printf("lseek.\n");
			break;

		case __NR_lstat64:
			{
				Bit32u path, buf;
				path = bx_mem.VirtualToRealAddress(EBX);
				buf = bx_mem.VirtualToRealAddress(ECX);
				EAX = lstat((const char *)path, (struct stat *)buf);
			}
			printf("lstat64.\n");
			break;

		case __NR_madvise:
			{
				Bit32u addr;
				addr = bx_mem.VirtualToRealAddress(EBX);
				EAX = madvise((void *)addr, ECX, EDX);
			}
			printf("madvise.\n");
			break;

		case __NR_mkdir:
			{
				Bit32u pathname;
				pathname = bx_mem.VirtualToRealAddress(EBX);
				EAX = mkdir((const char *)pathname, ECX);
			}
			printf("mkdir.\n");
			break;

		case __NR_mmap:
			printf("mmap.\n");
			break;

		case __NR_mmap2:
			printf("mmap2.\n");
			break;

		case __NR_modify_ldt:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(ECX);
				//				EAX = modify_ldt(EBX, (void *)ptr, EDX);
			}
			printf("modify ldt.\n");
			break;

		case __NR_mprotect:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(EBX);
				EAX = mprotect((void *)ptr, ECX, EDX);
			}
			printf("mprotect.\n");
			break;

		case __NR_mremap:
		  //void *mremap(void *old_address, size_t old_size,             size_t new_size,int flags, ... /* void *new_address */);
			printf("mremap.\n");
			break;

		case __NR_munmap:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(EBX);
				EAX = munmap((void *)ptr, ECX);
			}
			printf("munmap.\n");
			break;

		case __NR_nanosleep:
			{
				Bit32u ptr, ptr2;
				ptr = bx_mem.VirtualToRealAddress(EBX);
				ptr = bx_mem.VirtualToRealAddress(ECX);
				EAX = nanosleep((const timespec*)ptr, (timespec*)ptr2);
			}
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
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(EBX);
				EAX = pipe2((int *)ptr, ECX);
			}
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
			{
			  Bit32u path, buf;
			  path = bx_mem.VirtualToRealAddress(EBX);
			  buf = bx_mem.VirtualToRealAddress(ECX);
			  EAX = readlink((const char *)path, (char *)buf, EDX); 
			}
			printf("read link.\n");
			break;

		case __NR_rename:
			{
				Bit32u oldpath, newpath;
				oldpath = bx_mem.VirtualToRealAddress(EBX);
				newpath = bx_mem.VirtualToRealAddress(ECX);
				EAX = rename((const char *)oldpath, (const char *)newpath); 
			}
			printf("rename.\n");
			break;

		case __NR_restart_syscall:
		  //EAX = sys_restart_syscall();
			printf("restart_syscall.\n");
			break;

		case __NR_rmdir:
			{
				Bit32u pathname;
				pathname = bx_mem.VirtualToRealAddress(EBX);
				EAX = rmdir((const char *)pathname); 
			}
			printf("rmdir.\n");
			break;

		case __NR_rt_sigaction:
			{
				Bit32u act, oldact;
				act = bx_mem.VirtualToRealAddress(ECX);
				oldact = bx_mem.VirtualToRealAddress(EDX);
				EAX = sigaction(EBX, (const struct sigaction *)act, (struct sigaction *)oldact);
			}
			printf("rtsigaction.\n");
			break;

		case __NR_rt_sigprocmask:
			{
				Bit32u set, oldset;
				set = bx_mem.VirtualToRealAddress(ECX);
				oldset = bx_mem.VirtualToRealAddress(EDX);
				EAX = sigprocmask(EBX, (const sigset_t *)set, (sigset_t *)oldset); 
			}
			printf("rt_sigprocmask.\n");
			break;

		case __NR_rt_sigreturn:
		case __NR_sigreturn:
			{
				Bit32u ptr;
				ptr =  bx_mem.VirtualToRealAddress(EBX);
				EAX = sigreturn((sigcontext *)ptr);
			}
			printf("rt_sigreturn/sigreturn.\n");
			break;

		case __NR_socketcall:
			{
				Bit32u args;
				args =  bx_mem.VirtualToRealAddress(ECX);
				//				EAX = socketcall(EBX, (unsigned long *)args);
			}
			printf("socketcall.\n");
			break;

		case __NR_stat64:
			{
				Bit32u path, buf;
				path = bx_mem.VirtualToRealAddress(EBX);
				buf = bx_mem.VirtualToRealAddress(ECX);
				EAX = stat((const char *)path, (struct stat *)buf);
			}
			printf("stat64.\n");
			break;

		case __NR_statfs:
			{
				Bit32u path, buf;
				path = bx_mem.VirtualToRealAddress(EBX);
				buf = bx_mem.VirtualToRealAddress(ECX);
				EAX = statfs((const char *)path, (struct statfs *)buf);
			}
			printf("statfs.\n");
			break;

		case __NR_time:
			{
				Bit32u t;
				t = bx_mem.VirtualToRealAddress(EBX);
				EAX = time((time_t *)t); 
			}
			printf("time.\n");
			break;

		case __NR_times:
			{
				Bit32u buf;
				buf = bx_mem.VirtualToRealAddress(EBX);
				EAX = times((struct tms *)buf); 
			}
			printf("times.\n");
			break;

		case __NR_umask:
			EAX = umask(EBX); 
			printf("umask.\n");
			break;

		case __NR_unlink:
			{
				Bit32u pathname;
				pathname = bx_mem.VirtualToRealAddress(EBX);
				EAX = unlink((const char *)pathname);
			}
			printf("unlink.\n");
			break;

		case __NR_utime:
			{
				Bit32u filename, times;
				filename = bx_mem.VirtualToRealAddress(EBX);
				times = bx_mem.VirtualToRealAddress(ECX);
				EAX = utime((const char *)filename, (const struct utimbuf *)times);
			}
			printf("utime.\n");
			break;

		case __NR_waitpid:
			{
				Bit32u status;
				status = bx_mem.VirtualToRealAddress(ECX);
				EAX = waitpid(EBX, (int *)status, EDX); 
			}
			printf("waitpid.\n");
			break;

		case __NR_wait4:
			printf("wait4.\n");
			break;

		case __NR_uname: 
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(EBX);
				EAX = uname((struct utsname *) ptr);
				break;
			}

		case __NR_write: 
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(ECX);
				EAX = write(EBX, (const void *) ptr , EDX);
				break;
			}

		case __NR_writev:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(ECX);
				EAX = writev(EBX, (const struct iovec *) ptr , EDX);
				break;
			}
			printf("writev.\n");
			break;

		default:
			BX_PANIC(("Unknow system call: EAX: 0x%08x", EAX));
			break;
	}
}
