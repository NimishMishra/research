#include <vecintrin.h>
void do_vsum(double *z, double *x, double *y) // RETURNS: z = x + y
{
   vector double vx, vy;
   vx[0] = x[0];
   vx[1] = x[1];
   vy[0] = y[0];
   vy[1] = y[1];
   vy += vx;
   z[0] = vy[0];
   z[1] = vy[1];
}
