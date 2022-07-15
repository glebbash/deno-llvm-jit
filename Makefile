CC=clang-14
CFLAGS=-g `llvm-config-14 --cflags`
LD=clang++-14
LDFLAGS=`llvm-config-14 --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs`

all: sum sum-no-dl

sum.o: sum.c
	$(CC) $(CFLAGS) -c $<

sum: sum.o
	$(LD) $< $(LDFLAGS) -ldl -o $@

sum-no-dl.o: sum-no-dl.c
	$(CC) $(CFLAGS) -c $<

sum-no-dl: sum-no-dl.o
	$(LD) $< $(LDFLAGS) -o $@

clean:
	-rm -f sum.o sum
