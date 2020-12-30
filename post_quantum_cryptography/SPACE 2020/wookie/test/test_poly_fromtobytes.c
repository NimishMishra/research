#include <stdio.h>
#include "../params.h"
#include "../poly.h"
#include "../randombytes.h"

void printrings(void)
{
  printf("Zq = Integers(%d)\n", WOOKIE_Q);
  printf("Zqx.<x> = PolynomialRing(Zq)\n");
  printf("Rq = Zqx.quotient(x^1024+1)\n");
}

void printsage(const poly *a)
{
  int i;
  printf("Rq(");
  for(i=0;i<WOOKIE_N-1;i++)
    printf("%d*x^%d +",a->coeffs[i],i);
  printf("%d*x^%d)",a->coeffs[i],i);
}

int main(void)
{
  poly a,b;
  unsigned char seed[WOOKIE_SYMBYTES];
  unsigned char abytes[WOOKIE_POLYBYTES];

  randombytes(seed, WOOKIE_SYMBYTES);
  poly_uniform(&a,seed);
  poly_tobytes(abytes, &a);
  poly_frombytes(&b, abytes);
  printrings();
  printf("print ");
  printsage(&a);
  printf("==");
  printsage(&b);
  printf("\n");

  /*
  printf("\n");
  int i;
  for(i=0;i<WOOKIE_N;i++)
    printf("%02x ", abytes[i]);
  printf("\n");
  */

  return 0;
}
