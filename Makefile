
LIBSJS_SRC = src/duktape.c src/vm.c src/version.c
LIBSJS_LIB = libsjs.so

SJS_CLI_SRCS = $(wildcard src/cli/*.c)

CC ?= gcc
CFLAGS += -O2 -g -pedantic -std=c99 -Wall -fstrict-aliasing -fno-omit-frame-pointer
CFLAGS += -Iinclude/sjs
LDFLAGS	+= -lm

UNAME := $(shell uname -s)
ifeq ($(UNAME), Darwin)
EXTRA_CFLAGS =
else
EXTRA_CFLAGS = -Wl,-rpath='$$ORIGIN'
endif

all: sjs

$(LIBSJS_LIB): $(LIBSJS_SRC)
	$(CC) -shared -fPIC $(CFLAGS) -o $@ $^

sjs: $(SJS_CLI_SRCS) $(LIBSJS_LIB)
	$(CC) -o $@ $(CFLAGS) $(EXTRA_CFLAGS) -D_GNU_SOURCE -Iinclude $(SJS_CLI_SRCS) $(LDFLAGS) -L. -lsjs

clean:
	-rm -f sjs libsjs.so
	-rm -rf *.dSYM

.PHONY: all clean
