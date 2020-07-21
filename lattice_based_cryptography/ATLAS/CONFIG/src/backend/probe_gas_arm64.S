#include "atlas_asm.h"
#
# Linux ARM assembly for:
# int asm_probe(int i)
# RETURNS: i*3
#

.text
.globl  ATL_asmdecor(asm_probe)
.type   ATL_asmdecor(asm_probe), %function
ATL_asmdecor(asm_probe):
        add     x0, x0, x0, LSL #1
        ret
.size ATL_asmdecor(asm_probe),.-ATL_asmdecor(asm_probe)
