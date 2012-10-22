/*
 * Compilacao:
 *   gcc -o hello -nostdlib hello.c
 */

int teste(int a);
int teste2();
void end(void);
void write(char * text, int len);

extern int iGlobal;
int a;

void _start(void)
{
 char *msg = "Hello World!\n";

 for(a=0; a<2;a++)
  write(msg,13);

 write("Teste recursivo\n",16);
 teste(10);

 write("teste2() com iGlobal=2 (padrao)\n",32);
 teste2();

 iGlobal = 5;

 write("teste2() com iGlobal=5\n",23);
 teste2();

 end();
}

void end(void)
{
__asm__("movl $1,%eax\n\t"
        "movl $0,%ebx\n\t"
        "int $0x80");
}

void write(char * text, int len) 
{ 
 asm("movl $4,%%eax\n\t" 
         "movl $1,%%ebx\n\t" 
         "movl %0,%%edx\n\t" 
         "movl %1,%%ecx\n\t" 
         "int $0x80" 
         : 
         : "r" (len), "r" (text) 
         : "%eax", "%ebx", "%edx", "%ecx"); 
} 

