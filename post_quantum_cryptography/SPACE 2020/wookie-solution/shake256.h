#ifndef SHAKE256_H
#define SHAKE256_H

#include <stdint.h>

#define SHAKE256_RATE 136

void shake256(unsigned char *output, unsigned long long outbytes, const unsigned char *input,  unsigned long long inbytes);

#endif
