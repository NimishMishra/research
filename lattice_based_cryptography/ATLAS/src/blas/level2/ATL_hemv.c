/*
 *             Automatically Tuned Linear Algebra Software v3.10.3
 * Copyright (C) 2010, 2012 R. Clint Whaley
 *
 * Code contributers : R. Clint Whaley, Antoine P. Petitet
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions, and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the ATLAS group or the names of its contributers may
 *      not be used to endorse or promote products derived from this
 *      software without specific written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE ATLAS GROUP OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "atlas_misc.h"
#include "atlas_level1.h"
#include "atlas_kernel2.h"
#include "atlas_lvl2.h"

#include "atlas_reflvl2.h"          /* temporary for building purposes */
#include "atlas_reflevel2.h"        /* used for gbmv, gpmv and gpr.    */
#include Mstr(Mjoin(Mjoin(atlas_,PRE),sysinfo.h))
#include "atlas_cacheedge.h"

#ifdef CacheEdge
   #if !defined(CacheEdge) || CacheEdge == 0
      #define MY_CE (4*ATL_MulBySize(ATL_L1elts))
   #else
      #define MY_CE CacheEdge
   #endif
#else
   #define MY_CE (4*ATL_MulBySize(ATL_L1elts))
#endif

/*
 * Guess as to whether GEMV-based SYRK will beat ref.  Will on any arch
 * where the compiler is not too whoopy (ARM) or where vectorization provides
 * faster memory load as well as computation (x86 with vectorization).
 * Single precision complex has enough flops/load to be almost always
 * compute-bound enough that the auto-GEMV-tuning outweighs the double load.
 */
#if defined(ATL_GAS_ARM)
   #define USE_GEMV_BASED 1
#elif defined(SREAL)
   #ifdef ATL_SSE1
      #define USE_GEMV_BASED 1
   #endif
#elif defined(DREAL)
   #ifdef ATL_SSE2
      #define USE_GEMV_BASED 1
   #endif
#elif defined(SCPLX)
   #define USE_GEMV_BASED 1
#elif defined(DCPLX)
   #ifdef ATL_SSE2
      #define USE_GEMV_BASED 1
   #endif
#endif

#ifdef USE_GEMV_BASED
typedef void (*ATL_symvK_t)
   (const enum ATLAS_UPLO, const int, const SCALAR, const TYPE*, const int,
    const TYPE*, const int, const SCALAR, TYPE*, const int);

static void ATL_symvL
(
   ATL_symvK_t symvK,
   const int NB,
   ATL_CINT N,
   const TYPE *A,
   ATL_CINT lda,
   const TYPE *x,
   TYPE  *y,
   const TYPE *xt,
   TYPE  *yt
)
{
   const TYPE one[2] = {ATL_rone, ATL_rzero};
   ATL_INT Mmb2;
   ATL_INT Mmb, mr, MB, j;
   const size_t incA = (NB SHIFT)*lda;
   const size_t opsize = ((size_t)(N+8)*(N+4))*(sizeof(TYPE)>>1)SHIFT;
   void (*gemvT)(ATL_CINT, ATL_CINT, const SCALAR, const TYPE*, ATL_CINT,
                 const TYPE*, ATL_CINT, const SCALAR, TYPE*, ATL_CINT);
   void (*gemvN)(ATL_CINT, ATL_CINT, const SCALAR, const TYPE*, ATL_CINT,
                 const TYPE*, ATL_CINT, const SCALAR, TYPE*, ATL_CINT);

   if (opsize > MY_CE)
   {
      gemvT = Mjoin(PATL,gemvT);
      gemvN = Mjoin(PATL,gemvN_L2);
   }
   else if (opsize <= ATL_MulBySize(ATL_L1elts))
   {
      gemvT = Mjoin(PATL,gemvT_L1);
      gemvN = Mjoin(PATL,gemvN_L1);
   }
   else
   {
      gemvT = Mjoin(PATL,gemvT_L2);
      gemvN = Mjoin(PATL,gemvN_L2);
   }
/*
 * Choose MB such that A is retained in L2 cache for second GEMV call
 * If partial block is tiny, absorbe it into last block since cache is not
 * precise anyway.
 */
   MB = ATL_DivBySize(MY_CE) / NB;
   MB = (MB > N || MB < 240) ? N : MB;
   Mmb2 = Mmb+Mmb;
   for (j=0; j < N; j += NB, A += incA)
   {
      const register size_t j2 = j+j;
      register int i, nb=N-j;
      nb = (nb >= NB) ? NB : nb;
      symvK(AtlasLower, nb, one, A+j2, lda, x+j2, 1, one, y+j2, 1);
      for (i=j+nb; i < N; i += MB)
      {
         const register size_t i2 = i+i;
         register int mb = N-i;
         mb = (mb >= MB) ? MB : mb;
         gemvT(mb, nb, one, A+i2, lda, xt+i2, 1, one, yt+j2, 1);
         gemvN(mb, nb, one, A+i2, lda, x+j2, 1, one, y+i2, 1);
      }
   }
}
static void ATL_symvU
(
   ATL_symvK_t symvK,
   const int NB,
   ATL_CINT N,
   const TYPE *A,
   ATL_CINT lda,
   const TYPE *x,
   TYPE  *y,
   const TYPE *xt,
   TYPE  *yt
)
{
   const TYPE one[2] = {ATL_rone, ATL_rzero};
   ATL_INT Mmb2;
   ATL_INT Mmb, mr, MB, j;
   const size_t incA = (NB SHIFT)*lda;
   const size_t opsize = ((size_t)(N+8)*(N+4))*(sizeof(TYPE)>>1)SHIFT;
   void (*gemvT)(ATL_CINT, ATL_CINT, const SCALAR, const TYPE*, ATL_CINT,
                 const TYPE*, ATL_CINT, const SCALAR, TYPE*, ATL_CINT);
   void (*gemvN)(ATL_CINT, ATL_CINT, const SCALAR, const TYPE*, ATL_CINT,
                 const TYPE*, ATL_CINT, const SCALAR, TYPE*, ATL_CINT);

   if (opsize > MY_CE)
   {
      gemvT = Mjoin(PATL,gemvT);
      gemvN = Mjoin(PATL,gemvN_L2);
   }
   else if (opsize <= ATL_MulBySize(ATL_L1elts))
   {
      gemvT = Mjoin(PATL,gemvT_L1);
      gemvN = Mjoin(PATL,gemvN_L1);
   }
   else
   {
      gemvT = Mjoin(PATL,gemvT_L2);
      gemvN = Mjoin(PATL,gemvN_L2);
   }
/*
 * Choose MB such that A is retained in L2 cache for second GEMV call
 * If partial block is tiny, absorbe it into last block since cache is not
 * precise anyway.
 */
   MB = ATL_DivBySize(MY_CE) / NB;
   MB = (MB > N || MB < 240) ? N : MB;
   Mmb2 = Mmb+Mmb;
   for (j=0; j < N; j += NB, A += incA)
   {
      const register size_t j2 = j+j;
      register int i, nb=N-j;
      nb = (nb >= NB) ? NB : nb;
      for (i=0; i < j; i += MB)
      {
         const register size_t i2 = i+i;
         register int mb = j-i;
         mb = (mb >= MB) ? MB : mb;
         gemvT(mb, nb, one, A+i2, lda, xt+i2, 1, one, yt+j2, 1);
         gemvN(mb, nb, one, A+i2, lda, x+j2, 1, one, y+i2, 1);
      }
      symvK(AtlasUpper, nb, one, A+j2, lda, x+j2, 1, one, y+j2, 1);
   }
}
#endif

void Mjoin(PATL,hemv)
(
   const enum ATLAS_UPLO Uplo,
   const int             N,
   const SCALAR          alpha,
   const TYPE            *A,
   const int             lda,
   const TYPE            *X,
   const int             incX,
   const SCALAR          beta,
   TYPE                  *Y,
   const int             incY
)
/*
 * Purpose
 * =======
 *
 * Mjoin( PATL, hemv ) performs the matrix-vector operation
 *
 *    y := alpha * A * x + beta * y,
 *
 * where alpha and beta are scalars, x and y are n-element vectors and A
 * is an n by n Hermitian matrix.
 *
 * This is a blocked version of the algorithm.  For a more detailed des-
 * cription of  the arguments of this function, see the reference imple-
 * mentation in the ATLAS/src/blas/reference directory.
 *
 * ---------------------------------------------------------------------
 */
{
   const int BETA0 = (*beta == ATL_rzero && beta[1] == ATL_rzero);
   const int BETA1 = (*beta == ATL_rone && beta[1] == ATL_rzero);
   const int ALPHA1 = (*alpha == ATL_rone && alpha[1] == ATL_rzero);
   const int ALPHA0 = (*alpha == ATL_rzero && alpha[1] == ATL_rzero);
   if (N <= 0 || (ALPHA0 && BETA1))
      return;
   if (ALPHA0)
   {
      if (BETA0)
         Mjoin(PATL,zero)(N, Y, incY);
      else
         Mjoin(PATL,scal)(N, beta, Y, incY);
      return;
   }
#ifdef USE_GEMV_BASED
   if (N >= 240)
   {
      void *vp=NULL;
      TYPE *x=(TYPE*)X, *y=Y, *xh, *yh;
      const size_t tX = (size_t)X, tY = (size_t)Y, N2 = N+N;
      const int COPYY = !(incY == 1 &&
                          (ATL_MulByCachelen(ATL_DivByCachelen(tY)) == tY));
      const int COPYX = !(incX == 1 && (COPYY || ALPHA1) &&
                          (ATL_MulByCachelen(ATL_DivByCachelen(tX)) == tX));
      const TYPE one[2] = {ATL_rone, ATL_rzero};
      const TYPE *calp=one, *cbet=one;
      TYPE *tp;
      tp = vp = malloc((COPYX+COPYY+2)*(ATL_Cachelen+ATL_MulBySize(N)));
      if (!vp)
      {
         Mjoin(PATL,refhemv)(Uplo, N, alpha, A, lda, X, incX, beta, Y, incY);
         return;
      }
      yh = ATL_AlignPtr(tp);
      Mjoin(PATL,zero)(N, yh, 1);
      tp = yh + N2;
      xh = ATL_AlignPtr(tp);
      tp = xh + N2;
      if (COPYX)
      {
         x = ATL_AlignPtr(tp);
         if (COPYY || ALPHA1)
         {
            register ATL_INT i;
            register const size_t incX2 = incX+incX;
            const TYPE *xx=X;
            for (i=0; i < N2; i += 2, xx += incX2)
            {
               xh[i] = x[i] = *xx;
               xh[i+1] = -(x[i+1] = xx[1]);
            }
         }
         else if (alpha[1] == ATL_rzero)
         {
            register ATL_INT i;
            register const size_t incX2 = incX+incX;
            register const TYPE ra=(*alpha), ia=alpha[1];
            const TYPE *xx=X;
            for (i=0; i < N2; i += 2, xx += incX2)
            {
               register TYPE rx = *xx, ix = xx[1];
               x[i] = rx*ra;
               x[i+1] = ix*ra;
               xh[i] = rx;
               xh[i+1] = -ix;
            }
         }
         else
         {
            register ATL_INT i;
            register const size_t incX2 = incX+incX;
            register const TYPE ra=(*alpha), ia=alpha[1];
            const TYPE *xx=X;
            for (i=0; i < N2; i += 2, xx += incX2)
            {
               register TYPE rx = *xx, ix = xx[1];
               x[i] = rx*ra - ix*ia;
               x[i+1] = rx*ia + ix*ra;
               xh[i] = rx;
               xh[i+1] = -ix;
            }
         }
         tp = x + N2;
      }
      else
         Mjoin(PATL,copyConj)(N, X, incX, xh, 1);
      if (COPYY)
      {
         calp = alpha;
         cbet = beta;
         y = ATL_AlignPtr(tp);
         Mjoin(PATL,zero)(N, y, 1);
      }
      else if (BETA0)
         Mjoin(PATL,zero)(N, y, 1);
      else if (!BETA1)
         Mjoin(PATL,scal)(N, beta, y, 1);
      if (Uplo == AtlasLower)
         ATL_symvL(Mjoin(PATL,refhemv), 120, N, A, lda, x, y, xh, yh);
      else
         ATL_symvU(Mjoin(PATL,refhemv), 120, N, A, lda, x, y, xh, yh);
      if (COPYY)
      {
         Mjoin(PATL,axpbyConj)(N, alpha, yh, 1, calp, y, 1);
         Mjoin(PATL,axpby)(N, one, y, 1, cbet, Y, incY);
      }
      else
         Mjoin(PATL,axpyConj)(N, alpha, yh, 1, Y, incY);
      free(vp);
      return;
   }
#endif
   Mjoin(PATL,refhemv)(Uplo, N, alpha, A, lda, X, incX, beta, Y, incY);
}
