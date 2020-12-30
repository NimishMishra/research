#!/bin/bash

TESTDIR=$(realpath `dirname $0`)

echo "==== Testing poly_uniform ===="
$TESTDIR/test_poly_uniform

echo "==== Testing poly_getnoise ===="
$TESTDIR/test_poly_getnoise

echo "==== Testing poly_add (uses poly_uniform) ===="
$TESTDIR/test_poly_add > $TESTDIR/tmp.sage
sage $TESTDIR/tmp.sage

echo "==== Testing poly_sub (uses poly_uniform) ===="
$TESTDIR/test_poly_sub > $TESTDIR/tmp.sage
sage $TESTDIR/tmp.sage

echo "==== Testing poly_mul (uses poly_uniform) ===="
$TESTDIR/test_poly_mul > $TESTDIR/tmp.sage
sage $TESTDIR/tmp.sage

echo "==== Testing poly_tobytes and poly_frombytes (uses poly_uniform) ===="
$TESTDIR/test_poly_fromtobytes > $TESTDIR/tmp.sage
sage $TESTDIR/tmp.sage

echo "==== Testing poly_tomsg and poly_frommsg (uses poly_getnoise, poly_mul, poly_add, poly_sub) ===="
$TESTDIR/test_poly_fromtomsg

echo "==== Testing the Wookie KEM ===="
$TESTDIR/test_kem



rm $TESTDIR/tmp.sage
rm $TESTDIR/tmp.sage.py


