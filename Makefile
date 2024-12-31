flags=-O2 -Wall -std=c2x
ldflags=

all: clean Cache

Tree.o: Tree.c
	cc ${flags} -c $^

Cache: Cache.o Tree.o
	cc ${flags} $^ -o $@ ${ldflags}

Cache.o: Cache.c
	cc ${flags} -c $^

clean:
	rm -f *.o Cache *.txt
	