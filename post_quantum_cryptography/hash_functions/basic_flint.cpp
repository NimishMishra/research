// g++ -I/usr/local/include/flint/ basic_fint.cpp -lflint -lmpfr -lgmp -lpthread

#include <iostream>
using namespace std;
#include "fmpz_mat.h"
#include "flint.h"
#include "nmod_mat.h"
#include <stdlib.h>
#include <time.h>

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
    
    return 0;
}