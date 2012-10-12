/*
 * =====================================================================================
 *
 *       Filename:  lib.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/07/12 16:30:33
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dr. Fritz Mehner (mn), mehner@fh-swf.de
 *        Company:  FH Südwestfalen, Iserlohn
 *
 * =====================================================================================
 */

void teste2(int a);

extern void write(char * text, int len);

int teste(int a) 
{

 if ( a == 0 ) return 0;

 write("Teste\n",6);

 teste2(a);

 return teste(a-1);
}

void teste2(int a)
{
 int i;
 for(i=0;i<10;i++)
   write("Aleluia irmao!!!!\n",18);
}
