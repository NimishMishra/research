// g++ -I/usr/local/include/flint/ basic_tests.cpp -lflint -lmpfr -lgmp -lpthread


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

long q = 500;
long n = 50;
long l = 10;

int main(){


    nmod_mat_t mu;
    nmod_mat_t beta;
    nmod_mat_t E1;
    nmod_mat_t answer;
    mp_limb_t modulus = q;
    flint_rand_t random_state;
    flint_randinit(random_state);

    // creating mu
    nmod_mat_init(mu, 1, n, modulus);
    nmod_mat_randtest(mu, random_state);
    //nmod_mat_print_pretty(mu);

    // creating beta
    nmod_mat_init(beta, n, n, modulus);
    nmod_mat_randtest(beta, random_state);
    cout << endl;
    //nmod_mat_print_pretty(beta);

    // creating E1 (currently random)
    nmod_mat_init(E1, n, l, modulus);
    nmod_mat_randtest(E1, random_state);
    // nmod_mat_print_pretty(E1);

    // perform ((mu.beta)/q).E1
    nmod_mat_init(answer, 1, l, modulus);
    nmod_mat_mul(mu, mu, beta);
    nmod_mat_print_pretty(mu);
    //nmod_mat_scalar_mul(mu, mu, 1/q);
    // nmod_mat_print_pretty(mu);
    nmod_mat_mul(answer, mu, E1);
    nmod_mat_print_pretty(answer);

    nmod_mat_clear(mu);
    nmod_mat_clear(beta);
    nmod_mat_clear(E1);
    nmod_mat_clear(answer);
    flint_randclear(random_state);
}