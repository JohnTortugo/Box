#include "cpu/cpu.h"
#include "elf/ElfLoader.h"

void verifyParams(int argc, char **argv);
void printBanner();

void initialize();
int run();


void switchTo32bitsMode();


//Environment startup functions
void setup_start_environment(int argc, char *argv[], ElfLoader * loader);
