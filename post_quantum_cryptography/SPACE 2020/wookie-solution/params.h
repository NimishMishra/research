#ifndef PARAMS_H
#define PARAMS_H

#define WOOKIE_N      1024
#define WOOKIE_LOGQ     12
#define WOOKIE_Q      (1<<WOOKIE_LOGQ)
#define WOOKIE_K         8
#define WOOKIE_SYMBYTES 32

#define WOOKIE_POLYBYTES  ((WOOKIE_N*WOOKIE_LOGQ)/8)

#endif
