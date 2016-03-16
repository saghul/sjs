
LIBSJS_SRC = src/duktape/duktape.c src/vm/vm.c
LIBSJS_LIB = libsjs.so

SJS_CLI_SRCS = $(wildcard src/cli/*.c)

CC ?= gcc
CFLAGS += -O2 -g -pedantic -std=c99 -Wall -fstrict-aliasing -fno-omit-frame-pointer
CFLAGS += -I./src/duktape -I./src/vm
LDFLAGS	+= -lm

all: sjs

$(LIBSJS_LIB): $(LIBSJS_SRC)
	$(CC) -shared -fPIC $(CFLAGS) -o $@ $^

sjs: $(SJS_CLI_SRCS) $(LIBSJS_LIB)
	$(CC) -o $@ $(CFLAGS) -Wno-gnu-zero-variadic-macro-arguments $(SJS_CLI_SRCS) $(LDFLAGS) -L. -lsjs

clean:
	-rm -f sjs libsjs.so
	-rm -rf *.dSYM

.PHONY: all clean
