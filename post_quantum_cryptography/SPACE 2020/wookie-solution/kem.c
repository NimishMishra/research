#include <string.h>
#include <stdio.h>
#include "params.h"
#include "poly.h"
#include "shake256.h"
#include "randombytes.h"

int crypto_kem_keypair(unsigned char *pk, unsigned char *sk)
{
  unsigned char seed[WOOKIE_SYMBYTES];
  poly a,s,e,b;
  randombytes(seed, WOOKIE_SYMBYTES);
  poly_uniform(&a, seed);

  poly_getnoise(&s);
  poly_getnoise(&e);

  poly_mul(&b,&a,&s);
  poly_add(&b,&b,&e);
  poly_tobytes(sk, &s);

  memcpy(pk,seed,WOOKIE_SYMBYTES);
  poly_tobytes(pk+WOOKIE_SYMBYTES, &b);
  return 0;
}

int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk)
{
  poly a, b, k, sp, ep, epp, u, v;
  unsigned char prekey[WOOKIE_N/8];
  const unsigned char *seed = pk;

  poly_frombytes(&b, pk+WOOKIE_SYMBYTES);
  poly_uniform(&a, seed);

  poly_getnoise(&sp);
  poly_getnoise(&ep);
  poly_getnoise(&epp);

  randombytes(prekey, WOOKIE_N/8);
  poly_frommsg(&k, prekey);

  poly_mul(&u,&a,&sp);
  poly_add(&u,&u,&ep);

  poly_mul(&v,&b,&sp);
  poly_add(&v,&v,&epp);
  poly_add(&v,&v,&k);

  poly_tobytes(ct, &u);
  poly_tobytes(ct+WOOKIE_POLYBYTES, &v);

  shake256(ss,WOOKIE_SYMBYTES,prekey,WOOKIE_N/8);
  return 0;
}

int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk)
{
  poly u, v, s, t;
  unsigned char prekey[WOOKIE_N/8];

  poly_frombytes(&u, ct);
  poly_frombytes(&v, ct+WOOKIE_POLYBYTES);
  poly_frombytes(&s, sk);
  poly_mul(&t, &u, &s);
  poly_sub(&t, &v, &t);
  poly_tomsg(prekey, &t);

  shake256(ss,WOOKIE_SYMBYTES,prekey,WOOKIE_N/8);
  return 0;
}
