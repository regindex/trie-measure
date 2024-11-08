#ifndef TRIE_MEASURE_HPP_
#define TRIE_MEASURE_HPP_

#include "common.hpp"
#include "grammar-based/fourier-grammar.hpp"
  
class trieEncoding
{
public:
    trieEncoding(){};
    ~trieEncoding(){};

    trieEncoding(istream& input, bool grammar_ = false): sigma(0), best_shift(0), best_cost(0)
    {
        /* input sets */
        S = load( cin );
        n = S.size();

        bitwidth = ceil(log2(sigma));
        U = pow(2,bitwidth);
    }

    ulint no_sets(){ return n; }

    void compute_best_shift(boolt grammar = false, boolt all = false)
    {
        if( grammar )
        {
            cout << "running grammar-based algorithm..." << endl;

            lint i, j, k;
            x = create();

            for( i = 1; i < U; i <<= 1 )
            {
                for( k = 0; k < S.size(); ++k )
                {
                    int N = S[k].size();
                    for( j = 0; j < N; ++j ) {
                        lint current_leaf  = S[k][j];
                        lint previous_leaf = S[k][(j+N-1)%N];

                        lint gap = (current_leaf-previous_leaf+U-1)%U+1; 
                        if( gap >= i ) {
                            // Case 1: interval (gap) is too long (always costs 1 at this level regardless of shift)

                            // add an interval covering the whole range
                            x = add( 0, i-1, 1, x ); // unnecessary for finding a minimum
                                                     // but just for computing the correct cost
                            continue;
                        }

                        lint b = (i-(current_leaf%i))%i;
                        lint e = (b+gap-1)%i;
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

            best_shift = x->argmin;
            best_cost = getSum(x -> argmin, x, 0);
            //best_cost += this->S.size();
            //printSums(x -> argmin, x -> argmin, x, 0);
        }
        else
        {
            /* convert sets to list of gaps */
            vector<pair<lint,lint>> gaps = get_gaps_in_1_based_indexing(this->S,this->U);
            /* */
            tie(best_cost, best_shift) = improved_algorithm(gaps, this->sigma, this->bitwidth);
            //best_cost += this->S.size();
        }
    }

    lint get_best_shift(){ return best_shift; }

    lint get_trie_measure(){ return best_cost; }

    lint get_universe(){ return U; }

    void print_all_costs(){ printSums(0, x -> size - 1, x, 0); }

private:
    lint sigma, U, bitwidth; /* Universe size and bit width */
    lint n;
    lint best_shift, best_cost;
    vector< vector<lint> > S; /* input sets */
    node *x; /* root for grammar-based algorithm */

    /* Load input file and compute universe size */
    vector< vector<lint> > load( istream& in ) {
        // input format: each line contains a set of integers
        // e.g., 
        //     1 7 8
        //     1 8
        vector< vector<lint> > ret;
        string line;
        while( getline( in, line ) ) {
            stringstream ss( line );
            vector<lint> s;
            lint x;
            while( ss >> x ) {
                s.push_back(x);
                if( x < 0 ){ cerr << "ERROR: out of range" << endl; exit(1); }
                if( x > sigma ){ sigma = x; }
            }
            if( !s.empty() ) {
                sort( s.begin(), s.end() );
                ret.push_back(s);
            }
        }

        return ret;
    }

    // Sets are in 0-based indexing but gaps will be in 1-based indexing
    // Assumes sets are sorted and contain no duplicates
    vector<pair<lint,lint>> get_gaps_in_1_based_indexing(const vector<vector<lint>>& sets, lint u)
    {
        vector<pair<lint,lint>> gaps;
        for(const vector<lint>& set : sets){
            for(lint i = 0; i < set.size(); i++){
                lint x = set[i];
                lint y = set[(i+1) % set.size()];
                if(y <= x) y += u;
                gaps.push_back({1+x,1+y});
            }
        }
        return gaps;
    }

    // O(u + N log u) algorithm. N is the number of gaps and u is the universe size.
    // Returns the number of EDGES in the trie.
    pair<lint,lint> improved_algorithm(const vector<pair<lint,lint>>& gaps, lint u, lint logu)
    {
        vector<vector<lint>> levels;
        levels.push_back(vector<lint>()); // Indexes are shift amounts -> 0-indexing since smallest shift is 0
        for(lint k = 1; k <= logu; k++){
            vector<lint> level((1 << (k-1)));
            levels.push_back(level);
        }

        for(lint k = 1; k <= logu; k++){
            vector<lint> deltaC((1 << k) + 1); // Twice the length of level k, plus 1 to have half-open endpoints
            for(auto gap : gaps){
                lint l = gap.first;
                lint r = gap.second; // Exclusive end

                // The smallest $a$ shift is the one that translates r-1 to 2^(k-1)
                // r - 1 + a = 2^(k-1)
                // -> a = 2^(k-1) + 1 - r
                lint x = (1 << (k-1)) + 1 - r;
                x = mod0(x, (1 << (k-1)));
                lint len = (r-l);
                len = min(len, lint(1 << (k-1)) );
                lint y = x + len; // Exclusive end
                deltaC[x]++;
                deltaC[y]--;
            }

            lint csum = 0;
            for(lint i = 0; i < (1 << k); i++){
                csum += deltaC[i];
                levels[k][mod0(i, 1 << (k-1))] += csum;
            }

            if(k > 1){
                for(lint i = 0; i < (1 << (k-1)); i++){
                    levels[k][i] += levels[k-1][mod0(i, (1 << (k-2)))];
                }
            }
        }

        auto min_it = min_element(levels[logu].begin(), levels[logu].end());
        lint argmin = min_it - levels[logu].begin();
        return {*min_it, argmin};
    }

    lint mod1(lint n, lint mod){
        while(n < 1) n += mod;
        return (n - 1) % mod + 1;
    }

    lint mod0(lint n, lint mod){
        while(n < 0) n += mod;
        return n % mod;
    }
};

#endif