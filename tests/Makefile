all:
	rm -rf build
	mkdir -p build
	cd build && cmake ..  -DCMAKE_BUILD_TYPE=Debug
	cd build && make -j$(nproc) VERBOSE=1
	mv build/client .
	mv build/server .
	rm -rf build

clean:
	rm -rf build client server




