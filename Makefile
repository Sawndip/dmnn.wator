OBJ :=$(shell pwd)/object
build:
	uname -a
	mkdir -p $(OBJ)
	cd $(OBJ) && cmake ../
	cd $(OBJ) && make
test:build
	-cd $(OBJ) && test/test_wator
run:build
	-cd $(OBJ) && tools/wator
	
clean:
	rm -rf $(OBJ)