
MAKE?=make
PYTHON?=python

CMAKE_MK=build/Makefile

BUILDTYPE?=Debug
VERBOSE?=

all: build

build: $(CMAKE_MK)
	@$(MAKE) -C build VERBOSE=$(VERBOSE)

$(CMAKE_MK):
	cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=$(BUILDTYPE)

install: $(CMAKE_MK)
	@$(MAKE) -C build install

clean:
	@$(MAKE) -C build clean

distclean:
	@rm -rf build

run: build
	SJS_PATH=build ./build/sjs

test: build
	$(PYTHON) tools/test.py test/

.PHONY: all build install clean distclean run
