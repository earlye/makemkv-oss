/*
    libMakeMKV - MKV multiplexer library

    Copyright (C) 2007-2016 GuinpinSoft inc <libmkv@makemkv.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/
#ifndef LGPL_OSSLABI_H_INCLUDED
#define LGPL_OSSLABI_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* AES */
typedef struct _OSSL_AES_KEY OSSL_AES_KEY;

unsigned int OSSL_sizeof_AES_KEY();

int OSSL_AES_set_encrypt_key(const unsigned char *userKey, const int bits,
	OSSL_AES_KEY *key);
int OSSL_AES_set_decrypt_key(const unsigned char *userKey, const int bits,
	OSSL_AES_KEY *key);

void OSSL_AES_encrypt(const unsigned char *in, unsigned char *out,
	const OSSL_AES_KEY *key);
void OSSL_AES_decrypt(const unsigned char *in, unsigned char *out,
	const OSSL_AES_KEY *key);

void OSSL_AES_cbc_decrypt(const unsigned char *in, unsigned char *out,
	const unsigned long length, const OSSL_AES_KEY *key,
	unsigned char *ivec);

/* SHA */
typedef struct _OSSL_SHA_CTX OSSL_SHA_CTX;
#define OSSL_SHA_DIGEST_LENGTH  20

unsigned int OSSL_sizeof_SHA_CTX();

int OSSL_SHA1_Init(OSSL_SHA_CTX *c);
int OSSL_SHA1_Update(OSSL_SHA_CTX *c, const void *data, size_t len);
int OSSL_SHA1_Final(unsigned char *md, OSSL_SHA_CTX *c);

/* BN */
typedef struct _OSSL_BIGNUM     OSSL_BIGNUM;
typedef struct _OSSL_BN_CTX     OSSL_BN_CTX;

OSSL_BIGNUM *OSSL_BN_new(void);
void OSSL_BN_free(OSSL_BIGNUM *a);
OSSL_BIGNUM *OSSL_BN_bin2bn(const unsigned char *s,int len,OSSL_BIGNUM *ret);
int OSSL_BN_bn2bin(const OSSL_BIGNUM *a, unsigned char *to);
int OSSL_BN_dec2bn(OSSL_BIGNUM **a, const char *str);
int OSSL_BN_hex2bn(OSSL_BIGNUM **a, const char *str);
int OSSL_BN_ucmp(const OSSL_BIGNUM *a, const OSSL_BIGNUM *b);
int OSSL_BN_is_zero(const OSSL_BIGNUM *a);
int OSSL_BN_set_word(OSSL_BIGNUM *a, uint32_t w);
int OSSL_BN_nnmod(OSSL_BIGNUM *r, const OSSL_BIGNUM *m, const OSSL_BIGNUM *d, OSSL_BN_CTX *ctx);
OSSL_BIGNUM *OSSL_BN_mod_inverse(OSSL_BIGNUM *ret, const OSSL_BIGNUM *a, const OSSL_BIGNUM *n,OSSL_BN_CTX *ctx);
OSSL_BN_CTX *OSSL_BN_CTX_new(void);
void OSSL_BN_CTX_free(OSSL_BN_CTX *c);

/* EC */
typedef struct _OSSL_EC_GROUP	OSSL_EC_GROUP;
typedef struct _OSSL_EC_METHOD	OSSL_EC_METHOD;
typedef struct _OSSL_EC_POINT	OSSL_EC_POINT;
typedef struct _OSSL_EC_KEY     OSSL_EC_KEY;

const OSSL_EC_POINT *OSSL_EC_GROUP_get0_generator(const OSSL_EC_GROUP *);
int OSSL_EC_GROUP_set_generator(OSSL_EC_GROUP *, const OSSL_EC_POINT *generator, const OSSL_BIGNUM *order, const OSSL_BIGNUM *cofactor);
int OSSL_EC_GROUP_get_order(const OSSL_EC_GROUP *, OSSL_BIGNUM *order, OSSL_BN_CTX *);
OSSL_EC_GROUP *OSSL_EC_GROUP_new_curve_GFp(const OSSL_BIGNUM *p, const OSSL_BIGNUM *a, const OSSL_BIGNUM *b, OSSL_BN_CTX *);

OSSL_EC_POINT *OSSL_EC_POINT_new(const OSSL_EC_GROUP *);
void OSSL_EC_POINT_free(OSSL_EC_POINT *);
int OSSL_EC_POINT_mul(const OSSL_EC_GROUP *, OSSL_EC_POINT *r, const OSSL_BIGNUM *, const OSSL_EC_POINT *, const OSSL_BIGNUM *, OSSL_BN_CTX *);
int OSSL_EC_POINT_set_affine_coordinates_GFp(const OSSL_EC_GROUP *, OSSL_EC_POINT *,
    const OSSL_BIGNUM *x, const OSSL_BIGNUM *y, OSSL_BN_CTX *);
int OSSL_EC_POINT_get_affine_coordinates_GFp(const OSSL_EC_GROUP *, const OSSL_EC_POINT *,
    OSSL_BIGNUM *x, OSSL_BIGNUM *y, OSSL_BN_CTX *);

OSSL_EC_KEY *OSSL_EC_KEY_new(void);
void OSSL_EC_KEY_free(OSSL_EC_KEY *);
const OSSL_EC_GROUP *OSSL_EC_KEY_get0_group(const OSSL_EC_KEY *);
int OSSL_EC_KEY_set_group(OSSL_EC_KEY *, const OSSL_EC_GROUP *);
int OSSL_EC_KEY_set_public_key(OSSL_EC_KEY *, const OSSL_EC_POINT *);
int OSSL_EC_KEY_set_private_key(OSSL_EC_KEY *, const OSSL_BIGNUM *);

/* ECDSA */
typedef struct _OSSL_ECDSA_SIG OSSL_ECDSA_SIG;
typedef struct _OSSL_ECDSA_METHOD OSSL_ECDSA_METHOD;

int OSSL_ECDSA_do_verify_rs(const unsigned char *dgst, int dgst_len,const OSSL_BIGNUM *sig_r,const OSSL_BIGNUM *sig_s,OSSL_EC_KEY* eckey);
int OSSL_ECDSA_sign_setup(OSSL_EC_KEY *eckey, OSSL_BIGNUM **kinv, OSSL_BIGNUM **rp);
OSSL_ECDSA_SIG *OSSL_ECDSA_do_sign_ex(const unsigned char *dgst, int dgstlen,const OSSL_BIGNUM *kinv, const OSSL_BIGNUM *rp, OSSL_EC_KEY *eckey);
const OSSL_BIGNUM* OSSL_ecdsa_sig_get_r(const OSSL_ECDSA_SIG* sig);
const OSSL_BIGNUM* OSSL_ecdsa_sig_get_s(const OSSL_ECDSA_SIG* sig);
void OSSL_ECDSA_SIG_free(OSSL_ECDSA_SIG *a);

#ifdef __cplusplus
};
#endif

#endif // LGPL_OSSLABI_H_INCLUDED

