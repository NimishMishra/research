      SUBROUTINE DLARFB( SIDE, TRANS, DIRECT, STOREV, M, N, K, V, LDV,
     $                   T, LDT, C, LDC, WORK, LDWORK )
      IMPLICIT NONE
*
*  -- LAPACK auxiliary routine (version 3.2) --
*  -- LAPACK is a software package provided by Univ. of Tennessee,    --
*  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..--
*     November 2006
*     Modified by R. Clint Whaley for inclusion in ATLAS Dec 2009.
*
*     .. Scalar Arguments ..
      CHARACTER          DIRECT, SIDE, STOREV, TRANS
      INTEGER            K, LDC, LDT, LDV, LDWORK, M, N
*     ..
*     .. Array Arguments ..
      DOUBLE PRECISION C( LDC, * ), T( LDT, * ), V( LDV, * ),
     $        WORK( LDWORK, * )
*     ..
*
*  Purpose
*  =======
*
*  DLARFB applies a real block reflector H or its transpose H' to a
*  real m by n matrix C, from either the left or the right.
*
*  Arguments
*  =========
*
*  SIDE    (input) CHARACTER*1
*          = 'L': apply H or H**T from the Left
*          = 'R': apply H or H**T from the Right
*
*  TRANS   (input) CHARACTER*1
*          = 'N': apply H (No transpose)
*          = 'T': apply H**T (Transpose)
*
*  DIRECT  (input) CHARACTER*1
*          Indicates how H is formed from a product of elementary
*          reflectors
*          = 'F': H = H(1) H(2) . . . H(k) (Forward)
*          = 'B': H = H(k) . . . H(2) H(1) (Backward)
*
*  STOREV  (input) CHARACTER*1
*          Indicates how the vectors which define the elementary
*          reflectors are stored:
*          = 'C': Columnwise
*          = 'R': Rowwise
*
*  M       (input) INTEGER
*          The number of rows of the matrix C.
*
*  N       (input) INTEGER
*          The number of columns of the matrix C.
*
*  K       (input) INTEGER
*          The order of the matrix T (= the number of elementary
*          reflectors whose product defines the block reflector).
*
*  V       (input) DOUBLE PRECISION array, dimension
*                                (LDV,K) if STOREV = 'C'
*                                (LDV,M) if STOREV = 'R' and SIDE = 'L'
*                                (LDV,N) if STOREV = 'R' and SIDE = 'R'
*          The matrix V. See further details.
*
*  LDV     (input) INTEGER
*          The leading dimension of the array V.
*          If STOREV = 'C' and SIDE = 'L', LDV >= max(1,M);
*          if STOREV = 'C' and SIDE = 'R', LDV >= max(1,N);
*          if STOREV = 'R', LDV >= K.
*
*  T       (input) DOUBLE PRECISION array, dimension (LDT,K)
*          The triangular k by k matrix T in the representation of the
*          block reflector.
*
*  LDT     (input) INTEGER
*          The leading dimension of the array T. LDT >= K.
*
*  C       (input/output) DOUBLE PRECISION array, dimension (LDC,N)
*          On entry, the m by n matrix C.
*          On exit, C is overwritten by H*C or H'*C or C*H or C*H'.
*
*  LDC     (input) INTEGER
*          The leading dimension of the array C. LDC >= max(1,M).
*
*  WORK    (workspace) DOUBLE PRECISION array, dimension (LDWORK,K)
*
*  LDWORK  (input) INTEGER
*          The leading dimension of the array WORK.
*          If SIDE = 'L', LDWORK >= max(1,N);
*          if SIDE = 'R', LDWORK >= max(1,M).
*
*  =====================================================================
*
*     .. Parameters ..
      INTEGER ATLASROWMAJOR, ATLASCOLMAJOR
      PARAMETER (ATLASROWMAJOR=101, ATLASCOLMAJOR=102)
      INTEGER ATLASNOTRANS, ATLASTRANS, ATLASCONJTRANS
      PARAMETER (ATLASNOTRANS=111, ATLASTRANS=112, ATLASCONJTRANS=113)
      INTEGER ATLASUPPER, ATLASLOWER
      PARAMETER (ATLASUPPER=121, ATLASLOWER=122)
      INTEGER ATLASNONUNIT, ATLASUNIT
      PARAMETER (ATLASNONUNIT=131, ATLASUNIT=132)
      INTEGER ATLASLEFT, ATLASRIGHT
      PARAMETER (ATLASLEFT=141, ATLASRIGHT=142)
*     .. Local variables ..
      INTEGER ISIDE, ITRANS, IDIRECT, ISTORE
*     ..
*     .. Executable Statements ..
*
*     Quick return if possible
*
      IF( M.LE.0 .OR. N.LE.0 )
     $   RETURN
      IF (SIDE.EQ.'R' .OR. SIDE.EQ.'r') THEN
         ISIDE = ATLASRIGHT
      ELSE
         ISIDE = ATLASLEFT
      ENDIF
      IF (TRANS.EQ.'N' .OR. TRANS.EQ.'n') THEN
         ITRANS = ATLASNOTRANS
      ELSE
         ITRANS = ATLASTRANS
      ENDIF
      IF (DIRECT.EQ.'F' .OR. DIRECT.EQ.'f') THEN
         IDIRECT = 1
      ELSE
         IDIRECT = 2
      ENDIF
      IF (STOREV.EQ.'R' .OR. STOREV.EQ.'r') THEN
         ISTORE = 1
      ELSE
         ISTORE = 2
      ENDIF
*
      CALL ATL_F77WRAP_DLARFB(ISIDE, ITRANS, IDIRECT, ISTORE, M, N, K,
     $                        V, LDV, T, LDT, C, LDC, WORK, LDWORK)
*
*
      RETURN
*
*     End of DLARFB
*
      END
