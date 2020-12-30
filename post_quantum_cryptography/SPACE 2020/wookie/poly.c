#include "params.h"
#include "poly.h"
#include "shake256.h"
#include "randombytes.h"

void poly_tobytes(unsigned char *r, const poly *a)
{
  /* TODO: Implement */ 
}

void poly_frombytes(poly *r, const unsigned char *b)
{
  /* TODO: Implement */
}

void poly_mul(poly *r, const poly *a, const poly *b)
{
  /* TODO: Implement */
}

void poly_add(poly *r, const poly *a, const poly *b)
{
  /* TODO: Implement */
}

void poly_sub(poly *r, const poly *a, const poly *b)
{
  /* TODO: Implement */
}

void poly_getnoise(poly *r)
{
  /* TODO: Implement */
}

void poly_uniform(poly *r, const unsigned char seed[WOOKIE_SYMBYTES])
{
  /* TODO: Implement */
}

void poly_frommsg(poly *r, const unsigned char m[WOOKIE_N/8])
{
  /* TODO: Implement */
}

void poly_tomsg(unsigned char m[WOOKIE_N/8], const poly *a)
{
  /* TODO: Implement */
}

