CFLAGS =  -std=c99 -W -Wall -g
SRC = main.c fat.c

all: uf2extract
	arm-linux-gnueabihf-gcc -Os -s $(CFLAGS) $(SRC) -o uf2d

x86:
	gcc -DX86=1 -g $(CFLAGS) $(SRC) -o uf2d86

uf2extract: uf2extract.o
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

uf2extract.o: uf2extract.c
	gcc -o $@ -c $^ $(CFLAGS) $(CPPFLAGS) -D_DEFAULT_SOURCE
