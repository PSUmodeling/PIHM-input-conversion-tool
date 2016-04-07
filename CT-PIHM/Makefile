# -----------------------------------------------------------------
# ct-PIHM
# Version: V 0.1
# Date: Feb 26, 2015 
# -----------------------------------------------------------------
# Programmer: Dacheng Xiao (dzx102@psu.edu)
# -----------------------------------------------------------------
# Makefile for ct-PIHM 
# -----------------------------------------------------------------

CC = gcc
CFLAGS = -g -O0

SRCDIR = .

SRCS_ =  ct-pihm.c read_old.c
HEADERS_ = ct-pihm.h

EXECUTABLE = ct-pihm
MSG = "...  Compiling ct-PIHM  ..."

LIBS = -lm

SRCS = $(patsubst %,$(SRCDIR)/%,$(SRCS_))
MODULE_SRCS = $(patsubst %,$(SRCDIR)/%,$(MODULE_SRCS_))
HEADERS = $(patsubst %,$(SRCDIR)/%,$(HEADERS_))
MODULE_HEADERS = $(patsubst %,$(SRCDIR)/%,$(MODULE_HEADERS_))

#OBJS_ = $(SRCS_:.c=.o)
#OBJS = $(patsubst %,$(OBJDIR)/%,$(OBJS_))
#MODULE_OBJS_ = $(MODULE_SRCS_:.c=.o)
#MODULE_OBJS = $(patsubst %,$(OBJDIR)/%,$(MODULE_OBJS_))
OBJS = $(SRCS:.c=.o)
MODULE_OBJS = $(MODULE_SRCS:.c=.o)
  

all:	$(EXECUTABLE)
	@echo
	@echo $(MSG)
	@echo
	@rm $(SRCDIR)/*.o
$(EXECUTABLE): $(OBJS) $(MODULE_OBJS)
	@$(CC) $(CFLAGS)  -o $(EXECUTABLE) $(OBJS) $(MODULE_OBJS) $(LFLAGS) $(LIBS)

#$(OBJDIR)/%.o: $(SRCDIR)/%.c
%.o: %.c
	$(CC) $(CFLAGS)  -c $<  -o $@

#$(OBJS): | $(OBJDIR)
     
#$(OBJDIR):
#	mkdir $(OBJDIR)
.PHONY: clean

clean:
	$(RM) $(SRCDIR)/*.o ct-pihm
