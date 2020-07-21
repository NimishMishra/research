/*
 *             Automatically Tuned Linear Algebra Software v3.10.3
 *                    (C) Copyright 1999 R. Clint Whaley
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

#include "f77wrap_lapack.h"
#include "atlas_lapack.h"
void F77WRAP_GETRI(const F77_INTEGER *N, TYPE *A, const F77_INTEGER *lda,
                   const F77_INTEGER *ipiv0, TYPE *work, F77_INTEGER *lwork,
                   F77_INTEGER *info)
{
   const int n = *N;
   int *ipiv=NULL;
   int i, lwrk = *lwork;

   if (lwrk != -1)
   {
      ipiv = malloc(n*sizeof(int));
      ATL_assert(ipiv);
      for (i=0; i != n; i++) ipiv[i] = ipiv0[i] - 1;
   }
   *info = ATL_getri(AtlasColMajor, *N, A, *lda, ipiv, work, &lwrk);
   if (work) *work = lwrk;
   else if (*lwork == -1)
      ATL_xerbla(5, __FILE__,
                 "For workspace query, workspace cannot be NULL\n");
   if (ipiv) free(ipiv);
}
