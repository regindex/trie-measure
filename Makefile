TARGET=grammar-based/best-shift

all: ${TARGET}

clean:
	rm -f ${TARGET}

grammar-based/best-shift:  grammar-based/best-shift.cpp grammar-based/fourier-grammar.c grammar-based/fourier-grammar.h
	g++ grammar-based/best-shift.cpp grammar-based/fourier-grammar.c -O3 -o $@
