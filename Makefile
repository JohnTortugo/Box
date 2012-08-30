all:
	make -C elfparser/
	make -C elfloader/
	make -C cpu/
	g++ box.cc elfparser/ElfParser.o elfloader/ElfLoader.o cpu/test.o -o box
