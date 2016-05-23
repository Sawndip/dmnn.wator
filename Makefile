OBJ := object
build:pre-build
	cd $(OBJ) && cmake ../
	cd $(OBJ) && make
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