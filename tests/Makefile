bin:
	rm -rf cpp/build
	mkdir -p cpp/build
	cd cpp/build && cmake ..  -DCMAKE_BUILD_TYPE=Debug
	cd cpp/build && make -j$(nproc) VERBOSE=1

all: bin
	mv cpp/build/unittest .
	mv cpp/build/tst_client .
	mv cpp/build/tst_server .
	mv cpp/build/tst_bridge .
	rm -rf cpp/build

test: all
	./unittest
	rm unittest tst_client tst_server tst_bridge

clean:
	rm -rf build unittest tst_client tst_server tst_bridge




