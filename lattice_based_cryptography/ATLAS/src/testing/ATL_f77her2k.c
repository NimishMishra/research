/*
 *             Automatically Tuned Linear Algebra Software v3.10.3
 *                   (C) Copyright 1999 Antoine P. Petitet
 *
 * Code contributers : Antoine P. Petitet, R. Clint Whaley
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
#include "atlas_f77blas.h"

void Mjoin( PATL, f77her2k )
(
   const enum ATLAS_UPLO     UPLO,
   const enum ATLAS_TRANS    TRANS,
   const int                 N,
   const int                 K,
   const SCALAR              ALPHA,
   const TYPE                * A,
   const int                 LDA,
   const TYPE                * B,
   const int                 LDB,
   const TYPE                BETA,
   TYPE                      * C,
   const int                 LDC
)
{
#if defined( StringSunStyle )
   #if defined( ATL_FunkyInts )
   F77_INTEGER               ONE = 1;
   #else
   int                       ONE = 1;
   #endif
#elif defined( StringStructVal ) || defined( StringStructPtr )
   F77_CHAR                  fuplo;
   F77_CHAR                  ftran;
#elif defined( StringCrayStyle )
   F77_CHAR                  fuplo;
   F77_CHAR                  ftran;
#endif

   char                      ctran, cuplo;

#ifdef ATL_FunkyInts
   const F77_INTEGER         F77N   = N,   F77K   = K,
                             F77lda = LDA, F77ldb = LDB, F77ldc = LDC;
#else
   #define F77N              N
   #define F77K              K
   #define F77lda            LDA
   #define F77ldb            LDB
   #define F77ldc            LDC
#endif

   TYPE                      alpha[2], beta = BETA;

   *alpha = *ALPHA; alpha[1] = ALPHA[1];

   if(      UPLO  == AtlasLower     ) cuplo = 'L';
   else                               cuplo = 'U';

   if(      TRANS == AtlasNoTrans   ) ctran = 'N';
   else                               ctran = 'C';

#if   defined( StringSunStyle  )
   F77her2k( &cuplo, &ctran, &F77N, &F77K, SADD alpha, A, &F77lda,
             B, &F77ldb, &beta,     C, &F77ldc, ONE, ONE );
#elif defined( StringCrayStyle )
   fuplo = ATL_C2F_TransChar( cuplo );
   ftran = ATL_C2F_TransChar( ctran );
   F77her2k( fuplo,  ftran,  &F77N, &F77K, SADD alpha, A, &F77lda,
             B, &F77ldb, &beta,     C, &F77ldc );
#elif defined( StringStructVal )
   fuplo.len = 1; fuplo.cp = &cuplo;
   ftran.len = 1; ftran.cp = &ctran;
   F77her2k( fuplo,  ftran,  &F77N, &F77K, SADD alpha, A, &F77lda,
             B, &F77ldb, &beta,     C, &F77ldc );
#elif defined( StringStructPtr )
   fuplo.len = 1; fuplo.cp = &cuplo;
   ftran.len = 1; ftran.cp = &ctran;
   F77her2k( &fuplo, &ftran, &F77N, &F77K, SADD alpha, A, &F77lda,
             B, &F77ldb, &beta,     C, &F77ldc );
#else
   (void) fprintf( stderr, "\n\nF77/C interface not defined!!\n\n" );
   exit( -1 );
#endif
}
