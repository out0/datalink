all:
	rm -rf build
	mkdir -p build
	cd build && cmake ..  -DCMAKE_BUILD_TYPE=Debug
	cd build && make -j$(nproc) VERBOSE=1
	mv build/client .
	mv build/server .
	mv build/bserver .
	mv build/bridge .
	rm -rf build

clean:
	rm -rf build client server bserver bridge




