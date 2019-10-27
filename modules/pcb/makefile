CC = arm-none-eabi-gcc
SRCDIR = ./src
BUILDDIR = ./build
TARGET = ./

OBJECTS = $(BUILDDIR)/coda.o $(BUILDDIR)/alberi.o $(BUILDDIR)/hash.o $(BUILDDIR)/p1test.o
CFLAGS = -mcpu=arm7tdmi
HEADERS = ../../headers
DIP = $(HEADERS)/coda.h $(HEADERS)/alberi.h $(HEADERS)/hash.h
INC = -I /usr/include/uarm/ -I ../../headers/

all: $(OBJECTS)
	@echo "Done"


$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(DIP)
	@echo " Building objects pcb" 
	$(CC) -lc $(CFLAGS) $(INC) -c $< -o $@ 

clean:
	@echo " Cleaning..."
	find . -type f -name '*.o' -delete

