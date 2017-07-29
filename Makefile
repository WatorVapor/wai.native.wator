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
format:
	clang

