#include <unistd.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include "syscall.h"
#include "runtime/runtime.h"

extern BX_MEM_C bx_mem;
extern BX_RUNTIME bx_rnt;

void BX_SYSCALL::handle()
{
    BX_DEBUG(("Handling syscall 0x%x", EAX));

	switch (EAX) {
		case __NR_access:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(EBX);
				EAX = access((const char *)ptr, ECX);
			}
			break;

		case __NR_brk:
			{
				// if is greater than the original program break
				// that is, final of DATA segment of mainExecutable
				// if doesn't reach libs start
				if (EBX > bx_mem.getOrigProgramBreak() && EBX < bx_mem.getLibraryStart()) {
					bx_mem.setProgramBreak(EBX);
				}
#ifdef DEBUG
				else {
					if (EBX != 0) {
						BX_DEBUG(("sys_brk with value outside limits: 0x%h", EBX));
					}
				}
#endif

				EAX = bx_mem.getProgramBreak();
			}
		  break;

		case __NR_chmod:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(EBX);
				EAX = chmod((const char *)ptr, ECX);
			}
			break;

		case __NR_close:
		    {
		        bool found = false;

		        for (int i=0; i<bx_rnt.fileDescriptors.size(); i++) {
                    if (bx_rnt.fileDescriptors[i].fd == EBX) {
                        if (bx_rnt.fileDescriptors[i].mMapped == false) {
                            // simple case
                            EAX = close(EBX);
                        }
                        else {
                            lseek(bx_rnt.fileDescriptors[i].fd, bx_rnt.fileDescriptors[i].fileOffset, SEEK_SET);
                            write(EBX, (char *)(bx_mem.memory + bx_rnt.fileDescriptors[i].memOffset), bx_rnt.fileDescriptors[i].memLength);
                            EAX = close(EBX);

                            bx_rnt.fileDescriptors.erase(bx_rnt.fileDescriptors.begin() + i);
                        }

                        found = true;
                        break;
                    }
		        }

		        if (!found) {
		            BX_DEBUG(("Close syscall didnt found file descriptor."));
		        }
		    }
			break;

		case __NR_creat:
			{
				Bit32u ptr = bx_mem.VirtualToRealAddress(EBX);

				EAX = syscall(EAX, (const char *)ptr, ECX);

				if ( (Bit32s)EAX > -1 ) {
				   BX_FD fd;
				   fd.fileName = strdup((const char *)ptr);
				   fd.flags    = ECX;
			 	   fd.mode     = EDX;
				   fd.fd       = EAX;
				   fd.fileBacked = true;
			 	   fd.mMapped  = false;
				   fd.freed    = false;
				   fd.fileOffset = 0;
				   fd.memLength = 0;
				   fd.memOffset = 0;

				   bx_rnt.fileDescriptors.push_back(fd);
				}
				else {
				    BX_DEBUG(("creat failed for %s", ptr));
					EAX = ~errno +1;
				}
			}
			break;

		case __NR_dup:
			EAX = dup(EBX);
			break;

		case __NR_dup2:
			EAX = dup2(EBX, ECX);
			break;

		case __NR_execve:
			{
				Bit32u filename, argv, envp;
				filename = bx_mem.VirtualToRealAddress(EBX);
				argv = bx_mem.VirtualToRealAddress(ECX);
				envp = bx_mem.VirtualToRealAddress(EDX);
				EAX = execve((const char *)filename, (char *const *) argv, (char *const *) envp);
			}
			break;

		case __NR_exit:
			_exit(EBX);
			break;

		case __NR_fchmod:
			EAX = fchmod(EBX, ECX);
			break;

		case __NR_fchown:
			EAX = fchown(EBX, ECX, EDX);
			break;



		case __NR_flock:
			EAX = flock(EBX, ECX);
			break;


		case __NR_fstat64:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(ECX);
				EAX = syscall(EAX, EBX, (struct stat *)ptr);
				if ( EAX == 0xffffffff)
					EAX = ~errno +1;
			}
			break;

		case __NR_fsync:
			EAX = fsync(EBX);
			break;

		case __NR_ftruncate:
			EAX = ftruncate(EBX, ECX);
			break;

		case __NR_getcwd:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(EBX);
				EAX = bx_mem.RealToVirtualAddress((Bit32u)getcwd((char *)ptr, ECX));
			}
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
		case __NR_link:
			{
				Bit32u oldpath, newpath;
				oldpath = bx_mem.VirtualToRealAddress(EBX);
				newpath = bx_mem.VirtualToRealAddress(ECX);
				EAX = link((const char *)oldpath, (const char *)newpath);
			}
			break;
		case __NR_kill:
			EAX = kill(EBX, ECX);
			break;

		case __NR_lseek:
			EAX = syscall(EAX, EBX, ECX, EDX);
			if ( EAX == 0xffffffff)
				EAX = ~errno +1;
			break;
		case __NR_lstat64:
			{
				Bit32u path, buf;
				path = bx_mem.VirtualToRealAddress(EBX);
				buf = bx_mem.VirtualToRealAddress(ECX);
				EAX = lstat((const char *)path, (struct stat *)buf);
			}
			break;

		case __NR_madvise:
			{
				Bit32u addr;
				addr = bx_mem.VirtualToRealAddress(EBX);
				EAX = madvise((void *)addr, ECX, EDX);
			}
			break;

		case __NR_mkdir:
			{
				Bit32u pathname;
				pathname = bx_mem.VirtualToRealAddress(EBX);
				EAX = mkdir((const char *)pathname, ECX);
			}
			break;

		case __NR_mprotect:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(EBX);
				EAX = mprotect((void *)ptr, ECX, EDX);
			}
			break;

		case __NR_munmap:
			{
				Bit32u pos = bx_mem.virtualAddressToPosition(EBX);
				Bit32u len = ECX;
				bool found = false;

				for (int i=0; i<bx_rnt.fileDescriptors.size(); i++) {
				    if (bx_rnt.fileDescriptors[i].memOffset == pos) {
				        bx_rnt.fileDescriptors[i].freed   = true;

				        found = true;
				        break;
                    }
				}

				if (!found) {
				    BX_DEBUG(("Munmap didnt found range specified: %x.", pos));
				}
				else {
				    EAX = 0;
				}
			}
			break;

		case __NR_nanosleep:
			{
				Bit32u ptr, ptr2;
				ptr = bx_mem.VirtualToRealAddress(EBX);
				ptr = bx_mem.VirtualToRealAddress(ECX);
				EAX = nanosleep((const timespec*)ptr, (timespec*)ptr2);
			}
			break;

		case __NR_open:
			{
				Bit32u ptr = bx_mem.VirtualToRealAddress(EBX);

				EAX = syscall(EAX,(const char *)ptr, ECX, EDX);

				if ((Bit32s)EAX > -1) {
				   BX_FD fd;
				   fd.fileName = strdup((const char *)ptr);
				   fd.flags    = ECX;
			 	   fd.mode     = EDX;
				   fd.fd       = EAX;
				   fd.fileBacked = true;
			 	   fd.mMapped  = false;
				   fd.freed    = false;
				   fd.fileOffset = 0;
				   fd.memLength = 0;
				   fd.memOffset = 0;

				   bx_rnt.fileDescriptors.push_back(fd);
				}
				else {
				    BX_DEBUG(("open failed for %s", ptr));
				    EAX = ~errno +1;
				}
			}
			break;

		case __NR_pipe:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(EBX);
				EAX = pipe2((int *)ptr, ECX);
			}
			break;

		case __NR_read:
			{
				Bit32u ptr;
				ptr = bx_mem.VirtualToRealAddress(ECX);
				EAX = read(EBX, (void *)ptr, EDX);
			}
			break;

		case __NR_readlink:
			{
			  Bit32u path, buf;
			  path = bx_mem.VirtualToRealAddress(EBX);
			  buf = bx_mem.VirtualToRealAddress(ECX);
			  EAX = readlink((const char *)path, (char *)buf, EDX); 
			}
			break;

		case __NR_rename:
			{
				Bit32u oldpath, newpath;
				oldpath = bx_mem.VirtualToRealAddress(EBX);
				newpath = bx_mem.VirtualToRealAddress(ECX);
				EAX = rename((const char *)oldpath, (const char *)newpath); 
			}
			break;

		case __NR_rmdir:
			{
				Bit32u pathname;
				pathname = bx_mem.VirtualToRealAddress(EBX);

				EAX = syscall(EAX, pathname);

				if ( EAX == 0xffffffff)
					EAX = ~errno +1;
			}
			break;

		case __NR_rt_sigaction:
			{
				Bit32u act, oldact;
				act = bx_mem.VirtualToRealAddress(ECX);
				oldact = bx_mem.VirtualToRealAddress(EDX);
				EAX = sigaction(EBX, (const struct sigaction *)act, (struct sigaction *)oldact);
			}
			break;

		case __NR_rt_sigprocmask:
			{
				Bit32u set, oldset;
				set = bx_mem.VirtualToRealAddress(ECX);
				oldset = bx_mem.VirtualToRealAddress(EDX);
				EAX = sigprocmask(EBX, (const sigset_t *)set, (sigset_t *)oldset); 
			}
			break;

		case __NR_stat64:
			{
				Bit32u path, buf;
				path = bx_mem.VirtualToRealAddress(EBX);
				buf = bx_mem.VirtualToRealAddress(ECX);
				EAX = syscall(EAX,(const char *)path, (struct stat *)buf);
			}
			break;

		case __NR_statfs:
			{
				Bit32u path, buf;
				path = bx_mem.VirtualToRealAddress(EBX);
				buf = bx_mem.VirtualToRealAddress(ECX);
				EAX = syscall(EAX,(const char *)path, (struct statfs *)buf);
			}
			break;

		case __NR_time:
			{
				Bit32u t;
				t = bx_mem.VirtualToRealAddress(EBX);
				EAX = time((time_t *)t); 
			}
			break;

		case __NR_times:
			{
				Bit32u buf;
				buf = bx_mem.VirtualToRealAddress(EBX);
				EAX = times((struct tms *)buf); 
			}
			break;

		case __NR_umask:
			EAX = umask(EBX); 
			break;

		case __NR_unlink:
			{
				Bit32u pathname;
				pathname = bx_mem.VirtualToRealAddress(EBX);
				EAX = syscall(EAX, (const char *)pathname);
			}
			break;

		case __NR_utime:
			{
				Bit32u filename, times;
				filename = bx_mem.VirtualToRealAddress(EBX);
				times = bx_mem.VirtualToRealAddress(ECX);
				EAX = utime((const char *)filename, (const struct utimbuf *)times);
			}
			break;

		case __NR_waitpid:
			{
				Bit32u status;
				status = bx_mem.VirtualToRealAddress(ECX);
				EAX = waitpid(EBX, (int *)status, EDX); 
			}
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
			break;

        case __NR_mmap:
            {
                Bit32u addr     = bx_mem.readUINT32(bx_mem.virtualAddressToPosition(EBX));
                Bit32u length   = bx_mem.readUINT32(bx_mem.virtualAddressToPosition(EBX+4));
                Bit32s prot     = bx_mem.readSINT32(bx_mem.virtualAddressToPosition(EBX+8));
                Bit32s flags    = bx_mem.readSINT32(bx_mem.virtualAddressToPosition(EBX+12));
                Bit32s fd       = bx_mem.readSINT32(bx_mem.virtualAddressToPosition(EBX+16));
                Bit32u offset   = bx_mem.readUINT32(bx_mem.virtualAddressToPosition(EBX+20));
                bool found      = false;

                // check if the region is backed by a file on disk or not
                if (flags & MAP_ANONYMOUS) {
                    // identify where we should write the data and register the new mapping
                    Bit32u memOffset = bx_rnt.newMappedIOSpace(-1, 0, length);

                    // copy data from file to memory
                    memset((char *)(bx_mem.memory + memOffset), 0, length);

                    // return the info telling where the data was copied
                    EAX = bx_mem.positionToVirtualAddress(memOffset);
                }
                else {
                    for (int i=0; i<bx_rnt.fileDescriptors.size(); i++) {
                        if (bx_rnt.fileDescriptors[i].fd == fd) {
                            // we assume the file already exist because a previous call to open have already been made
                            FILE *fp = fopen(bx_rnt.fileDescriptors[i].fileName, "r");

                            if (!fp) {
                                BX_PANIC(("Mmap failed. Cant open file %s", bx_rnt.fileDescriptors[i].fileName));
                                break;
                            }

                            // identify where we should write the data and register the new mapping
                            Bit32u memOffset = bx_rnt.newMappedIOSpace(i, offset, length);

                            // offset from begin of file
                            fseek(fp, offset, SEEK_SET);

                            // copy data from file to memory
                            Bit32s elength = fread((char *)(bx_mem.memory + memOffset), length, 1, fp);

                            // fill remainder length with zero if file wasnt sufficiently large
                            if (elength < length) memset((char *)(bx_mem.memory + elength), 0, length - elength);

                            // return the info telling where the data was copied
                            EAX = bx_mem.positionToVirtualAddress(memOffset);

                            // close the tmp file descriptor
                            fclose(fp);

                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        BX_DEBUG(("Mmap failed to find FD opened."));
                        EAX = -1;
                    }
                }
            }
            break;
        case __NR_ioctl:
        	EAX = ioctl(EBX,ECX,bx_mem.VirtualToRealAddress(EDX));
        	break;
		case __NR_modify_ldt:
		case __NR_restart_syscall:
		case __NR_socketcall:
		case __NR_getdents:
        case __NR_rt_sigreturn:
        case __NR_sigreturn:
        case __NR_fcntl:
        case __NR_fcntl64:
        case __NR_fork:
        case __NR_wait4:
        case __NR_mmap2:
        case __NR_mremap:
            BX_DEBUG(("Unimplemented syscall: EAX: 0x%08x", EAX));
            break;
		default:
			BX_PANIC(("Unknow system call: EAX: 0x%08x", EAX));
			break;
	}
}
