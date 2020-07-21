#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define _GNU_SOURCE 1 /* what manpage says you need to get CPU_SET */
#define __USE_GNU   1 /* what you actually have to set on linuxes I've seen */
#include <sched.h>    /* must include this before pthreads */
#include <pthread.h>

static int rank=0;

void *DumbTest(void *vp)
{
   pthread_exit(vp);
}
int main(int nargs, char **args)
{
   pthread_attr_t attr;
   pthread_t thr;
   cpu_set_t *cpuset;
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
/*
 *    On POWER8/Linux, pthread_attr_setaffinity_np sometimes reallocs() the
 *    cpuset variable, thus it must be malloced and not taken from stack!
 */
      cpuset = malloc(sizeof(cpu_set_t));
      CPU_ZERO(cpuset);
      CPU_SET(rank, cpuset);
      assert(!pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t),cpuset));
      free(cpuset);
   assert(!pthread_create(&thr, &attr, DumbTest, vp));
   assert(!pthread_attr_destroy(&attr));
   assert(!pthread_join(thr, &vpret));
   if (vpret == vp)
      printf("SUCCESS rank %d\n", rank);
   else
      printf("FAILURE rank %d\n", rank);
   exit(vpret == vp ? 0 : 1);
}
