
all:
	cmake -H. -Bbuild
	cmake --build build

clean:
	rm -rf build

.PHONY: all clean
