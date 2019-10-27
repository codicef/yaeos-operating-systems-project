CC = arm-none-eabi-gcc
SRCDIR = ./src
BUILDDIR_KERNEL = ./modules/kernel/build
BUILDDIR_PCB = ./modules/pcb/build
TARGET = ./

OBJECTS = $(BUILDDIR_KERNEL)/scheduler.o $(BUILDDIR_KERNEL)/interrupt.o $(BUILDDIR_KERNEL)/traphandlers.o $(BUILDDIR_KERNEL)/init.o $(BUILDDIR_KERNEL)/systemcalls.o $(BUILDDIR_KERNEL)/p2test.o $(BUILDDIR_PCB)/hash.o $(BUILDDIR_PCB)/coda.o $(BUILDDIR_PCB)/alberi.o /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o

P1OBJECTS = $(BUILDDIR_PCB)/p1test.o $(BUILDDIR_PCB)/hash.o $(BUILDDIR_PCB)/coda.o $(BUILDDIR_PCB)/alberi.o /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o

CFLAGS = --specs=nosys.specs -T /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x
HEADERS = ./headers
DIP = $(HEADERS)/scheduler.h $(HEADERS)/interrupt.h $(HEADERS)/traphandlers.h $(HEADERS)/init.h $(HEADERS)/systemcalls.h $(HEADERS)/alberi.h $(HEADERS)/coda.h $(HEADERS)/hash.h
INC = -I /usr/include/uarm/ -I ../../headers/

all: kernel pcb init p1test
	@echo "Done"

init: $(OBJECTS)
	$(CC) $(CFLAGS) $(INC) $(OBJECTS) -o bin/init.e

p1test: $(P1OBJECTS)
	$(CC) $(CFLAGS) $(INC) $(P1OBJECTS) -o bin/p1test.e

kernel:
	cd modules/kernel/ && make all

pcb:
	cd modules/pcb/ && make all

clean:
	@echo " Cleaning..."
	find . -type f -name '*.o' -delete
