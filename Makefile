make:
	cmake -S . -B build -G "Unix Makefiles"
b:
	cmake --build build
r:
	./build/OpenOGEHL
clean:
	rm -rf build