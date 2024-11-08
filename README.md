# Trie measure and applications

Tools related to the trie-based measures (computation of the measures, data structures for a sequence of sets, etc. )
The $trie-measure$ software takes in input a collection of sets and computes either the optimal shifted trie or the optimal ordered prefix-free encoding size of the input collection.

### Requirements

* A modern C++17 compiler such as `g++` version 8.4 or higher.
* A Linux or MacOS 64-bit operating system.

### Download and Install

~~~~
git clone https://github.com/regindex/trie-measure.git
cd trie-measure && make
~~~~

### Input format

This tool can take in input collections of sets, containing sequences of unique integers separated by newline characters (see the example below),
```
1 2 3 
1 2    
1 0 3
3 
```

### Run

To compute an optimal shift of the fixed-height tries for a collection of sets:
~~~~
./trie-measure < data/book_ratings.inp
~~~~
To compute an optimal ordered prefix-free encoding size for a collection of sets:
~~~~
./trie-measure --optimal-ordered-encoding < data/book_ratings.inp
~~~~
Call help function to print usage instructions:
~~~~
./trie-measure -h
~~~~

### Funding

Funded by the European Union (ERC, REGINDEX, 101039208). Views and opinions expressed are however those of the author(s) only and do not necessarily reflect those of the European Union or the European Research Council. Neither the European Union nor the granting authority can be held responsible for them.