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
  int i, nz=0;
  uint16_t t,r=0;

  randombytes(seed, WOOKIE_SYMBYTES);
  poly_getnoise(&a);
  poly_getnoise(&b);

  for(i=0;i<WOOKIE_N;i++)
  {
    t = a.coeffs[i] & (WOOKIE_Q-1);
    if(t > WOOKIE_K && t < WOOKIE_Q-WOOKIE_K)
    {
      printf("False\n");
      return 0;
    }
    if(t != 0) nz++;
    r |= a.coeffs[i] ^ b.coeffs[i];
  }

  if(nz == 0 || r == 0)
    printf("False\n");
  else
    printf("True\n");

  return 0;
}
