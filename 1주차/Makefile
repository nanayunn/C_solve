PROGRAM = main

INCLUDES = -I.

CFLAGS = $(INCLUDES) $(EXTRA_CFLAGS)

CC = gcc -Wstrict-prototypes -Wall -O2 -g

LIB =

MAKE = make

.c.o:
	$(CC) $(CFLAGS) -c $*.c

SRC = main.c parse_detail.c parse.c

OBJ = $(SRC:.c=.o)

all: $(PROGRAM)

$(PROGRAM): $(OBJ)
	$(CC) $(DEFINES) $(CFLAGS) -o $@ $(OBJ) $(LDIR) $(LIB)

install:

clean:
	rm -f $(PROGRAM) $(OBJ)
