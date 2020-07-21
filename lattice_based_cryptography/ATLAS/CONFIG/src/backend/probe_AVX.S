#include "atlas_asm.h"
#
# x86 assembler for:
# void do_vmacc(z, x, y)
# RETURNS: z[0:3] += x[0:3] *  y[0:3];
#
.text
.globl ATL_asmdecor(do_vmacc)
ATL_asmdecor(do_vmacc):
#if defined(ATL_GAS_x8664) || defined(ATL_GAS_WOW64)
   #ifdef ATL_GAS_x8664
      #define Z %rdi
      #define X %rsi
      #define Y %rdx
   #else
      #define Z %rcx
      #define X %rdx
      #define Y %r8
   #endif
     vmovupd (Z), %ymm0                  /* load z */
     vmovupd (X), %ymm1                  /* load x */
     vmovupd (Y), %ymm2                  /* load y */
        vmulpd %ymm1, %ymm2, %ymm3
        vaddpd %ymm3, %ymm0, %ymm0
        vmovupd %ymm0, (Z)                      /* store z */
        ret
#else
        movl    12(%esp), %eax
        vmovupd 0(%eax), %ymm2                  /* load y */
        movl    8(%esp), %eax
        vmovupd  (%eax), %xmm1                  /* load x */
        movl    4(%esp), %eax
        vmovupd  (%eax), %xmm0                  /* load z */
        vmulpd %ymm1, %ymm2, %ymm3
        vaddpd %ymm3, %ymm0, %ymm0
        vmovupd %xmm0, 0(%eax)                  /* store z */
        ret
#endif
