/* ---------------------------------------------------------------------
 *
 * -- Automatically Tuned Linear Algebra Software (ATLAS)
 *    (C) Copyright 2000 All Rights Reserved
 *
 * -- ATLAS routine -- Version 3.9.24 -- December 25, 2000
 *
 * Author         : Antoine P. Petitet
 * Originally developed at the University of Tennessee,
 * Innovative Computing Laboratory, Knoxville TN, 37996-1301, USA.
 *
 * ---------------------------------------------------------------------
 *
 * -- Copyright notice and Licensing terms:
 *
 *  Redistribution  and  use in  source and binary forms, with or without
 *  modification, are  permitted provided  that the following  conditions
 *  are met:
 *
 * 1. Redistributions  of  source  code  must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce  the above copyright
 *    notice,  this list of conditions, and the  following disclaimer in
 *    the documentation and/or other materials provided with the distri-
 *    bution.
 * 3. The name of the University,  the ATLAS group,  or the names of its
 *    contributors  may not be used to endorse or promote products deri-
 *    ved from this software without specific written permission.
 *
 * -- Disclaimer:
 *
 * THIS  SOFTWARE  IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,  INCLUDING,  BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,  INDIRECT, INCIDENTAL, SPE-
 * CIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO,  PROCUREMENT  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEO-
 * RY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  (IN-
 * CLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ---------------------------------------------------------------------
 */
/*
 * Include files
 */
#include "atlas_refmisc.h"
#include "atlas_reflvl2.h"
#include "atlas_reflevel2.h"

void ATL_dreftbmv
(
   const enum ATLAS_UPLO      UPLO,
   const enum ATLAS_TRANS     TRANS,
   const enum ATLAS_DIAG      DIAG,
   const int                  N,
   const int                  K,
   const double               * A,
   const int                  LDA,
   double                     * X,
   const int                  INCX
)
{
/*
 * Purpose
 * =======
 *
 * ATL_dreftbmv performs one of the matrix-vector operations
 *
 *    x := A * x,   or   x := A'*x,
 *
 * where x is an n-element vector and  A is an n by n unit, or non-unit,
 * upper or lower triangular band matrix, with ( k + 1 ) diagonals.
 *
 * Arguments
 * =========
 *
 * UPLO    (input)                       const enum ATLAS_UPLO
 *         On entry, UPLO  specifies whether  the  matrix is an upper or
 *         lower triangular matrix as follows:
 *
 *             UPLO = AtlasUpper   A is an upper triangular matrix.
 *
 *             UPLO = AtlasLower   A is a lower triangular matrix.
 *
 *         Unchanged on exit.
 *
 * TRANS   (input)                       const enum ATLAS_TRANS
 *         On entry,  TRANS  specifies the  operation to be performed as
 *         follows:
 *
 *            TRANS = AtlasNoTrans     x := A *x,
 *
 *            TRANS = AtlasConj        x := A *x,
 *
 *            TRANS = AtlasTrans       x := A'*x,
 *
 *            TRANS = AtlasConjTrans   x := A'*x.
 *
 *         Unchanged on exit.
 *
 * DIAG    (input)                       const enum ATLAS_DIAG
 *         On entry, DIAG specifies whether or not A is unit triangu-
 *         lar as follows:
 *
 *            DIAG = AtlasUnit       A is assumed to be unit triangular,
 *
 *            DIAG = AtlasNonUnit    A is not assumed to be unit trian-
 *                                   gular.
 *
 *         Unchanged on exit.
 *
 * N       (input)                       const int
 *         On entry, N specifies the order of the matrix A. N must be at
 *         least zero. Unchanged on exit.
 *
 * K       (input)                       const int
 *         On entry, with UPLO = AtlasUpper,  K  specifies the number of
 *         super-diagonals of the matrix A.  With  UPLO = AtlasLower,  K
 *         specifies the number of sub-diagonals of the matrix A. K must
 *         satisfy  0 <= K. Unchanged on exit.
 *
 * A       (input)                       const double *
 *         On entry,  A  points  to an array of size equal to or greater
 *         than   LDA * n * sizeof(   double  ).   Before   entry   with
 *         UPLO = AtlasUpper, the leading (k + 1) by n part of the array
 *         A  must  contain the upper triangular band part of the matrix
 *         of coefficients, supplied column  by column, with the leading
 *         diagonal  of the matrix in row k of the  array, the first su-
 *         per-diagonal starting at position 1 in row  k-1,  and  so on.
 *         The top left k by k triangle of the array  A  is not referen-
 *         ced.  The  following  program segment  will transfer an upper
 *         triangular band matrix from conventional full  matrix storage
 *         to band storage:
 *
 *            for( j = 0; j < n; j++ )
 *            {
 *               m  = k - j;
 *               for( i = ( m < 0 ? -m : 0 ); i < j; i++ )
 *               {
 *                  a[m+i+j*LDA] = matrix( i, j );
 *               }
 *            }
 *
 *         Before entry with UPLO = AtlasLower, the leading (k + 1) by n
 *         part of the array  A  must contain the lower triangular  band
 *         part  of  the  matrix of coefficients, supplied column by co-
 *         lumn, with the leading diagonal of the matrix in row 0 of the
 *         array,  the  first sub-diagonal starting at position 0 in row
 *         1, and so on. The bottom right k by k triangle of the array A
 *         is not referenced.  The following program segment will trans-
 *         fer a lower  real triangular  band  matrix from  conventional
 *         full matrix storage to band storage:
 *
 *            for( j = 0; j < n; j++ )
 *            {
 *               i1 = ( n > j + k + 1 ? j + k + 1 : n );
 *               for( i = j; i < i1; i++ )
 *               {
 *                  a[i-j+j*LDA] = matrix( i, j );
 *               }
 *            }
 *
 *         Note that when DIAG = AtlasUnit  the elements of the array  A
 *         corresponding to the diagonal elements of the matrix are  not
 *         referenced, but are assumed to be unity. Unchanged on exit.
 *
 * LDA     (input)                       const int
 *         On entry, LDA  specifies the leading dimension of A as decla-
 *         red  in  the  calling (sub) program.  LDA  must  be  at least
 *         k + 1. Unchanged on exit.
 *
 * X       (input/output)                double *
 *         On entry,  X  points to the  first entry to be accessed of an
 *         incremented array of size equal to or greater than
 *            ( 1 + ( n - 1 ) * abs( INCX ) ) * sizeof(   double  ),
 *         that contains the vector x.  On exit,  X  is overwritten with
 *         the tranformed vector x.
 *
 * INCX    (input)                       const int
 *         On entry, INCX specifies the increment for the elements of X.
 *         INCX must not be zero. Unchanged on exit.
 *
 * ---------------------------------------------------------------------
 */
/* ..
 * .. Executable Statements ..
 *
 */
   if( N == 0 ) return;

   if( UPLO == AtlasUpper )
   {
      if( ( TRANS == AtlasNoTrans ) || ( TRANS == AtlasConj ) )
      {
         if( DIAG == AtlasNonUnit )
         {
            ATL_dreftbmvUNN( N, K, A, LDA, X, INCX );
         }
         else
         {
            ATL_dreftbmvUNU( N, K, A, LDA, X, INCX );
         }
      }
      else
      {
         if( DIAG == AtlasNonUnit )
         {
            ATL_dreftbmvUTN( N, K, A, LDA, X, INCX );
         }
         else
         {
            ATL_dreftbmvUTU( N, K, A, LDA, X, INCX );
         }
      }
   }
   else
   {
      if( ( TRANS == AtlasNoTrans ) || ( TRANS == AtlasConj ) )
      {
         if( DIAG == AtlasNonUnit )
         {
            ATL_dreftbmvLNN( N, K, A, LDA, X, INCX );
         }
         else
         {
            ATL_dreftbmvLNU( N, K, A, LDA, X, INCX );
         }
      }
      else
      {
         if( DIAG == AtlasNonUnit )
         {
            ATL_dreftbmvLTN( N, K, A, LDA, X, INCX );
         }
         else
         {
            ATL_dreftbmvLTU( N, K, A, LDA, X, INCX );
         }
      }
   }
/*
 * End of ATL_dreftbmv
 */
}
