
MAKE?=make
PYTHON?=python
CMAKE_MK=build/Makefile

all: build

build: $(CMAKE_MK)
	@$(MAKE) -C build

$(CMAKE_MK):
	cmake -H. -Bbuild

install: $(CMAKE_MK)
	@$(MAKE) -C build install

clean:
	@$(MAKE) -C build clean

distclean: clean
	@rm -rf build

run: build
	SJS_PATH=build ./build/sjs

test: build
	$(PYTHON) tools/test.py test/

.PHONY: all build install clean distclean run
