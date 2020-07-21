!              Automatically Tuned Linear Algebra Software v3.10.3
!                     (C) Copyright 2002 R. Clint Whaley
!
!  Redistribution and use in source and binary forms, with or without
!  modification, are permitted provided that the following conditions
!  are met:
!    1. Redistributions of source code must retain the above copyright
!       notice, this list of conditions and the following disclaimer.
!    2. Redistributions in binary form must reproduce the above copyright
!       notice, this list of conditions, and the following disclaimer in the
!       documentation and/or other materials provided with the distribution.
!    3. The name of the ATLAS group or the names of its contributers may
!       not be used to endorse or promote products derived from this
!       software without specific written permission.
!
!  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
!  ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
!  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
!  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE ATLAS GROUP OR ITS CONTRIBUTORS
!  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
!  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
!  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
!  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
!  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
!  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
!  POSSIBILITY OF SUCH DAMAGE.
!

#if !defined(KB)
   #define KB 0
#endif
#ifndef ATL_GAS_SPARC
   #error "This kernel requires sparc assembler!"
#endif
#define	M	%i0
#define N	%i1
#define ldab	%i2
#define pA1	%i3
#define pA0	%i4
#define pA2	%i5
#define pA3	%g1
#define pB0	%g2
#define pB1	%g3
#define pB2	%g4
#define pB3	%l3
#define pC0	%l7
#define pC1	%l6
#define pC2	%o0
#define pC3	%o1
#define incCn	%o2
#define Kstart	%o3
#define incBn	%o4
#define incAm	%o5
#define incBm	%l0
#define incAn	%l1
#define pfA	%l2
#define pfB	%i2   /* aliased with ldab */
#define	II	%l4
#define KK	%l5
!
! fp reg defines
!
#define rA0	%f0
#define ra0	%f1
#define rA1	%f2
#define ra1	%f3
#define rA2	%f4
#define ra2	%f5
#define rA3	%f6
#define ra3	%f7
#define rB0	%f8
#define rb0	%f9
#define rB1	%f10
#define rb1	%f11
#define m0	%f12
#define m1	%f13
#define m2	%f14
#define m3	%f15
#define rC00	%f16
#define rC10	%f17
#define rC20	%f18
#define rC30	%f19
#define rC01	%f20
#define rC11	%f21
#define rC21	%f22
#define rC31	%f23
#define rC02	%f24
#define rC12	%f25
#define rC22	%f26
#define rC32	%f27
#define rC03	%f28
#define rC13	%f29
#define rC23	%f30
#define rC33	%f31
#ifdef ATL_USE64BITS
   #define FSIZE	144
   #define BIAS         2047
   #define BOFF         FSIZE-8
#else
   #define FSIZE	64
   #define BIAS         0
#endif
#ifdef SCPLX
   #define CSH 3
   #define CMUL(arg_) ((arg_)*2)
#else
   #define CSH 2
   #define CMUL(arg_) arg_
#endif
#define PFA 64
#define PFB 64
#define PFC 64

#if 1
   #define prefR1(mem) prefetch mem, 0
   #define prefR2(mem) prefetch mem, 0
   #define prefW2(mem) prefetch mem, 2
#else
   #define prefW2(mem)
   #define prefR2(mem)
   #define prefR1(mem)
#endif
! 32 bit:
!                         i0,          i1           i2                i3
!void ATL_USERMM(const int M, const int N, const int K, const TYPE alpha,
!                           i4             i5,      [%fp+92]       [%fp+96]
!                const TYPE *A, const int lda, const TYPE *B, const int ldb,
!                      [%fp+100] [%fp+104]     [%fp+108]
!                const TYPE beta, TYPE *C, const int ldc)
!64 bits:
!                         i0,          i1           i2                f1
!void ATL_USERMM(const int M, const int N, const int K, const TYPE alpha,
!                           i4             i5      [%fp+176]      [%fp+184]
!                const TYPE *A, const int lda, const TYPE *B, const int ldb,
!                 f17, [%fp+192] [%fp+200]     [%fp+208]
!                const TYPE beta, TYPE *C, const int ldc)
#ifdef ATL_USE64BITS
        .register       %g2, #scratch
        .register       %g3, #scratch
#endif
	.section	".text"
	.align	8
	.global	ATL_USERMM
ATL_USERMM:
	save	%sp, -FSIZE, %sp
#ifdef ATL_USE64BITS
	ldx	[%fp+BIAS+176], pB0
	ldx	[%fp+BIAS+200], pC0
	ldsw	[%fp+BIAS+212], incCn
   #ifdef BETAX
        st      %f17, [%sp+BIAS+BOFF]  ! Store beta to local stack
   #endif
#else
	ld	[%fp+92], pB0
	ld	[%fp+104], pC0
	ld	[%fp+108], incCn
#endif
	srl	ldab, 4, Kstart
	sub	Kstart, 1, Kstart       ! Kstart = KB/16 - 1
	sll	ldab, 2, ldab
	sll	ldab, 2, incBn		! incBn = ldab * 4
	sll	incCn, CSH, incCn	! incCn = ldc * size
	sll	ldab, 1, incAm
	add	incAm, ldab, incAm
	add	incAm, 64, incAm	! incAm = (ldab*3+16)*size
	mov	64, incBm
	sub	incBm, ldab, incBm	! incBm = (16-ldab)*size
	add	pA0, ldab, pA1
	add	pA1, ldab, pA2
	add	pA2, ldab, pA3
	add	pB0, ldab, pB1
	add	pB1, ldab, pB2
	add	pB2, ldab, pB3
	add	pC0, incCn, pC1
	add	pC1, incCn, pC2
	add	pC2, incCn, pC3
	smul	ldab, M, incAn
	add	pA0, incAn, pfA
	smul	ldab, N, pfB
	add	pfB, pB0, pfB
	sub	%g0, incAn, incAn
	sll	incCn, 2, incCn
	sll	M, CSH, M
	sub	incCn, M, incCn
	srl	M, CSH, M
NLOOP:
	mov	M, II
MLOOP:
!
!	Load C[0..3][0..3] and apply beta if necessary
!       NOTE: later, specialize for case where we can use ldd for $C$
!             if ((ldc/2)*2 == ldc) && ( (pC0 && 0x7) == 0 )
!
							prefW2([pC0+PFC])
							prefW2([pC1+PFC])
							prefW2([pC2+PFC])
							prefW2([pC3+PFC])
#ifdef BETA0
	fzero	rC00
	fzero	rC20
	fzero	rC01
	fzero	rC21
	fzero	rC02
	fzero	rC22
	fzero	rC03
	fzero	rC23
#else
   #ifdef BETAX
      #ifdef ATL_USE64BITS
   	ld	[%sp+BIAS+BOFF], ra3
      #else
   	ld	[%fp+100], ra3
      #endif
   #endif
   	ld	[pC0], rC00
   	ld	[pC0+CMUL(4)], rC10
   	ld	[pC0+CMUL(8)], rC20
   	ld	[pC0+CMUL(12)], rC30
   	ld	[pC1], rC01
   	ld	[pC1+CMUL(4)], rC11
   	ld	[pC1+CMUL(8)], rC21
   	ld	[pC1+CMUL(12)], rC31
   	ld	[pC2], rC02
   	ld	[pC2+CMUL(4)], rC12
   	ld	[pC2+CMUL(8)], rC22
   	ld	[pC2+CMUL(12)], rC32
   	ld	[pC3], rC03
   	ld	[pC3+CMUL(4)], rC13
   	ld	[pC3+CMUL(8)], rC23
   	ld	[pC3+CMUL(12)], rC33
   #ifdef BETAX
   	fmuls	rC00, ra3, rC00
   	fmuls	rC10, ra3, rC10
   	fmuls	rC20, ra3, rC20
   	fmuls	rC30, ra3, rC30
   	fmuls	rC01, ra3, rC01
   	fmuls	rC11, ra3, rC11
   	fmuls	rC21, ra3, rC21
   	fmuls	rC31, ra3, rC31
   	fmuls	rC02, ra3, rC02
   	fmuls	rC12, ra3, rC12
   	fmuls	rC22, ra3, rC22
   	fmuls	rC32, ra3, rC32
   	fmuls	rC03, ra3, rC03
   	fmuls	rC13, ra3, rC13
   	fmuls	rC23, ra3, rC23
   	fmuls	rC33, ra3, rC33
   #endif
#endif
!
!	Load A & B registers and fill multiply pipe
!
	ldd	[pB0], rB0
	ldd	[pA0], rA0
	ldd	[pA1], rA1
	ldd	[pA2], rA2
	ldd	[pA3], rA3
	ldd	[pB1], rB1
	fmuls	rA0, rB0, m0
	fmuls	rA1, rB0, m1
	fmuls	rA2, rB0, m2
	fmuls	rA3, rB0, m3
#if (KB != 16)
   #if KB == 0
   	subcc	Kstart, %g0, %g0
	bz	KDRAIN
	nop
   #endif
	mov	Kstart, KK
	.align	4
KLOOP:
!
!	K=0 iteration
!
							prefR1([pA0+PFA])
							prefR1([pA1+PFA])
							prefR1([pA2+PFA])
							prefR1([pA3+PFA])
	fadds	rC00, m0, rC00
				fmuls	rA0, rB1, m0
	fadds	rC10, m1, rC10
				fmuls	rA1, rB1, m1
	fadds	rC20, m2, rC20
				fmuls	rA2, rB1, m2
	fadds	rC30, m3, rC30
				fmuls	rA3, rB1, m3
	fadds	rC01, m0, rC01
				fmuls	ra0, rb0, m0
	fadds	rC11, m1, rC11
				fmuls	ra1, rb0, m1
	fadds	rC21, m2, rC21
				fmuls	ra2, rb0, m2
	fadds	rC31, m3, rC31
				fmuls	ra3, rb0, m3
							ldd	[pB2+0], rB0
	fadds	rC00, m0, rC00
				fmuls	ra0, rb1, m0
	fadds	rC10, m1, rC10
				fmuls	ra1, rb1, m1
	fadds	rC20, m2, rC20
				fmuls	ra2, rb1, m2
	fadds	rC30, m3, rC30
				fmuls	ra3, rb1, m3
							ldd	[pB3+0], rB1
	fadds	rC01, m0, rC01
				fmuls	rA0, rB0, m0
	fadds	rC11, m1, rC11
				fmuls	rA1, rB0, m1
	fadds	rC21, m2, rC21
				fmuls	rA2, rB0, m2
	fadds	rC31, m3, rC31
				fmuls	rA3, rB0, m3
	fadds	rC02, m0, rC02
				fmuls	rA0, rB1, m0
	fadds	rC12, m1, rC12
				fmuls	rA1, rB1, m1
	fadds	rC22, m2, rC22
				fmuls	rA2, rB1, m2
	fadds	rC32, m3, rC32
				fmuls	rA3, rB1, m3
	fadds	rC03, m0, rC03
				fmuls	ra0, rb0, m0
	fadds	rC13, m1, rC13
				fmuls	ra1, rb0, m1
	fadds	rC23, m2, rC23
				fmuls	ra2, rb0, m2
	fadds	rC33, m3, rC33
				fmuls	ra3, rb0, m3
							ldd	[pB0+8], rB0
	fadds	rC02, m0, rC02
				fmuls	ra0, rb1, m0
							ldd	[pA0+8], rA0
	fadds	rC12, m1, rC12
				fmuls	ra1, rb1, m1
							ldd	[pA1+8], rA1
	fadds	rC22, m2, rC22
				fmuls	ra2, rb1, m2
							ldd	[pA2+8], rA2
	fadds	rC32, m3, rC32
				fmuls	ra3, rb1, m3
							ldd	[pA3+8], rA3
	fadds	rC03, m0, rC03
				fmuls	rA0, rB0, m0
							ldd	[pB1+8], rB1
	fadds	rC13, m1, rC13
				fmuls	rA1, rB0, m1
	fadds	rC23, m2, rC23
				fmuls	rA2, rB0, m2
	fadds	rC33, m3, rC33
				fmuls	rA3, rB0, m3
!
!	K=2 iteration
!
	fadds	rC00, m0, rC00
				fmuls	rA0, rB1, m0
	fadds	rC10, m1, rC10
				fmuls	rA1, rB1, m1
	fadds	rC20, m2, rC20
				fmuls	rA2, rB1, m2
	fadds	rC30, m3, rC30
				fmuls	rA3, rB1, m3
	fadds	rC01, m0, rC01
				fmuls	ra0, rb0, m0
	fadds	rC11, m1, rC11
				fmuls	ra1, rb0, m1
	fadds	rC21, m2, rC21
				fmuls	ra2, rb0, m2
	fadds	rC31, m3, rC31
				fmuls	ra3, rb0, m3
							ldd	[pB2+8], rB0
	fadds	rC00, m0, rC00
				fmuls	ra0, rb1, m0
	fadds	rC10, m1, rC10
				fmuls	ra1, rb1, m1
	fadds	rC20, m2, rC20
				fmuls	ra2, rb1, m2
	fadds	rC30, m3, rC30
				fmuls	ra3, rb1, m3
							ldd	[pB3+8], rB1
	fadds	rC01, m0, rC01
				fmuls	rA0, rB0, m0
	fadds	rC11, m1, rC11
				fmuls	rA1, rB0, m1
	fadds	rC21, m2, rC21
				fmuls	rA2, rB0, m2
	fadds	rC31, m3, rC31
				fmuls	rA3, rB0, m3
	fadds	rC02, m0, rC02
				fmuls	rA0, rB1, m0
	fadds	rC12, m1, rC12
				fmuls	rA1, rB1, m1
	fadds	rC22, m2, rC22
				fmuls	rA2, rB1, m2
	fadds	rC32, m3, rC32
				fmuls	rA3, rB1, m3
	fadds	rC03, m0, rC03
				fmuls	ra0, rb0, m0
	fadds	rC13, m1, rC13
				fmuls	ra1, rb0, m1
	fadds	rC23, m2, rC23
				fmuls	ra2, rb0, m2
	fadds	rC33, m3, rC33
				fmuls	ra3, rb0, m3
							ldd	[pB0+16], rB0
	fadds	rC02, m0, rC02
				fmuls	ra0, rb1, m0
							ldd	[pA0+16], rA0
	fadds	rC12, m1, rC12
				fmuls	ra1, rb1, m1
							ldd	[pA1+16], rA1
	fadds	rC22, m2, rC22
				fmuls	ra2, rb1, m2
							ldd	[pA2+16], rA2
	fadds	rC32, m3, rC32
				fmuls	ra3, rb1, m3
							ldd	[pA3+16], rA3
	fadds	rC03, m0, rC03
				fmuls	rA0, rB0, m0
							ldd	[pB1+16], rB1
	fadds	rC13, m1, rC13
				fmuls	rA1, rB0, m1
	fadds	rC23, m2, rC23
				fmuls	rA2, rB0, m2
	fadds	rC33, m3, rC33
				fmuls	rA3, rB0, m3
!
!	K=4 iteration
!
	fadds	rC00, m0, rC00
				fmuls	rA0, rB1, m0
	fadds	rC10, m1, rC10
				fmuls	rA1, rB1, m1
	fadds	rC20, m2, rC20
				fmuls	rA2, rB1, m2
	fadds	rC30, m3, rC30
				fmuls	rA3, rB1, m3
	fadds	rC01, m0, rC01
				fmuls	ra0, rb0, m0
	fadds	rC11, m1, rC11
				fmuls	ra1, rb0, m1
	fadds	rC21, m2, rC21
				fmuls	ra2, rb0, m2
	fadds	rC31, m3, rC31
				fmuls	ra3, rb0, m3
							ldd	[pB2+16], rB0
	fadds	rC00, m0, rC00
				fmuls	ra0, rb1, m0
	fadds	rC10, m1, rC10
				fmuls	ra1, rb1, m1
	fadds	rC20, m2, rC20
				fmuls	ra2, rb1, m2
	fadds	rC30, m3, rC30
				fmuls	ra3, rb1, m3
							ldd	[pB3+16], rB1
	fadds	rC01, m0, rC01
				fmuls	rA0, rB0, m0
	fadds	rC11, m1, rC11
				fmuls	rA1, rB0, m1
	fadds	rC21, m2, rC21
				fmuls	rA2, rB0, m2
	fadds	rC31, m3, rC31
				fmuls	rA3, rB0, m3
	fadds	rC02, m0, rC02
				fmuls	rA0, rB1, m0
	fadds	rC12, m1, rC12
							prefR1([pB0+PFB])
							prefR1([pB1+PFB])
							prefR1([pB2+PFB])
							prefR1([pB3+PFB])
				fmuls	rA1, rB1, m1
	fadds	rC22, m2, rC22
				fmuls	rA2, rB1, m2
	fadds	rC32, m3, rC32
				fmuls	rA3, rB1, m3
	fadds	rC03, m0, rC03
				fmuls	ra0, rb0, m0
	fadds	rC13, m1, rC13
				fmuls	ra1, rb0, m1
	fadds	rC23, m2, rC23
				fmuls	ra2, rb0, m2
	fadds	rC33, m3, rC33
				fmuls	ra3, rb0, m3
							ldd	[pB0+24], rB0
	fadds	rC02, m0, rC02
				fmuls	ra0, rb1, m0
							ldd	[pA0+24], rA0
	fadds	rC12, m1, rC12
				fmuls	ra1, rb1, m1
							ldd	[pA1+24], rA1
	fadds	rC22, m2, rC22
				fmuls	ra2, rb1, m2
							ldd	[pA2+24], rA2
	fadds	rC32, m3, rC32
				fmuls	ra3, rb1, m3
							ldd	[pA3+24], rA3
	fadds	rC03, m0, rC03
				fmuls	rA0, rB0, m0
							ldd	[pB1+24], rB1
	fadds	rC13, m1, rC13
				fmuls	rA1, rB0, m1
	fadds	rC23, m2, rC23
				fmuls	rA2, rB0, m2
	fadds	rC33, m3, rC33
				fmuls	rA3, rB0, m3
!
!	K=6 iteration
!
	fadds	rC00, m0, rC00
				fmuls	rA0, rB1, m0
	fadds	rC10, m1, rC10
				fmuls	rA1, rB1, m1
	fadds	rC20, m2, rC20
				fmuls	rA2, rB1, m2
	fadds	rC30, m3, rC30
				fmuls	rA3, rB1, m3
	fadds	rC01, m0, rC01
				fmuls	ra0, rb0, m0
	fadds	rC11, m1, rC11
				fmuls	ra1, rb0, m1
	fadds	rC21, m2, rC21
				fmuls	ra2, rb0, m2
	fadds	rC31, m3, rC31
				fmuls	ra3, rb0, m3
							ldd	[pB2+24], rB0
	fadds	rC00, m0, rC00
				fmuls	ra0, rb1, m0
	fadds	rC10, m1, rC10
				fmuls	ra1, rb1, m1
	fadds	rC20, m2, rC20
				fmuls	ra2, rb1, m2
	fadds	rC30, m3, rC30
				fmuls	ra3, rb1, m3
							ldd	[pB3+24], rB1
	fadds	rC01, m0, rC01
				fmuls	rA0, rB0, m0
	fadds	rC11, m1, rC11
				fmuls	rA1, rB0, m1
	fadds	rC21, m2, rC21
				fmuls	rA2, rB0, m2
	fadds	rC31, m3, rC31
				fmuls	rA3, rB0, m3
	fadds	rC02, m0, rC02
				fmuls	rA0, rB1, m0
	fadds	rC12, m1, rC12
				fmuls	rA1, rB1, m1
	fadds	rC22, m2, rC22
				fmuls	rA2, rB1, m2
	fadds	rC32, m3, rC32
				fmuls	rA3, rB1, m3
	fadds	rC03, m0, rC03
				fmuls	ra0, rb0, m0
	fadds	rC13, m1, rC13
				fmuls	ra1, rb0, m1
	fadds	rC23, m2, rC23
				fmuls	ra2, rb0, m2
	fadds	rC33, m3, rC33
				fmuls	ra3, rb0, m3
							ldd	[pB0+32], rB0
	fadds	rC02, m0, rC02
				fmuls	ra0, rb1, m0
							ldd	[pA0+32], rA0
	fadds	rC12, m1, rC12
				fmuls	ra1, rb1, m1
							ldd	[pA1+32], rA1
	fadds	rC22, m2, rC22
				fmuls	ra2, rb1, m2
							ldd	[pA2+32], rA2
	fadds	rC32, m3, rC32
				fmuls	ra3, rb1, m3
							ldd	[pA3+32], rA3
	fadds	rC03, m0, rC03
				fmuls	rA0, rB0, m0
							ldd	[pB1+32], rB1
	fadds	rC13, m1, rC13
				fmuls	rA1, rB0, m1
	fadds	rC23, m2, rC23
				fmuls	rA2, rB0, m2
	fadds	rC33, m3, rC33
				fmuls	rA3, rB0, m3
!
!	K=8 iteration
!
	fadds	rC00, m0, rC00
				fmuls	rA0, rB1, m0
	fadds	rC10, m1, rC10
				fmuls	rA1, rB1, m1
	fadds	rC20, m2, rC20
				fmuls	rA2, rB1, m2
	fadds	rC30, m3, rC30
				fmuls	rA3, rB1, m3
	fadds	rC01, m0, rC01
				fmuls	ra0, rb0, m0
	fadds	rC11, m1, rC11
				fmuls	ra1, rb0, m1
	fadds	rC21, m2, rC21
				fmuls	ra2, rb0, m2
	fadds	rC31, m3, rC31
				fmuls	ra3, rb0, m3
							ldd	[pB2+32], rB0
	fadds	rC00, m0, rC00
				fmuls	ra0, rb1, m0
	fadds	rC10, m1, rC10
				fmuls	ra1, rb1, m1
	fadds	rC20, m2, rC20
				fmuls	ra2, rb1, m2
	fadds	rC30, m3, rC30
				fmuls	ra3, rb1, m3
							ldd	[pB3+32], rB1
	fadds	rC01, m0, rC01
				fmuls	rA0, rB0, m0
	fadds	rC11, m1, rC11
				fmuls	rA1, rB0, m1
	fadds	rC21, m2, rC21
				fmuls	rA2, rB0, m2
	fadds	rC31, m3, rC31
				fmuls	rA3, rB0, m3
	fadds	rC02, m0, rC02
				fmuls	rA0, rB1, m0
	fadds	rC12, m1, rC12
				fmuls	rA1, rB1, m1
	fadds	rC22, m2, rC22
				fmuls	rA2, rB1, m2
	fadds	rC32, m3, rC32
				fmuls	rA3, rB1, m3
	fadds	rC03, m0, rC03
				fmuls	ra0, rb0, m0
	fadds	rC13, m1, rC13
				fmuls	ra1, rb0, m1
	fadds	rC23, m2, rC23
				fmuls	ra2, rb0, m2
	fadds	rC33, m3, rC33
				fmuls	ra3, rb0, m3
							ldd	[pB0+40], rB0
	fadds	rC02, m0, rC02
				fmuls	ra0, rb1, m0
							ldd	[pA0+40], rA0
	fadds	rC12, m1, rC12
				fmuls	ra1, rb1, m1
							ldd	[pA1+40], rA1
	fadds	rC22, m2, rC22
				fmuls	ra2, rb1, m2
							ldd	[pA2+40], rA2
	fadds	rC32, m3, rC32
				fmuls	ra3, rb1, m3
							ldd	[pA3+40], rA3
	fadds	rC03, m0, rC03
				fmuls	rA0, rB0, m0
							ldd	[pB1+40], rB1
	fadds	rC13, m1, rC13
				fmuls	rA1, rB0, m1
	fadds	rC23, m2, rC23
				fmuls	rA2, rB0, m2
	fadds	rC33, m3, rC33
				fmuls	rA3, rB0, m3
!
!	K=10 iteration
!
	fadds	rC00, m0, rC00
				fmuls	rA0, rB1, m0
	fadds	rC10, m1, rC10
				fmuls	rA1, rB1, m1
	fadds	rC20, m2, rC20
				fmuls	rA2, rB1, m2
	fadds	rC30, m3, rC30
				fmuls	rA3, rB1, m3
	fadds	rC01, m0, rC01
				fmuls	ra0, rb0, m0
	fadds	rC11, m1, rC11
				fmuls	ra1, rb0, m1
	fadds	rC21, m2, rC21
				fmuls	ra2, rb0, m2
	fadds	rC31, m3, rC31
				fmuls	ra3, rb0, m3
							ldd	[pB2+40], rB0
	fadds	rC00, m0, rC00
				fmuls	ra0, rb1, m0
	fadds	rC10, m1, rC10
				fmuls	ra1, rb1, m1
	fadds	rC20, m2, rC20
				fmuls	ra2, rb1, m2
	fadds	rC30, m3, rC30
				fmuls	ra3, rb1, m3
							ldd	[pB3+40], rB1
	fadds	rC01, m0, rC01
				fmuls	rA0, rB0, m0
	fadds	rC11, m1, rC11
				fmuls	rA1, rB0, m1
	fadds	rC21, m2, rC21
				fmuls	rA2, rB0, m2
	fadds	rC31, m3, rC31
				fmuls	rA3, rB0, m3
	fadds	rC02, m0, rC02
				fmuls	rA0, rB1, m0
	fadds	rC12, m1, rC12
				fmuls	rA1, rB1, m1
	fadds	rC22, m2, rC22
				fmuls	rA2, rB1, m2
	fadds	rC32, m3, rC32
				fmuls	rA3, rB1, m3
	fadds	rC03, m0, rC03
				fmuls	ra0, rb0, m0
	fadds	rC13, m1, rC13
				fmuls	ra1, rb0, m1
	fadds	rC23, m2, rC23
				fmuls	ra2, rb0, m2
	fadds	rC33, m3, rC33
				fmuls	ra3, rb0, m3
							ldd	[pB0+48], rB0
	fadds	rC02, m0, rC02
				fmuls	ra0, rb1, m0
							ldd	[pA0+48], rA0
	fadds	rC12, m1, rC12
				fmuls	ra1, rb1, m1
							ldd	[pA1+48], rA1
	fadds	rC22, m2, rC22
				fmuls	ra2, rb1, m2
							ldd	[pA2+48], rA2
	fadds	rC32, m3, rC32
				fmuls	ra3, rb1, m3
							ldd	[pA3+48], rA3
	fadds	rC03, m0, rC03
				fmuls	rA0, rB0, m0
							ldd	[pB1+48], rB1
	fadds	rC13, m1, rC13
				fmuls	rA1, rB0, m1
	fadds	rC23, m2, rC23
				fmuls	rA2, rB0, m2
	fadds	rC33, m3, rC33
				fmuls	rA3, rB0, m3
!
!	K=12 iteration
!
	fadds	rC00, m0, rC00
				fmuls	rA0, rB1, m0
	fadds	rC10, m1, rC10
				fmuls	rA1, rB1, m1
	fadds	rC20, m2, rC20
				fmuls	rA2, rB1, m2
	fadds	rC30, m3, rC30
				fmuls	rA3, rB1, m3
	fadds	rC01, m0, rC01
				fmuls	ra0, rb0, m0
	fadds	rC11, m1, rC11
				fmuls	ra1, rb0, m1
	fadds	rC21, m2, rC21
				fmuls	ra2, rb0, m2
	fadds	rC31, m3, rC31
				fmuls	ra3, rb0, m3
							ldd	[pB2+48], rB0
	fadds	rC00, m0, rC00
				fmuls	ra0, rb1, m0
	fadds	rC10, m1, rC10
				fmuls	ra1, rb1, m1
	fadds	rC20, m2, rC20
				fmuls	ra2, rb1, m2
	fadds	rC30, m3, rC30
				fmuls	ra3, rb1, m3
							ldd	[pB3+48], rB1
	fadds	rC01, m0, rC01
				fmuls	rA0, rB0, m0
	fadds	rC11, m1, rC11
				fmuls	rA1, rB0, m1
	fadds	rC21, m2, rC21
				fmuls	rA2, rB0, m2
	fadds	rC31, m3, rC31
				fmuls	rA3, rB0, m3
	fadds	rC02, m0, rC02
				fmuls	rA0, rB1, m0
	fadds	rC12, m1, rC12
				fmuls	rA1, rB1, m1
	fadds	rC22, m2, rC22
				fmuls	rA2, rB1, m2
	fadds	rC32, m3, rC32
				fmuls	rA3, rB1, m3
	fadds	rC03, m0, rC03
				fmuls	ra0, rb0, m0
	fadds	rC13, m1, rC13
				fmuls	ra1, rb0, m1
	fadds	rC23, m2, rC23
				fmuls	ra2, rb0, m2
	fadds	rC33, m3, rC33
				fmuls	ra3, rb0, m3
							ldd	[pB0+56], rB0
	fadds	rC02, m0, rC02
				fmuls	ra0, rb1, m0
							ldd	[pA0+56], rA0
	fadds	rC12, m1, rC12
				fmuls	ra1, rb1, m1
							ldd	[pA1+56], rA1
	fadds	rC22, m2, rC22
				fmuls	ra2, rb1, m2
							ldd	[pA2+56], rA2
	fadds	rC32, m3, rC32
				fmuls	ra3, rb1, m3
							ldd	[pA3+56], rA3
	fadds	rC03, m0, rC03
				fmuls	rA0, rB0, m0
							ldd	[pB1+56], rB1
	fadds	rC13, m1, rC13
				fmuls	rA1, rB0, m1
	fadds	rC23, m2, rC23
				fmuls	rA2, rB0, m2
	fadds	rC33, m3, rC33
				fmuls	rA3, rB0, m3
!
!	K=14 iteration
!
	fadds	rC00, m0, rC00
				fmuls	rA0, rB1, m0
	fadds	rC10, m1, rC10
				fmuls	rA1, rB1, m1
	fadds	rC20, m2, rC20
				fmuls	rA2, rB1, m2
	fadds	rC30, m3, rC30
				fmuls	rA3, rB1, m3
	fadds	rC01, m0, rC01
				fmuls	ra0, rb0, m0
	fadds	rC11, m1, rC11
				fmuls	ra1, rb0, m1
	fadds	rC21, m2, rC21
				fmuls	ra2, rb0, m2
	fadds	rC31, m3, rC31
				fmuls	ra3, rb0, m3
							ldd	[pB2+56], rB0
	fadds	rC00, m0, rC00
				fmuls	ra0, rb1, m0
	fadds	rC10, m1, rC10
				fmuls	ra1, rb1, m1
	fadds	rC20, m2, rC20
				fmuls	ra2, rb1, m2
	fadds	rC30, m3, rC30
				fmuls	ra3, rb1, m3
							ldd	[pB3+56], rB1
	fadds	rC01, m0, rC01
				fmuls	rA0, rB0, m0
							add	pA0, 64, pA0
	fadds	rC11, m1, rC11
				fmuls	rA1, rB0, m1
							add	pA1, 64, pA1
	fadds	rC21, m2, rC21
				fmuls	rA2, rB0, m2
							add	pA2, 64, pA2
	fadds	rC31, m3, rC31
				fmuls	rA3, rB0, m3
							add	pA3, 64, pA3
	fadds	rC02, m0, rC02
				fmuls	rA0, rB1, m0
							add	pB0, 64, pB0
	fadds	rC12, m1, rC12
				fmuls	rA1, rB1, m1
							add	pB1, 64, pB1
	fadds	rC22, m2, rC22
				fmuls	rA2, rB1, m2
							add	pB2, 64, pB2
	fadds	rC32, m3, rC32
				fmuls	rA3, rB1, m3
							add	pB3, 64, pB3
	fadds	rC03, m0, rC03
				fmuls	ra0, rb0, m0
	fadds	rC13, m1, rC13
				fmuls	ra1, rb0, m1
	fadds	rC23, m2, rC23
				fmuls	ra2, rb0, m2
	fadds	rC33, m3, rC33
				fmuls	ra3, rb0, m3
							ldd	[pB0], rB0
	fadds	rC02, m0, rC02
				fmuls	ra0, rb1, m0
							ldd	[pA0], rA0
	fadds	rC12, m1, rC12
				fmuls	ra1, rb1, m1
							ldd	[pA1], rA1
	fadds	rC22, m2, rC22
				fmuls	ra2, rb1, m2
							ldd	[pA2], rA2
	fadds	rC32, m3, rC32
				fmuls	ra3, rb1, m3
							ldd	[pA3], rA3
	fadds	rC03, m0, rC03
				fmuls	rA0, rB0, m0
							ldd	[pB1], rB1
	fadds	rC13, m1, rC13
				fmuls	rA1, rB0, m1
	subcc	KK, 1, KK
	fadds	rC23, m2, rC23
				fmuls	rA2, rB0, m2
	fadds	rC33, m3, rC33
				fmuls	rA3, rB0, m3
	bnz	KLOOP
	nop
#endif
!
!	Drain mul pipe on last iteration of K-loop
!
#if K == 0
KDRAIN:
#endif
!
!	K=0 iteration
!
	fadds	rC00, m0, rC00
				fmuls	rA0, rB1, m0
	fadds	rC10, m1, rC10
				fmuls	rA1, rB1, m1
	fadds	rC20, m2, rC20
				fmuls	rA2, rB1, m2
	fadds	rC30, m3, rC30
				fmuls	rA3, rB1, m3
	fadds	rC01, m0, rC01
				fmuls	ra0, rb0, m0
	fadds	rC11, m1, rC11
				fmuls	ra1, rb0, m1
	fadds	rC21, m2, rC21
				fmuls	ra2, rb0, m2
	fadds	rC31, m3, rC31
				fmuls	ra3, rb0, m3
							ldd	[pB2+0], rB0
	fadds	rC00, m0, rC00
				fmuls	ra0, rb1, m0
	fadds	rC10, m1, rC10
				fmuls	ra1, rb1, m1
	fadds	rC20, m2, rC20
				fmuls	ra2, rb1, m2
	fadds	rC30, m3, rC30
				fmuls	ra3, rb1, m3
							ldd	[pB3+0], rB1
	fadds	rC01, m0, rC01
				fmuls	rA0, rB0, m0
	fadds	rC11, m1, rC11
				fmuls	rA1, rB0, m1
	fadds	rC21, m2, rC21
				fmuls	rA2, rB0, m2
	fadds	rC31, m3, rC31
				fmuls	rA3, rB0, m3
	fadds	rC02, m0, rC02
				fmuls	rA0, rB1, m0
	fadds	rC12, m1, rC12
				fmuls	rA1, rB1, m1
	fadds	rC22, m2, rC22
				fmuls	rA2, rB1, m2
	fadds	rC32, m3, rC32
				fmuls	rA3, rB1, m3
	fadds	rC03, m0, rC03
				fmuls	ra0, rb0, m0
	fadds	rC13, m1, rC13
				fmuls	ra1, rb0, m1
	fadds	rC23, m2, rC23
				fmuls	ra2, rb0, m2
	fadds	rC33, m3, rC33
				fmuls	ra3, rb0, m3
							ldd	[pB0+8], rB0
	fadds	rC02, m0, rC02
				fmuls	ra0, rb1, m0
							ldd	[pA0+8], rA0
	fadds	rC12, m1, rC12
				fmuls	ra1, rb1, m1
							ldd	[pA1+8], rA1
	fadds	rC22, m2, rC22
				fmuls	ra2, rb1, m2
							ldd	[pA2+8], rA2
	fadds	rC32, m3, rC32
				fmuls	ra3, rb1, m3
							ldd	[pA3+8], rA3
	fadds	rC03, m0, rC03
				fmuls	rA0, rB0, m0
							ldd	[pB1+8], rB1
	fadds	rC13, m1, rC13
				fmuls	rA1, rB0, m1
	fadds	rC23, m2, rC23
				fmuls	rA2, rB0, m2
	fadds	rC33, m3, rC33
				fmuls	rA3, rB0, m3
!
!	K=2 iteration
!
	fadds	rC00, m0, rC00
				fmuls	rA0, rB1, m0
	fadds	rC10, m1, rC10
				fmuls	rA1, rB1, m1
	fadds	rC20, m2, rC20
				fmuls	rA2, rB1, m2
	fadds	rC30, m3, rC30
				fmuls	rA3, rB1, m3
	fadds	rC01, m0, rC01
				fmuls	ra0, rb0, m0
	fadds	rC11, m1, rC11
				fmuls	ra1, rb0, m1
	fadds	rC21, m2, rC21
				fmuls	ra2, rb0, m2
	fadds	rC31, m3, rC31
				fmuls	ra3, rb0, m3
							ldd	[pB2+8], rB0
	fadds	rC00, m0, rC00
				fmuls	ra0, rb1, m0
	fadds	rC10, m1, rC10
				fmuls	ra1, rb1, m1
	fadds	rC20, m2, rC20
				fmuls	ra2, rb1, m2
	fadds	rC30, m3, rC30
				fmuls	ra3, rb1, m3
							ldd	[pB3+8], rB1
	fadds	rC01, m0, rC01
				fmuls	rA0, rB0, m0
	fadds	rC11, m1, rC11
				fmuls	rA1, rB0, m1
	fadds	rC21, m2, rC21
				fmuls	rA2, rB0, m2
	fadds	rC31, m3, rC31
				fmuls	rA3, rB0, m3
	fadds	rC02, m0, rC02
				fmuls	rA0, rB1, m0
	fadds	rC12, m1, rC12
				fmuls	rA1, rB1, m1
	fadds	rC22, m2, rC22
				fmuls	rA2, rB1, m2
	fadds	rC32, m3, rC32
				fmuls	rA3, rB1, m3
	fadds	rC03, m0, rC03
				fmuls	ra0, rb0, m0
	fadds	rC13, m1, rC13
				fmuls	ra1, rb0, m1
	fadds	rC23, m2, rC23
				fmuls	ra2, rb0, m2
	fadds	rC33, m3, rC33
				fmuls	ra3, rb0, m3
							ldd	[pB0+16], rB0
	fadds	rC02, m0, rC02
				fmuls	ra0, rb1, m0
							ldd	[pA0+16], rA0
	fadds	rC12, m1, rC12
				fmuls	ra1, rb1, m1
							ldd	[pA1+16], rA1
	fadds	rC22, m2, rC22
				fmuls	ra2, rb1, m2
							ldd	[pA2+16], rA2
	fadds	rC32, m3, rC32
				fmuls	ra3, rb1, m3
							ldd	[pA3+16], rA3
	fadds	rC03, m0, rC03
				fmuls	rA0, rB0, m0
							ldd	[pB1+16], rB1
	fadds	rC13, m1, rC13
				fmuls	rA1, rB0, m1
	fadds	rC23, m2, rC23
				fmuls	rA2, rB0, m2
	fadds	rC33, m3, rC33
				fmuls	rA3, rB0, m3
!
!	K=4 iteration
!
	fadds	rC00, m0, rC00
				fmuls	rA0, rB1, m0
	fadds	rC10, m1, rC10
				fmuls	rA1, rB1, m1
	fadds	rC20, m2, rC20
				fmuls	rA2, rB1, m2
	fadds	rC30, m3, rC30
				fmuls	rA3, rB1, m3
	fadds	rC01, m0, rC01
				fmuls	ra0, rb0, m0
	fadds	rC11, m1, rC11
				fmuls	ra1, rb0, m1
	fadds	rC21, m2, rC21
				fmuls	ra2, rb0, m2
	fadds	rC31, m3, rC31
				fmuls	ra3, rb0, m3
							ldd	[pB2+16], rB0
	fadds	rC00, m0, rC00
				fmuls	ra0, rb1, m0
	fadds	rC10, m1, rC10
				fmuls	ra1, rb1, m1
	fadds	rC20, m2, rC20
				fmuls	ra2, rb1, m2
	fadds	rC30, m3, rC30
				fmuls	ra3, rb1, m3
							ldd	[pB3+16], rB1
	fadds	rC01, m0, rC01
				fmuls	rA0, rB0, m0
	fadds	rC11, m1, rC11
				fmuls	rA1, rB0, m1
	fadds	rC21, m2, rC21
				fmuls	rA2, rB0, m2
	fadds	rC31, m3, rC31
				fmuls	rA3, rB0, m3
	fadds	rC02, m0, rC02
				fmuls	rA0, rB1, m0
	fadds	rC12, m1, rC12
				fmuls	rA1, rB1, m1
	fadds	rC22, m2, rC22
				fmuls	rA2, rB1, m2
	fadds	rC32, m3, rC32
				fmuls	rA3, rB1, m3
	fadds	rC03, m0, rC03
				fmuls	ra0, rb0, m0
	fadds	rC13, m1, rC13
				fmuls	ra1, rb0, m1
	fadds	rC23, m2, rC23
				fmuls	ra2, rb0, m2
	fadds	rC33, m3, rC33
				fmuls	ra3, rb0, m3
							ldd	[pB0+24], rB0
	fadds	rC02, m0, rC02
				fmuls	ra0, rb1, m0
							ldd	[pA0+24], rA0
	fadds	rC12, m1, rC12
				fmuls	ra1, rb1, m1
							ldd	[pA1+24], rA1
	fadds	rC22, m2, rC22
				fmuls	ra2, rb1, m2
							ldd	[pA2+24], rA2
	fadds	rC32, m3, rC32
				fmuls	ra3, rb1, m3
							ldd	[pA3+24], rA3
	fadds	rC03, m0, rC03
				fmuls	rA0, rB0, m0
							ldd	[pB1+24], rB1
	fadds	rC13, m1, rC13
				fmuls	rA1, rB0, m1
	fadds	rC23, m2, rC23
				fmuls	rA2, rB0, m2
	fadds	rC33, m3, rC33
				fmuls	rA3, rB0, m3
!
!	K=6 iteration
!
	fadds	rC00, m0, rC00
				fmuls	rA0, rB1, m0
	fadds	rC10, m1, rC10
				fmuls	rA1, rB1, m1
	fadds	rC20, m2, rC20
				fmuls	rA2, rB1, m2
	fadds	rC30, m3, rC30
				fmuls	rA3, rB1, m3
	fadds	rC01, m0, rC01
				fmuls	ra0, rb0, m0
	fadds	rC11, m1, rC11
				fmuls	ra1, rb0, m1
	fadds	rC21, m2, rC21
				fmuls	ra2, rb0, m2
	fadds	rC31, m3, rC31
				fmuls	ra3, rb0, m3
							ldd	[pB2+24], rB0
	fadds	rC00, m0, rC00
				fmuls	ra0, rb1, m0
	fadds	rC10, m1, rC10
				fmuls	ra1, rb1, m1
	fadds	rC20, m2, rC20
				fmuls	ra2, rb1, m2
	fadds	rC30, m3, rC30
				fmuls	ra3, rb1, m3
							ldd	[pB3+24], rB1
	fadds	rC01, m0, rC01
				fmuls	rA0, rB0, m0
	fadds	rC11, m1, rC11
				fmuls	rA1, rB0, m1
	fadds	rC21, m2, rC21
				fmuls	rA2, rB0, m2
	fadds	rC31, m3, rC31
				fmuls	rA3, rB0, m3
	fadds	rC02, m0, rC02
				fmuls	rA0, rB1, m0
	fadds	rC12, m1, rC12
				fmuls	rA1, rB1, m1
	fadds	rC22, m2, rC22
				fmuls	rA2, rB1, m2
	fadds	rC32, m3, rC32
				fmuls	rA3, rB1, m3
	fadds	rC03, m0, rC03
				fmuls	ra0, rb0, m0
	fadds	rC13, m1, rC13
				fmuls	ra1, rb0, m1
	fadds	rC23, m2, rC23
				fmuls	ra2, rb0, m2
	fadds	rC33, m3, rC33
				fmuls	ra3, rb0, m3
							ldd	[pB0+32], rB0
	fadds	rC02, m0, rC02
				fmuls	ra0, rb1, m0
							ldd	[pA0+32], rA0
	fadds	rC12, m1, rC12
				fmuls	ra1, rb1, m1
							ldd	[pA1+32], rA1
	fadds	rC22, m2, rC22
				fmuls	ra2, rb1, m2
							ldd	[pA2+32], rA2
	fadds	rC32, m3, rC32
				fmuls	ra3, rb1, m3
							ldd	[pA3+32], rA3
	fadds	rC03, m0, rC03
				fmuls	rA0, rB0, m0
							ldd	[pB1+32], rB1
	fadds	rC13, m1, rC13
				fmuls	rA1, rB0, m1
	fadds	rC23, m2, rC23
				fmuls	rA2, rB0, m2
	fadds	rC33, m3, rC33
				fmuls	rA3, rB0, m3
!
!	K=8 iteration
!
	fadds	rC00, m0, rC00
				fmuls	rA0, rB1, m0
	fadds	rC10, m1, rC10
				fmuls	rA1, rB1, m1
	fadds	rC20, m2, rC20
				fmuls	rA2, rB1, m2
	fadds	rC30, m3, rC30
				fmuls	rA3, rB1, m3
	fadds	rC01, m0, rC01
				fmuls	ra0, rb0, m0
	fadds	rC11, m1, rC11
				fmuls	ra1, rb0, m1
	fadds	rC21, m2, rC21
				fmuls	ra2, rb0, m2
	fadds	rC31, m3, rC31
				fmuls	ra3, rb0, m3
							ldd	[pB2+32], rB0
	fadds	rC00, m0, rC00
				fmuls	ra0, rb1, m0
	fadds	rC10, m1, rC10
				fmuls	ra1, rb1, m1
	fadds	rC20, m2, rC20
				fmuls	ra2, rb1, m2
	fadds	rC30, m3, rC30
				fmuls	ra3, rb1, m3
							ldd	[pB3+32], rB1
	fadds	rC01, m0, rC01
				fmuls	rA0, rB0, m0
	fadds	rC11, m1, rC11
				fmuls	rA1, rB0, m1
	fadds	rC21, m2, rC21
				fmuls	rA2, rB0, m2
	fadds	rC31, m3, rC31
				fmuls	rA3, rB0, m3
							prefW2([pC0+incCn])
							prefW2([pC1+incCn])
							prefW2([pC2+incCn])
							prefW2([pC3+incCn])
	fadds	rC02, m0, rC02
				fmuls	rA0, rB1, m0
	fadds	rC12, m1, rC12
				fmuls	rA1, rB1, m1
	fadds	rC22, m2, rC22
				fmuls	rA2, rB1, m2
	fadds	rC32, m3, rC32
				fmuls	rA3, rB1, m3
	fadds	rC03, m0, rC03
				fmuls	ra0, rb0, m0
	fadds	rC13, m1, rC13
				fmuls	ra1, rb0, m1
	fadds	rC23, m2, rC23
				fmuls	ra2, rb0, m2
	fadds	rC33, m3, rC33
				fmuls	ra3, rb0, m3
							ldd	[pB0+40], rB0
	fadds	rC02, m0, rC02
				fmuls	ra0, rb1, m0
							ldd	[pA0+40], rA0
	fadds	rC12, m1, rC12
				fmuls	ra1, rb1, m1
							ldd	[pA1+40], rA1
	fadds	rC22, m2, rC22
				fmuls	ra2, rb1, m2
							ldd	[pA2+40], rA2
	fadds	rC32, m3, rC32
				fmuls	ra3, rb1, m3
							ldd	[pA3+40], rA3
	fadds	rC03, m0, rC03
				fmuls	rA0, rB0, m0
							ldd	[pB1+40], rB1
	fadds	rC13, m1, rC13
				fmuls	rA1, rB0, m1
	fadds	rC23, m2, rC23
				fmuls	rA2, rB0, m2
	fadds	rC33, m3, rC33
				fmuls	rA3, rB0, m3
!
!	K=10 iteration
!
	fadds	rC00, m0, rC00
				fmuls	rA0, rB1, m0
	fadds	rC10, m1, rC10
				fmuls	rA1, rB1, m1
	fadds	rC20, m2, rC20
				fmuls	rA2, rB1, m2
	fadds	rC30, m3, rC30
				fmuls	rA3, rB1, m3
	fadds	rC01, m0, rC01
				fmuls	ra0, rb0, m0
	fadds	rC11, m1, rC11
				fmuls	ra1, rb0, m1
	fadds	rC21, m2, rC21
				fmuls	ra2, rb0, m2
	fadds	rC31, m3, rC31
				fmuls	ra3, rb0, m3
							ldd	[pB2+40], rB0
	fadds	rC00, m0, rC00
				fmuls	ra0, rb1, m0
	fadds	rC10, m1, rC10
				fmuls	ra1, rb1, m1
	fadds	rC20, m2, rC20
				fmuls	ra2, rb1, m2
	fadds	rC30, m3, rC30
				fmuls	ra3, rb1, m3
							ldd	[pB3+40], rB1
	fadds	rC01, m0, rC01
				fmuls	rA0, rB0, m0
	fadds	rC11, m1, rC11
				fmuls	rA1, rB0, m1
	fadds	rC21, m2, rC21
				fmuls	rA2, rB0, m2
	fadds	rC31, m3, rC31
				fmuls	rA3, rB0, m3
	fadds	rC02, m0, rC02
				fmuls	rA0, rB1, m0
	fadds	rC12, m1, rC12
				fmuls	rA1, rB1, m1
	fadds	rC22, m2, rC22
				fmuls	rA2, rB1, m2
	fadds	rC32, m3, rC32
				fmuls	rA3, rB1, m3
	fadds	rC03, m0, rC03
				fmuls	ra0, rb0, m0
	fadds	rC13, m1, rC13
				fmuls	ra1, rb0, m1
	fadds	rC23, m2, rC23
				fmuls	ra2, rb0, m2
	fadds	rC33, m3, rC33
				fmuls	ra3, rb0, m3
							ldd	[pB0+48], rB0
	fadds	rC02, m0, rC02
				fmuls	ra0, rb1, m0
							ldd	[pA0+48], rA0
	fadds	rC12, m1, rC12
				fmuls	ra1, rb1, m1
							ldd	[pA1+48], rA1
	fadds	rC22, m2, rC22
				fmuls	ra2, rb1, m2
							ldd	[pA2+48], rA2
	fadds	rC32, m3, rC32
				fmuls	ra3, rb1, m3
							ldd	[pA3+48], rA3
	fadds	rC03, m0, rC03
				fmuls	rA0, rB0, m0
							ldd	[pB1+48], rB1
	fadds	rC13, m1, rC13
				fmuls	rA1, rB0, m1
	fadds	rC23, m2, rC23
				fmuls	rA2, rB0, m2
	fadds	rC33, m3, rC33
				fmuls	rA3, rB0, m3
!
!	K=12 iteration
!
	fadds	rC00, m0, rC00
				fmuls	rA0, rB1, m0
	fadds	rC10, m1, rC10
				fmuls	rA1, rB1, m1
	fadds	rC20, m2, rC20
				fmuls	rA2, rB1, m2
	fadds	rC30, m3, rC30
				fmuls	rA3, rB1, m3
	fadds	rC01, m0, rC01
				fmuls	ra0, rb0, m0
	fadds	rC11, m1, rC11
				fmuls	ra1, rb0, m1
	fadds	rC21, m2, rC21
				fmuls	ra2, rb0, m2
	fadds	rC31, m3, rC31
				fmuls	ra3, rb0, m3
							ldd	[pB2+48], rB0
	fadds	rC00, m0, rC00
				fmuls	ra0, rb1, m0
	fadds	rC10, m1, rC10
				fmuls	ra1, rb1, m1
	fadds	rC20, m2, rC20
				fmuls	ra2, rb1, m2
	fadds	rC30, m3, rC30
				fmuls	ra3, rb1, m3
							ldd	[pB3+48], rB1
	fadds	rC01, m0, rC01
				fmuls	rA0, rB0, m0
	fadds	rC11, m1, rC11
				fmuls	rA1, rB0, m1
	fadds	rC21, m2, rC21
				fmuls	rA2, rB0, m2
	fadds	rC31, m3, rC31
				fmuls	rA3, rB0, m3
	fadds	rC02, m0, rC02
				fmuls	rA0, rB1, m0
	fadds	rC12, m1, rC12
				fmuls	rA1, rB1, m1
	fadds	rC22, m2, rC22
				fmuls	rA2, rB1, m2
	fadds	rC32, m3, rC32
				fmuls	rA3, rB1, m3
	fadds	rC03, m0, rC03
				fmuls	ra0, rb0, m0
	fadds	rC13, m1, rC13
				fmuls	ra1, rb0, m1
	fadds	rC23, m2, rC23
				fmuls	ra2, rb0, m2
	fadds	rC33, m3, rC33
				fmuls	ra3, rb0, m3
							ldd	[pB0+56], rB0
	fadds	rC02, m0, rC02
				fmuls	ra0, rb1, m0
							ldd	[pA0+56], rA0
	fadds	rC12, m1, rC12
				fmuls	ra1, rb1, m1
							ldd	[pA1+56], rA1
	fadds	rC22, m2, rC22
				fmuls	ra2, rb1, m2
							ldd	[pA2+56], rA2
	fadds	rC32, m3, rC32
				fmuls	ra3, rb1, m3
							ldd	[pA3+56], rA3
	fadds	rC03, m0, rC03
				fmuls	rA0, rB0, m0
							ldd	[pB1+56], rB1
	fadds	rC13, m1, rC13
				fmuls	rA1, rB0, m1
	fadds	rC23, m2, rC23
				fmuls	rA2, rB0, m2
	fadds	rC33, m3, rC33
				fmuls	rA3, rB0, m3
!
!	K=14 iteration
!
	fadds	rC00, m0, rC00
				fmuls	rA0, rB1, m0
	fadds	rC10, m1, rC10
				fmuls	rA1, rB1, m1
	fadds	rC20, m2, rC20
				fmuls	rA2, rB1, m2
	fadds	rC30, m3, rC30
				fmuls	rA3, rB1, m3
	fadds	rC01, m0, rC01
				fmuls	ra0, rb0, m0
	fadds	rC11, m1, rC11
				fmuls	ra1, rb0, m1
	fadds	rC21, m2, rC21
				fmuls	ra2, rb0, m2
	fadds	rC31, m3, rC31
				fmuls	ra3, rb0, m3
							ldd	[pB2+56], rB0
	fadds	rC00, m0, rC00
				fmuls	ra0, rb1, m0
	fadds	rC10, m1, rC10
				fmuls	ra1, rb1, m1
	fadds	rC20, m2, rC20
				fmuls	ra2, rb1, m2
	fadds	rC30, m3, rC30
				fmuls	ra3, rb1, m3
							ldd	[pB3+56], rB1
	fadds	rC01, m0, rC01
				fmuls	rA0, rB0, m0
	fadds	rC11, m1, rC11
				fmuls	rA1, rB0, m1
	fadds	rC21, m2, rC21
				fmuls	rA2, rB0, m2
	fadds	rC31, m3, rC31
				fmuls	rA3, rB0, m3
	fadds	rC02, m0, rC02
				fmuls	rA0, rB1, m0
	fadds	rC12, m1, rC12
				fmuls	rA1, rB1, m1
	fadds	rC22, m2, rC22
				fmuls	rA2, rB1, m2
	fadds	rC32, m3, rC32
				fmuls	rA3, rB1, m3
	fadds	rC03, m0, rC03
				fmuls	ra0, rb0, m0
							prefR2([pfA])
							prefR2([pfB])
	fadds	rC13, m1, rC13
				fmuls	ra1, rb0, m1
							add	pfA, 64, pfA
	fadds	rC23, m2, rC23
				fmuls	ra2, rb0, m2
							add	pfB, 64, pfB
	fadds	rC33, m3, rC33
				fmuls	ra3, rb0, m3
	fadds	rC02, m0, rC02
				fmuls	ra0, rb1, m0
				add	pA0, incAm, pA0
	fadds	rC12, m1, rC12
				fmuls	ra1, rb1, m1
				add	pA1, incAm, pA1
	fadds	rC22, m2, rC22
				fmuls	ra2, rb1, m2
				add	pA2, incAm, pA2
	fadds	rC32, m3, rC32
				fmuls	ra3, rb1, m3
				add	pA3, incAm, pA3
	fadds	rC03, m0, rC03
				add	pB0, incBm, pB0
	fadds	rC13, m1, rC13
				add	pB1, incBm, pB1
	fadds	rC23, m2, rC23
				add	pB2, incBm, pB2
	fadds	rC33, m3, rC33
				add	pB3, incBm, pB3
!
!       Write answer back to C
!
	st	rC00, [pC0]
	st	rC10, [pC0+CMUL(4)]
	st	rC20, [pC0+CMUL(8)]
	st	rC30, [pC0+CMUL(12)]
					add	pC0, CMUL(16), pC0
	st	rC01, [pC1]
	st	rC11, [pC1+CMUL(4)]
	st	rC21, [pC1+CMUL(8)]
	st	rC31, [pC1+CMUL(12)]
					add	pC1, CMUL(16), pC1
	st	rC02, [pC2]
	st	rC12, [pC2+CMUL(4)]
	st	rC22, [pC2+CMUL(8)]
	st	rC32, [pC2+CMUL(12)]
					add	pC2, CMUL(16), pC2
	st	rC03, [pC3]
	st	rC13, [pC3+CMUL(4)]
	st	rC23, [pC3+CMUL(8)]
	st	rC33, [pC3+CMUL(12)]
	subcc	II, 4, II
	bnz	MLOOP
					add	pC3, CMUL(16), pC3
	add	pC0, incCn, pC0
	add	pC1, incCn, pC1
	add	pC2, incCn, pC2
	add	pC3, incCn, pC3
	add	pA0, incAn, pA0
	add	pA1, incAn, pA1
	add	pA2, incAn, pA2
	add	pA3, incAn, pA3
	add	pB0, incBn, pB0
	add	pB1, incBn, pB1
	add	pB2, incBn, pB2
!
!       while(N);
!
	subcc	N, 4, N
	bnz	NLOOP
	add	pB3, incBn, pB3
!
!	Epilogue
!
	ret
	restore
