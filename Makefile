OBJ := object
build:pre-build
	export CC=/usr/bin/clang && export CXX=/usr/bin/clang++ && \
	cd $(OBJ) && cmake ../
	cd $(OBJ) && make VERBOSE=1
pre-build:
	uname -a
	mkdir -p $(OBJ)
	ln -sf $(OBJ) build
ut:build
	build/test/test_wator
test:build
	build/tools/wator -p test -c model/retina/model.conf
train:build
	build/tools/wator -p train -c model/retina/model.conf

st: build
	build/tools/shotThough
		
clean:
	rm -rf $(OBJ)
clean-png:
	rm -f *.png
