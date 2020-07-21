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

void ATL_srefsymmRL
(
   const int                  M,
   const int                  N,
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
 * ATL_srefsymmRL( ... )
 *
 * <=>
 *
 * ATL_srefsymm( AtlasRight, AtlasLower, ... )
 *
 * See ATL_srefsymm for details.
 *
 * ---------------------------------------------------------------------
 */
/*
 * .. Local Variables ..
 */
   register float             t0;
   int                        i, iaj, iajk, iakj, ibij, ibik, icij, j,
                              jaj, jbj, jbk, jcj, k;
/* ..
 * .. Executable Statements ..
 *
 */
   for( j = 0,      iaj  = 0, jaj  = 0,   jbj  = 0,   jcj  = 0;
        j < N; j++, iaj += 1, jaj += LDA, jbj += LDB, jcj += LDC )
   {
      t0 = ALPHA * A[j+jaj];
      for( i = 0, ibij = jbj, icij = jcj; i < M; i++, ibij += 1, icij += 1 )
      { Mselscal( BETA, C[icij] ); C[icij] += t0 * B[ibij]; }
      for( k = 0, iajk = iaj, jbk = 0; k < j; k++, iajk += LDA, jbk += LDB )
      {
         t0 = ALPHA * A[iajk];
         for( i = 0, ibik = jbk, icij = jcj; i < M; i++, ibik += 1, icij += 1 )
         { C[icij] += t0 * B[ibik]; }
      }
      for( k = j+1,    iakj  = j+1+jaj, jbk  = (j+1)*LDB;
           k < N; k++, iakj += 1,       jbk += LDB )
      {
         t0 = ALPHA * A[iakj];
         for( i = 0, ibik = jbk, icij = jcj; i < M; i++, ibik += 1, icij += 1 )
         { C[icij] += t0 * B[ibik]; }
      }
   }
/*
 * End of ATL_srefsymmRL
 */
}
