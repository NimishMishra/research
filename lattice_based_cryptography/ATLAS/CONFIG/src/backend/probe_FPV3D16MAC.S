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
   fldd d0, [r0]
   fldd d1, [r1]
   fldd d2, [r2]
   fmacd d0, d1, d2
   fstd d0, [r0]
   bx lr
.size ATL_asmdecor(do_fmac),.-ATL_asmdecor(do_fmac)
