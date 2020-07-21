#include "atlas_asm.h"
/*
  ARM vfpv3 assembler for:
                      r0        r1       r2
  void do_fmac(double* z, double* x,double* y)
   RETURNS: *z += *x * *y
*/
.code   32
.fpu    vfpv3
.text
.align 2
.globl  ATL_asmdecor(do_fmac)
.type   ATL_asmdecor(do_fmac), %function
ATL_asmdecor(do_fmac):
   fldd d31, [r0]
   fldd d30, [r1]
   fldd d29, [r2]
   fmacd d31, d30, d29
   fstd d31, [r0]
   bx lr
.size ATL_asmdecor(do_fmac),.-ATL_asmdecor(do_fmac)
