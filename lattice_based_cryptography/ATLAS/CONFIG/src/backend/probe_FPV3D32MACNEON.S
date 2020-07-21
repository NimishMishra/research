#include "atlas_asm.h"
/*
  ARM NEON assembler for:
                     r0        r1       r2
  void do_vsum(float* z, float* x,float* y)
   where x, y, and z are vectors of length 4
   RETURNS: z = x + y
*/
.code   32
.fpu    neon
.text
.align 2
.globl  ATL_asmdecor(do_vsum)
.type   ATL_asmdecor(do_vsum), %function
ATL_asmdecor(do_vsum):
   vldm     r1,     {d16-d17}
   vldm     r2,     {d18-d19}
   vadd.f32 d16,     d16,      d18
   vadd.f32 d17,     d17,      d19
   vstm     r0,     {d16-d17}
.size ATL_asmdecor(do_vsum),.-ATL_asmdecor(do_vsum)
