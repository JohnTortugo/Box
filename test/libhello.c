void _init(int argc, char *argv[], char *env[]) __attribute__ ((constructor));

void teste2();

extern void write(char * text, int len);

int iGlobal;

int teste(int a) 
{

 if ( a == 0 ) return 0;

 write("Teste\n",6);

 return teste(a-1);
}

void teste2()
{
 int i;
 for(i=0;i<iGlobal;i++)
   write("Aleluia irmao!!!!\n",18);
}

void _init(int argc, char *argv[], char *env[]){
 iGlobal = 2;
}
