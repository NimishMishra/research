#include <stdio.h>
#define STOMP_FPREGS __asm__ __volatile__ ( \
   "mov r0, #0 ; fmsr s0, r0 ; fcpys s1, s0 ; fcpys s2, s0" \
   : /* no output */ \
   : /* no input */ \
   : "r0", "s0", "s1", "s2"  /* clobbered */ \
)
int main(int nargs, char **args)
{
   float hardfptst(float f0, float f1, float f2);
   STOMP_FPREGS;
   if (hardfptst(-2.0, 2.0, 4.0) == 6.0)
      printf("FPABI='HARDFP'\n");
   else
      printf("FPABI='SOFTFP'\n");
   return(0);
}
