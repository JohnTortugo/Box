/*
 * Compilacao:
 *   gcc -o args -nostdlib crt2.S args.c printf.c
 */

#include <elf.h>
#include "printf.h"

int main(int argc, char *argv[], char *env[])
{
 int i;
 Elf32_auxv_t *auxv;

 prntf("Argumentos %d\n",argc);

 for(i=0; i<argc;i++)
  prntf("arg[%d]: %s\n",i,argv[i]);

 prntf("\nVariaveis de ambiente\n");

 i=0;

 while (env[i] != 0) 
  prntf("env[%d]: %s\n",i,env[i++]);

  prntf("\nVetor auxiliar\n");

  while(*env++ != NULL);

  for (auxv = (Elf32_auxv_t *)env; auxv->a_type != AT_NULL; auxv++){
      switch(auxv->a_type){
	case AT_EXECFD:
	 prntf("AT_EXECFD:\t\t0x%04x\n", auxv->a_un.a_val);
         break;
	case AT_PHDR:
	 prntf("AT_PHDR:\t\t0x%08x\n", auxv->a_un.a_val);
         break;
	case AT_PHENT:
	 prntf("AT_PHENT:\t\t%d\n", auxv->a_un.a_val);
         break;
	case AT_PHNUM:
	 prntf("AT_PHNUM:\t\t%d\n", auxv->a_un.a_val);
         break;
	case AT_PAGESZ:
	 prntf("AT_PAGESZ:\t\t%d\n", auxv->a_un.a_val);
         break;
	case AT_BASE:
	 prntf("AT_BASE:\t\t0x%08x\n", auxv->a_un.a_val);
         break;
	case AT_FLAGS:
	 prntf("AT_FLAGS:\t\t%d\n", auxv->a_un.a_val);
         break;
	case AT_ENTRY:
	 prntf("AT_ENTRY:\t\t0x%08x\n", auxv->a_un.a_val);
         break;
	case AT_NOTELF:
	 prntf("AT_NOTELF:\t\t%d\n", auxv->a_un.a_val);
         break;
	case AT_UID:
	 prntf("AT_UID:\t\t\t%d\n", auxv->a_un.a_val);
         break;
	case AT_EUID:
	 prntf("AT_EUID:\t\t%d\n", auxv->a_un.a_val);
         break;
	case AT_GID:
	 prntf("AT_GID:\t\t\t%d\n", auxv->a_un.a_val);
         break;
	case AT_EGID:
	 prntf("AT_EGID:\t\t%d\n", auxv->a_un.a_val);
         break;
	case AT_CLKTCK:
	 prntf("AT_CLKTCK:\t\t%d\n", auxv->a_un.a_val);
         break;
	case AT_EXECFN:
	 prntf("AT_EXECFN:\t\t%s\n", (char *) auxv->a_un.a_val);
         break;
	case AT_PLATFORM:
	 prntf("AT_PLATFORM:\t\t%s\n", (char *) auxv->a_un.a_val);
         break;
	case AT_SYSINFO:
	 prntf("AT_SYSINFO:\t\t0x%08x\n", auxv->a_un.a_val);
         break;
	case AT_SYSINFO_EHDR:
	 prntf("AT_SYSINFO_EHDR:\t0x%08x\n", auxv->a_un.a_val);
         break;
	case AT_SECURE:
	 prntf("AT_SECURE:\t\t%d\n", auxv->a_un.a_val);
         break;
	case AT_RANDOM:
	 prntf("AT_RANDOM:\t\t0x%08x\n", auxv->a_un.a_val);
         break;
	case AT_HWCAP:
	 prntf("AT_HWCAP:\t\t0x%x\n", auxv->a_un.a_val);
         break;
	default:
	 prntf("Tipo desconhecido %04x:\t0x%08x\n",auxv->a_type, auxv->a_un.a_val);
      }
  }


 prntf("\nFim!\n");
 return 0;
}
