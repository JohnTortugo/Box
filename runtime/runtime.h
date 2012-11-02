#ifndef __BX_RUNTIME_H__
#define __BX_RUNTIME_H__

#include <vector>
#include "cpu/cpu.h"
#include "elf/ElfLoader.h"

void verifyParams(int argc, char **argv);
void printBanner();

void initialize();
int run();


void switchTo32bitsMode();


//Environment startup functions
void setup_environment(int argc, char *argv[], ElfLoader * loader);

// this object describe an file descriptor
struct BX_FD {
    // name of the file
    char * fileName;

    // access mode (RW, R, W)
    int flags;

    mode_t mode;

    // file descriptor id
    int fd;

    // offset within the file where start mapping to memory
    Bit32u fileOffset;

    // have a on disk backup file ?
    bool fileBacked;

    // memory mapped ?
    bool mMapped;

    // indicate if an munmap have been executed on this region
    bool freed;

    // where in bx_memory it was mapped
    Bit32u memOffset;

    // length of mapped region
    Bit32u memLength;
};

class BX_RUNTIME {
public:
    // the list of opened file descriptors
    vector<BX_FD> fileDescriptors;

    Bit32u newMappedIOSpace(int i, Bit32u offset, Bit32u length);
};
#endif
