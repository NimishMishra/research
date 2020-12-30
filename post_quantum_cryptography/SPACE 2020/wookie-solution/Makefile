CC = /usr/bin/gcc
CFLAGS = -Wall -Wextra -Wpedantic -g -O3

all: test/test_kem \
     test/test_poly_uniform \
     test/test_poly_getnoise \
     test/test_poly_add \
     test/test_poly_sub \
     test/test_poly_mul \
     test/test_poly_fromtobytes \
     test/test_poly_fromtomsg \

test/test_kem: kem.c params.h poly.c poly.h test/test_kem.c randombytes.c randombytes.h shake256.c shake256.h
	$(CC) $(CFLAGS) -o $@ kem.c poly.c randombytes.c shake256.c test/test_kem.c

test/test_poly_uniform: params.h poly.c poly.h test/test_poly_uniform.c randombytes.c randombytes.h shake256.c shake256.h
	$(CC) $(CFLAGS) -o $@ poly.c randombytes.c shake256.c test/test_poly_uniform.c

test/test_poly_getnoise: params.h poly.c poly.h test/test_poly_getnoise.c randombytes.c randombytes.h shake256.c shake256.h
	$(CC) $(CFLAGS) -o $@ poly.c randombytes.c shake256.c test/test_poly_getnoise.c

test/test_poly_add: params.h poly.c poly.h test/test_poly_add.c randombytes.c randombytes.h shake256.c shake256.h
	$(CC) $(CFLAGS) -o $@ poly.c randombytes.c shake256.c test/test_poly_add.c

test/test_poly_sub: params.h poly.c poly.h test/test_poly_sub.c randombytes.c randombytes.h shake256.c shake256.h
	$(CC) $(CFLAGS) -o $@ poly.c randombytes.c shake256.c test/test_poly_sub.c

test/test_poly_mul: params.h poly.c poly.h test/test_poly_mul.c randombytes.c randombytes.h shake256.c shake256.h
	$(CC) $(CFLAGS) -o $@ poly.c randombytes.c shake256.c test/test_poly_mul.c

test/test_poly_fromtobytes: params.h poly.c poly.h test/test_poly_fromtobytes.c randombytes.c randombytes.h shake256.c shake256.h
	$(CC) $(CFLAGS) -o $@ poly.c randombytes.c shake256.c test/test_poly_fromtobytes.c

test/test_poly_fromtomsg: params.h poly.c poly.h test/test_poly_fromtomsg.c randombytes.c randombytes.h shake256.c shake256.h
	$(CC) $(CFLAGS) -o $@ poly.c randombytes.c shake256.c test/test_poly_fromtomsg.c


.PHONY: clean

clean:
	-rm test/test_kem
	-rm test/test_poly_uniform
	-rm test/test_poly_getnoise
	-rm test/test_poly_add
	-rm test/test_poly_sub
	-rm test/test_poly_mul
	-rm test/test_poly_fromtobytes
	-rm test/test_poly_fromtomsg


