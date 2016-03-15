
LIBSJS_SRC = src/duktape/duktape.c
LIBSJS_LIB = libsjs.so

SJS_CLI_SRC = src/main.c

CC ?= gcc
CFLAGS += -O2 -g -pedantic -std=c99 -Wall -fstrict-aliasing -fno-omit-frame-pointer
CFLAGS += -I./src/duktape
LDFLAGS	+= -lm

all: sjs

$(LIBSJS_LIB): $(LIBSJS_SRC)
	$(CC) -shared -fPIC $(CFLAGS) -o $@ $^

sjs: $(SJS_CLI_SRC) $(LIBSJS_LIB)
	$(CC) -o $@ $(CFLAGS) $< $(LDFLAGS) -L. -lsjs

clean:
	-rm -f sjs libsjs.so
	-rm -rf *.dSYM

.PHONY: all clean
