OBJ :=$(shell pwd)/object
all:
	uname -a
	mkdir -p $(OBJ)
	cd $(OBJ) && cmake ../
	cd $(OBJ) && make
