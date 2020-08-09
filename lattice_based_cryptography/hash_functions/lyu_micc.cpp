// g++ -I/usr/local/include/flint/ lyu_micc.cpp -lflint -lmpfr -lgmp -lpthread
// Generalized compact knapsacks are collision resistant

/*
    Concrete parameterization:
    * f = x^256 + 1
    * p = 2^25
    
    

    * Time in milliseconds: 20072 msec
*/

#include <iostream>
#include "nmod_poly.h"
#include "flint.h"
#include <math.h>
using namespace std;


int n = 257;
long double p = pow(2, 25);
void main_hashing_algorithm(flint_rand_t random_f_state, flint_rand_t random_key_state, flint_rand_t random_sample_state){

    // creating a monic, irreducible f: x^n + 1 that works in most cases
    
    cout << "Value of p: " << p << endl;

    nmod_poly_t f;
    nmod_poly_init(f, n); 

    nmod_poly_randtest(f, random_f_state, n);

    for(int i = 0; i < n; i++){
        if(i == 0 || i == n-1){
            nmod_poly_set_coeff_ui(f, i, 1);
        }
        else{
            nmod_poly_set_coeff_ui(f, i, 0);
        }
    }
    
    cout << "f initialized..." << endl;

    nmod_poly_t key;
    nmod_poly_t reduced_key;
    nmod_poly_init(key, p);
    nmod_poly_init(reduced_key, p);
    nmod_poly_randtest(key, random_key_state, p);
    cout << "Key sampled from ring..." << endl;
    nmod_poly_rem(reduced_key, key, f);
    cout << "Key reduced wrt f..." << endl;

    // nmod_poly_print_pretty(reduced_key, "x");
    // cout << "\n\n\n\n";

    // sampling from D
    nmod_poly_t sample;
    nmod_poly_t reduced_sample;
    nmod_poly_init(sample, p);
    nmod_poly_init(reduced_sample, p);
    nmod_poly_randtest(sample, random_sample_state, p);
    cout << "Message polynomial sampled from D..." << endl;
    nmod_poly_rem(reduced_sample, sample, f);
    cout << "Message polynomial reduced wrt f..." << endl;

    // nmod_poly_print_pretty(reduced_sample, "x");

    // multiplying
    nmod_poly_t output;
    nmod_poly_init(output, p);
    nmod_poly_randtest(output, random_sample_state, p);
    cout << "Multiplying..." << endl;
    std::chrono::steady_clock::time_point total_begin = std::chrono::steady_clock::now();
    for(int i = 0; i < n; i++){
        long double key_coeff = nmod_poly_get_coeff_ui(reduced_key, i);
        long double sample_coeff = nmod_poly_get_coeff_ui(reduced_sample, i);
        nmod_poly_set_coeff_ui(output, i, key_coeff * sample_coeff);
    }
    std::chrono::steady_clock::time_point total_end = std::chrono::steady_clock::now();
    cout << std::chrono::duration_cast<std::chrono::nanoseconds>(total_end - total_begin).count() << endl;
    cout << "Reducing multiplied polynomial..." << endl;
    nmod_poly_rem(output, output, f);

    // nmod_poly_print_pretty(output, "x");
    
    nmod_poly_clear(f);
    nmod_poly_clear(key);
    nmod_poly_clear(reduced_key);
    nmod_poly_clear(reduced_sample);
    nmod_poly_clear(sample);
}

int main(){

    flint_rand_t random_f_state;
    flint_randinit(random_f_state);
    flint_rand_t random_key_state;
    flint_randinit(random_key_state);
    flint_rand_t random_sample_state;
    flint_randinit(random_sample_state);

    double time;
    double total_msec = 0;
    int STEPS = 100;
    for(int i = 0; i < STEPS; i++){
        std::chrono::steady_clock::time_point total_begin = std::chrono::steady_clock::now();
        main_hashing_algorithm(random_f_state, random_key_state, random_sample_state);
        std::chrono::steady_clock::time_point total_end = std::chrono::steady_clock::now();
        time = std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_begin).count();
        cout << i << " took time in msec: " <<  time << endl;
        total_msec += time;
    }
    cout << "total time in msec: " << (total_msec/STEPS) << endl;
    
    flint_randclear(random_f_state);
    flint_randclear(random_key_state);
    flint_randclear(random_sample_state);
    return 1;
}