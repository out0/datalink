bin:
	rm -rf cpp/build
	mkdir -p cpp/build
	cd cpp/build && cmake ..  -DCMAKE_BUILD_TYPE=Debug
	cd cpp/build && make -j$(nproc) VERBOSE=1
	mv cpp/build/unittest .

test: bin
	./unittest
	rm -rf unittest tst_client tst_server tst_bridge

clean:
	rm -rf build unittest tst_client tst_server tst_bridge




