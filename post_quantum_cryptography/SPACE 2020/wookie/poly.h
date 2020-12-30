#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "params.h"

/* Element of Rq = Z_q[X]/(X^n+1) */
/* coeffs[0] + X*coeffs[1] + X^2*xoeffs[2] + ... + X^(n-1)*xcoeffs[n-1] */
typedef struct{
  uint16_t coeffs[WOOKIE_N];
} poly;

/* Compress a polynomial to a byte array of length WOOKIE_POLYBYTES */
void poly_tobytes(unsigned char r[WOOKIE_POLYBYTES], const poly *a);

/* Decompress a polynomial from a byte array of length WOOKIE_POLYBYTES */
void poly_frombytes(poly *r, const unsigned char b[WOOKIE_POLYBYTES]);

/* Multiply polynomial a by polynomial b, write result to r */
void poly_mul(poly *r, const poly *a, const poly *b);

/* Add polynomial a to polynomial b, write result to r */
void poly_add(poly *r, const poly *a, const poly *b);

/* Subtract polynomial b from polynomial a, write result to r */
void poly_sub(poly *r, const poly *a, const poly *b);

/* Randomly generate a polynomial with centered binomial noise distribution */
void poly_getnoise(poly *r);

/* Generate a uniform polynomial pseudorandomly from a given seed */
void poly_uniform(poly *r, const unsigned char seed[WOOKIE_SYMBYTES]);

/* Extract encoded n-bit message m from (noisy) polynomial a */
void poly_tomsg(unsigned char m[WOOKIE_N/8], const poly *a);

/* Encode n-bit message m into polynomial r */
void poly_frommsg(poly *r, const unsigned char m[WOOKIE_N]);

#endif
