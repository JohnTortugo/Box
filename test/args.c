/*
 * Compilacao:
 *   gcc -o args -nostdlib crt2.S args.c printf.c
 */

#include <printf.h>

int main(int argc, char *argv[], char *env[])
{
 int i;
 prntf("Argumentos %d\n",argc);

 for(i=0; i<argc;i++)
  prntf("arg[%d]: %s\n",i,argv[i]);

 prntf("\nVariaveis de ambiente\n");

 i=0;

 while (env[i] != 0)
  prntf("env[%d]: %s\n",i,env[i++]);

 prntf("\nFim!\n");
 return 0;
}
