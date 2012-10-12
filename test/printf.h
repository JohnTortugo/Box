/*
 	Copyright 2001, 2002 Georges Menie (www.menie.org)
 	stdarg version contributed by Christian Ettinger
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
    putchar is the only external dependency for this file,
    if you have a working putchar, leave it commented out.
    If not, uncomment the define below and
    replace outbyte(c) by your own function call.
 
    #define putchar(c) outbyte(c)
*/

#include <stdarg.h>

#define putchar(c)  write(&c,1) 

#define PAD_RIGHT 1
#define PAD_ZERO 2

#define PRINT_BUF_LEN 12

void write(char * text, int len);
static void printchar(char **str, char c);
static int prints(char **out, const char *string, int width, int pad);
static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase);
static int print(char **out, const char *format, va_list args );
int prntf(const char *format, ...);
int sprntf(char *out, const char *format, ...);
