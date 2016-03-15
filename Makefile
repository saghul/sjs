#
#  Example Makefile for building a program with embedded Duktape.
#  The example program here is the Duktape command line tool.
#

DUKTAPE_SOURCES = src/duktape/duktape.c
DUKTAPE_CMDLINE_SOURCES = src/main.c

CC ?= gcc
CFLAGS += -O2 -g -pedantic -std=c99 -Wall -fstrict-aliasing -fno-omit-frame-pointer
CFLAGS += -I./src/duktape   # duktape.h and duk_config.h must be in include path
LDFLAGS	+= -lm

# If you have readline, you may want to enable these.  On some platforms
# -lreadline also requires -lncurses (e.g. RHEL), so it is added by default
# (you may be able to remove it)
#CCOPTS += -DDUK_CMDLINE_FANCY
#CCLIBS += -lreadline
#CCLIBS += -lncurses


sjs: $(DUKTAPE_SOURCES) $(DUKTAPE_CMDLINE_SOURCES)
	$(CC) -o $@ $(CFLAGS) $(DUKTAPE_SOURCES) $(DUKTAPE_CMDLINE_SOURCES) $(LDFLAGS)

clean:
	rm -f sjs
	rm -rf sjs.dSYM

.PHONY: clean
