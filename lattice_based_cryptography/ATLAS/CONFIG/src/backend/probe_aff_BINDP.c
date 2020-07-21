#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <sys/thread.h>      /* thread_self header */
#include <sys/processor.h>   /* bindprocessor header */

static int rank=0;

void *DumbTest(void *vp)
{
   assert(!bindprocessor(BINDTHREAD, thread_self(), rank));
   pthread_exit(vp);
}
int main(int nargs, char **args)
{
   pthread_attr_t attr;
   pthread_t thr;
   void *vp, *vpret;

   if (nargs > 1)
      rank = atoi(args[1]);
   vp = (void*) &thr;
   vpret = NULL;
   assert(!pthread_attr_init(&attr));
   #ifdef IBM_PT_ERROR
      assert(!pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_UNDETACHED));
   #else
      assert(!pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE));
   #endif
   pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM); /* OK to fail */
   assert(!pthread_create(&thr, &attr, DumbTest, vp));
   assert(!pthread_attr_destroy(&attr));
   assert(!pthread_join(thr, &vpret));
   if (vpret == vp)
      printf("SUCCESS rank %d\n", rank);
   else
      printf("FAILURE rank %d\n", rank);
   exit(vpret == vp ? 0 : 1);
}
