# Java implementation for pairing based crytography protocols

Used [jPBC](http://gas.dia.unisa.it/projects/jpbc/schemes/bls.html) to develop Java implementation of pairing based protocols. Served as foundation for current research in lattice based cryptography.

[Current Bachelors project]



# Installing and running FLINT

1. Install GNU gmp

2. Install MPFR

3. Build FLINT and have it's path (i.e. `/usr/local/include`)

4. Run code as `g++ -I/usr/local/include/flint/ajtai.cpp -lflint -lmpfr -lgmp -lpthread`. Search in your system documentation for specifics of linkers if the above command throws an error of the form:

```s
ld: symbol(s) not found for architecture x86_64
clang: error: linker command failed with exit code 1
```