#include <stdio.h>
#include <stdlib.h>
int main(int nargs, char **args)
{
   float x=2.0, y=4.0, z=(-8.0), ans;
   void do_fmac(float*, float*, float*);

   ans = z + x*y;
   do_fmac(&z, &x, &y);
   if (z != ans)
   {
      fprintf(stderr, "wanted=%.2f, got=%.2f\n", ans, z);
      printf("FAILURE\n");
      exit(1);
   }
   printf("SUCCESS\n");
   return(0);
}
