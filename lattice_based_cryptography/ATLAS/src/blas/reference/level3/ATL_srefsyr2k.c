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
#include "atlas_reflvl3.h"
#include "atlas_reflevel3.h"

void ATL_srefsyr2k
(
   const enum ATLAS_UPLO      UPLO,
   const enum ATLAS_TRANS     TRANS,
   const int                  N,
   const int                  K,
   const float                ALPHA,
   const float                * A,
   const int                  LDA,
   const float                * B,
   const int                  LDB,
   const float                BETA,
   float                      * C,
   const int                  LDC
)
{
/*
 * Purpose
 * =======
 *
 * ATL_srefsyr2k  performs one of the symmetric rank 2k operations
 *
 *    C := alpha * A * B' + alpha * B * A' + beta * C,
 *
 * or
 *
 *    C := alpha * A' * B + alpha * B' * A + beta * C,
 *
 * where alpha and beta are scalars, C is an n by n symmetric matrix and
 * A and B are n by k matrices in the first case and k by n  matrices in
 * the second case.
 *
 * Arguments
 * =========
 *
 * UPLO    (input)                       const enum ATLAS_UPLO
 *         On entry, UPLO  specifies whether the upper or lower triangu-
 *         lar part of the array C is to be referenced as follows:
 *
 *             UPLO = AtlasUpper   Only the upper triangular part of C
 *                                 is to be referenced.
 *
 *             UPLO = AtlasLower   Only the lower triangular part of C
 *                                 is to be referenced.
 *
 *         Unchanged on exit.
 *
 * TRANS   (input)                       const enum ATLAS_TRANS
 *         On entry,  TRANS  specifies the  operation to be performed as
 *         follows:
 *
 *            TRANS = AtlasNoTrans    C := alpha*A*B' + alpha*B*A' +
 *                                         beta*C,
 *
 *            TRANS = AtlasTrans      C := alpha*B'*A + alpha*A'*B +
 *                                         beta*C,
 *
 *            TRANS = AtlasConjTrans  C := alpha*B'*A + alpha*A'*B +
 *                                         beta*C.
 *
 *         Unchanged on exit.
 *
 * N       (input)                       const int
 *         On entry, N specifies the order of the matrix C. N must be at
 *         least zero. Unchanged on exit.
 *
 * K       (input)                       const int
 *         On entry, with TRANS = AtlasNoTrans,  K  specifies the number
 *         of columns of the matrices A and B, and otherwise K specifies
 *         the  number  of  rows of the matrices  A and B.  K must be at
 *         least zero. Unchanged on exit.
 *
 * ALPHA   (input)                       const float
 *         On entry, ALPHA specifies the scalar alpha.   When  ALPHA  is
 *         supplied  as  zero  then  the entries of the matrices A and B
 *         need not be set on input. Unchanged on exit.
 *
 * A       (input)                       const float *
 *         On entry,  A  points  to an array of size equal to or greater
 *         than   LDA * ka * sizeof(   float   ),   where  ka  is k when
 *         TRANS = AtlasNoTrans, and is  n otherwise. Before entry  with
 *         TRANS = AtlasNoTrans, the  leading n by k part of the array A
 *         must contain the matrix A,  otherwise the leading k by n part
 *         of the array A must contain the matrix A. Unchanged  on exit.
 *
 * LDA     (input)                       const int
 *         On entry, LDA  specifies the leading dimension of A as decla-
 *         red  in  the  calling  (sub) program.  LDA  must be  at least
 *         MAX( 1, n ) when TRANS = AtlasNoTrans, and MAX( 1, k ) other-
 *         wise. Unchanged on exit.
 *
 * B       (input)                       const float *
 *         On entry,  B  points  to an array of size equal to or greater
 *         than   LDB * kb * sizeof(   float   ),   where  kb  is k when
 *         TRANS = AtlasNoTrans, and is  n otherwise. Before entry  with
 *         TRANS = AtlasNoTrans, the  leading n by k part of the array B
 *         must contain the matrix B,  otherwise the leading k by n part
 *         of the array B must contain the matrix B. Unchanged  on exit.
 *
 * LDB     (input)                       const int
 *         On entry, LDB  specifies the leading dimension of B as decla-
 *         red  in  the  calling  (sub) program.  LDB  must be  at least
 *         MAX( 1, n ) when TRANS = AtlasNoTrans, and MAX( 1, k ) other-
 *         wise. Unchanged on exit.
 *
 * BETA    (input)                       const float
 *         On entry,  BETA  specifies the scalar  beta.   When  BETA  is
 *         supplied as zero  then  the  entries of the matrix C need not
 *         be set on input. Unchanged on exit.
 *
 * C       (input/output)                float *
 *         On entry,  C  points  to an array of size equal to or greater
 *         than   LDC * n * sizeof(   float   ),   Before   entry   with
 *         UPLO = AtlasUpper,  the  leading n by n upper triangular part
 *         of the array C must contain the upper  triangular part of the
 *         symmetric matrix  and the strictly lower triangular part of C
 *         is not referenced.  On exit, the upper triangular part of the
 *         array  C  is  overwritten by the upper triangular part of the
 *         updated  matrix.  Before  entry  with UPLO = AtlasLower,  the
 *         leading n by n lower triangular part of the array C must con-
 *         tain the lower  triangular  part  of the symmetric matrix and
 *         the strictly upper triangular part of C is not referenced. On
 *         exit, the lower triangular part of the array C is overwritten
 *         by the lower triangular part of the updated matrix.
 *
 * LDC     (input)                       const int
 *         On entry, LDC  specifies the leading dimension of A as decla-
 *         red  in  the  calling  (sub) program.  LDC  must be  at least
 *         MAX( 1, n ). Unchanged on exit.
 *
 * ---------------------------------------------------------------------
 */
/*
 * .. Local Variables ..
 */
   int                        i, icij, j, jcj;
/* ..
 * .. Executable Statements ..
 *
 */
   if( ( N == 0 ) || ( ( ( ALPHA == ATL_sZERO ) || ( K == 0 ) ) &&
                       ( BETA == ATL_sONE ) ) ) return;

   if( ALPHA == ATL_sZERO )
   {
      if( UPLO == AtlasUpper )
      {
         if( BETA == ATL_sZERO )
         {
            for( j = 0, jcj = 0; j < N; j++, jcj += LDC )
            {
               for( i = 0, icij = jcj; i <= j; i++, icij += 1 )
               { C[icij] = ATL_sZERO; }
            }
         }
         else if( BETA != ATL_sONE )
         {
            for( j = 0, jcj = 0; j < N; j++, jcj += LDC )
            {
               for( i = 0, icij = jcj; i <= j; i++, icij += 1 )
               { C[icij] *= BETA; }
            }
         }
      }
      else
      {
         if( BETA == ATL_sZERO )
         {
            for( j = 0, jcj = 0; j < N; j++, jcj += LDC )
            {
               for( i = j, icij = j+jcj; i < N; i++, icij += 1 )
               { C[icij] = ATL_sZERO; }
            }
         }
         else if( BETA != ATL_sONE )
         {
            for( j = 0, jcj = 0; j < N; j++, jcj += LDC )
            {
               for( i = j, icij = j+jcj; i < N; i++, icij += 1 )
               { C[icij] *= BETA; }
            }
         }
      }
      return;
   }

   if( UPLO == AtlasUpper )
   {
      if( TRANS == AtlasNoTrans )
      { ATL_srefsyr2kUN( N, K, ALPHA, A, LDA, B, LDB, BETA, C, LDC ); }
      else
      { ATL_srefsyr2kUT( N, K, ALPHA, A, LDA, B, LDB, BETA, C, LDC ); }
   }
   else
   {
      if( TRANS == AtlasNoTrans )
      { ATL_srefsyr2kLN( N, K, ALPHA, A, LDA, B, LDB, BETA, C, LDC ); }
      else
      { ATL_srefsyr2kLT( N, K, ALPHA, A, LDA, B, LDB, BETA, C, LDC ); }
   }
/*
 * End of ATL_srefsyr2k
 */
}
