
CC?=gcc
LD?=gcc

CFLAGS = -Wall -c -g -ansi -pedantic
GFLAGS = $(CFLAGS) `sdl-config --cflags`
LDFLAGS =
LDLIBS = `sdl-config --cflags --libs` -lm -lGL -lGLU -ljpeg -lpng -lz

CFILES = main.c jpeg.c png.c s3tc.c boxen.c glextd.c bmp.c
HFILES = main.h jpeg.h png.h s3tc.h boxen.h glextd.h bmp.h
OFILES = main.o jpeg.o png.o s3tc.o boxen.o glextd.o bmp.o
EXECUT = s3tc-boxen


all: $(OFILES) makefile
	$(LD) $(LDFLAGS) $(OFILES) -o $(EXECUT) $(LDLIBS)

main.o: main.c defines.h
	$(CC) $(GFLAGS) main.c

jpeg.o: jpeg.c defines.h
	$(CC) $(CFLAGS) jpeg.c

png.o: png.c defines.h
	$(CC) $(CFLAGS) png.c

s3tc.o: s3tc.c defines.h
	$(CC) $(CFLAGS) s3tc.c

boxen.o: boxen.c defines.h
	$(CC) $(CFLAGS) boxen.c

glextd.o: glextd.c defines.h
	$(CC) $(CFLAGS) glextd.c

bmp.o: bmp.c defines.h
	$(CC) $(CFLAGS) bmp.c


$(OFILES): $(HFILES)


clean:
	rm -f $(OFILES) core *~ $(EXECUT)

nice:
	rm -f *~
