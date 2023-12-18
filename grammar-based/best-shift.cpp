#include "fourier-grammar.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

#define U (16)   /* Universe: [0..U-1] */

vector< vector<int> > load( istream& in ) {
    // input format: each line contains a set of integers
    // e.g., 
    //     1 7 8
    //     1 8
    vector< vector<int> > ret;
    string line;
    while( getline( in, line ) ) {
        stringstream ss( line );
        vector<int> s;
        int x;
        while( ss >> x ) {
            s.push_back(x);
            if( x < 0 || x >= U ) cerr << "ERROR: out of range" << endl;
        }
        if( !s.empty() ) {
            sort( s.begin(), s.end() );
            ret.push_back(s);
        }
    }
    return ret;
}

int main( int argc, char **argv ) {
    node *x = create();
    int i, j, k;

    //input is given from stdin like:
    //
    //     1 7 8
    //     1 8
    vector< vector<int> > S = load( cin );

    for( i = 1; i < U; i <<= 1 ) {
        for( k = 0; k < S.size(); ++k ) {
            int N = S[k].size();
            for( j = 0; j < N; ++j ) {
                int current_leaf  = S[k][j];
                int previous_leaf = S[k][(j+N-1)%N];

                int gap = (current_leaf-previous_leaf+U-1)%U+1; 
                if( gap >= i ) {
                    // Case 1: interval (gap) is too long (always costs 1 at this level regardless of shift)

                    // add an interval covering the whole range
                    x = add( 0, i-1, 1, x ); // unnecessary for finding a minimum
                                             // but just for computing the correct cost
                    continue;
                }

                int b = (i-(current_leaf%i))%i;
                int e = (b+gap-1)%i;
                if( b <= e ) {
                    // Case 2a: interval is contained in the current range [0..i-1]
                    // 0              i-1
                    // ...+----+.......
                    //    b     e
                    x = add( b, e, 1, x );
                } else {
                    // Case 2b: interval crosses over the boundary of the current range [0..i-1]
                    // 0              i-1
                    // --+..........+--
                    //   e          b
                    x = add( b, i-1, 1, x );
                    x = add( 0, e%i, 1, x );
                }
            }
        }
        x = duplicate(x);
    }

    printf("cost_vs_shift=\t");
	printSums(0, x -> size - 1, x, 0);
	putchar('\n');
    printf("min_shift=\t%d\n", x->argmin);
    printf("min_cost=\t");
	printSums(x -> argmin, x -> argmin, x, 0);
    putchar('\n');

    return 0;
}

