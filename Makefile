bin:
	rm -rf build
	mkdir -p build
	cd build && cmake ..
	cd build && make -j$(nproc)
	mkdir -p python/app/pyatalink/cpp
	cp build/libdatalink.so.1.0.0	python/app/pydatalink/cpp/libdatalink.so

all: bin 	
	mv build/unittest .
	mv build/tst_client .
	mv build/tst_server .
	mv build/tst_bridge .
	rm -rf build

test: all
	./unittest
	rm unittest

install: bin
	cd build && sudo make install
	cd python && pip3 install . --break-system-packages
	cd python && rm -rf build
	cd python && rm -rf dist
	rm -rf dist

clean:
	rm -rf build unittest tst_client tst_server tst_bridge




