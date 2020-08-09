// g++ -I/usr/local/include/flint regev.cpp -lflint -lpthread -lmpfr -lgmp

/*
    * Security parameter n
    * N = 2^(8*n*n)
    * m = cm*n*n (collisions exist for cm > 8)
    * Choose the m length key from {0, 1, 2, 3, 4, ...., N - 1}
    * The message to be hashed is the binary vector {0, 1}.
    * Calculate the modular product; it lies in {0, 1, 2, 3, 4, ...., N - 1}



    Average time in nanoseconds averaged over 100 iterations: 20565.8 nsec
    (probably due to the fact that there is no matrix multiplication involved here...)
*/


#include <iostream>
#include "nmod_mat.h"
#include <chrono>
#include <time.h>
#include <math.h>
using namespace std;

// generating the parameters
double n = 2.80624304008;
int c_m = 9;
long double N = pow(2, 8*n*n);
long m = 9 * n * n;

double hashing_time = 0;
void main_hashing_algorithm(flint_rand_t random_state, flint_rand_t message_random_state){
    nmod_mat_t key;
    nmod_mat_t message;
    nmod_mat_t output;
    mp_limb_t modulus = N;
    nmod_mat_init(key, 1, m, modulus);
    nmod_mat_init(message, m, 1, 2);
    nmod_mat_init(output, 1, 1, modulus);

    nmod_mat_randtest(key, random_state); 
    nmod_mat_randtest(message, message_random_state);    
    std::chrono::steady_clock::time_point total_begin = std::chrono::steady_clock::now();
    nmod_mat_mul(output, key, message);
    std::chrono::steady_clock::time_point total_end = std::chrono::steady_clock::now();
    hashing_time += std::chrono::duration_cast<std::chrono::nanoseconds>(total_end - total_begin).count();
    nmod_mat_clear(key);
    nmod_mat_clear(message);
    nmod_mat_clear(output);
}

int main(){
        
    // generating the key

    flint_rand_t random_state;
    flint_rand_t message_random_state;
    flint_randinit(random_state);
    flint_randinit(message_random_state);
   
    long double total_msec = 0;
    double time;
    int STEPS = 100;
    for(int i = 0; i < STEPS; i++){
        std::chrono::steady_clock::time_point total_begin = std::chrono::steady_clock::now();
        main_hashing_algorithm(random_state, message_random_state);
        std::chrono::steady_clock::time_point total_end = std::chrono::steady_clock::now();
        time = std::chrono::duration_cast<std::chrono::nanoseconds>(total_end - total_begin).count();
        cout << i << " took time in nsec: " <<  time << endl;
        total_msec += time;
    }
    
    cout << "Average time in nsec: " << total_msec/STEPS << endl;
    cout << "Average time for hashing: " << hashing_time/STEPS << endl;

    flint_randclear(random_state);
    flint_randclear(message_random_state);
}