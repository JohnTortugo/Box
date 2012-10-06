#include "cpu/cpu.h"

void verifyParams(int argc, char **argv);
void printBanner();

void initialize();
int run();


void switchTo32bitsMode();


//Environment startup functions
void setup_start_environment(int argc, char *argv[], Bit32u memsize, Bit32u entry);
