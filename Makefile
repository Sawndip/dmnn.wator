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
	build/tools/wator -m test
train:build
	build/tools/wator -m train	
clean:
	rm -rf $(OBJ)