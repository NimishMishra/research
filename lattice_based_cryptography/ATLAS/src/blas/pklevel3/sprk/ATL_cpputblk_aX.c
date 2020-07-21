/*
 *             Automatically Tuned Linear Algebra Software v3.10.3
 *                    (C) Copyright 2003 R. Clint Whaley
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
#include "atlas_pkblas.h"
void Mjoin(PATL,pputblk_diag)
   (const int M, const int N, const TYPE *V, const enum ATLAS_UPLO UC,
    TYPE *C, int ldc, int ldcinc, const SCALAR alpha, const SCALAR beta)
/*
 * Copies only the Upper or Lower portion of V to C
 */
/*
 * NOTE: specialize alpha, beta cases once it works!
 */
{
   int i, j, k;
   const int mn = M*N;
   const register TYPE rbeta=(*beta), ibeta=beta[1];
   const register TYPE ralpha=(*alpha), ialpha=alpha[1];
   register TYPE rc, ic, rv, iv, t0;

   if (UC == AtlasUpper)
   {
      ldc += ldc;
      ldcinc += ldcinc;
      if (ibeta == ATL_rzero)  /* real beta */
      {
         if (rbeta == ATL_rzero) /* simple assignment to C */
         {
            for (j=0; j < N; j++)
            {
               for (i=0; i <= j; i++)
               {
                  k = i+i;
                  t0 = rv = V[i+mn];
                  iv = V[i];
                  rv = ralpha * rv - ialpha *iv;
                  iv = ialpha * t0 + ralpha * iv;
                  C[k] = rv;
                  C[k+1] = iv;
               }
               C += ldc;
               V += M;
               ldc += ldcinc;
            }
         }
         else
         {
            for (j=0; j < N; j++)
            {
               for (i=0; i <= j; i++)
               {
                  k = i+i;
                  rc = C[k];
                  ic = C[k+1];
                  t0 = rv = V[i+mn];
                  iv = V[i];
                  rv = ralpha * rv - ialpha *iv;
                  iv = ialpha * t0 + ralpha * iv;
                  t0 = rc;
                  rc = rbeta * rc;
                  ic = rbeta * ic;
                  rc += rv;
                  ic += iv;
                  C[k] = rc;
                  C[k+1] = ic;
               }
               C += ldc;
               V += M;
               ldc += ldcinc;
            }
         }
      }
      else
      {
         for (j=0; j < N; j++)
         {
            for (i=0; i <= j; i++)
            {
               k = i+i;
               rc = C[k];
               ic = C[k+1];
               t0 = rv = V[i+mn];
               iv = V[i];
               rv = ralpha * rv - ialpha *iv;
               iv = ialpha * t0 + ralpha * iv;
               t0 = rc;
               rc = rbeta * rc - ibeta * ic;
               ic = ibeta * t0 + rbeta * ic;
               rc += rv;
               ic += iv;
               C[k] = rc;
               C[k+1] = ic;
            }
            C += ldc;
            V += M;
            ldc += ldcinc;
         }
      }
   }
   else  /* Lower triangular */
   {
      if (ibeta == ATL_rzero && rbeta == ATL_rzero)
      {
         ldc += ldc;
         ldcinc += ldcinc;
         for (j=0; j < N; j++)
         {
            ldc += ldcinc;
            for (i=j; i < M; i++)
            {
               k = i+i;
               t0 = rv = V[i+mn];
               iv = V[i];
               rv = ralpha * rv - ialpha *iv;
               iv = ialpha * t0 + ralpha * iv;
               C[k] = rv;
               C[k+1] = iv;
            }
            C += ldc;
            V += M;
         }
      }
      else
      {
         ldc += ldc;
         ldcinc += ldcinc;
         for (j=0; j < N; j++)
         {
            ldc += ldcinc;
            for (i=j; i < M; i++)
            {
               k = i+i;
               rc = C[k];
               ic = C[k+1];
               t0 = rv = V[i+mn];
               iv = V[i];
               rv = ralpha * rv - ialpha *iv;
               iv = ialpha * t0 + ralpha * iv;
               t0 = rc;
               rc = rbeta * rc - ibeta * ic;
               ic = ibeta * t0 + rbeta * ic;
               rc += rv;
               ic += iv;
               C[k] = rc;
               C[k+1] = ic;
            }
            C += ldc;
            V += M;
         }
      }
   }
}

void Mjoin(PATL,pputblk_aX)(const int M, const int N, const TYPE *V,
                            TYPE *C, int ldc, int ldcinc,
                            const SCALAR alpha, const SCALAR beta)
/*
 * Given a MxN block-major block V, copy to a packed C
 *    ldcinc =  0 : General rectangular
 *    ldcinc =  1 : Upper
 *    ldcinc = -1 : Lower
 * NOTE: specialize alpha,beta once it works!
 */
{
   int i, j, k;
   const int mn = M*N;
   const register TYPE rbeta=(*beta), ibeta=beta[1];
   const register TYPE ralpha=(*alpha), ialpha=alpha[1];
   register TYPE rc, ic, rv, iv, t0;

   ldc -= M;
   if (ldcinc == -1) ldc--;
   ldc += ldc;
   ldcinc += ldcinc;
   if (rbeta == ATL_rzero && ibeta == ATL_rzero)
   {
      for (j=N; j; j--)
      {
         for (i=M; i; i--, C += 2, V++)
         {
            t0 = rv = V[mn];
            iv = *V;
            rv = ralpha * rv - ialpha *iv;
            iv = ialpha * t0 + ralpha * iv;
            *C = rv;
            C[1] = iv;
         }
         C += ldc;
         ldc += ldcinc;
      }
   }
   else
   {
      for (j=N; j; j--)
      {
         for (i=M; i; i--, C += 2, V++)
         {
            rc = *C;
            ic = C[1];
            t0 = rv = V[mn];
            iv = *V;
            rv = ralpha * rv - ialpha *iv;
            iv = ialpha * t0 + ralpha * iv;
            t0 = rc;
            rc = rbeta * rc - ibeta * ic;
            ic = ibeta * t0 + rbeta * ic;
            rc += rv;
            ic += iv;
            *C = rc;
            C[1] = ic;
         }
         C += ldc;
         ldc += ldcinc;
      }
   }
}
