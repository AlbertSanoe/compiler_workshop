# compile option
CC = gcc
CFLAGS=-c -std=c11 -g
LDFLAGS=-lm
WFLAGS =-Wall
DEPFLAG= -MM

EXECUTABLE_NAME = chibicc


# file tree
TOPDIR =$(shell dirname $$PWD)
BINDIR = $(TOPDIR)/bin
OBJDIR = $(TOPDIR)/obj
SRCDIR = $(TOPDIR)/src
DEPDIR = $(TOPDIR)/dep

EXECUTABLE= $(BINDIR)/$(EXECUTABLE_NAME)
ALL_OBJFILES=$(wildcard $(OBJDIR)/*.o)

DIRNAME_CODEGEN= codegen
DIRNAME_LEXER = lexer
DIRNAME_PARSER = parser

PATH_LOCAL = $(SRCDIR)
PATH_CODEGEN = $(SRCDIR)/$(DIRNAME_CODEGEN)
PATH_LEXER = $(SRCDIR)/$(DIRNAME_LEXER)
PATH_PARSER = $(SRCDIR)/$(DIRNAME_PARSER)

ALL_OBJFILES=$(wildcard $(OBJDIR)/*.o)

LOCAL_FLAG=global$(DELIMITER)
DELIMITER=@
LOCAL_OBJ_PREFIX=$(OBJDIR)/$(LOCAL_FLAG)
LOCAL_DEP_PREFIX=$(DEPDIR)/$(LOCAL_FLAG)
LOCAL_FILES =$(wildcard *.c)
LOCAL_OBJFILES =$(LOCAL_FILES:%.c=$(LOCAL_OBJ_PREFIX)%.o)
LOCAL_DEPFILES =$(LOCAL_FILES:%.c=$(LOCAL_DEP_PREFIX)%.d)

.PHONY: all global lexer parser codegen

all: $(EXECUTABLE)

$(EXECUTABLE): global lexer parser codegen
	$(CC) $(LDFLAGS) $(ALL_OBJFILES) -o $@

global: $(LOCAL_OBJFILES)

lexer:
	cd $(PATH_LEXER) && $(MAKE)

parser:
	cd $(PATH_PARSER) && $(MAKE)

codegen:
	cd $(PATH_CODEGEN) && $(MAKE)


clean:
	@rm -rf $(OBJDIR)/*
	@rm -rf $(BINDIR)/*

cleanobj:
	rm -rf $(OBJDIR)/*

cleandep:
	rm -rf $(DEPDIR)/*

info:
	echo $(LOCAL_OBJ_PREFIX)
	echo $(LOCAL_OBJFILES)

-include $(LOCAL_DEPFILES)

$(LOCAL_OBJ_PREFIX)%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

$(LOCAL_DEP_PREFIX)%.d:%.c
	$(CC) $(CFLAGS) $(DEPFLAG) $< | sed '1s|^|'"$(LOCAL_OBJ_PREFIX)"'|' > $@

export TOPDIR BINDIR OBJDIR SRCDIR DEPDIR
export CC CFLAGS WFLAGS DEPFLAG
export DELIMITER
export DIRNAME_LEXER DIRNAME_PARSER DIRNAME_CODEGEN







