/*
 *             Automatically Tuned Linear Algebra Software v3.10.3
 *                   (C) Copyright 1999 Antoine P. Petitet
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
#include "atlas_tst.h"
#include "atlas_level1.h"

TYPE Mjoin(PATL,tbnrm1)(const enum ATLAS_UPLO UPLO, const enum ATLAS_DIAG DIAG,
                        const int N, const int K, const TYPE *A, const int LDA)
/*
 * Calculates the 1-norm of a triangular band matrix
 */
{
   int i, i0, i1, iaij, j, jaj, l, lda2 = ( LDA SHIFT );
   TYPE max=0.0, t0;

   if( UPLO == AtlasUpper )
   {
      for( j = 0, jaj  = 0; j < N; j++, jaj += lda2 )
      {
         l       = K - j;
         i0      = ( j - K > 0 ? j - K : 0 );

         t0 = ATL_rzero;
         for( i = i0, iaij = ((l+i0) SHIFT)+jaj; i < j; i++, iaij += (1 SHIFT) )
         {
#ifdef TREAL
            t0 += Mabs( A[iaij] );
#else
            t0 += Mabs( A[iaij] ) + Mabs( A[iaij+1] );
#endif
         }
         if (t0 != t0)
            return(t0);
         if( DIAG == AtlasNonUnit ) t0 += ATL_rone;

         if (t0 > max) max = t0;
      }
   }
   else
   {
      for( j = N-1, jaj = (N-1)*lda2; j >= 0; j--, jaj -= lda2 )
      {
         t0 = ATL_rzero;
         if( DIAG == AtlasNonUnit ) t0 += ATL_rone;

         i1   = ( N - 1 > j + K ? j + K : N - 1 );
         for( i  = j+1, iaij = (1 SHIFT)+jaj; i <= i1; i++, iaij += (1 SHIFT) )
         {
#ifdef TREAL
            t0 += Mabs( A[iaij] );
#else
            t0 += Mabs( A[iaij] ) + Mabs( A[iaij+1] );
#endif
         }
         if (t0 != t0)
            return(t0);
         if (t0 > max) max = t0;
      }
   }
   return(max);
}
