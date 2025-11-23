# ==========================================================
# Main app
# ==========================================================
all: clean
	cmake -S . -B builds/app -G "Unix Makefiles"
b:
	cmake --build builds/app
r:
	./builds/app/src/OpenOGEHL
clean:
	rm -rf builds/app

# ==========================================================
# Unit tests
# ==========================================================
t: clean_test
	cmake -S . -B builds/utest -DUNIT_TESTS=1 -G "Unix Makefiles"
tb:
	cmake --build builds/utest
tr:
	./builds/utest/test/OpenOGEHL_utest
clean_test:
	rm -rf builds/utest
