OBJ :=$(shell pwd)/object
all:
	mkdir -p $(OBJ)
	cd $(OBJ) && cmake ../
	cd $(OBJ) && make
