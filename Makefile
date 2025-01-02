flags=-O2 -Wall -std=c2x
ldflags=

all: clean Meow

Tree.o: Tree.c
	cc ${flags} -c $^

Meow: Meow.o Tree.o
	cc ${flags} $^ -o $@ ${ldflags}

Meow.o: Meow.c
	cc ${flags} -c $^

clean:
	rm -f *.o Meow *.txt
	