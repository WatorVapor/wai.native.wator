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
SRC4 := $(shell find utilities/ -type f -name *.cpp)
SRC5 := $(shell find utilities/ -type f -name *.hpp)
SRC := $(SRC1)
SRC += $(SRC2)
SRC += $(SRC3)
SRC += $(SRC4)
SRC += $(SRC5)

FORMAT      := $(addsuffix .fmt,$(SRC))
SRC_FOR	    := $(basename $(FORMAT))

format: $(FORMAT)
$(FORMAT):
	clang-format -style=-style="{BasedOnStyle: llvm, IndentWidth: 2}" $(basename $@) > $@
	#clang-format -style=Google $(basename $@) > $@
	mv $@ $(basename $@) 
	
