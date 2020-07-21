/*
 * Automatically Tuned Linear Algebra Software v3.10.3
 * Copyright (C) 2016 R. Clint Whaley
 */
#if !defined(SREAL) && !defined(DREAL) && !defined(SCPLX) && !defined(DCPLX)
   #define DREAL 1
#endif
#include <stddef.h>
#include "atlas_simd.h"
#include "atlas_prefetch.h"
#if defined(SCPLX) || defined(DCPLX)
   #include "atlas_cplxsimd.h"
   #ifndef TCPLX
      #define TCPLX 1
   #endif
   #define SHIFT <<1
#else
   #define SHIFT
   #ifndef TCPLX
      #define TREAL 1
   #endif
#endif
#ifndef TYPE
   #if defined(SREAL) || defined(SCPLX)
      #define TYPE float
   #else
      #define TYPE double
   #endif
#endif
#ifndef ATL_MM_KB
   #ifdef KB
      #if KB > 0
         #define ATL_KBCONST 1
         #define ATL_MM_KB KB
      #else
         #define ATL_KBCONST 0
         #define ATL_MM_KB K
      #endif
   #else
      #define ATL_KBCONST 0
      #define ATL_MM_KB K
   #endif
#else
   #if ATL_MM_KB > 0
      #define ATL_KBCONST 1
   #else
      #undef ATL_MM_KB
      #define ATL_MM_KB K
      #define ATL_KBCONST 0
   #endif
#endif
#ifdef BETA1
   #define ATL_vbeta(p_, d_) \
   { \
      ATL_vuld(rA0, p_); \
      ATL_vadd(d_, d_, rA0); \
      ATL_vust(p_, d_); \
   }
#elif defined(BETA0)
   #define ATL_vbeta(p_, d_) ATL_vust(p_, d_)
#else
   #define ATL_vbeta(p_, d_) \
   { \
      ATL_vuld(rA0, p_); \
      ATL_vmac(d_, rA0, vBE); \
      ATL_vust(p_, d_); \
   }
#endif

#if defined (TCPLX) && ATL_VLEN == 4
   #ifdef BETA0
      #define wrtC(p_, r1_) \
      { \
         (p_)[ 0] = r1_[0]; \
         (p_)[ 2] = r1_[1]; \
         (p_)[ 4] = r1_[2]; \
         (p_)[ 6] = r1_[3]; \
      }
   #elif defined(BETA1)
      #define wrtC(p_, r1_) \
      { \
         (p_)[ 0] += r1_[0]; \
         (p_)[ 2] += r1_[1]; \
         (p_)[ 4] += r1_[2]; \
         (p_)[ 6] += r1_[3]; \
      }
   #else
      #define wrtC(p_, r1_) \
      { \
         (p_)[ 0] = beta*(p_)[ 0] + r1_[0]; \
         (p_)[ 2] = beta*(p_)[ 2] + r1_[1]; \
         (p_)[ 4] = beta*(p_)[ 4] + r1_[2]; \
         (p_)[ 6] = beta*(p_)[ 6] + r1_[3]; \
      }
   #endif
   #ifndef BETA0
      #define vwrtC(p_, rc_, rt_, rz_) \
      { \
         ATL_vunpckHI(rt_, rc_, rz_); /* rt_={0, r3, 0, r2} */ \
         ATL_vunpckLO(rc_, rc_, rz_); /* rc_={0, r1, 0, r0} */ \
         ATL_vbeta(p_, rc_); \
         ATL_vbeta((p_)+4, rt_); \
      }
    #endif
#elif defined (TCPLX) && ATL_VLEN == 2
   #ifdef BETA0
      #define wrtC(p_, r1_) \
      { \
         (p_)[ 0] = r1_[0]; \
         (p_)[ 2] = r1_[1]; \
      }
   #elif defined(BETA1)
      #define wrtC(p_, r1_) \
      { \
         (p_)[ 0] += r1_[0]; \
         (p_)[ 2] += r1_[1]; \
      }
   #else
      #define wrtC(p_, r1_) \
      { \
         (p_)[ 0] = beta*(p_)[ 0] + r1_[0]; \
         (p_)[ 2] = beta*(p_)[ 2] + r1_[1]; \
      }
   #endif
#endif
#ifndef ATL_RESTRICT
   #if defined(__STDC_VERSION__) && (__STDC_VERSION__/100 >= 1999)
      #define ATL_RESTRICT restrict
   #else
      #define ATL_RESTRICT
   #endif
#endif
void ATL_USERMM
   (const int M, const int N, const int K, const TYPE alpha,
    const TYPE * ATL_RESTRICT A, const int lda,
    const TYPE * ATL_RESTRICT B, const int ldb, const TYPE beta,
    TYPE * ATL_RESTRICT C, const int ldc)
/*
 * Performs a GEMM with M,N,K unrolling (& jam) of (8,6,VLEN).
 * Vectorization of VLEN=[4,8] (d,s) along K dim, vec unroll=(8,6,1).
 * You may set compile-time constant K dim by defining ATL_MM_KB.
 */
{
   const TYPE *pB0=B, *pB2=pB0+(ldb<<1), *pB4=pB2+(ldb<<1), *aa=A, *pA0=A,
              *pA2=pA0+(lda<<1), *pA4=pA2+(lda<<1), *pA6=pA4+(lda<<1),
              *pfA=A+lda*M, *pfB=B+ldb*N;
   const size_t ldc2 = ldc SHIFT;
   TYPE *pC0=C;
   int i, j, k;
   #if !defined(BETA0) && !defined(BETA1) && !defined(TCPLX)
      ATL_VTYPE vBE;
   #elif ATL_VLEN == 4 && defined(TCPLX) && defined(BETAX)
      const ATL_VTYPE vBE={beta, 1.0, beta, 1.0};
   #endif
   #if ATL_KBCONST == 0
      const size_t incAm = (lda<<3), incBn = ldb*5;
   #else
      #define incAm (ATL_MM_KB<<3)
      #define incBn (5*ATL_MM_KB)
   #endif
   const size_t incC=ldc2*5;
   #if !defined(BETA0) && !defined(BETA1) && !defined(TCPLX)
      ATL_vbcast(vBE, &beta);
   #endif

   for (j=0; j < N; j += 5)
   {
      for (i=0; i < M; i += 8)
      {
         register ATL_VTYPE rA0, rA1, rA2, rA3, rA4, rA5, rA6, rA7, rB0, rC00,
                            rC10, rC20, rC30, rC40, rC50, rC60, rC70, rB1, rC01,
                            rC11, rC21, rC31, rC41, rC51, rC61, rC71, rB2, rC02,
                            rC12, rC22, rC32, rC42, rC52, rC62, rC72, rB3, rC03,
                            rC13, rC23, rC33, rC43, rC53, rC63, rC73, rB4, rC04,
                            rC14, rC24, rC34, rC44, rC54, rC64, rC74;
         /* Peel K=0 iteration to avoid zero of rCxx and extra add  */
         ATL_vld(rB0, pB0);
         ATL_vld(rA0, pA0);
         ATL_vmul(rC00, rA0, rB0);
         ATL_vld(rA1, pA0+lda); pA0 += ATL_VLEN;
         ATL_vmul(rC10, rA1, rB0);
         ATL_vld(rA2, pA2);
         ATL_vmul(rC20, rA2, rB0);
         ATL_vld(rA3, pA2+lda); pA2 += ATL_VLEN;
         ATL_vmul(rC30, rA3, rB0);
         ATL_vld(rA4, pA4);
         ATL_vmul(rC40, rA4, rB0);
         ATL_vld(rA5, pA4+lda); pA4 += ATL_VLEN;
         ATL_vmul(rC50, rA5, rB0);
         ATL_vld(rA6, pA6);
         ATL_vmul(rC60, rA6, rB0);
         ATL_vld(rA7, pA6+lda); pA6 += ATL_VLEN;
         ATL_vmul(rC70, rA7, rB0);

            ATL_vld(rB1, pB0+ldb); pB0 += ATL_VLEN;
            ATL_vld(rB0, pB0);
         ATL_vmul(rC01, rA0, rB1);
            ATL_vld(rB2, pB2);
         ATL_vmul(rC11, rA1, rB1);
            ATL_vld(rB3, pB2+ldb); pB2 += ATL_VLEN;
            ATL_vld(rB4, pB4);
            pB4 += ATL_VLEN;
         ATL_vmul(rC21, rA2, rB1);
         ATL_vmul(rC31, rA3, rB1);
         ATL_vmul(rC41, rA4, rB1);
         ATL_vmul(rC51, rA5, rB1);
            ATL_pfl1R(pfA);
            ATL_pfl1R(pfA+8);
         ATL_vmul(rC61, rA6, rB1);
            ATL_pfl1R(pfA+16);
            ATL_pfl1R(pfA+24);
         ATL_vmul(rC71, rA7, rB1);
            ATL_vld(rB1, pB0+ldb);  pB0 += ATL_VLEN;

         ATL_vmul(rC02, rA0, rB2);
            ATL_pfl1R(pfB);
            ATL_pfl1R(pfB+8);
         ATL_vmul(rC12, rA1, rB2);
            ATL_pfl1R(pfB+16);
            ATL_pfl1R(pfB+24);
         ATL_vmul(rC22, rA2, rB2);
            ATL_pfl1R(pfB+32);
            ATL_pfl1R(pfB+40);
         ATL_vmul(rC32, rA3, rB2);
         ATL_vmul(rC42, rA4, rB2);
            ATL_pfl1R(pfA+32);
            ATL_pfl1R(pfA+40);
         ATL_vmul(rC52, rA5, rB2);
         ATL_vmul(rC62, rA6, rB2);
         ATL_vmul(rC72, rA7, rB2);
            ATL_vld(rB2, pB2);

         ATL_vmul(rC03, rA0, rB3);
         ATL_vmul(rC13, rA1, rB3);
         ATL_vmul(rC23, rA2, rB3);
         ATL_vmul(rC33, rA3, rB3);
         ATL_vmul(rC43, rA4, rB3);
         ATL_vmul(rC53, rA5, rB3);
         ATL_vmul(rC63, rA6, rB3);
         ATL_vmul(rC73, rA7, rB3);
            ATL_vld(rB3, pB2+lda); pB2 += ATL_VLEN;

         ATL_vmul(rC04, rA0, rB4);
            ATL_vld(rA0, pA0);
         ATL_vmul(rC14, rA1, rB4);
            ATL_vld(rA1, pA0+lda); pA0 += ATL_VLEN;
         ATL_vmul(rC24, rA2, rB4);
            ATL_vld(rA2, pA2);
         ATL_vmul(rC34, rA3, rB4);
            ATL_vld(rA3, pA2+lda); pA2 += ATL_VLEN;
         ATL_vmul(rC44, rA4, rB4);
            ATL_vld(rA4, pA4);
         ATL_vmul(rC54, rA5, rB4);
            ATL_vld(rA5, pA4+lda); pA4 += ATL_VLEN;
         ATL_vmul(rC64, rA6, rB4);
            ATL_vld(rA6, pA6);
         ATL_vmul(rC74, rA7, rB4);
            ATL_vld(rA7, pA6+lda); pA6 += ATL_VLEN;
         pfA += 40; pfB += 40;
/*
 *       Stop one iteration early to drain preload pipline of A/B
 */
         for (k=(ATL_VLEN<<1); k < ATL_MM_KB; k += ATL_VLEN)
         {
            ATL_vmac(rC00, rA0, rB0);
            ATL_vld(rB4, pB4);  pB4 += ATL_VLEN;
            ATL_vmac(rC10, rA1, rB0);
            ATL_vmac(rC20, rA2, rB0);
            ATL_vmac(rC30, rA3, rB0);
            ATL_vmac(rC40, rA4, rB0);
            ATL_vmac(rC50, rA5, rB0);
            ATL_vmac(rC60, rA6, rB0);
            ATL_vmac(rC70, rA7, rB0);
               ATL_vld(rB0, pB0);

            ATL_vmac(rC01, rA0, rB1);
            ATL_vmac(rC11, rA1, rB1);
            ATL_vmac(rC21, rA2, rB1);
            ATL_vmac(rC31, rA3, rB1);
            ATL_vmac(rC41, rA4, rB1);
            ATL_vmac(rC51, rA5, rB1);
            ATL_vmac(rC61, rA6, rB1);
            ATL_vmac(rC71, rA7, rB1);
               ATL_vld(rB1, pB0+ldb); pB0 += ATL_VLEN;

            ATL_vmac(rC02, rA0, rB2);
            ATL_vmac(rC12, rA1, rB2);
            ATL_vmac(rC22, rA2, rB2);
            ATL_vmac(rC32, rA3, rB2);
            ATL_vmac(rC42, rA4, rB2);
            ATL_vmac(rC52, rA5, rB2);
            ATL_vmac(rC62, rA6, rB2);
            ATL_vmac(rC72, rA7, rB2);
               ATL_vld(rB2, pB2);

            ATL_vmac(rC03, rA0, rB3);
            ATL_vmac(rC13, rA1, rB3);
            ATL_vmac(rC23, rA2, rB3);
            ATL_vmac(rC33, rA3, rB3);
            ATL_vmac(rC43, rA4, rB3);
            ATL_vmac(rC53, rA5, rB3);
            ATL_vmac(rC63, rA6, rB3);
            ATL_vmac(rC73, rA7, rB3);
               ATL_vld(rB3, pB2+ldb); pB2 += ATL_VLEN;


            ATL_vmac(rC04, rA0, rB4);
               ATL_vld(rA0, pA0);
            ATL_vmac(rC14, rA1, rB4);
               ATL_vld(rA1, pA0+lda); pA0 += ATL_VLEN;
            ATL_vmac(rC24, rA2, rB4);
               ATL_vld(rA2, pA2);
            ATL_vmac(rC34, rA3, rB4);
               ATL_vld(rA3, pA2+lda); pA2 += ATL_VLEN;
            ATL_vmac(rC44, rA4, rB4);
               ATL_vld(rA4, pA4);
            ATL_vmac(rC54, rA5, rB4);
               ATL_vld(rA5, pA4+lda); pA4 += ATL_VLEN;
            ATL_vmac(rC64, rA6, rB4);
               ATL_vld(rA6, pA6);
            ATL_vmac(rC74, rA7, rB4);
               ATL_vld(rA7, pA6+lda); pA6 += ATL_VLEN;
         }  /* end K-loop */
/*
 *       Last iteration peeled out to drain preload pipeline
 */
            ATL_vmac(rC00, rA0, rB0);
               ATL_vld(rB4, pB4);
            ATL_vmac(rC10, rA1, rB0);
            ATL_vmac(rC20, rA2, rB0);
            ATL_vmac(rC30, rA3, rB0);
            ATL_vmac(rC40, rA4, rB0);
            ATL_vmac(rC50, rA5, rB0);
            ATL_vmac(rC60, rA6, rB0);
            ATL_vmac(rC70, rA7, rB0);

            ATL_vmac(rC01, rA0, rB1);
            ATL_vmac(rC11, rA1, rB1);
            ATL_vmac(rC21, rA2, rB1);
            ATL_vmac(rC31, rA3, rB1);
            ATL_vmac(rC41, rA4, rB1);
            ATL_vmac(rC51, rA5, rB1);
            ATL_vmac(rC61, rA6, rB1);
            ATL_vmac(rC71, rA7, rB1);

            ATL_vmac(rC02, rA0, rB2);
            ATL_vmac(rC12, rA1, rB2);
            ATL_vmac(rC22, rA2, rB2);
            ATL_vmac(rC32, rA3, rB2);
            ATL_vmac(rC42, rA4, rB2);
            ATL_vmac(rC52, rA5, rB2);
            ATL_vmac(rC62, rA6, rB2);
            ATL_vmac(rC72, rA7, rB2);

            ATL_vmac(rC03, rA0, rB3);
            ATL_vmac(rC13, rA1, rB3);
            ATL_vmac(rC23, rA2, rB3);
            ATL_vmac(rC33, rA3, rB3);
            ATL_vmac(rC43, rA4, rB3);
            ATL_vmac(rC53, rA5, rB3);
            ATL_vmac(rC63, rA6, rB3);
            ATL_vmac(rC73, rA7, rB3);

            ATL_vmac(rC04, rA0, rB4);
            ATL_vmac(rC14, rA1, rB4);
            ATL_vmac(rC24, rA2, rB4);
            ATL_vmac(rC34, rA3, rB4);
            ATL_vmac(rC44, rA4, rB4);
            ATL_vmac(rC54, rA5, rB4);
            ATL_vmac(rC64, rA6, rB4);
            ATL_vmac(rC74, rA7, rB4);

         #if ATL_VLEN == 2
            ATL_vvrsum2(rC00, rC10);
            ATL_vvrsum2(rC20, rC30);
            ATL_vvrsum2(rC40, rC50);
            ATL_vvrsum2(rC60, rC70);
            ATL_vvrsum2(rC01, rC11);
            ATL_vvrsum2(rC21, rC31);
            ATL_vvrsum2(rC41, rC51);
            ATL_vvrsum2(rC61, rC71);
            ATL_vvrsum2(rC02, rC12);
            ATL_vvrsum2(rC22, rC32);
            ATL_vvrsum2(rC42, rC52);
            ATL_vvrsum2(rC62, rC72);
            ATL_vvrsum2(rC03, rC13);
            ATL_vvrsum2(rC23, rC33);
            ATL_vvrsum2(rC43, rC53);
            ATL_vvrsum2(rC63, rC73);
            ATL_vvrsum2(rC04, rC14);
            ATL_vvrsum2(rC24, rC34);
            ATL_vvrsum2(rC44, rC54);
            ATL_vvrsum2(rC64, rC74);
            #ifdef TCPLX
               wrtC(pC0, rC00);
               wrtC(pC0+4, rC20);
               wrtC(pC0+8, rC40);
               wrtC(pC0+12, rC60);
               pC0 += ldc2;
               wrtC(pC0, rC01);
               wrtC(pC0+4, rC21);
               wrtC(pC0+8, rC41);
               wrtC(pC0+12, rC61);
               pC0 += ldc2;
               wrtC(pC0, rC02);
               wrtC(pC0+4, rC22);
               wrtC(pC0+8, rC42);
               wrtC(pC0+12, rC62);
               pC0 += ldc2;
               wrtC(pC0, rC03);
               wrtC(pC0+4, rC23);
               wrtC(pC0+8, rC43);
               wrtC(pC0+12, rC63);
               pC0 += ldc2;
               wrtC(pC0, rC04);
               wrtC(pC0+4, rC24);
               wrtC(pC0+8, rC44);
               wrtC(pC0+12, rC64);
            #else
               ATL_vbeta(pC0, rC00);
               ATL_vbeta(pC0+2, rC20);
               ATL_vbeta(pC0+4, rC40);
               ATL_vbeta(pC0+6, rC60);
               pC0 += ldc2;
               ATL_vbeta(pC0, rC01);
               ATL_vbeta(pC0+2, rC21);
               ATL_vbeta(pC0+4, rC41);
               ATL_vbeta(pC0+6, rC61);
               pC0 += ldc2;
               ATL_vbeta(pC0, rC02);
               ATL_vbeta(pC0+2, rC22);
               ATL_vbeta(pC0+4, rC42);
               ATL_vbeta(pC0+6, rC62);
               pC0 += ldc2;
               ATL_vbeta(pC0, rC03);
               ATL_vbeta(pC0+2, rC23);
               ATL_vbeta(pC0+4, rC43);
               ATL_vbeta(pC0+6, rC63);
               pC0 += ldc2;
               ATL_vbeta(pC0, rC04);
               ATL_vbeta(pC0+2, rC24);
               ATL_vbeta(pC0+4, rC44);
               ATL_vbeta(pC0+6, rC64);
            #endif
         #elif ATL_VLEN == 4
            #if defined(TCPLX) && !defined(BETA0) // want permuted order
                  ATL_vvrsum4(rC00, rC20, rC10, rC30); // rC00={r3, r1, r2, r0}
                  ATL_vvrsum4(rC40, rC50, rC60, rC70);
                  ATL_vvrsum4(rC01, rC21, rC11, rC31); // rC00={r3, r1, r2, r0}
                  ATL_vvrsum4(rC41, rC51, rC61, rC71);
                  ATL_vvrsum4(rC02, rC22, rC12, rC32); // rC00={r3, r1, r2, r0}
                  ATL_vvrsum4(rC42, rC52, rC62, rC72);
                  ATL_vvrsum4(rC03, rC23, rC13, rC33); // rC00={r3, r1, r2, r0}
                  ATL_vvrsum4(rC43, rC53, rC63, rC73);
                  ATL_vvrsum4(rC04, rC24, rC14, rC34); // rC00={r3, r1, r2, r0}
                  ATL_vvrsum4(rC44, rC54, rC64, rC74);
            #else                                 // want natural order
                  ATL_vvrsum4(rC00, rC10, rC20, rC30);
                  ATL_vvrsum4(rC40, rC50, rC60, rC70);
                  ATL_vvrsum4(rC01, rC11, rC21, rC31);
                  ATL_vvrsum4(rC41, rC51, rC61, rC71);
                  ATL_vvrsum4(rC02, rC12, rC22, rC32);
                  ATL_vvrsum4(rC42, rC52, rC62, rC72);
                  ATL_vvrsum4(rC03, rC13, rC23, rC33);
                  ATL_vvrsum4(rC43, rC53, rC63, rC73);
                  ATL_vvrsum4(rC04, rC14, rC24, rC34);
                  ATL_vvrsum4(rC44, rC54, rC64, rC74);
            #endif
            #ifdef TCPLX
               #ifndef BETA0
                  ATL_vzero(rB0);                      // rB0 ={ 0,  0,  0,  0}
                  vwrtC(pC0, rC00, rC20, rB0);
                  wrtC(pC0+8, rC40);
                  pC0 += ldc2;
                  vwrtC(pC0, rC01, rC21, rB0);
                  wrtC(pC0+8, rC41);
                  pC0 += ldc2;
                  vwrtC(pC0, rC02, rC22, rB0);
                  wrtC(pC0+8, rC42);
                  pC0 += ldc2;
                  vwrtC(pC0, rC03, rC23, rB0);
                  wrtC(pC0+8, rC43);
                  pC0 += ldc2;
                  vwrtC(pC0, rC04, rC24, rB0);
                  wrtC(pC0+8, rC44);
               #else
                  wrtC(pC0, rC00);
                  wrtC(pC0+8, rC40);
                  pC0 += ldc2;
                  wrtC(pC0, rC01);
                  wrtC(pC0+8, rC41);
                  pC0 += ldc2;
                  wrtC(pC0, rC02);
                  wrtC(pC0+8, rC42);
                  pC0 += ldc2;
                  wrtC(pC0, rC03);
                  wrtC(pC0+8, rC43);
                  pC0 += ldc2;
                  wrtC(pC0, rC04);
                  wrtC(pC0+8, rC44);
               #endif
            #else  /* real */
               ATL_vbeta(pC0, rC00);
               ATL_vbeta(pC0+4, rC40);
               pC0 += ldc;
               ATL_vbeta(pC0, rC01);
               ATL_vbeta(pC0+4, rC41);
               pC0 += ldc;
               ATL_vbeta(pC0, rC02);
               ATL_vbeta(pC0+4, rC42);
               pC0 += ldc;
               ATL_vbeta(pC0, rC03);
               ATL_vbeta(pC0+4, rC43);
               pC0 += ldc;
               ATL_vbeta(pC0, rC04);
               ATL_vbeta(pC0+4, rC44);
            #endif
         #else
            #error "VLEN NOT SUPPORTED!"
         #endif
         pC0 -= ldc2*4 - (8 SHIFT);
         pB0 = B;
         pB2 = B   + (ldb<<1);
         pB4 = pB2 + (ldb<<1);
         A += incAm;
         pA0 = A;
         pA2 = pA0 + (lda<<1);
         pA4 = pA2 + (lda<<1);
         pA6 = pA4 + (lda<<1);
      }  /* end of loop over M */
      A = aa;
      pA0 = A;
      pA2 = A   + (lda<<1);
      pA4 = pA2 + (lda<<1);
      pA6 = pA4 + (lda<<1);
      C += incC;
      pC0 = C;
      B += incBn;
      pB0 = B;
      pB2 =   B + (ldb<<1);
      pB4 = pB2 + (ldb<<1);
   }  /* end of loop over N */
}
