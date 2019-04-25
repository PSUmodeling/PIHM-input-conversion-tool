# -----------------------------------------------------------------
# Makefile for PIHM input conversion tool
# -----------------------------------------------------------------
CC = gcc
CFLAGS = -g -O0

SRCDIR = ./src

SRCS_ = ct-pihm.c read_old.c
HEADERS_ = ct-pihm.h

EXECUTABLE = ct-pihm
MSG = "...  Compiling ct-PIHM  ..."

LIBS = -lm
INCLUDES = -I${SRCDIR}/include

SRCS = $(patsubst %,$(SRCDIR)/%,$(SRCS_))
HEADERS = $(patsubst %,$(SRCDIR)/%,$(HEADERS_))
OBJS = $(SRCS:.c=.o)

all:	$(EXECUTABLE)
	@echo
	@echo $(MSG)
	@echo

$(EXECUTABLE): $(OBJS) $(MODULE_OBJS)
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(EXECUTABLE) $(OBJS) $(MODULE_OBJS) $(LFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

.PHONY: clean

clean:
	@$(RM) $(SRCDIR)/*.o ct-pihm
