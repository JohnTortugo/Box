# Copyright (C) 2001  The Bochs Project
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA



CC = gcc

TEST_BINS = \
	args \
	hello-asm \
	hello-c \
	hello-dl

all: $(TEST_BINS)

clean:
	rm -f $(TEST_BINS) *.o *.so*

args: crt.S args.c printf.c
	gcc -o args -nostdlib crt.S args.c printf.c

hello-asm: hello.S
	as -o hello-asm.o hello.S && \
	ld -o hello-asm hello-asm.o

hello-c: hello.c
	gcc -o hello-c -nostdlib hello.c

hello-dl: libhello.c hello-dl.c
	@rm -f libhello.so && \
	gcc -shared -nostartfiles -nostdlib -fPIC -Wl,-export-dynamic -Wl,-soname,libhello.so.1 -o libhello.so.1 libhello.c && \
	ln -s libhello.so.1 libhello.so && \
	gcc -nostdlib -o hello-dl -L . -lhello hello-dl.c && \
	echo "AVISO: Para executar o comando hello-dl defina a variavel: LD_LIBRARY_PATH=`pwd`"
