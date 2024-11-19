// Copyright (c) 2024, REGINDEX, Jarno Alanko and Travis Gagie.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

#include "internal/common.hpp"
#include "internal/trieMeasure.hpp"
#include "internal/optimalEncoding.hpp"

// function that prints the instructions for using the tool
void print_help(char** argv) { 
    cout << endl <<
        "Usage: " << argv[0] << " [options] [< input]" << endl 

        << "Tool to compute the trie measure for collections of sets." << endl << endl

        << "    -g, --grammar-based" << endl 
        << "        Run the grammar-based algorithm (more efficient when the universe is large), Default: False" << endl 

        << "    -o, --optimal-ordered-encoding" << endl 
        << "        Compute the optimal ordered encoding size, Default: False" << endl 

        << "    -s, --optimal-shifted-ordered-encoding" << endl 
        << "        Compute the size of the optimal shifted ordered encoding, Default: False" << endl 

        << "    -t, --get-encoding" << endl 
        << "        Get the shifted encoding or the binary tree for the ordered prefix-free encoding, Default: False" << endl 

        << "    -a, --all-shifts" << endl 
        << "        Compute the trie measure for all shifts in the input's universe, Default: False" << endl 
        << endl;
}

// function for parsing the input arguments
void parseArgs(int argc, char** argv, Args& arg) {

    if(argc < 1){ print_help(argv); exit(1); }

    // read and parse input parameters
    for(size_t i=1;i<argc;++i)
    {
        string param = argv[i];

        if( param == "-g" or param == "--grammar-based" )
        {
            arg.grammar = true;
        }
        else if( param == "-o" or param == "--optimal-ordered-encoding" )
        {
            arg.optimalOrdered = true;
        }
        else if( param == "-s" or param == "--optimal-shifted-ordered-encoding" )
        {
            arg.optimalOrderedShifted = true;
        }
        else if( param == "-t" or param == "--get-encoding" )
        {
            arg.getEncoding = true;
        }
        else if( param == "-a" or param == "--all-shifts" )
        {
            arg.all = true;
        }
        else if( param == "-h" or param == "--help")
        {
                print_help(argv); exit(-1);
        }
        else if( param == "<")
        {   
            break; // skip
        }
        else
        {
            cerr << "Unknown option. Use -h for help." << endl;
            exit(-1);
        }
    }

    if( (arg.grammar and arg.optimalOrdered) or (arg.grammar and arg.optimalOrderedShifted) ){ cerr << "Select either -g or -o/-s flag! exiting..." << endl; exit(1); }
    if( arg.optimalOrdered and arg.optimalOrderedShifted ){ arg.optimalOrderedShifted = true; arg.optimalOrdered = false; cout << "-s flag overriding -o flag" << endl; }
    if( arg.all ){ arg.grammar = true; cout << "forcing grammar-based algorithm" << endl; }
}

/* main */
int main(int argc, char* argv[]){

    Args arg;
    parseArgs(argc, argv, arg);

    if(arg.optimalOrdered or arg.optimalOrderedShifted)
    {
        // load input sets
        optimalOrderedEncoding opt(cin);

        if(arg.optimalOrdered)
        {
            cout << "estimating optimal ordered encoding size..." << endl;
            cout << "no. of sets = " << opt.no_sets() << endl;
            if(arg.getEncoding)
                cout << "prefix-free encoding tree = " << opt.get_encoding_tree() << endl;
            else
                cout << "optimal ordered encoding size = " << opt.encoding_size() << endl;
        }
        else
        {
            cout << "estimating optimal shifted ordered encoding size..." << endl;
            cout << "no. of sets = " << opt.no_sets() << endl;
            if(arg.getEncoding)
                cout << "prefix-free encoding tree = " << opt.best_shift_encoding_tree() << endl;
            else
                cout << "optimal shifted ordered encoding size = " << opt.best_shift_encoding_size() << endl;
        }
    }
    else
    {
        cout << "estimating trie measure..." << endl;
        trieEncoding enc(cin);
        enc.compute_best_shift(arg.grammar,arg.all);

        cout << "no. of sets = " << enc.no_sets() << endl;
        if(arg.all){ /*enc.print_all_costs();*/ }
        else
        {
            if(arg.getEncoding)
                cout << "shifted tree encoding: (a = " << enc.get_best_shift() << ",u = " << enc.get_universe() << ")" << endl;
            else 
                cout << "shifted trie encoding size = " << enc.get_trie_measure() << endl;
        }
    }

    return 0;
}
