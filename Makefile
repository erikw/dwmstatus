NAME = dwmstatus

# Paths
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man
X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

# Includes and libs
INCS = -I. -I/usr/include -I$(X11INC)
LIBS = -L/usr/lib -lc -L$(X11LIB) -lX11

# Compiler and linker
CC = clang
CFLAGS = -std=c99 -pedantic -Wall -O3 $(INCS) $(CPPFLAGS)
LDFLAGS = -s $(LIBS)

# Files
SRC = $(NAME).c
OBJ = $(SRC:.c=.o)

all: $(NAME) 

dwmstatus.o: dwmstatus.c
	@$(CC) -c $(CFLAGS) $<

$(NAME): $(OBJ)
	@$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	$(RM) $(OBJ)

.PHONY: all clean
