/*
 *             Automatically Tuned Linear Algebra Software v3.10.3
 *                    (C) Copyright 1999 R. Clint Whaley
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

#define SCPLX
#include "atlas_misc.h"
#include "cblas.h"
#ifdef ATL_USEPTHREADS
   #include "atlas_ptalias2.h"
#endif
#include "atlas_level2.h"

void cblas_chbmv(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                 const int N, const int K, const void *alpha, const void *A,
                 const int lda, const void *X, const int incX,
                 const void *beta, void *Y, const int incY)
{
   int info = 2000;
   const enum CBLAS_UPLO ruplo = (Uplo == CblasUpper) ? CblasLower : CblasUpper;
   void *vx;
   float *X0, *x = (float*) X;
   float *y = Y;
   const float *alp=alpha;
   const float *bet=beta;
   float calpha[2], cbeta[2];
   const float one[2] = {ATL_rone, ATL_rzero};
   calpha[0] = *alp;
   calpha[1] = -alp[1];
   cbeta[0] = *bet;
   cbeta[1] = -bet[1];

#ifndef NoCblasErrorChecks
   if (Order != CblasColMajor && Order != CblasRowMajor)
      info = cblas_errprn(1, info, "Order must be %d or %d, but is set to %d",
                          CblasRowMajor, CblasColMajor, Order);
   if (Uplo != CblasUpper && Uplo != CblasLower)
      info = cblas_errprn(2, info,
                          "Uplo must be %d or %d, but is set to %d",
                          CblasUpper, CblasLower, Uplo);

   if (N < 0) info = cblas_errprn(3, info,
                        "N cannot be less than zero; is set to %d.", N);
   if (K < 0)
      info = cblas_errprn(4, info, "Valid K: 0 < K < N; K=%d, N=%d.", K, N);
   if (lda < K+1) info = cblas_errprn(7, info,
      "lda cannot be less than K+1;  K=%d, lda=%d\n", K, lda);
   if (!incX) info = cblas_errprn(9, info,
                                  "incX cannot be zero; is set to %d.", incX);
   if (!incY) info = cblas_errprn(12, info,
                                  "incY cannot be zero; is set to %d.", incY);
   if (info != 2000)
   {
      cblas_xerbla(info, "cblas_chbmv", "");
      return;
   }
#endif

   if (incX < 0) x += (1-N)*incX<<1;
   if (incY < 0) y += (1-N)*incY<<1;
   if (Order == CblasColMajor)
      ATL_chbmv(Uplo, N, K, alpha, A, lda, x, incX, beta, y, incY);
   else
   {
      vx = malloc(ATL_Cachelen + 2*N*sizeof(float));
      ATL_assert(vx);
      X0 = x;
      x = ATL_AlignPtr(vx);
      ATL_cmoveConj(N, calpha, X0, incX, x, 1);
      if (*bet != ATL_rzero || bet[1] != ATL_rzero)
      {
         ATL_cscalConj(N, cbeta, y, incY);
         ATL_chbmv(ruplo, N, K, one, A, lda, x, 1, one, y, incY);
      }
      else ATL_chbmv(ruplo, N, K, one, A, lda, x, 1, beta, y, incY);
      free(vx);
      ATL_cscalConj(N, one, y, incY);
   }
}
