CC = arm-none-eabi-gcc
SRCDIR = ./src
BUILDDIR = ./build
TARGET = ./

OBJECTS = $(BUILDDIR)/scheduler.o $(BUILDDIR)/interrupt.o $(BUILDDIR)/traphandlers.o $(BUILDDIR)/init.o $(BUILDDIR)/systemcalls.o $(BUILDDIR)/p2test.o
CFLAGS = -mcpu=arm7tdmi
HEADERS = ../../headers
DIP = $(HEADERS)/scheduler.h $(HEADERS)/interrupt.h $(HEADERS)/traphandlers.h $(HEADERS)/init.h $(HEADERS)/systemcalls.h $(HEADERS)/alberi.h $(HEADERS)/coda.h $(HEADERS)/hash.h
INC = -I /usr/include/uarm/ -I ../../headers/

all: $(OBJECTS)
	@echo "Done"


$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(DIP)
	@echo " Building objects kernel" 
	$(CC) -lc $(CFLAGS) $(INC) -c $< -o $@ 

clean:
	@echo " Cleaning..."
	find . -type f -name '*.o' -delete
