#include "atlas_misc.h"
void ATL_USCAL(const int N, const SCALAR alpha, TYPE *X, const int incX)
{
   int i;
   for (i=N; i; i--, X += incX) *X *= alpha;
}
