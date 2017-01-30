/* crypto/ec/ec_key.c */
/*
 * Written by Nils Larsch for the OpenSSL project.
 */
/* ====================================================================
 * Copyright (c) 1998-2005 The OpenSSL Project.  All rights reserved.
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
 * Portions originally developed by SUN MICROSYSTEMS, INC., and 
 * contributed to the OpenSSL project.
 */

#include <string.h>
#include "ec_lcl.h"

static EC_KEY *EC_KEY_new(void)
	{
	EC_KEY *ret;

	ret=(EC_KEY *)OPENSSL_malloc(sizeof(EC_KEY));
	if (ret == NULL)
		{
		ECerr(EC_F_EC_KEY_NEW, ERR_R_MALLOC_FAILURE);
		return(NULL);
		}

	ret->group   = NULL;
	ret->pub_key = NULL;
	ret->priv_key= NULL;
	return(ret);
	}

static void EC_KEY_free(EC_KEY *r)
	{
	if (r == NULL) return;

	if (r->group    != NULL) 
		EC_GROUP_free(r->group);
	if (r->pub_key  != NULL)
		EC_POINT_free(r->pub_key);
	if (r->priv_key != NULL)
		BN_clear_free(r->priv_key);

	OPENSSL_free(r);
	}

static const EC_GROUP *EC_KEY_get0_group(const EC_KEY *key)
	{
	return key->group;
	}

static int EC_KEY_set_group(EC_KEY *key, const EC_GROUP *group)
	{
	if (key->group != NULL)
		EC_GROUP_free(key->group);
	key->group = EC_GROUP_dup(group);
	return (key->group == NULL) ? 0 : 1;
	}

static const BIGNUM *EC_KEY_get0_private_key(const EC_KEY *key)
	{
	return key->priv_key;
	}

static int EC_KEY_set_private_key(EC_KEY *key, const BIGNUM *priv_key)
	{
	if (key->priv_key)
		BN_clear_free(key->priv_key);
	key->priv_key = BN_dup(priv_key);
	return (key->priv_key == NULL) ? 0 : 1;
	}

static const EC_POINT *EC_KEY_get0_public_key(const EC_KEY *key)
	{
	return key->pub_key;
	}

static int EC_KEY_set_public_key(EC_KEY *key, const EC_POINT *pub_key)
	{
	if (key->pub_key != NULL)
		EC_POINT_free(key->pub_key);
	key->pub_key = EC_POINT_dup(pub_key, key->group);
	return (key->pub_key == NULL) ? 0 : 1;
	}

