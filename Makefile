
MAKE?=make
CMAKE_MK=build/Makefile

all: $(CMAKE_MK)
	@$(MAKE) -C build
	ln -sf build/sjs sjs

$(CMAKE_MK):
	cmake -H. -Bbuild

install: $(CMAKE_MK)
	@$(MAKE) -C build install

clean:
	@$(MAKE) -C build clean

distclean: clean
	@rm -rf build
	@rm -f sjs

.PHONY: all install clean distclean
