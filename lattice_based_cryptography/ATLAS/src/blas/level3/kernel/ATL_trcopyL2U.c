/*
 *             Automatically Tuned Linear Algebra Software v3.10.3
 *                    (C) Copyright 1997 R. Clint Whaley
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
#include "atlas_kern3.h"

#ifdef TREAL

#ifdef SDREAL
   #ifdef UnitDiag_
      void Mjoin(ATL_sdtrcopyL2U_U,NM)
   #else
      void Mjoin(ATL_sdtrcopyL2U_N,NM)
   #endif
   (const int N, const double alpha0, const float *A, const int lda, double *C)
#else
   #ifdef UnitDiag_
      void Mjoin(Mjoin(PATL,trcopyL2U_U),NM)
   #else
      void Mjoin(Mjoin(PATL,trcopyL2U_N),NM)
   #endif
   (const int N, const SCALAR alpha0, const TYPE *A, const int lda, TYPE *C)
#endif
/*
 * Takes a Lower matrix, transposes it, and copies it to a dense matrix with
 * zeros below the diagonal (i.e., upper form)
 */
{
   int i, j;
   const register TYPE alpha=alpha0;
   const TYPE *Ar;

   if (N > 1)
   {
      for (j=0; j != N; j++)
      {
         Ar = A + j;
         for (i=0; i != j; i++, Ar += lda) C[i] = ATL_MulByALPHA(*Ar);
         #ifdef UnitDiag_
            C[j] = alpha;
         #else
            C[j] = ATL_MulByALPHA(*Ar);
         #endif
         for (i=j+1; i < N; i++) C[i] = 0.0;
         C += N;
      }
   }
   else if (N == 1)
   {
      #ifdef UnitDiag_
         *C = alpha;
      #else
         *C = ATL_MulByALPHA(*A);
      #endif
   }
}

#else

#ifdef SDCPLX
   #ifdef UnitDiag_
      #ifdef ConjTrans_
         void ATL_cztrcopyL2Uc_U
      #else
         void ATL_cztrcopyL2U_U
      #endif
   #else
      #ifdef ConjTrans_
         void ATL_cztrcopyL2Uc_N
      #else
         void ATL_cztrcopyL2U_N
      #endif
   #endif
   (const int N, const float *A, const int lda, double *C)
#else
   #ifdef UnitDiag_
      #ifdef ConjTrans_
         void Mjoin(PATL,trcopyL2Uc_U)
      #else
         void Mjoin(PATL,trcopyL2U_U)
      #endif
   #else
      #ifdef ConjTrans_
         void Mjoin(PATL,trcopyL2Uc_N)
      #else
         void Mjoin(PATL,trcopyL2U_N)
      #endif
   #endif
   (const int N, const TYPE *A, const int lda, TYPE *C)
#endif
/*
 * Takes a Lower matrix, transposes it, and copies it to a dense matrix with
 * zeros below the diagonal (i.e., upper form)
 */
{
   int i, j2;
   const int N2=N<<1, lda2=lda<<1;
   #define ldc N2
   const TYPE *a;
   const TYPE zero = 0.0;
   #ifdef UnitDiag_
      const TYPE one = 1.0;
   #endif

   for (j2=0; j2 != N2; j2 += 2)
   {
      a = A;
      for (i=0; i != j2; i += 2)
      {
         C[i] = *a;
         #ifdef ConjTrans_
            C[i+1] = -a[1];
         #else
            C[i+1] = a[1];
         #endif
         a += lda2;
      }
      #ifdef UnitDiag_
         C[j2] = one;
         C[j2+1] = zero;
      #else
         C[i] = *a;
         #ifdef ConjTrans_
            C[i+1] = -a[1];
         #else
            C[i+1] = a[1];
         #endif
      #endif
      if (j2 != N2) for (i=j2+2; i != N2; i += 2) C[i] = C[i+1] = zero;
      A += 2;
      C += ldc;
   }
}

#endif
