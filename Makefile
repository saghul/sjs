
MAKE?=make
PYTHON?=python3

CMAKE_MK=build/Makefile

BUILDTYPE?=Debug
PREFIX?=/usr/local
VERBOSE?=

all: build

build: $(CMAKE_MK)
	@$(MAKE) -C build VERBOSE=$(VERBOSE)

$(CMAKE_MK):
	@mkdir -p build;                                                            \
	cd build;                                                                   \
	cmake ../ -DCMAKE_BUILD_TYPE=$(BUILDTYPE) -DCMAKE_INSTALL_PREFIX=$(PREFIX); \
	cd ..

install: $(CMAKE_MK)
	@$(MAKE) -C build install

clean:
	@$(MAKE) -C build clean

distclean:
	@rm -rf build

test: build
	$(PYTHON) tools/test.py test/

.PHONY: all build install clean distclean run
