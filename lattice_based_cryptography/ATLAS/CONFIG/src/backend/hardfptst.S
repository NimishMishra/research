#define ATL_GAS_ARM
#include "atlas_asm.h"
.code   32
.text
.align  2
.globl  ATL_asmdecor(hardfptst)
.type   ATL_asmdecor(hardfptst), %function
/*
 * float hardfptst(float f0, float f1, float f2);
 * RETURNS: f0 + f1*f2, assuming HARDFP ABI
 */
ATL_asmdecor(hardfptst):
        fmuls s1, s1, s2
        fadds s0, s0, s1
        bx      lr
.size ATL_asmdecor(hardfptst),.-ATL_asmdecor(hardfptst)

