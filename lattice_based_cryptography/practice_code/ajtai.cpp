// g++ -I/usr/local/include/flint/ ajtai.cpp -lflint -lmpfr -lgmp -lpthread

#include <iostream>
using namespace std;
#include "fmpz_mat.h"
#include "flint.h"
#include "nmod_mat.h"


long n = 100;
long m = 100;
long q = 97;

int main(){

    // Matrix initialisation
    fmpz_mat_t initial_matrix;
    fmpz_mat_init(initial_matrix, n, m);
    // fmpz_mat_print_pretty(initial_matrix);
    cout << "\nMatrix intialization done\n";

    // Initialize random state

    flint_rand_t random_state;
    flint_randinit(random_state);
    cout << "\nRandom state initialized\n";

    // Random matrix generation

    flint_bitcnt_t bits = 10;
    fmpz_mat_randbits(initial_matrix, random_state, bits);
    // fmpz_mat_print_pretty(initial_matrix);
    cout << "\nRandom matrix generated\n";

    // Modular reduction

    nmod_mat_t reduced_matrix;
    mp_limb_t modulus = q;
    nmod_mat_init(reduced_matrix, n, m, modulus);
    fmpz_mat_get_nmod_mat(reduced_matrix, initial_matrix);
    // nmod_mat_print_pretty(reduced_matrix);
    cout << "\nModular matrix generated\n";

    // Binary vector generation

    nmod_mat_t vector;
    mp_limb_t vector_modulus = 2;
    nmod_mat_init(vector, m, 1, vector_modulus);
    nmod_mat_randtest(vector, random_state);
    // nmod_mat_print_pretty(vector);
    cout << "\nBinary vector generated";

    return 0;
}