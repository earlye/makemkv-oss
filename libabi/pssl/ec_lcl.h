/* crypto/ec/ec_lcl.h */
/*
 * Originally written by Bodo Moeller for the OpenSSL project.
 */
/* ====================================================================
 * Copyright (c) 1998-2010 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@openssl.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */
/* ====================================================================
 * Copyright 2002 Sun Microsystems, Inc. ALL RIGHTS RESERVED.
 *
 * Portions of the attached software ("Contribution") are developed by 
 * SUN MICROSYSTEMS, INC., and are contributed to the OpenSSL project.
 *
 * The Contribution is licensed pursuant to the OpenSSL open source
 * license provided above.
 *
 * The elliptic curve binary polynomial software is originally written by 
 * Sheueling Chang Shantz and Douglas Stebila of Sun Microsystems Laboratories.
 *
 */
#ifndef HEADER_EC_LCL_H
#define HEADER_EC_LCL_H

#include <stdlib.h>

#include <openssl/obj_mac.h>
#include "ec.h"
#include <openssl/bn.h>

#if defined(__SUNPRO_C)
# if __SUNPRO_C >= 0x520
# pragma error_messages (off,E_ARRAY_OF_INCOMPLETE_NONAME,E_ARRAY_OF_INCOMPLETE)
# endif
#endif

typedef struct ec_pre_comp_st EC_PRE_COMP;

struct ec_group_st {
        struct ec_pre_comp_st *precomp;
	EC_POINT *generator; /* optional */
	BIGNUM order, cofactor;

	/* The following members are handled by the method functions,
	 * even if they appear generic */
	
	BIGNUM field; /* Field specification.
	               * For curves over GF(p), this is the modulus;
	               * for curves over GF(2^m), this is the 
	               * irreducible polynomial defining the field.
	               */

	int poly[6]; /* Field specification for curves over GF(2^m).
	              * The irreducible f(t) is then of the form:
	              *     t^poly[0] + t^poly[1] + ... + t^poly[k]
	              * where m = poly[0] > poly[1] > ... > poly[k] = 0.
	              * The array is terminated with poly[k+1]=-1.
	              * All elliptic curve irreducibles have at most 5
	              * non-zero terms.
	              */

	BIGNUM a, b; /* Curve coefficients.
	              * (Here the assumption is that BIGNUMs can be used
	              * or abused for all kinds of fields, not just GF(p).)
	              * For characteristic  > 3,  the curve is defined
	              * by a Weierstrass equation of the form
	              *     y^2 = x^3 + a*x + b.
	              * For characteristic  2,  the curve is defined by
	              * an equation of the form
	              *     y^2 + x*y = x^3 + a*x^2 + b.
	              */

	int a_is_minus3; /* enable optimized point arithmetics for special case */

	void *field_data1; /* method-specific (e.g., Montgomery structure) */
	void *field_data2; /* method-specific */
	int (*field_mod_func)(BIGNUM *, const BIGNUM *, const BIGNUM *,	BN_CTX *); /* method-specific */
} /* EC_GROUP */;

struct ec_key_st {
	EC_GROUP *group;

	EC_POINT *pub_key;
	BIGNUM	 *priv_key;

} /* EC_KEY */;


struct ec_point_st {
	/* All members except 'meth' are handled by the method functions,
	 * even if they appear generic */

	BIGNUM X;
	BIGNUM Y;
	BIGNUM Z; /* Jacobian projective coordinates:
	           * (X, Y, Z)  represents  (X/Z^2, Y/Z^3)  if  Z != 0 */
	int Z_is_one; /* enable optimized point arithmetics for special case */
} /* EC_POINT */;



/* method functions in ec_mult.c
 * (ec_lib.c uses these as defaults if group->method->mul is 0) */
static int ec_wNAF_mul(const EC_GROUP *group, EC_POINT *r, const BIGNUM *scalar,
	size_t num, const EC_POINT *points[], const BIGNUM *scalars[], BN_CTX *);
static int ec_wNAF_precompute_mult(EC_GROUP *group, BN_CTX *);


/* method functions in ecp_smpl.c */
static int ec_GFp_simple_group_init(EC_GROUP *);
static void ec_GFp_simple_group_finish(EC_GROUP *);
static void ec_GFp_simple_group_clear_finish(EC_GROUP *);
static int ec_GFp_simple_group_copy(EC_GROUP *, const EC_GROUP *);
static int ec_GFp_simple_group_set_curve(EC_GROUP *, const BIGNUM *p, const BIGNUM *a, const BIGNUM *b, BN_CTX *);
static int ec_GFp_simple_point_init(EC_POINT *);
static void ec_GFp_simple_point_finish(EC_POINT *);
static void ec_GFp_simple_point_clear_finish(EC_POINT *);
static int ec_GFp_simple_point_copy(EC_POINT *, const EC_POINT *);
static int ec_GFp_simple_point_set_to_infinity(const EC_GROUP *, EC_POINT *);
static int ec_GFp_simple_set_Jprojective_coordinates_GFp(const EC_GROUP *, EC_POINT *,
	const BIGNUM *x, const BIGNUM *y, const BIGNUM *z, BN_CTX *);
static int ec_GFp_simple_point_set_affine_coordinates(const EC_GROUP *, EC_POINT *,
	const BIGNUM *x, const BIGNUM *y, BN_CTX *);
static int ec_GFp_simple_point_get_affine_coordinates(const EC_GROUP *, const EC_POINT *,
	BIGNUM *x, BIGNUM *y, BN_CTX *);
static int ec_GFp_simple_add(const EC_GROUP *, EC_POINT *r, const EC_POINT *a, const EC_POINT *b, BN_CTX *);
static int ec_GFp_simple_dbl(const EC_GROUP *, EC_POINT *r, const EC_POINT *a, BN_CTX *);
static int ec_GFp_simple_invert(const EC_GROUP *, EC_POINT *, BN_CTX *);
static int ec_GFp_simple_is_at_infinity(const EC_GROUP *, const EC_POINT *);
static int ec_GFp_simple_cmp(const EC_GROUP *, const EC_POINT *a, const EC_POINT *b, BN_CTX *);
static int ec_GFp_simple_points_make_affine(const EC_GROUP *, size_t num, EC_POINT *[], BN_CTX *);


/* method functions in ecp_mont.c */
static int ec_GFp_mont_group_init(EC_GROUP *);
static int ec_GFp_mont_group_set_curve(EC_GROUP *, const BIGNUM *p, const BIGNUM *a, const BIGNUM *b, BN_CTX *);
static void ec_GFp_mont_group_finish(EC_GROUP *);
static void ec_GFp_mont_group_clear_finish(EC_GROUP *);
static int ec_GFp_mont_group_copy(EC_GROUP *, const EC_GROUP *);
static int ec_GFp_mont_field_mul(const EC_GROUP *, BIGNUM *r, const BIGNUM *a, const BIGNUM *b, BN_CTX *);
static int ec_GFp_mont_field_sqr(const EC_GROUP *, BIGNUM *r, const BIGNUM *a, BN_CTX *);
static int ec_GFp_mont_field_encode(const EC_GROUP *, BIGNUM *r, const BIGNUM *a, BN_CTX *);
static int ec_GFp_mont_field_decode(const EC_GROUP *, BIGNUM *r, const BIGNUM *a, BN_CTX *);
static int ec_GFp_mont_field_set_to_one(const EC_GROUP *, BIGNUM *r, BN_CTX *);

/* functions to manage EC_PRE_COMP within the EC_GROUP extra_data framework */
static EC_PRE_COMP *ec_pre_comp_dup(EC_PRE_COMP *);
static void ec_pre_comp_free(EC_PRE_COMP *);
static void ec_pre_comp_clear_free(EC_PRE_COMP *);

#endif /* HEADER_EC_LCL_H */
