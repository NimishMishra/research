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
  poly a,b,r;
  unsigned char seed[WOOKIE_SYMBYTES];

  randombytes(seed, WOOKIE_SYMBYTES);
  poly_uniform(&a,seed);
  seed[0]++;
  poly_uniform(&b,seed);
  poly_sub(&r, &a, &b);
  printrings();
  printf("print ");
  printsage(&a);
  printf("-");
  printsage(&b);
  printf("==");
  printsage(&r);
  printf("\n");
  return 0;
}
