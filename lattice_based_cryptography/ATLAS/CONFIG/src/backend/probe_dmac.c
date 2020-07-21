#include <stdio.h>
#include <stdlib.h>
int main(int nargs, char **args)
{
   double x=2.0, y=4.0, z=(-8.0), ans;
   void do_fmac(double*, double*, double*);

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
