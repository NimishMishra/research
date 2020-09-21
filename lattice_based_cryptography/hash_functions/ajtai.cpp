// g++ -I/usr/local/include/flint/ ajtai.cpp -lflint -lmpfr -lgmp -lpthread

// Ajai's hash function ---> given a modular matrix A, the message x is chosen such that
// the second norm is less than n.

// Parameters: n log q < m <= q/2n^{4}. q = O(n^{c}) for some positive constant c.


// Average: 4377.59 msec over 100 iters.

// The function: Ax (mod q)

#include <iostream>
using namespace std;
#include "fmpz_mat.h"
#include "flint.h"
#include "nmod_mat.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "ulong_extras.h"
#include <chrono>

// parameters: n is the security parameter.
long n = 500;
long m = pow(n, 2);
long q = pow(n, 7);

void main_hashing_algorithm(flint_rand_t matrix_random_state, flint_rand_t vector_random_state){

    // preventing any print statement to get accurate time results

    // Modular matrix generation
    nmod_mat_t reduced_matrix;
    mp_limb_t modulus = q;
    nmod_mat_init(reduced_matrix, n, m, modulus);
    nmod_mat_randtest(reduced_matrix, matrix_random_state);
    //nmod_mat_print_pretty(reduced_matrix);
    // cout << "\nModular matrix generated\n";

    // Binary vector generation

    nmod_mat_t vector;
    mp_limb_t vector_modulus = 2;
    nmod_mat_init(vector, m, 1, vector_modulus);
    nmod_mat_randtest(vector, vector_random_state);
    //nmod_mat_print_pretty(vector);
    // cout << "\nBinary vector generated";

    // multiplication
    nmod_mat_t answer_vector;
    mp_limb_t answer_modulus = q;
    nmod_mat_init(answer_vector, n, 1, answer_modulus);
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    nmod_mat_mul(answer_vector, reduced_matrix, vector);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << endl;
    // nmod_mat_print_pretty(answer_vector);


    nmod_mat_clear(answer_vector);
    nmod_mat_clear(reduced_matrix);
    nmod_mat_clear(vector);
}

int main(){
    flint_rand_t matrix_random_state;
    flint_randinit(matrix_random_state);
    flint_rand_t vector_random_state;
    flint_randinit(vector_random_state);

    // setting q to be the prime of order it was set earlier
    flint_rand_t prime_random_state;
    flint_randinit(prime_random_state);
    int steps = rand() % 30;
    long prime_q;
    for(int i = 0; i < steps; i++){

        prime_q = n_randprime(prime_random_state, log(q), 1); // get a prime of the bits of the order of q
        flint_randclear(prime_random_state);
    }
    q = prime_q;    
    
    long double msec = 0;
    int STEPS = 100;
    for(int i = 0; i < STEPS; i++){
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        main_hashing_algorithm(matrix_random_state, vector_random_state);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        double time =  std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        cout << i << " - " << time << endl;
        msec += time;
    }
    cout << "Average msec: " << (msec/STEPS) << endl;
    flint_randclear(matrix_random_state);
    flint_randclear(vector_random_state);
    return 0;
}