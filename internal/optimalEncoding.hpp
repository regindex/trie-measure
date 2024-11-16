#ifndef OPT_ENCODING_HPP_
#define OPT_ENCODING_HPP_

#include "common.hpp"
  
class optimalOrderedEncoding
{
public:
    optimalOrderedEncoding(){};
    ~optimalOrderedEncoding(){};

    optimalOrderedEncoding(istream& input): U(0)
    {
        /* input sets */
        vector< vector<ulint> > S = load( cin );
        this->n = S.size();
        //cout << "U size= " << U << endl;

        /* computing the A_{x} sets */
        A = compute_A(S);
        // print_matrix(A,"A");
    }

    ulint encoding_size()
    {
        /* computing the constants for the dynamic programming algorithm */
        vector< vector<lint> > T = compute_T(this->A, this->n);
        //print_matrix(T,"T");
        /* computing the size of the optimal ordered encoding */
        ulint encoding_size = compute_optimal_ordered_encoding_size_(T);

        return encoding_size;
    }

    string get_encoding_tree()
    {
        /* computing the constants for the dynamic programming algorithm */
        vector< vector<lint> > T = compute_T(this->A, this->n);
        //print_matrix(T,"T");
        /* computing the optimal prefix-free ordered encoding represented
           as a binary tree */
        string binTree = compute_optimal_ordered_encoding_(T);

        return binTree;
    }

    ulint best_shift_encoding_size()
    {
        /* double A sets size */
        double_A(this->A);
        //print_matrix(A,"A");
        /* computing the constants for the dynamic programming algorithm */
        vector< vector<lint> > T = compute_T(A, n);
        //print_matrix(T,"T");
        /* computing the size of the optimal ordered encoding */
        ulint encoding_size = compute_optimal_ordered_shifted_encoding_size_(T);

        return encoding_size;
    }

    string best_shift_encoding_tree()
    {
        /* double A sets size */
        double_A(this->A);
        //print_matrix(A,"A");
        /* computing the constants for the dynamic programming algorithm */
        vector< vector<lint> > T = compute_T(A, n);
        //print_matrix(T,"T");
        /* computing the size of the optimal ordered encoding */
        string binTree = compute_optimal_ordered_shifted_encoding_(T);

        return binTree;
    }

    ulint no_sets()
    {
        return n;
    }

private:
    lint U; /* Universe size */
    ulint n;
    vector< vector<ulint> > A;

    void double_A(vector< vector<ulint> >& A)
    {
        for(lint i=0;i<U;++i){ A.push_back(A[i]); }
        U *= 2;
    }

    /* Load input file and compute universe size */
    vector< vector<ulint> > load( istream& in ) {
        // input format: each line contains a set of integers
        // e.g., 
        //     1 7 8
        //     1 8
        vector< vector<ulint> > ret;
        string line;
        while( getline( in, line ) ) {
            stringstream ss( line );
            vector<ulint> s;
            ulint x;
            while( ss >> x ) {
                s.push_back(x);
                if( x < 0 ){ cerr << "ERROR: out of range" << endl; exit(1); }
                if( x > U ){ U = x; }
            }
            if( !s.empty() ) {
                //sort( s.begin(), s.end() );
                ret.push_back(s);
            }
        }
        // compute universe size
        U = pow(2,ceil(log2(U+1)));

        return ret;
    }

    /* function to compute A_{x} vectors: j \in A_{x} iff x \in S_{j} */
    vector< vector<ulint> > compute_A( vector< vector<ulint> >& S )
    {
        // initialize A fragmented matrix
        vector< vector<ulint> > A;
        for(ulint i=0;i<U;++i)
            A.push_back(vector<ulint>{});

        // fill in the matrix
        for(ulint i=0;i<S.size();++i)
            for(ulint j=0;j<S[i].size();++j)
                A[S[i][j]].push_back(i);

        return A;
    }

    template<typename T>
    void print_matrix( const vector< vector<T> >& M, string name)
    {
        cout << "###############" << endl;
        for(ulint i=0;i<M.size();++i)
        {
            cout << name << "_" << i << " = { ";
            for(ulint j=0;j<M[i].size();++j)
                cout << M[i][j] << " ";
            cout << "}\n";    
        }
        cout << "###############" << endl;
    }

    /* compute the matrix containing the constants a_{i,j} for the dynamic programming algorithm
       a_{i,j} = size of the union of the sets S_i,S_{i+1},...,S_{j} */
    vector< vector<lint> > compute_T( vector< vector<ulint> >& A, ulint n )
    {
        vector< vector<lint> > T;

        /* add dummy A_{-1} = A_{u} set */
        vector<ulint> dummy{};
        for(ulint i=0;i<n;++i){ dummy.push_back(i); }
        A.push_back(dummy);

        for(ulint i=0;i<U+1;++i){ T.push_back(vector<lint>(U+1,0)); } /* Init entries to zero, index -1 corresponds to 0*/
        vector<lint> P(n,-1); /* Previous occurrence of each x in A_{-1},...,A_{u-1} */

        /* compute maximal sequences not containing x: A_{i+1},...,A_{j-1} */
        for(lint j=0;j<U+1;++j)
            for(const ulint& x : A[j])
            {
                lint i = P[x];
                P[x] = j;
                if( i < (j-1) )
                {
                    T[j][j] += 1;
                    T[i+1][i+1] += 1;
                    T[i+1][j] -= 1;
                    T[j][i+1] -= 1;
                }
            }

        //print_matrix(T,"T1");

        /* compute partial sums, row wise */
        for(ulint i=0;i<U;++i)
            for(lint j=U-2;j>=0;--j)
                T[i+1][j+1] += T[i+1][j+2];

        /* compute partial sums, column wise */
        for(ulint j=0;j<U;++j)
            for(lint i=U-2;i>=0;--i)
                T[i+1][j+1] += T[i+2][j+1];

        //print_matrix(T,"T2");

        /* replace each T[i,j] with n - T[i,j] */
        for(ulint i=0;i<U+1;++i)
            for(ulint j=0;j<U+1;++j)
                T[i][j] = (n) - T[i][j];

        //print_matrix(T,"T3");

        return T;
    }

    vector< vector<ulint> > dynamic_programming_algorithm(const vector< vector<lint> >& T, ulint U)
    {
        /* init dynamic programming matrix */
        vector< vector<ulint> > D;
        for(ulint i=0;i<U;++i)
            D.push_back(vector<ulint>(U,0));
        /* init matrix diagonal */
        for(ulint i=0;i<U;++i)
            D[i][i] = T[i+1][i+1];

        /* dynamic programming recursion */
        for(ulint j=1;j<U;++j)
            for(lint i=j-1;i>=0;--i)
            {
                ulint k = i + 1;
                ulint Min = D[i][k-1] + D[k][j];
                for(lint k = i + 2; k <= j; ++k)
                {
                    ulint tmp = D[i][k-1] + D[k][j];
                    if(tmp < Min)
                        Min = tmp;
                }
                D[i][j] = T[i+1][j+1] + Min;
            }

        return D;
    }

    ulint compute_optimal_ordered_encoding_size_(const vector< vector<lint> >& T)
    {
        /* init and compute dynamic programming matrix */
        vector< vector<ulint> > D = dynamic_programming_algorithm(T, this->U);

        return D[0][U-1] - T[1][U];     
    }

    string compute_tree(vector< vector<ulint> >& D, ulint U, ulint i, ulint j)
    {
        if(i == j){ return to_string(i%U); }
        else
        {
            ulint c = D[i][i] + D[i+1][j];
            ulint argmin = i+1;
            for(ulint k = i+1;k<=j;++k)
            {
                ulint tmp = D[i][k-1] + D[k][j];
                if(tmp < c){ c = tmp; argmin = k; }
            }

            return "(" + compute_tree(D,U,i,argmin-1) + ")(" + compute_tree(D,U,argmin,j) + ")";
        }
    }

    string compute_optimal_ordered_encoding_(const vector< vector<lint> >& T)
    {
        /* init and compute dynamic programming matrix */
        vector< vector<ulint> > D = dynamic_programming_algorithm(T, this->U);

        string binTree = "";
        binTree = compute_tree(D,U,0,(this->U)-1);

        return binTree;  
    }

    ulint compute_optimal_ordered_shifted_encoding_size_(const vector< vector<lint> >& T)
    {
        /* init and compute dynamic programming matrix */
        vector< vector<ulint> > D = dynamic_programming_algorithm(T, this->U);

        ulint c = D[0][(U/2)-1];
        for(ulint i=1;i<U/2;++i)
        {
            ulint tmp = D[i][i+(U/2)-1];
            if( tmp < c ){ c = tmp; }
        }

        return c - T[1][(U/2)];      
    }

    string compute_optimal_ordered_shifted_encoding_(const vector< vector<lint> >& T)
    {
        /* init and compute dynamic programming matrix */
        vector< vector<ulint> > D = dynamic_programming_algorithm(T, this->U);

        /* find best shifted submatrix */
        ulint c = D[0][(this->U/2)-1];
        ulint argmin = 0;
        for(ulint a = 1;a<(this->U/2);++a)
        {
            ulint tmp = D[a][a+(this->U/2)-1];
            if(tmp < c){ c = tmp; argmin = a; }
        }

        string binTree = "";
        binTree = compute_tree(D,U/2,argmin,argmin+(this->U/2)-1);

        return binTree;   
    }
};

#endif