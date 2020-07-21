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

TYPE Mjoin(PATL,sbnrm)
(const enum ATLAS_UPLO UPLO, const int N, const int K,
 const TYPE *A, const int LDA)
{
   int                        i, i0, i1, iaij, iy, j, jaj, ky = 0, l,
                              lda2 = (LDA SHIFT);
   TYPE max=ATL_rzero, t0, * work= NULL;

   if( N <= 0 ) return( ATL_rzero );

   work = (TYPE *)malloc( N * sizeof( TYPE ) );
   if( work == NULL )
   {fprintf( stderr, "mem alloc failed in [sb,hb]nrm, bye ...\n" ); exit( 1 );}
   else { for( i = 0; i < N; i++ ) work[i] = ATL_rzero; }

   if( UPLO == AtlasUpper )
   {
      for( j = 0, jaj = 0; j < N; j++, jaj += lda2 )
      {
         t0      = ATL_rzero;

         l     = K - j;
         i0    = ( j - K > 0 ? j - K : 0 );

         for( i = i0, iaij  = ((l+i0) SHIFT)+jaj, iy = ky;
              i < j;  i++, iaij += (1 SHIFT), iy += 1 )
         {
#ifdef TREAL
            work[iy] += Mabs( A[iaij] );
            t0       += Mabs( A[iaij] );
#else
            work[iy] += Mabs( A[iaij] ) + Mabs( A[iaij+1] );
            t0       += Mabs( A[iaij] ) + Mabs( A[iaij+1] );
#endif
         }
#ifdef TREAL
         work[j] += Mabs( A[iaij] ) + t0;
#else
         work[j] += Mabs( A[iaij] ) + Mabs( A[iaij+1] ) + t0;
#endif

         if( j >= K ) { ky += 1; }
      }
   }
   else
   {
      for( j = 0, jaj = 0; j < N; j++, jaj += lda2 )
      {
         t0      = ATL_rzero;
#ifdef TREAL
         work[j] = Mabs( A[jaj] );
#else
         work[j] = Mabs( A[jaj] ) + Mabs( A[jaj+1] );
#endif
         i1     = ( N - 1 > j + K ? j + K : N - 1 );
         for( i = j+1, iaij = (1 SHIFT)+jaj; i <= i1; i++,
              iaij += (1 SHIFT) )
         {
#ifdef TREAL
            work[i] += Mabs( A[iaij] );
            t0      += Mabs( A[iaij] );
#else
            work[i] += Mabs( A[iaij] ) + Mabs( A[iaij+1] );
            t0      += Mabs( A[iaij] ) + Mabs( A[iaij+1] );
#endif
         }
         work[j] += t0;
      }
   }

   max = work[0];
   for( j = 1; j < N; j++ )
      if (work[j] != work[j] || max < work[j])
         max = work[j];
   if( work ) free( work );

   return( max );
}
