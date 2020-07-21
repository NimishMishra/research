#include "atlas_asm.h"
# WOW64 assembler for:
# int asm_probe(int i)
# RETURNS: i*3
#
.text
.global ATL_asmdecor(asm_probe)
ATL_asmdecor(asm_probe):
        xor     %rax, %rax
        lea (%ecx, %ecx,2), %eax
        ret
