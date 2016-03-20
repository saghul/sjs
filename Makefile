
all:
	cmake -H. -Bbuild
	cmake --build build
	ln -sf build/sjs sjs

clean:
	rm -rf build
	rm -f sjs

.PHONY: all clean
