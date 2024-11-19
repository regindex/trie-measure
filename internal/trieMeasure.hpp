#ifndef TRIE_MEASURE_HPP_
#define TRIE_MEASURE_HPP_

#include <iostream>
#include <vector>
#include <sstream>
#include "common.hpp"

using namespace std;

class trieEncoding
{
public:
    trieEncoding(){};
    ~trieEncoding(){};

    trieEncoding(istream& input): best_shift(0), best_cost(0) {
        S = load( input );
    }

    void compute_best_shift(boolt grammar = false, boolt print_all = false) {
        if( grammar )
        {
            solve<SegmentTree>( S, print_all );
        } else {
            solve<SimpleArray>( S, print_all ); 
        }
    }

    template <class T>
    void solve( const vector< vector<lint> >& seqs, boolt print_all=false ) {
        lint logu = get_log_universe( seqs );
        vector< pair<lint,lint> > pairs = convert_into_pairs( seqs, (1<<logu) );
        U = (1<<logu);

        // main algorithm
        T D;
        D.initialize();
        for( lint k = 1; k <= logu; ++k ) {
            for( lint i = 0; i < pairs.size(); ++i ) {
                vector< pair<lint,lint> > intervals = get_subintervals( pairs[i], k, (1<<logu) );
                for( lint j = 0; j < intervals.size(); ++j ) {
                    lint l = intervals[j].first;
                    lint r = intervals[j].second;
                    D.add( l, r );
                }
            } 
            D.extend();
        }
        tie(best_shift, best_cost) = D.argmin(print_all);
    }

    // O(u+Nlogu)-time algorithm
    struct SimpleArray {
        void initialize( void ) {
            Delta = vector<lint>(1,0);
        }
        void add( lint l, lint r ) {
            Delta[l]++;
            if( r < Delta.size() ) Delta[r]--;
        }
        void extend( void ) {
            lint i = Delta.size();
            lint s = 0;
            for( lint a = 0; a < Delta.size(); ++a ) s += Delta[a];

            Delta.insert(Delta.end(), Delta.begin(), Delta.end()); // duplicate Delta

            Delta[i] -= s;
        }
        pair<lint,lint> argmin( boolt print_all=false ) {
            lint a_ = 0;
            lint v_ = Delta[0];
            lint v = Delta[0];

            if( print_all ) cout << v << endl;

            for( lint a = 1; a < Delta.size(); ++a ) {
                v += Delta[a];
                if( v_ > v ) {
                    a_ = a;
                    v_ = v;
                }

                if( print_all ) cout << v << endl;
            }
            return make_pair(a_,v_);
        }

        private:
        vector<lint> Delta;
    };

    // O(Nlog^2u)-time algorithm
    struct SegmentTree {
        void initialize( void ) {
            root = reallocate_node(-1,-1,-1);
            height = 1;
        }

        void add( lint l, lint r ) {
            root = increment( root, l, r, height );
        }

        void extend( void ) {
            root = reallocate_node(root,root,root);
            node[root].val = 0;
            height = height+1;
        }

        pair<lint,lint> argmin( boolt print_all=false ) {
            if( print_all ) {
                for( lint a = 0; a < (1<<(height-1)); ++a ) {
                    cout << get_value(root,a,height) << endl;
                }
            }

            // (argmin,min) can be obtained from the root node in O(1) 
            return make_pair( node[root].argmin, node[root].min );
        }

        private:
        lint root;
        lint height;

        struct node_t {
            lint left;
            lint right;

            lint min;
            lint argmin;
            lint val;

            lint ref;
        };
        vector<node_t> node;

        lint reallocate_node( lint u, lint w_L, lint w_R ) {
            lint v = node.size();
            node_t w;
            if( u != -1 ) {
                node[u].ref--;
                w.min    = node[u].min;
                w.argmin = node[u].argmin;
                w.val    = node[u].val;
            } else {
                w.val = w.min = w.argmin = 0;
            }
            w.left  = w_L;
            w.right = w_R;
            if( w_L != -1 ) node[w_L].ref++;
            if( w_R != -1 ) node[w_R].ref++;
            w.ref = 1;
            node.push_back( w );
            return v;
        }

        lint increment( lint v, lint l, lint r, lint h ) {
            if( l >= r ) return v;
            if( node[v].ref > 1 ) {
                v = reallocate_node( v, node[v].left, node[v].right );
            }
            if( r - l == (1<<(h-1)) ) {
                node[v].val++;
                node[v].min++;
                return v;
            }

            node[v].left  = increment( node[v].left ,      l                , min( r           , 1<<(h-2)), h-1 );
            node[v].right = increment( node[v].right, max( l-(1<<(h-2)), 0 ),      r-(1<<(h-2))           , h-1 );
            if( node[ node[v].left ].min <= node[ node[v].right ].min ) {
                node[v].min    = node[ node[v].left ].min   + node[v].val;
                node[v].argmin = node[ node[v].left ].argmin;
            } else {
                node[v].min    = node[ node[v].right ].min    + node[v].val;
                node[v].argmin = node[ node[v].right ].argmin + (1<<(h-2));
            }
            return v;
        }

        // function for obtaining a specified value
        lint get_value( lint v, lint a, lint h ) {
            if( h == 1 ) {
                return node[v].val;
            }
            if( a < (1<<(h-2)) ) {
                return get_value( node[v].left , a           , h-1 );
            } else {
                return get_value( node[v].right, a-(1<<(h-2)), h-1 );
            }
        }
    };

    // util functions
    vector<vector<lint> > load( istream& in ) {
        vector<vector<lint> > seqs;
        string line;
        while( getline( in, line ) ) {
            stringstream ss( line );
            vector<lint> seq;
            lint v;
            while( ss >> v ) {
                seq.push_back(v);
            }
            sort( seq.begin(), seq.end() );
            if( !seq.empty() ) seqs.push_back(seq);
        }
        return seqs;
    }

    lint get_log_universe( const vector< vector<lint> >& seqs ) {
        lint m = 0;
        for( lint i = 0; i < seqs.size(); ++i ) {
            for( lint j = 0; j < seqs[i].size(); ++j ) {
                if( m < seqs[i][j] ) m = seqs[i][j];
            }
        }
        lint logu = 0;
        while( (1<<logu) <= m ) {
            ++logu;
        }
        return logu;
    }

    vector< pair<lint,lint> > convert_into_pairs( const vector< vector<lint> >& seqs, lint u ) {
        vector< pair<lint,lint> > pairs;
        for( lint i = 0; i < seqs.size(); ++i ) {
            for( lint j = 1; j < seqs[i].size(); ++j ) {
                pairs.push_back( make_pair( seqs[i][j-1], seqs[i][j] ) );
            }
            pairs.push_back( make_pair( seqs[i][seqs[i].size()-1], seqs[i][0]+u ) );
        }
        return pairs;
    }

    vector< pair<lint,lint> > get_subintervals( const pair<lint,lint>& p, lint k, lint u ) {
        lint x = p.first;
        lint y = p.second;

        vector< pair<lint,lint> > ret;
        if( x >= y ) return ret;

        lint l = ( 2*u - p.second ) % (1<<(k-1));
        lint r = l + (y-x);
        
        if( y - x >= (1<<(k-1)) ) {
            ret.push_back( make_pair( 0, (1<<(k-1)) ) );
        } else if( r <= (1<<(k-1)) ) {
            ret.push_back( make_pair( l, r ) );
        } else {
            ret.push_back( make_pair( 0, r-(1<<(k-1)) ) );
            ret.push_back( make_pair( l, (1<<(k-1)) ) );
        }   
        return ret;
    }

    lint get_best_shift(){ return best_shift; }
    lint get_trie_measure(){ return best_cost; }
    lint get_universe(){ return U; }
    lint no_sets(){ return S.size(); }

private:
    lint best_shift, best_cost;
    lint U;
    vector< vector<lint> > S; /* input sets */
};

#endif
