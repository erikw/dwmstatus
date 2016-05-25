NAME = dwmstatus

# Paths
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man
X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib
INSTPATH= $(PREFIX)/bin

# Other
INSTMODE = 0755

# Includes and libs
INCS = -I. -I/usr/include -I$(X11INC)
LIBS = -L/usr/lib -lc -L$(X11LIB) -lX11

# Compiler and linker
CC = clang

CFLAGS = -std=c99 -pedantic -Wall  $(INCS) $(CPPFLAGS)
CFLAGS += -O3
#CFLAGS += -g -O0

LDFLAGS = -s $(LIBS)

# Files
SRC = $(NAME).c
OBJ = $(SRC:.c=.o)

.PHONY: all clean uninstall

all: $(NAME) 

%.o: %.c
	@$(CC) -c $(CFLAGS) $<

$(NAME): $(OBJ)
	@$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	$(RM) $(OBJ) $(NAME)

install: $(NAME)
	install -m $(INSTMODE) $< $(INSTPATH)

uninstall:
	$(RM) $(INSTPATH)/$(NAME)

