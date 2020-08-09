// g++ -I/usr/local/include/flint/ micc.cpp -lflint -lmpfr -lgmp -lpthread


// implementing Improved cryptographic hash functions with worst-case/average-case connection

// .....


/*  
    * Full rank lattice to begin with
    * Check if that is "almost perfect": certain relation on the ratio of covering radius and shortest vector
        * Get the shortest vector: Lemma 2.8 and Theorem 10 in Complexity of Lattice Based
            Problems: A Cryptographic Perspective
        * Get the covering radius (no known algorithms to achieve less than exponential approximation factors). 
            We will use the general notion of covering radius as given on Pg. 137 of the same book.
    *  Forming the group G will be done by y = x mod B as defined in "Cryptographic functions from worst-case complexity assumptions" by Daniel Micciancio
    * No big deal after that. Sample the key and begin hashing.



    * Defintiion of fmpz_lll_t from fmpz_lll.h
           typedef struct
            {
                double delta;
                double eta;
                rep_type rt;
                gram_type gt;
            } fmpz_lll_struct;
*/


// Average time over 100 iterations: 113581.16129032258 msec
// Average time over 100 iterations (hashing): 106293.06451612903 msec

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
#include "fmpz_lll.h"
#include "fmpq.h"

// parameters: n is the security parameter.
long n = 50;
long m = 100;
long q = pow(n, 7);
double main_time = 0;

void LLL_reduction(fmpz_mat_t* lattice){
    // delta = 1; eta= 0.5
    // double delta = (1/4) + powl(0.75, n/(n-1)); <--- nearly gives 0.75
    if(!fmpz_mat_is_reduced(*lattice, 1, 0.5)){
        // if the matrix is not LLL reduced
        fmpq_t delta;
        delta->num = 1;
        delta->den = 1;
        fmpq_t eta;
        eta->num = 1;
        eta->den = 2;        
        fmpz_mat_lll_original(*lattice, delta, eta);
        if(!fmpz_mat_is_reduced(*lattice, 1, 0.5)){
            cout << "\nLattice can't be LLL reduced\n";
            exit(EXIT_SUCCESS);
        }
    }
}

void get_shortest_vector(fmpz_mat_t* lattice, fmpz_mat_t* shortest_vector){

    // getting the shortest vector by constructing a unit vector [1 0 0 0 ......] 
    // and carrying out a multiplication to extract the first column of the LLL
    // reduced basis
    
    fmpz_mat_t unit_vector;
    fmpz_mat_init(unit_vector, n, 1);
    fmpz_mat_one(unit_vector);
    fmpz_mat_mul(*shortest_vector, *lattice, unit_vector);
    fmpz_mat_clear(unit_vector);
}

double get_shortest_vector_length(fmpz_mat_t* shortest_vector){
    // simply do the inner product with itself and square root the result

    fmpz_mat_t scalar;
    fmpz_mat_init(scalar, 1, 1);
    fmpz_mat_t shortest_vector_transpose;
    fmpz_mat_init(shortest_vector_transpose, 1, n);
    fmpz_mat_transpose(shortest_vector_transpose, *shortest_vector);
    fmpz_mat_mul(scalar, shortest_vector_transpose, *shortest_vector);
    double norm = scalar->rows[0][0];
    fmpz_mat_clear(shortest_vector_transpose);
    fmpz_mat_clear(scalar);
    return sqrt(norm);
}

void approximate_closest_vector(fmpz_mat_t* lattice, fmpz_mat_t* target_vector, fmpz_mat_t* result){
    // Refer Pg. 40 on Complexity of Lattice problems: A Cryptographic Perspective

    // LLL reduction has already taken place so no need to rerun that. Just include a simple check

    if(fmpz_mat_is_reduced(*lattice, 1, 0.5)){
        // if the lattice is LLL reduced according to specs

        // the idea is to create multipliers with 1's at specific positions that lets us 
        // extract columns of the lattice by simple matrix-vector multiplications


        // setting up the transpose of the output
        fmpz_mat_t output;
        fmpz_mat_init(output, n, 1);
        fmpz_mat_set(output, *target_vector); // b = t
        fmpz_mat_t output_transpose;
        fmpz_mat_init(output_transpose, 1, n);
        

        fmpz_mat_t multiplier;  // helps us to extract columns of basis for processing
        fmpz_mat_t lattice_column; // the column extracted after the multiplier step
        fmpz_mat_t lattice_column_transpose; // to calculate the transpose
        fmpz_mat_t multiplication_output; // the output of various inner products
        fmpz_mat_init(multiplication_output, 1, 1);
        fmpz_mat_init(multiplier, n, 1);
        fmpz_mat_init(lattice_column, n, 1);
        fmpz_mat_init(lattice_column_transpose, 1, n);
        
        for(int j = n-1; j >= 0; j--){
            fmpz_mat_transpose(output_transpose, output);  // finding <b|

            // extracting the lattice column b_j by setting the appropriate multiplier
            fmpz* entry = fmpz_mat_entry(multiplier, j, 0);
            *entry = 1;
            fmpz_mat_mul(lattice_column, *lattice, multiplier);
            fmpz_mat_transpose(lattice_column_transpose, lattice_column);

            // main algorithm ------
            fmpz_mat_mul(multiplication_output, output_transpose, lattice_column);
            double numerator = multiplication_output->rows[0][0];  // <b, b_j>
            fmpz_mat_mul(multiplication_output, lattice_column_transpose, lattice_column);
            double denominator = multiplication_output->rows[0][0]; // <b_j, b_j>
            double c = numerator/denominator;
            fmpz_mat_scalar_mul_si(lattice_column, lattice_column, round(c));
            fmpz_mat_sub(output, output, lattice_column); // b = b - c_j.b_j
            // main algorithm ------
            *entry = 0;
        }

        fmpz_mat_sub(*result, *target_vector, output); // return t - b
        

        fmpz_mat_clear(multiplier);
        fmpz_mat_clear(lattice_column);
        fmpz_mat_clear(lattice_column_transpose);
        fmpz_mat_clear(multiplication_output);
        fmpz_mat_clear(output);
        fmpz_mat_clear(output_transpose);
    }

    else{

        cout << "Closest vector problem can't be sufficiently solved. Basis is not LLL reduced\n";
        exit(EXIT_SUCCESS);
    }
}

double choose_target(fmpz_mat_t* lattice){

    // setting up the scaling vector <---- set up a unit vector 
    fmpz_mat_t unit_scaling_vector;
    fmpz_mat_init(unit_scaling_vector, n, 1);
    
    for(int i = 0; i < n; i++){
        fmpz* entry = fmpz_mat_entry(unit_scaling_vector, i, 0);
        *entry = 1;
    }
    
    // trying to find a 'deep hole' by scaling all basis vectors equally until a point is reached
    // continuing after which we have diminishing returns
    fmpz_mat_t target_vector; // setting up the target
    fmpz_mat_t closest_vector_result;
    fmpz_mat_t closest_vector_transpose;
    fmpz_mat_t closest_vector_norm;
    fmpz_mat_init(closest_vector_result, n, 1);
    fmpz_mat_init(closest_vector_transpose, 1, n);
    fmpz_mat_init(closest_vector_norm, 1, 1);
    fmpz_mat_init(target_vector, n, 1);
    double norm;
    double prev_norm;
    
    for(int i = 0; ; i++){
        fmpz_mat_add(target_vector, target_vector, unit_scaling_vector);    
        approximate_closest_vector(lattice, &target_vector, &closest_vector_result); // get the approximated closest vector
        fmpz_mat_sub(closest_vector_result, closest_vector_result, target_vector); // get the difference vector from the lattice
        // get the norm of the difference vector
        fmpz_mat_transpose(closest_vector_transpose, closest_vector_result);
        fmpz_mat_mul(closest_vector_norm, closest_vector_transpose, closest_vector_result);
        prev_norm = norm;
        norm = sqrt(closest_vector_norm->rows[0][0]);
        if((prev_norm > norm || i % 5 == 0) && i != 0){
            return norm;    // if the present norm is less than the previous one, we have encountered a maxima. Break then.
        }
    }


    // clearing memory
    fmpz_mat_clear(unit_scaling_vector);
    fmpz_mat_clear(closest_vector_norm);
    fmpz_mat_clear(closest_vector_result);
    fmpz_mat_clear(closest_vector_transpose);
    fmpz_mat_clear(target_vector);
}

double get_covering_radius(fmpz_mat_t* lattice){
    
    double covering_radius = choose_target(lattice);
    return covering_radius;
}


void main_hashing_algorithm(flint_rand_t matrix_random_state, flint_rand_t vector_random_state, flint_rand_t random_matrix_random_state){

    // preventing any print statement to get accurate time results

    // Full rank matrix generation...
    fmpz_mat_t initial_lattice;
    fmpz_mat_init(initial_lattice, n, n);
    fmpz_mat_randtest(initial_lattice, matrix_random_state, 10); // beginning with 10 bits
    // fmpz_mat_print_pretty(initial_lattice);
    cout << "\nFull rank matrix generated...\n";
    
    // extract shortest vector length (approximation)

    LLL_reduction(&initial_lattice);  // LLL reduce the basis
    cout << "LLL reduction done...\n";
    fmpz_mat_t shortest_vector;
    fmpz_mat_init(shortest_vector, n, 1);
    get_shortest_vector(&initial_lattice, &shortest_vector); // get the shortest vector
    double shortest_vector_length = get_shortest_vector_length(&shortest_vector); // get the shortest vector length
    cout << "Shortest vector length obtained...\n";

    // extract the covering radius (approximation)
    double covering_radius = get_covering_radius(&initial_lattice);
    cout << "Covering radius length obtained...\n";
    if((2*covering_radius)/shortest_vector_length < sqrt(n)){
        cout << "Lattice is 'almost' perfect. Moving on...\n";
    }

    else{
        cout << "Lattice is not 'almost' perfect. Exiting...\n";
        return;
    }

    // Creating the lattice M = (alpha).(covering_radius).I  +  R
    fmpz_mat_t M;
    fmpz_mat_init(M, n, n);
    fmpz_mat_t I;
    fmpz_mat_init(I, n, n);
    fmpz_mat_t R;
    fmpz_mat_init(R, n, n);
    fmpz_mat_one(I);
    double alpha = pow(n, 1.5)*log(n);
    fmpz_mat_scalar_mul_si(I, I, alpha * covering_radius);
    fmpz_mat_randtest(R, random_matrix_random_state, 10);
    fmpz_mat_add(M, I, R);
    LLL_reduction(&M);

    /* forming the group G: y = x mod M
        * Create a random integer vector
        * reduce it wrt M
    */
   cout << "Hashing now...\n";
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    fmpz_mat_t key;
    fmpz_mat_t closest_lattice_vector;
    fmpz_mat_t reduced_vector;
    fmpz_mat_t message_vector;
    fmpz_mat_t hash_result;
    for(int i = 0; i < m; i++){

        fmpz_mat_init(key, n, 1);
        fmpz_mat_init(reduced_vector, n, 1);
        fmpz_mat_init(closest_lattice_vector, n, 1);
    
        fmpz_mat_randtest(key, vector_random_state, 10);
        approximate_closest_vector(&M, &key, &closest_lattice_vector);
        fmpz_mat_sub(reduced_vector, key, closest_lattice_vector);

    // forming the message binary vector
        fmpz_mat_init(message_vector, 1, n);
        fmpz_mat_randtest(message_vector, vector_random_state, 1); // signed vector, need to make it unsigned
        for(int i = 0; i < n; i++){
            fmpz* entry = fmpz_mat_entry(message_vector, 0, i);
            if(*entry == -1)
                *entry = 1;
        }        
        fmpz_mat_init(hash_result, 1, 1);
        fmpz_mat_mul(hash_result, message_vector, key);
        fmpz* hash = fmpz_mat_entry(hash_result, 0, 0);
        // cout << *hash << endl;  // <----- prints out the m length hash output
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    double hashing_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    fmpz_mat_clear(initial_lattice);
    fmpz_mat_clear(shortest_vector);
    fmpz_mat_clear(M);
    fmpz_mat_clear(I);
    fmpz_mat_clear(R);
    fmpz_mat_clear(reduced_vector);
    fmpz_mat_clear(key);
    fmpz_mat_clear(message_vector);
    fmpz_mat_clear(hash_result);
    cout << "Hashing done...\n";
    cout << "Hashing time: " << hashing_time << endl;
    main_time += hashing_time;
}

int main(){
    flint_rand_t matrix_random_state;
    flint_randinit(matrix_random_state);
    flint_rand_t vector_random_state;
    flint_randinit(vector_random_state);
    flint_rand_t random_matrix_random_state;
    flint_randinit(random_matrix_random_state);

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
        cout << "STEP " << i << endl;
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        main_hashing_algorithm(matrix_random_state, vector_random_state, random_matrix_random_state);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        double time =  std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        cout << "Overall time: " << time << endl;
        msec += time;
    }
    cout << "Average msec: " << (msec/STEPS) << endl;
    cout << "Main algorithm msec: " << (main_time/STEPS) << endl;
    flint_randclear(matrix_random_state);
    flint_randclear(vector_random_state);
    flint_randclear(random_matrix_random_state);
    return 0;
}