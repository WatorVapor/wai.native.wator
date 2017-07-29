OBJDir = object

.PHONY : all run
all:
	mkdir -p $(OBJDir)
	cd $(OBJDir) && cmake ../
	cd $(OBJDir) && make
	
run:all
	ulimit -c unlimited
	./object/wator.charcut.app
clean:
	rm -f *.o *.app
	rm -rf object
	
SRC1 := $(shell find watorword/ -type f -name *.cpp)
SRC2 := $(shell find watorword/ -type f -name *.hpp)
SRC3 := $(shell find tools/ -type f -name *.cpp)
SRC := $(SRC1)
SRC += $(SRC2)
SRC += $(SRC3)

format: $(SRC)
	clang-format $^ > $^.fmt

