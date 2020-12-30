#include <stdio.h>
#include <string.h>
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
  poly a,b,s,e;
  unsigned char msg0[WOOKIE_N/8];
  unsigned char msg1[WOOKIE_N/8];
  unsigned char msg2[WOOKIE_N/8];

  randombytes(msg0, WOOKIE_N/8);

  poly_frommsg(&a, msg0);

  poly_getnoise(&s);
  poly_getnoise(&e);
  poly_mul(&s,&s,&e);
  poly_sub(&b,&s,&a);
  poly_add(&a,&s,&a);
  poly_tomsg(msg1, &a);

  if(memcmp(msg0, msg1, WOOKIE_N/8) && memcmp(msg0, msg2, WOOKIE_N/8))
    printf("False\n");
  else
    printf("True\n");

  return 0;
}
