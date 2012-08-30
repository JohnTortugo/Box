#include <stdlib.h>
#include <stdio.h>

#include <bochs.h>
#include <config.h>

void cpu(void);

int main(void) {
 printf("Hello World!\n");
 
 cpu();
 return 0;
}
