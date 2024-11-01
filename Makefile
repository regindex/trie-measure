TARGET=trie-measure

all: ${TARGET}

clean:
	rm -f ${TARGET}

trie-measure:  trie_measure.cpp internal/grammar-based/fourier-grammar.cpp
	g++ trie_measure.cpp internal/grammar-based/fourier-grammar.cpp -std=c++17 -march=native -O3 -o $@
