OBJ :=$(shell pwd)/object
all:
	unmae -a
	mkdir -p $(OBJ)
	cd $(OBJ) && cmake ../
	cd $(OBJ) && make
