/* crypto/ec/ec_lib.c */
/*
 * Originally written by Bodo Moeller for the OpenSSL project.
 */
/* ====================================================================
 * Copyright (c) 1998-2003 The OpenSSL Project.  All rights reserved.
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
 * Binary polynomial ECC support in OpenSSL originally developed by 
 * SUN MICROSYSTEMS, INC., and contributed to the OpenSSL project.
 */

#include <string.h>

#include "ec_lcl.h"

/* functions for EC_GROUP objects */

static EC_GROUP *EC_GROUP_new(const EC_METHOD *meth)
	{
	EC_GROUP *ret;

	if (meth == NULL)
		{
		ECerr(EC_F_EC_GROUP_NEW, EC_R_SLOT_FULL);
		return NULL;
		}

	ret = OPENSSL_malloc(sizeof *ret);
	if (ret == NULL)
		{
		ECerr(EC_F_EC_GROUP_NEW, ERR_R_MALLOC_FAILURE);
		return NULL;
		}

	ret->generator = NULL;
	ret->precomp = NULL;
	BN_init(&ret->order);
	BN_init(&ret->cofactor);

	if (!ec_GFp_mont_group_init(ret))
		{
		OPENSSL_free(ret);
		return NULL;
		}
	
	return ret;
	}


static void EC_GROUP_free(EC_GROUP *group)
	{
	if (!group) return;

		ec_GFp_mont_group_finish(group);

	if (group->generator != NULL)
		EC_POINT_free(group->generator);
	if (group->precomp != NULL)
		ec_pre_comp_free(group->precomp);
	BN_free(&group->order);
	BN_free(&group->cofactor);

	OPENSSL_free(group);
	}
 

static void EC_GROUP_clear_free(EC_GROUP *group)
	{
	if (!group) return;

		ec_GFp_mont_group_clear_finish(group);

	if (group->generator != NULL)
		EC_POINT_clear_free(group->generator);
	if (group->precomp != NULL)
		ec_pre_comp_clear_free(group->precomp);
	BN_clear_free(&group->order);
	BN_clear_free(&group->cofactor);

	OPENSSL_cleanse(group, sizeof *group);
	OPENSSL_free(group);
	}


static int EC_GROUP_copy(EC_GROUP *dest, const EC_GROUP *src)
	{

	if (dest == src)
		return 1;
	
	if (src->generator != NULL)
		{
		if (dest->generator == NULL)
			{
			dest->generator = EC_POINT_new(dest);
			if (dest->generator == NULL) return 0;
			}
		if (!EC_POINT_copy(dest->generator, src->generator)) return 0;
		}
	else
		{
		/* src->generator == NULL */
		if (dest->generator != NULL)
			{
			EC_POINT_clear_free(dest->generator);
			dest->generator = NULL;
			}
		}

	if (dest->precomp != NULL)
		ec_pre_comp_free(dest->precomp);

	dest->precomp = NULL;
	if (src->precomp != NULL)
		{
		dest->precomp = ec_pre_comp_dup(src->precomp);
		}

	if (!BN_copy(&dest->order, &src->order)) return 0;
	if (!BN_copy(&dest->cofactor, &src->cofactor)) return 0;

	return ec_GFp_mont_group_copy(dest, src);
	}


static EC_GROUP *EC_GROUP_dup(const EC_GROUP *a)
	{
	EC_GROUP *t = NULL;
	int ok = 0;

	if (a == NULL) return NULL;

	if ((t = EC_GROUP_new((EC_METHOD *)(void*)2)) == NULL) return(NULL);
	if (!EC_GROUP_copy(t, a)) goto err;

	ok = 1;

  err:	
	if (!ok)
		{
		if (t) EC_GROUP_free(t);
		return NULL;
		}
	else return t;
	}


static int EC_GROUP_set_generator(EC_GROUP *group, const EC_POINT *generator, const BIGNUM *order, const BIGNUM *cofactor)
	{
	if (generator == NULL)
		{
		ECerr(EC_F_EC_GROUP_SET_GENERATOR, ERR_R_PASSED_NULL_PARAMETER);
		return 0   ;
		}

	if (group->generator == NULL)
		{
		group->generator = EC_POINT_new(group);
		if (group->generator == NULL) return 0;
		}
	if (!EC_POINT_copy(group->generator, generator)) return 0;

	if (order != NULL)
		{ if (!BN_copy(&group->order, order)) return 0; }	
	else
		BN_zero(&group->order);

	if (cofactor != NULL)
		{ if (!BN_copy(&group->cofactor, cofactor)) return 0; }	
	else
		BN_zero(&group->cofactor);

	return 1;
	}


static const EC_POINT *EC_GROUP_get0_generator(const EC_GROUP *group)
	{
	return group->generator;
	}


static int EC_GROUP_get_order(const EC_GROUP *group, BIGNUM *order, BN_CTX *ctx)
	{
	if (!BN_copy(order, &group->order))
		return 0;

	return !BN_is_zero(order);
	}


static int EC_GROUP_set_curve_GFp(EC_GROUP *group, const BIGNUM *p, const BIGNUM *a, const BIGNUM *b, BN_CTX *ctx)
	{
	return ec_GFp_mont_group_set_curve(group, p, a, b, ctx);
	}


/* functions for EC_POINT objects */

static EC_POINT *EC_POINT_new(const EC_GROUP *group)
	{
	EC_POINT *ret;

	if (group == NULL)
		{
		ECerr(EC_F_EC_POINT_NEW, ERR_R_PASSED_NULL_PARAMETER);
		return NULL;
		}

	ret = OPENSSL_malloc(sizeof *ret);
	if (ret == NULL)
		{
		ECerr(EC_F_EC_POINT_NEW, ERR_R_MALLOC_FAILURE);
		return NULL;
		}

	if (!ec_GFp_simple_point_init(ret))
		{
		OPENSSL_free(ret);
		return NULL;
		}
	
	return ret;
	}


static void EC_POINT_free(EC_POINT *point)
	{
	if (!point) return;

		ec_GFp_simple_point_finish(point);
	OPENSSL_free(point);
	}
 

static void EC_POINT_clear_free(EC_POINT *point)
	{
	if (!point) return;

		ec_GFp_simple_point_clear_finish(point);
	OPENSSL_cleanse(point, sizeof *point);
	OPENSSL_free(point);
	}


static int EC_POINT_copy(EC_POINT *dest, const EC_POINT *src)
	{
	if (dest == src)
		return 1;
	return ec_GFp_simple_point_copy(dest, src);
	}


static EC_POINT *EC_POINT_dup(const EC_POINT *a, const EC_GROUP *group)
	{
	EC_POINT *t;
	int r;

	if (a == NULL) return NULL;

	t = EC_POINT_new(group);
	if (t == NULL) return(NULL);
	r = EC_POINT_copy(t, a);
	if (!r)
		{
		EC_POINT_free(t);
		return NULL;
		}
	else return t;
	}


static int EC_POINT_set_to_infinity(const EC_GROUP *group, EC_POINT *point)
	{
	return ec_GFp_simple_point_set_to_infinity(group, point);
	}


static int EC_POINT_set_affine_coordinates_GFp(const EC_GROUP *group, EC_POINT *point,
	const BIGNUM *x, const BIGNUM *y, BN_CTX *ctx)
	{
	return ec_GFp_simple_point_set_affine_coordinates(group, point, x, y, ctx);
	}

static int EC_POINT_get_affine_coordinates_GFp(const EC_GROUP *group, const EC_POINT *point,
	BIGNUM *x, BIGNUM *y, BN_CTX *ctx)
	{
	return ec_GFp_simple_point_get_affine_coordinates(group, point, x, y, ctx);
	}

static int EC_POINT_add(const EC_GROUP *group, EC_POINT *r, const EC_POINT *a, const EC_POINT *b, BN_CTX *ctx)
	{
	return ec_GFp_simple_add(group, r, a, b, ctx);
	}


static int EC_POINT_dbl(const EC_GROUP *group, EC_POINT *r, const EC_POINT *a, BN_CTX *ctx)
	{
	return ec_GFp_simple_dbl(group, r, a, ctx);
	}


static int EC_POINT_invert(const EC_GROUP *group, EC_POINT *a, BN_CTX *ctx)
	{
	return ec_GFp_simple_invert(group, a, ctx);
	}


static int EC_POINT_is_at_infinity(const EC_GROUP *group, const EC_POINT *point)
	{
	return ec_GFp_simple_is_at_infinity(group, point);
	}


static int EC_POINT_cmp(const EC_GROUP *group, const EC_POINT *a, const EC_POINT *b, BN_CTX *ctx)
	{
	return ec_GFp_simple_cmp(group, a, b, ctx);
	}


static int EC_POINTs_make_affine(const EC_GROUP *group, size_t num, EC_POINT *points[], BN_CTX *ctx)
	{
	return ec_GFp_simple_points_make_affine(group, num, points, ctx);
	}


/* Functions for point multiplication.
 *
 * If group->meth->mul is 0, we use the wNAF-based implementations in ec_mult.c;
 * otherwise we dispatch through methods.
 */

static int EC_POINTs_mul(const EC_GROUP *group, EC_POINT *r, const BIGNUM *scalar,
	size_t num, const EC_POINT *points[], const BIGNUM *scalars[], BN_CTX *ctx)
	{
		/* use default */
		return ec_wNAF_mul(group, r, scalar, num, points, scalars, ctx);
	}

static int EC_POINT_mul(const EC_GROUP *group, EC_POINT *r, const BIGNUM *g_scalar,
	const EC_POINT *point, const BIGNUM *p_scalar, BN_CTX *ctx)
	{
	/* just a convenient interface to EC_POINTs_mul() */

	const EC_POINT *points[1];
	const BIGNUM *scalars[1];

	points[0] = point;
	scalars[0] = p_scalar;

	return EC_POINTs_mul(group, r, g_scalar, (point != NULL && p_scalar != NULL), points, scalars, ctx);
	}

static int EC_GROUP_precompute_mult(EC_GROUP *group, BN_CTX *ctx)
	{
		/* use default */
		return ec_wNAF_precompute_mult(group, ctx);
	}
