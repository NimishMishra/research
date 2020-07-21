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

#if ATL_VLEN == 8
   #ifdef BETA0
      #define wrtC0(p_, r1_) \
      { \
         (p_)[ 0] = r1_[0]; \
         (p_)[ 2] = r1_[1]; \
         (p_)[ 4] = r1_[2]; \
         (p_)[ 6] = r1_[3]; \
      }
      #define wrtC(p_, r1_) \
      { \
         (p_)[ 0] = r1_[0]; \
         (p_)[ 2] = r1_[1]; \
         (p_)[ 4] = r1_[2]; \
         (p_)[ 6] = r1_[3]; \
         ((p_)+ldc2)[ 0] = r1_[4]; \
         ((p_)+ldc2)[ 2] = r1_[5]; \
         ((p_)+ldc2)[ 4] = r1_[6]; \
         ((p_)+ldc2)[ 6] = r1_[7]; \
      }
   #elif defined(BETA1)
      #define wrtC0(p_, r1_) \
      { \
         (p_)[ 0] += r1_[0]; \
         (p_)[ 2] += r1_[1]; \
         (p_)[ 4] += r1_[2]; \
         (p_)[ 6] += r1_[3]; \
      }
      #define wrtC(p_, r1_) \
      { \
         (p_)[ 0] += r1_[0]; \
         (p_)[ 2] += r1_[1]; \
         (p_)[ 4] += r1_[2]; \
         (p_)[ 6] += r1_[3]; \
         ((p_)+ldc2)[ 0] += r1_[4]; \
         ((p_)+ldc2)[ 2] += r1_[5]; \
         ((p_)+ldc2)[ 4] += r1_[6]; \
         ((p_)+ldc2)[ 6] += r1_[7]; \
      }
   #else
      #define wrtC0(p_, r1_) \
      { \
         (p_)[ 0] = beta*(p_)[ 0] + r1_[0]; \
         (p_)[ 2] = beta*(p_)[ 2] + r1_[1]; \
         (p_)[ 4] = beta*(p_)[ 4] + r1_[2]; \
         (p_)[ 6] = beta*(p_)[ 6] + r1_[3]; \
      }
      #define wrtC(p_, r1_) \
      { \
         (p_)[ 0] = beta*(p_)[ 0] + r1_[0]; \
         (p_)[ 2] = beta*(p_)[ 2] + r1_[1]; \
         (p_)[ 4] = beta*(p_)[ 4] + r1_[2]; \
         (p_)[ 6] = beta*(p_)[ 6] + r1_[3]; \
         ((p_)+ldc2)[ 0] = beta*((p_)+ldc2)[ 0] + r1_[4]; \
         ((p_)+ldc2)[ 2] = beta*((p_)+ldc2)[ 2] + r1_[5]; \
         ((p_)+ldc2)[ 4] = beta*((p_)+ldc2)[ 4] + r1_[6]; \
         ((p_)+ldc2)[ 6] = beta*((p_)+ldc2)[ 6] + r1_[7]; \
      }
   #endif
#elif ATL_VLEN == 4
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
#elif ATL_VLEN == 2
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
              *pA2=pA0+(lda<<1), *pfA=A+lda*M, *pfB=B+ldb*N;
   const size_t ldc2 = ldc SHIFT;
   TYPE *pC0=C, *pC2=pC0+(ldc2<<1), *pC4=pC2+(ldc2<<1);
   int i, j, k;
   #if !defined(BETA0) && !defined(BETA1) && !defined(TCPLX)
      ATL_VTYPE vBE;
   #elif ATL_VLEN == 8 && defined(TCPLX) && defined(BETAX)
      const ATL_VTYPE vBE={beta, 1.0, beta, 1.0, beta, 1.0, beta, 1.0};
   #elif ATL_VLEN == 4 && defined(TCPLX) && defined(BETAX)
      const ATL_VTYPE vBE={beta, 1.0, beta, 1.0};
   #endif
   const size_t incAm = (lda<<2), incBn = ldb*6;
   const size_t incC=ldc2*6;
   #if !defined(BETA0) && !defined(BETA1) && !defined(TCPLX)
      ATL_vbcast(vBE, &beta);
   #endif

   for (j=0; j < N; j += 6)
   {
      for (i=0; i < M; i += 4)
      {
         register ATL_VTYPE rA0, rA1, rA2, rA3, rB0, rC00, rC10, rC20, rC30,
                            rB1, rC01, rC11, rC21, rC31, rB2, rC02, rC12, rC22,
                            rC32, rB3, rC03, rC13, rC23, rC33, rB4, rC04, rC14,
                            rC24, rC34, rB5, rC05, rC15, rC25, rC35;
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

            ATL_vld(rB1, pB0+ldb); pB0 += ATL_VLEN;
            ATL_vld(rB0, pB0);
         ATL_vmul(rC01, rA0, rB1);
            ATL_vld(rB2, pB2);
            ATL_vld(rB3, pB2+ldb); pB2 += ATL_VLEN;
            ATL_vld(rB4, pB4);
         ATL_vmul(rC11, rA1, rB1);
         ATL_vmul(rC21, rA2, rB1);
            ATL_vld(rB5, pB4+ldb);  pB4 += ATL_VLEN;
         ATL_vmul(rC31, rA3, rB1);
            ATL_pfl1R(pfA);
            ATL_pfl1R(pfA+8);
            ATL_pfl1R(pfA+16);
            ATL_pfl1R(pfA+24);
            ATL_vld(rB1, pB0+ldb);  pB0 += ATL_VLEN;

         ATL_vmul(rC02, rA0, rB2);
            ATL_pfl1R(pfB);
            ATL_pfl1R(pfB+8);
         ATL_vmul(rC12, rA1, rB2);
            ATL_pfl1R(pfB+16);
            ATL_pfl1R(pfB+24);
         ATL_vmul(rC22, rA2, rB2);
         ATL_vmul(rC32, rA3, rB2);
            ATL_vld(rB2, pB2);

         ATL_vmul(rC03, rA0, rB3);
         ATL_vmul(rC13, rA1, rB3);
         ATL_vmul(rC23, rA2, rB3);
         ATL_vmul(rC33, rA3, rB3);
            ATL_vld(rB3, pB2+ldb); pB2 += ATL_VLEN;

         ATL_vmul(rC04, rA0, rB4);
         ATL_vmul(rC14, rA1, rB4);
         ATL_vmul(rC24, rA2, rB4);
         ATL_vmul(rC34, rA3, rB4);
            ATL_vld(rB4, pB4);

         ATL_vmul(rC05, rA0, rB5);
            ATL_vld(rA0, pA0);
         ATL_vmul(rC15, rA1, rB5);
            ATL_vld(rA1, pA0+lda); pA0 += ATL_VLEN;
         ATL_vmul(rC25, rA2, rB5);
            ATL_vld(rA2, pA2);
         ATL_vmul(rC35, rA3, rB5);
            ATL_vld(rA3, pA2+lda); pA2 += ATL_VLEN;
         pfA += 24; pfB += 24;
/*
 *       Stop one iteration early to drain preload pipline of A/B
 */
         for (k=(ATL_VLEN<<1); k < ATL_MM_KB; k += ATL_VLEN)
         {
            ATL_vmac(rC00, rA0, rB0);
            ATL_vld(rB5, pB4+ldb);  pB4 += ATL_VLEN;
            ATL_vmac(rC10, rA1, rB0);
            ATL_vmac(rC20, rA2, rB0);
            ATL_vmac(rC30, rA3, rB0);
               ATL_vld(rB0, pB0);

            ATL_vmac(rC01, rA0, rB1);
            ATL_vmac(rC11, rA1, rB1);
            ATL_vmac(rC21, rA2, rB1);
            ATL_vmac(rC31, rA3, rB1);
               ATL_vld(rB1, pB0+ldb); pB0 += ATL_VLEN;

            ATL_vmac(rC02, rA0, rB2);
            ATL_vmac(rC12, rA1, rB2);
            ATL_vmac(rC22, rA2, rB2);
            ATL_vmac(rC32, rA3, rB2);
               ATL_vld(rB2, pB2);

            ATL_vmac(rC03, rA0, rB3);
            ATL_vmac(rC13, rA1, rB3);
            ATL_vmac(rC23, rA2, rB3);
            ATL_vmac(rC33, rA3, rB3);
               ATL_vld(rB3, pB2+ldb); pB2 += ATL_VLEN;

            ATL_vmac(rC04, rA0, rB4);
            ATL_vmac(rC14, rA1, rB4);
            ATL_vmac(rC24, rA2, rB4);
            ATL_vmac(rC34, rA3, rB4);
               ATL_vld(rB4, pB4);


            ATL_vmac(rC05, rA0, rB5);
               ATL_vld(rA0, pA0);
            ATL_vmac(rC15, rA1, rB5);
               ATL_vld(rA1, pA0+lda); pA0 += ATL_VLEN;
            ATL_vmac(rC25, rA2, rB5);
               ATL_vld(rA2, pA2);
            ATL_vmac(rC35, rA3, rB5);
               ATL_vld(rA3, pA2+lda); pA2 += ATL_VLEN;
         }  /* end K-loop */
/*
 *       Last iteration peeled out to drain preload pipeline
 */
         ATL_vmac(rC00, rA0, rB0);
            ATL_vld(rB5, pB4+ldb);
         ATL_vmac(rC10, rA1, rB0);
         ATL_vmac(rC20, rA2, rB0);
         ATL_vmac(rC30, rA3, rB0);

         ATL_vmac(rC01, rA0, rB1);
         ATL_vmac(rC11, rA1, rB1);
         ATL_vmac(rC21, rA2, rB1);
         ATL_vmac(rC31, rA3, rB1);

         ATL_vmac(rC02, rA0, rB2);
         ATL_vmac(rC12, rA1, rB2);
         ATL_vmac(rC22, rA2, rB2);
         ATL_vmac(rC32, rA3, rB2);

         ATL_vmac(rC03, rA0, rB3);
         ATL_vmac(rC13, rA1, rB3);
         ATL_vmac(rC23, rA2, rB3);
         ATL_vmac(rC33, rA3, rB3);

         ATL_vmac(rC04, rA0, rB4);
         ATL_vmac(rC14, rA1, rB4);
         ATL_vmac(rC24, rA2, rB4);
         ATL_vmac(rC34, rA3, rB4);

         ATL_vmac(rC05, rA0, rB5);
         ATL_vmac(rC15, rA1, rB5);
         ATL_vmac(rC25, rA2, rB5);
         ATL_vmac(rC35, rA3, rB5);

         #if ATL_VLEN == 2
            ATL_vvrsum2(rC00, rC10);
            ATL_vvrsum2(rC20, rC30);
            ATL_vvrsum2(rC01, rC11);
            ATL_vvrsum2(rC21, rC31);
            ATL_vvrsum2(rC02, rC12);
            ATL_vvrsum2(rC22, rC32);
            ATL_vvrsum2(rC03, rC13);
            ATL_vvrsum2(rC23, rC33);
            ATL_vvrsum2(rC04, rC14);
            ATL_vvrsum2(rC24, rC34);
            ATL_vvrsum2(rC05, rC15);
            ATL_vvrsum2(rC25, rC35);
            #ifdef TCPLX
               wrtC(pC0, rC00);
               wrtC(pC0+4, rC20);
               wrtC(pC0+ldc2, rC01);
               wrtC(pC0+ldc2+4, rC21);
               wrtC(pC2, rC02);
               wrtC(pC2+4, rC22);
               wrtC(pC2+ldc2, rC03);
               wrtC(pC2+ldc2+4, rC23);
               wrtC(pC4, rC04);
               wrtC(pC4+4, rC24);
               wrtC(pC4+ldc2, rC05);
               wrtC(pC4+ldc2+4, rC25);
            #else
               ATL_vbeta(pC0, rC00);
               ATL_vbeta(pC0+2, rC20);
               ATL_vbeta(pC0+ldc2, rC01);
               ATL_vbeta(pC0+ldc2+2, rC21);
               ATL_vbeta(pC2, rC02);
               ATL_vbeta(pC2+2, rC22);
               ATL_vbeta(pC2+ldc2, rC03);
               ATL_vbeta(pC2+ldc2+2, rC23);
               ATL_vbeta(pC4, rC04);
               ATL_vbeta(pC4+2, rC24);
               ATL_vbeta(pC4+ldc2, rC05);
               ATL_vbeta(pC4+ldc2+2, rC25);
            #endif
         #elif ATL_VLEN == 4
            ATL_vvrsum4(rC00, rC10, rC20, rC30);
            ATL_vvrsum4(rC01, rC11, rC21, rC31);
            ATL_vvrsum4(rC02, rC12, rC22, rC32);
            ATL_vvrsum4(rC03, rC13, rC23, rC33);
            ATL_vvrsum4(rC04, rC14, rC24, rC34);
            ATL_vvrsum4(rC05, rC15, rC25, rC35);
            #ifdef TCPLX
               wrtC(pC0, rC00);
               wrtC(pC0+ldc2, rC01);
               wrtC(pC2, rC02);
               wrtC(pC2+ldc2, rC03);
               wrtC(pC4, rC04);
               wrtC(pC4+ldc2, rC05);
            #else  /* real */
               ATL_vbeta(pC0, rC00);
               ATL_vbeta(pC0+ldc2, rC01);
               ATL_vbeta(pC2, rC02);
               ATL_vbeta(pC2+ldc2, rC03);
               ATL_vbeta(pC4, rC04);
               ATL_vbeta(pC4+ldc2, rC05);
            #endif
         #elif ATL_VLEN == 8 && defined(ATL_AVX) && defined(ATL_SSE1) \
               && defined(SCPLX)
         {
            #ifndef BETA0
               ATL_vzero(rB0);
            #endif
            #ifdef BETA0
               ATL_vvrsum8(rC00,rC10,rC20,rC30, rC01,rC11,rC21,rC31);
               wrtC(pC0, rC00);
            #else
               ATL_vvrsum8(rC00,rC10, rC01, rC11, rC20,rC30, rC21,rC31);
                                     //    7    6    5    4    3    2    1    0
                                     // {r31, r21, r30, r20, r11, r01, r10, r00}
               ATL_vuld(rC30, pC0+ldc2);
                                     // {i31, r31, i21, r21, i11, r11, i01, r01}
               ATL_vuld(rC20, pC0);
                                     // {i30, r30, i20, r20, i10, r10, i00, r00}
               ATL_vunpckHI(rC10, rC00, rB0);
                                     // {  0, r31,   0, r21,   0, r11,   0, r01}
               ATL_vunpckLO(rC00, rC00, rB0);
                                     // {  0, r30,   0, r20,   0, r10,   0, r00}
               #ifdef BETAX
                  ATL_vmul(rC30, rC30, vBE);
                  ATL_vmul(rC20, rC20, vBE);
               #endif
               ATL_vadd(rC10, rC10, rC30);
               ATL_vadd(rC00, rC00, rC20);
               ATL_vust(pC0+ldc2, rC10);
               ATL_vust(pC0, rC00);
            #endif
            #ifdef BETA0
               ATL_vvrsum8(rC02,rC12,rC22,rC32, rC03,rC13,rC23,rC33);
               wrtC(pC2, rC02);
            #else
               ATL_vvrsum8(rC02,rC12, rC03, rC13, rC22,rC32, rC23,rC33);
                                     //    7    6    5    4    3    2    1    0
                                     // {r31, r21, r30, r20, r11, r01, r10, r00}
               ATL_vuld(rC30, pC2+ldc2);
                                     // {i31, r31, i21, r21, i11, r11, i01, r01}
               ATL_vuld(rC20, pC2);
                                     // {i30, r30, i20, r20, i10, r10, i00, r00}
               ATL_vunpckHI(rC10, rC02, rB0);
                                     // {  0, r31,   0, r21,   0, r11,   0, r01}
               ATL_vunpckLO(rC00, rC02, rB0);
                                     // {  0, r30,   0, r20,   0, r10,   0, r00}
               #ifdef BETAX
                  ATL_vmul(rC30, rC30, vBE);
                  ATL_vmul(rC20, rC20, vBE);
               #endif
               ATL_vadd(rC10, rC10, rC30);
               ATL_vadd(rC00, rC00, rC20);
               ATL_vust(pC2+ldc2, rC10);
               ATL_vust(pC2, rC00);
            #endif
            ATL_vvrsum8(rC04,rC14,rC24,rC34, rC05,rC15,rC25,rC35);
            wrtC(pC4, rC04);
         }
         #elif ATL_VLEN == 8 && defined(ATL_AVX) && defined(ATL_SSE1) \
                && defined(SREAL)
         {
            ATL_vvrsum8(rC00, rC10, rC20, rC30, rC01, rC11, rC21, rC31);
            #ifndef BETA0
               rC10 = _mm256_insertf128_ps(rC10, _mm_loadu_ps(pC0), 0);
               rC10 = _mm256_insertf128_ps(rC10, _mm_loadu_ps(pC0+ldc2), 1);
               #ifdef BETAX
                  ATL_vmul(rC10, rC10, vBE);
               #endif
               ATL_vadd(rC00, rC00, rC10);
            #endif
            _mm_storeu_ps(pC0, _mm256_extractf128_ps(rC00, 0));
            _mm_storeu_ps(pC0+ldc2, _mm256_extractf128_ps(rC00, 1));
            ATL_vvrsum8(rC02, rC12, rC22, rC32, rC03, rC13, rC23, rC33);
            #ifndef BETA0
               rC10 = _mm256_insertf128_ps(rC10, _mm_loadu_ps(pC2), 0);
               rC10 = _mm256_insertf128_ps(rC10, _mm_loadu_ps(pC2+ldc2), 1);
               #ifdef BETAX
                  ATL_vmul(rC10, rC10, vBE);
               #endif
               ATL_vadd(rC02, rC02, rC10);
            #endif
            _mm_storeu_ps(pC2, _mm256_extractf128_ps(rC02, 0));
            _mm_storeu_ps(pC2+ldc2, _mm256_extractf128_ps(rC02, 1));
            ATL_vvrsum8(rC04, rC14, rC24, rC34, rC05, rC15, rC25, rC35);
            #ifndef BETA0
               rC10 = _mm256_insertf128_ps(rC10, _mm_loadu_ps(pC4), 0);
               rC10 = _mm256_insertf128_ps(rC10, _mm_loadu_ps(pC4+ldc2), 1);
               #ifdef BETAX
                  ATL_vmul(rC10, rC10, vBE);
               #endif
               ATL_vadd(rC04, rC04, rC10);
            #endif
            _mm_storeu_ps(pC4, _mm256_extractf128_ps(rC04, 0));
            _mm_storeu_ps(pC4+ldc2, _mm256_extractf128_ps(rC04, 1));
         }
         #else
            #error "VLEN NOT SUPPORTED!"
         #endif
         pB0 = B;
         pB2 = B   + (ldb<<1);
         pC0 += 4 SHIFT;
         pC2 += 4 SHIFT;
         pC4 += 4 SHIFT;
         pB4 = pB2 + (ldb<<1);
         A += incAm;
         pA0 = A;
         pA2 = pA0 + (lda<<1);
      }  /* end of loop over M */
      A = aa;
      pA0 = A;
      pA2 = A   + (lda<<1);
      C += incC;
      pC0 = C;
      B += incBn;
      pB0 = B;
      pC2 =   C + (ldc2<<1);
      pB2 =   B + (ldb<<1);
      pB4 = pB2 + (ldb<<1);
      pC4 = pC2 + (ldc2<<1);
   }  /* end of loop over N */
}
