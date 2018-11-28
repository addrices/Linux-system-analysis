CC = $(NDK_PATH)/clang
LD = $(NDK_PATH)/clang

INCLUDE = ./include
LDLIBS = ./lib
EXTRALIBS = -lc -lhardware

CFLAGS = -Wall -Werror -g -O2 -I$(INCLUDE) -fPIE
LDFLAGS = -L$(LDLIBS) $(EXTRALIBS) -pie -fPIE

orientd: orientd.o
	$(LD) $^ -o $@ $(LDFLAGS)

orientd.o: orientd.c orientd.h
	$(CC) -c $(CFLAGS) $<

.PHONY: all clean

all: orientd

clean:
	rm -f orientd *.o
