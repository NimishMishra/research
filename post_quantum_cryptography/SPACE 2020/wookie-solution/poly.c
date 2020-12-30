#include "params.h"
#include "poly.h"
#include "shake256.h"
#include "randombytes.h"

static int16_t coeff_freeze(int16_t a)
{
  return a & (WOOKIE_Q-1); // reduces an integer modulo q
}

static unsigned char hamming(unsigned char a)
{
  // calculates the hamming weight of the byte
  int i;
  unsigned char r = 0;
  for(i=0;i<8;i++)
    r += ((a>>i) & 1);
  return r;
}

#if (WOOKIE_Q == 4096)
void poly_tobytes(unsigned char *r, const poly *a)
{
  // freezes two coefficients and puts them into 3 bytes.
  int i,j=0;
  uint16_t t0, t1;
  for(i=0;i<WOOKIE_N;i+=2)
  {
    t0 = coeff_freeze(a->coeffs[i  ]);
    t1 = coeff_freeze(a->coeffs[i+1]);
    r[j++] = t0 & 0xff;
    r[j++] = (t0 >> 8) | ((t1 & 0xf) << 4);
    r[j++] = (t1 >> 4);
  }
}
#else
#error "poly_tobytes only works for WOOKIE_Q == 4096"
#endif

#if (WOOKIE_Q == 4096)
void poly_frombytes(poly *r, const unsigned char *b)
{
  int i,j=0;
  for(i=0;i<WOOKIE_N;i+=2)
  {
    r->coeffs[i  ]  = b[j++];
    r->coeffs[i  ] |= ((uint16_t)b[j] & 0xf) << 8;
    r->coeffs[i+1]  = (b[j++] >> 4);
    r->coeffs[i+1] |= ((uint16_t)b[j++] << 4);
  }
}
#else
#error "poly_frombytes only works for WOOKIE_Q == 4096"
#endif

void poly_mul(poly *r, const poly *a, const poly *b)
{
  // takes two polynomials and multiplies them
  int i,j;
  uint16_t t[2*WOOKIE_N];
  for(i=0;i<2*WOOKIE_N;i++)
    t[i] = 0;
  for(i=0;i<WOOKIE_N;i++)
    for(j=0;j<WOOKIE_N;j++)
      t[i+j] += a->coeffs[i] * b->coeffs[j];

  for(i=0;i<WOOKIE_N;i++)
    r->coeffs[i] = t[i] - t[i+WOOKIE_N];
}

void poly_add(poly *r, const poly *a, const poly *b)
{
  int i;
  for(i=0;i<WOOKIE_N;i++)
    r->coeffs[i] = a->coeffs[i] + b->coeffs[i];
}

void poly_sub(poly *r, const poly *a, const poly *b)
{
  int i;
  for(i=0;i<WOOKIE_N;i++)
    r->coeffs[i] = a->coeffs[i] - b->coeffs[i];
}

#if (WOOKIE_K == 8)
void poly_getnoise(poly *r)
{
  int i;
  unsigned char buf[2*WOOKIE_N];
  randombytes(buf, 2*WOOKIE_N);
  for(i=0;i<WOOKIE_N;i++)
    r->coeffs[i] = hamming(buf[2*i]) - hamming(buf[2*i+1]);
}
#else
#error "poly_getnoise only works for WOOKIE_K == 8"
#endif

void poly_uniform(poly *r, const unsigned char seed[WOOKIE_SYMBYTES])
{
  //XXX: make bigendian compatible
  shake256((unsigned char *)r->coeffs,sizeof(r->coeffs),seed,WOOKIE_SYMBYTES);
}

void poly_frommsg(poly *r, const unsigned char m[WOOKIE_N/8])
{
  int i,j;
  for(i=0;i<WOOKIE_N/8;i++)
    for(j=0;j<8;j++)
      r->coeffs[i*8+j] = (WOOKIE_Q/2)*((m[i]>>j)&1);
}

void poly_tomsg(unsigned char m[WOOKIE_N/8], const poly *a)
{
  int i,j;
  uint16_t t;
  for(i=0;i<WOOKIE_N/8;i++)
  {
    m[i] = 0;
    for(j=0;j<8;j++)
    {
      t = coeff_freeze(a->coeffs[i*8+j]);
      t += WOOKIE_Q/4;
      t >>= (WOOKIE_LOGQ-1);
      t &= 1;
      m[i] |= (t<<j);
    }
  }
}

