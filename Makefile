CC=clang-14
CFLAGS=-g `llvm-config-14 --cflags`
LD=clang++-14
LDFLAGS=`llvm-config-14 --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs`

all: sum

sum.o: sum.c
	$(CC) $(CFLAGS) -c $<

sum: sum.o
	$(LD) $< $(LDFLAGS) -o $@

clean:
	-rm -f sum.o sum
