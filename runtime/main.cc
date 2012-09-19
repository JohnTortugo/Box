#include <stdlib.h>
#include <stdio.h>

#include "../bochs.h"
#include "../config.h"
#include "../elf/ElfLoader.h"

int bxmain(int argc, char **argv);

int main(int argc, char **argv) {
  printf("\n* Informacoes do ELF\n");

  ElfLoader loader(argv[0], getenv("LD_LIBRARY_PATH"));

  printf("\n* Passando o controle para o interpretador\n");

  return bxmain(argc, argv);
}
