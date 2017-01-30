/* crypto/ec/ec.h */
/*
 * Originally written by Bodo Moeller for the OpenSSL project.
 */
/**
 * \file crypto/ec/ec.h Include file for the OpenSSL EC functions
 * \author Originally written by Bodo Moeller for the OpenSSL project
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

#ifndef HEADER_EC_H
#define HEADER_EC_H

#include <openssl/opensslconf.h>
#include <openssl/bn.h>

#ifdef  __cplusplus
extern "C" {
#elif defined(__SUNPRO_C)
# if __SUNPRO_C >= 0x520
# pragma error_messages (off,E_ARRAY_OF_INCOMPLETE_NONAME,E_ARRAY_OF_INCOMPLETE)
# endif
#endif

  
#ifndef OPENSSL_ECC_MAX_FIELD_BITS
# define OPENSSL_ECC_MAX_FIELD_BITS 661
#endif

typedef struct ec_method_st EC_METHOD;

typedef struct ec_group_st
	/*
	 EC_METHOD *meth;
	 -- field definition
	 -- curve coefficients
	 -- optional generator with associated information (order, cofactor)
	 -- optional extra data (precomputed table for fast computation of multiples of generator)
	 -- ASN1 stuff
	*/
	EC_GROUP;

typedef struct ec_point_st EC_POINT;


/********************************************************************/
/*               EC_METHODs for curves over GF(p)                   */       
/********************************************************************/

/** Returns GFp methods using montgomery multiplication.
 *  \return  EC_METHOD object
 */
static const EC_METHOD *EC_GFp_mont_method(void);


/********************************************************************/
/*                   EC_GROUP functions                             */
/********************************************************************/

/** Creates a new EC_GROUP object
 *  \param   meth  EC_METHOD to use
 *  \return  newly created EC_GROUP object or NULL in case of an error.
 */
static EC_GROUP *EC_GROUP_new(const EC_METHOD *meth);

/** Frees a EC_GROUP object
 *  \param  group  EC_GROUP object to be freed.
 */
static void EC_GROUP_free(EC_GROUP *group);

/** Clears and frees a EC_GROUP object
 *  \param  group  EC_GROUP object to be cleared and freed.
 */
static void EC_GROUP_clear_free(EC_GROUP *group);

/** Copies EC_GROUP objects. Note: both EC_GROUPs must use the same EC_METHOD.
 *  \param  dst  destination EC_GROUP object
 *  \param  src  source EC_GROUP object
 *  \return 1 on success and 0 if an error occurred.
 */
static int EC_GROUP_copy(EC_GROUP *dst, const EC_GROUP *src);

/** Creates a new EC_GROUP object and copies the copies the content
 *  form src to the newly created EC_KEY object
 *  \param  src  source EC_GROUP object
 *  \return newly created EC_GROUP object or NULL in case of an error.
 */
static EC_GROUP *EC_GROUP_dup(const EC_GROUP *src);

/** Sets the generator and it's order/cofactor of a EC_GROUP object.
 *  \param  group      EC_GROUP object 
 *  \param  generator  EC_POINT object with the generator.
 *  \param  order      the order of the group generated by the generator.
 *  \param  cofactor   the index of the sub-group generated by the generator
 *                     in the group of all points on the elliptic curve.
 *  \return 1 on success and 0 if an error occured
 */
static int EC_GROUP_set_generator(EC_GROUP *group, const EC_POINT *generator, const BIGNUM *order, const BIGNUM *cofactor);

/** Returns the generator of a EC_GROUP object.
 *  \param  group  EC_GROUP object
 *  \return the currently used generator (possibly NULL).
 */
static const EC_POINT *EC_GROUP_get0_generator(const EC_GROUP *group);

/** Gets the order of a EC_GROUP
 *  \param  group  EC_GROUP object
 *  \param  order  BIGNUM to which the order is copied
 *  \param  ctx    BN_CTX object (optional)
 *  \return 1 on success and 0 if an error occured
 */
static int EC_GROUP_get_order(const EC_GROUP *group, BIGNUM *order, BN_CTX *ctx);

/** Sets the parameter of a ec over GFp defined by y^2 = x^3 + a*x + b
 *  \param  group  EC_GROUP object
 *  \param  p      BIGNUM with the prime number
 *  \param  a      BIGNUM with parameter a of the equation
 *  \param  b      BIGNUM with parameter b of the equation
 *  \param  ctx    BN_CTX object (optional)
 *  \return 1 on success and 0 if an error occured
 */
static int EC_GROUP_set_curve_GFp(EC_GROUP *group, const BIGNUM *p, const BIGNUM *a, const BIGNUM *b, BN_CTX *ctx);

/********************************************************************/
/*                    EC_POINT functions                            */
/********************************************************************/

/** Creates a new EC_POINT object for the specified EC_GROUP
 *  \param  group  EC_GROUP the underlying EC_GROUP object
 *  \return newly created EC_POINT object or NULL if an error occurred
 */
static EC_POINT *EC_POINT_new(const EC_GROUP *group);

/** Frees a EC_POINT object
 *  \param  point  EC_POINT object to be freed
 */
static void EC_POINT_free(EC_POINT *point);

/** Clears and frees a EC_POINT object
 *  \param  point  EC_POINT object to be cleared and freed
 */
static void EC_POINT_clear_free(EC_POINT *point);

/** Copies EC_POINT object
 *  \param  dst  destination EC_POINT object
 *  \param  src  source EC_POINT object
 *  \return 1 on success and 0 if an error occured
 */
static int EC_POINT_copy(EC_POINT *dst, const EC_POINT *src);

/** Creates a new EC_POINT object and copies the content of the supplied
 *  EC_POINT
 *  \param  src    source EC_POINT object
 *  \param  group  underlying the EC_GROUP object
 *  \return newly created EC_POINT object or NULL if an error occurred 
 */
static EC_POINT *EC_POINT_dup(const EC_POINT *src, const EC_GROUP *group);

/** Sets a point to infinity (neutral element)
 *  \param  group  underlying EC_GROUP object
 *  \param  point  EC_POINT to set to infinity
 *  \return 1 on success and 0 if an error occured
 */
static int EC_POINT_set_to_infinity(const EC_GROUP *group, EC_POINT *point);

/** Sets the affine coordinates of a EC_POINT over GFp
 *  \param  group  underlying EC_GROUP object
 *  \param  p      EC_POINT object
 *  \param  x      BIGNUM with the x-coordinate
 *  \param  y      BIGNUM with the y-coordinate
 *  \param  ctx    BN_CTX object (optional)
 *  \return 1 on success and 0 if an error occured
 */
static int EC_POINT_set_affine_coordinates_GFp(const EC_GROUP *group, EC_POINT *p,
	const BIGNUM *x, const BIGNUM *y, BN_CTX *ctx);

/** Gets the affine coordinates of a EC_POINT over GFp
 *  \param  group  underlying EC_GROUP object
 *  \param  p      EC_POINT object
 *  \param  x      BIGNUM for the x-coordinate
 *  \param  y      BIGNUM for the y-coordinate
 *  \param  ctx    BN_CTX object (optional)
 *  \return 1 on success and 0 if an error occured
 */
static int EC_POINT_get_affine_coordinates_GFp(const EC_GROUP *group,
	const EC_POINT *p, BIGNUM *x, BIGNUM *y, BN_CTX *ctx);


/********************************************************************/
/*         functions for doing EC_POINT arithmetic                  */
/********************************************************************/

/** Computes the sum of two EC_POINT 
 *  \param  group  underlying EC_GROUP object
 *  \param  r      EC_POINT object for the result (r = a + b)
 *  \param  a      EC_POINT object with the first summand
 *  \param  b      EC_POINT object with the second summand
 *  \param  ctx    BN_CTX object (optional)
 *  \return 1 on success and 0 if an error occured
 */
static int EC_POINT_add(const EC_GROUP *group, EC_POINT *r, const EC_POINT *a, const EC_POINT *b, BN_CTX *ctx);

/** Computes the double of a EC_POINT
 *  \param  group  underlying EC_GROUP object
 *  \param  r      EC_POINT object for the result (r = 2 * a)
 *  \param  a      EC_POINT object 
 *  \param  ctx    BN_CTX object (optional)
 *  \return 1 on success and 0 if an error occured
 */
static int EC_POINT_dbl(const EC_GROUP *group, EC_POINT *r, const EC_POINT *a, BN_CTX *ctx);

/** Computes the inverse of a EC_POINT
 *  \param  group  underlying EC_GROUP object
 *  \param  a      EC_POINT object to be inverted (it's used for the result as well)
 *  \param  ctx    BN_CTX object (optional)
 *  \return 1 on success and 0 if an error occured
 */
static int EC_POINT_invert(const EC_GROUP *group, EC_POINT *a, BN_CTX *ctx);

/** Checks whether the point is the neutral element of the group
 *  \param  group  the underlying EC_GROUP object
 *  \param  p      EC_POINT object
 *  \return 1 if the point is the neutral element and 0 otherwise
 */
static int EC_POINT_is_at_infinity(const EC_GROUP *group, const EC_POINT *p);

/** Compares two EC_POINTs 
 *  \param  group  underlying EC_GROUP object
 *  \param  a      first EC_POINT object
 *  \param  b      second EC_POINT object
 *  \param  ctx    BN_CTX object (optional)
 *  \return 0 if both points are equal and a value != 0 otherwise
 */
static int EC_POINT_cmp(const EC_GROUP *group, const EC_POINT *a, const EC_POINT *b, BN_CTX *ctx);

static int EC_POINTs_make_affine(const EC_GROUP *group, size_t num, EC_POINT *points[], BN_CTX *ctx);

/** Computes r = generator * n sum_{i=0}^num p[i] * m[i]
 *  \param  group  underlying EC_GROUP object
 *  \param  r      EC_POINT object for the result
 *  \param  n      BIGNUM with the multiplier for the group generator (optional)
 *  \param  num    number futher summands
 *  \param  p      array of size num of EC_POINT objects
 *  \param  m      array of size num of BIGNUM objects
 *  \param  ctx    BN_CTX object (optional)
 *  \return 1 on success and 0 if an error occured
 */
static int EC_POINTs_mul(const EC_GROUP *group, EC_POINT *r, const BIGNUM *n, size_t num, const EC_POINT *p[], const BIGNUM *m[], BN_CTX *ctx);

/** Computes r = generator * n + q * m
 *  \param  group  underlying EC_GROUP object
 *  \param  r      EC_POINT object for the result
 *  \param  n      BIGNUM with the multiplier for the group generator (optional)
 *  \param  q      EC_POINT object with the first factor of the second summand
 *  \param  m      BIGNUM with the second factor of the second summand
 *  \param  ctx    BN_CTX object (optional)
 *  \return 1 on success and 0 if an error occured
 */
static int EC_POINT_mul(const EC_GROUP *group, EC_POINT *r, const BIGNUM *n, const EC_POINT *q, const BIGNUM *m, BN_CTX *ctx);

/** Stores multiples of generator for faster point multiplication
 *  \param  group  EC_GROUP object
 *  \param  ctx    BN_CTX object (optional)
 *  \return 1 on success and 0 if an error occured
 */
static int EC_GROUP_precompute_mult(EC_GROUP *group, BN_CTX *ctx);


/********************************************************************/
/*                      EC_KEY functions                            */
/********************************************************************/

typedef struct ec_key_st EC_KEY;

/** Creates a new EC_KEY object.
 *  \return EC_KEY object or NULL if an error occurred.
 */
static EC_KEY *EC_KEY_new(void);

/** Frees a EC_KEY object.
 *  \param  key  EC_KEY object to be freed.
 */
static void EC_KEY_free(EC_KEY *key);

/** Returns the EC_GROUP object of a EC_KEY object
 *  \param  key  EC_KEY object
 *  \return the EC_GROUP object (possibly NULL).
 */
static const EC_GROUP *EC_KEY_get0_group(const EC_KEY *key);

/** Sets the EC_GROUP of a EC_KEY object.
 *  \param  key    EC_KEY object
 *  \param  group  EC_GROUP to use in the EC_KEY object (note: the EC_KEY
 *                 object will use an own copy of the EC_GROUP).
 *  \return 1 on success and 0 if an error occurred.
 */
static int EC_KEY_set_group(EC_KEY *key, const EC_GROUP *group);

/** Returns the private key of a EC_KEY object.
 *  \param  key  EC_KEY object
 *  \return a BIGNUM with the private key (possibly NULL).
 */
static const BIGNUM *EC_KEY_get0_private_key(const EC_KEY *key);

/** Sets the private key of a EC_KEY object.
 *  \param  key  EC_KEY object
 *  \param  prv  BIGNUM with the private key (note: the EC_KEY object
 *               will use an own copy of the BIGNUM).
 *  \return 1 on success and 0 if an error occurred.
 */
static int EC_KEY_set_private_key(EC_KEY *key, const BIGNUM *prv);

/** Returns the public key of a EC_KEY object.
 *  \param  key  the EC_KEY object
 *  \return a EC_POINT object with the public key (possibly NULL)
 */
static const EC_POINT *EC_KEY_get0_public_key(const EC_KEY *key);

/** Sets the public key of a EC_KEY object.
 *  \param  key  EC_KEY object
 *  \param  pub  EC_POINT object with the public key (note: the EC_KEY object
 *               will use an own copy of the EC_POINT object).
 *  \return 1 on success and 0 if an error occurred.
 */
static int EC_KEY_set_public_key(EC_KEY *key, const EC_POINT *pub);

#ifndef __cplusplus
#if defined(__SUNPRO_C)
#  if __SUNPRO_C >= 0x520
# pragma error_messages (default,E_ARRAY_OF_INCOMPLETE_NONAME,E_ARRAY_OF_INCOMPLETE)
#  endif
# endif
#endif

/* BEGIN ERROR CODES */
/* The following lines are auto generated by the script mkerr.pl. Any changes
 * made after this point may be overwritten when the script is next run.
 */

/* Error codes for the EC functions. */

/* Function codes. */
#define EC_F_BN_TO_FELEM				 224
#define EC_F_COMPUTE_WNAF				 143
#define EC_F_D2I_ECPARAMETERS				 144
#define EC_F_D2I_ECPKPARAMETERS				 145
#define EC_F_D2I_ECPRIVATEKEY				 146
#define EC_F_DO_EC_KEY_PRINT				 221
#define EC_F_ECKEY_PARAM2TYPE				 223
#define EC_F_ECKEY_PARAM_DECODE				 212
#define EC_F_ECKEY_PRIV_DECODE				 213
#define EC_F_ECKEY_PRIV_ENCODE				 214
#define EC_F_ECKEY_PUB_DECODE				 215
#define EC_F_ECKEY_PUB_ENCODE				 216
#define EC_F_ECKEY_TYPE2PARAM				 220
#define EC_F_ECPARAMETERS_PRINT				 147
#define EC_F_ECPARAMETERS_PRINT_FP			 148
#define EC_F_ECPKPARAMETERS_PRINT			 149
#define EC_F_ECPKPARAMETERS_PRINT_FP			 150
#define EC_F_ECP_NIST_MOD_192				 203
#define EC_F_ECP_NIST_MOD_224				 204
#define EC_F_ECP_NIST_MOD_256				 205
#define EC_F_ECP_NIST_MOD_521				 206
#define EC_F_EC_ASN1_GROUP2CURVE			 153
#define EC_F_EC_ASN1_GROUP2FIELDID			 154
#define EC_F_EC_ASN1_GROUP2PARAMETERS			 155
#define EC_F_EC_ASN1_GROUP2PKPARAMETERS			 156
#define EC_F_EC_ASN1_PARAMETERS2GROUP			 157
#define EC_F_EC_ASN1_PKPARAMETERS2GROUP			 158
#define EC_F_EC_EX_DATA_SET_DATA			 211
#define EC_F_EC_GF2M_MONTGOMERY_POINT_MULTIPLY		 208
#define EC_F_EC_GF2M_SIMPLE_GROUP_CHECK_DISCRIMINANT	 159
#define EC_F_EC_GF2M_SIMPLE_GROUP_SET_CURVE		 195
#define EC_F_EC_GF2M_SIMPLE_OCT2POINT			 160
#define EC_F_EC_GF2M_SIMPLE_POINT2OCT			 161
#define EC_F_EC_GF2M_SIMPLE_POINT_GET_AFFINE_COORDINATES 162
#define EC_F_EC_GF2M_SIMPLE_POINT_SET_AFFINE_COORDINATES 163
#define EC_F_EC_GF2M_SIMPLE_SET_COMPRESSED_COORDINATES	 164
#define EC_F_EC_GFP_MONT_FIELD_DECODE			 133
#define EC_F_EC_GFP_MONT_FIELD_ENCODE			 134
#define EC_F_EC_GFP_MONT_FIELD_MUL			 131
#define EC_F_EC_GFP_MONT_FIELD_SET_TO_ONE		 209
#define EC_F_EC_GFP_MONT_FIELD_SQR			 132
#define EC_F_EC_GFP_MONT_GROUP_SET_CURVE		 189
#define EC_F_EC_GFP_MONT_GROUP_SET_CURVE_GFP		 135
#define EC_F_EC_GFP_NISTP224_GROUP_SET_CURVE		 225
#define EC_F_EC_GFP_NISTP224_POINTS_MUL			 228
#define EC_F_EC_GFP_NISTP224_POINT_GET_AFFINE_COORDINATES 226
#define EC_F_EC_GFP_NISTP256_GROUP_SET_CURVE		 230
#define EC_F_EC_GFP_NISTP256_POINTS_MUL			 231
#define EC_F_EC_GFP_NISTP256_POINT_GET_AFFINE_COORDINATES 232
#define EC_F_EC_GFP_NISTP521_GROUP_SET_CURVE		 233
#define EC_F_EC_GFP_NISTP521_POINTS_MUL			 234
#define EC_F_EC_GFP_NISTP521_POINT_GET_AFFINE_COORDINATES 235
#define EC_F_EC_GFP_NIST_FIELD_MUL			 200
#define EC_F_EC_GFP_NIST_FIELD_SQR			 201
#define EC_F_EC_GFP_NIST_GROUP_SET_CURVE		 202
#define EC_F_EC_GFP_SIMPLE_GROUP_CHECK_DISCRIMINANT	 165
#define EC_F_EC_GFP_SIMPLE_GROUP_SET_CURVE		 166
#define EC_F_EC_GFP_SIMPLE_GROUP_SET_CURVE_GFP		 100
#define EC_F_EC_GFP_SIMPLE_GROUP_SET_GENERATOR		 101
#define EC_F_EC_GFP_SIMPLE_MAKE_AFFINE			 102
#define EC_F_EC_GFP_SIMPLE_OCT2POINT			 103
#define EC_F_EC_GFP_SIMPLE_POINT2OCT			 104
#define EC_F_EC_GFP_SIMPLE_POINTS_MAKE_AFFINE		 137
#define EC_F_EC_GFP_SIMPLE_POINT_GET_AFFINE_COORDINATES	 167
#define EC_F_EC_GFP_SIMPLE_POINT_GET_AFFINE_COORDINATES_GFP 105
#define EC_F_EC_GFP_SIMPLE_POINT_SET_AFFINE_COORDINATES	 168
#define EC_F_EC_GFP_SIMPLE_POINT_SET_AFFINE_COORDINATES_GFP 128
#define EC_F_EC_GFP_SIMPLE_SET_COMPRESSED_COORDINATES	 169
#define EC_F_EC_GFP_SIMPLE_SET_COMPRESSED_COORDINATES_GFP 129
#define EC_F_EC_GROUP_CHECK				 170
#define EC_F_EC_GROUP_CHECK_DISCRIMINANT		 171
#define EC_F_EC_GROUP_COPY				 106
#define EC_F_EC_GROUP_GET0_GENERATOR			 139
#define EC_F_EC_GROUP_GET_COFACTOR			 140
#define EC_F_EC_GROUP_GET_CURVE_GF2M			 172
#define EC_F_EC_GROUP_GET_CURVE_GFP			 130
#define EC_F_EC_GROUP_GET_DEGREE			 173
#define EC_F_EC_GROUP_GET_ORDER				 141
#define EC_F_EC_GROUP_GET_PENTANOMIAL_BASIS		 193
#define EC_F_EC_GROUP_GET_TRINOMIAL_BASIS		 194
#define EC_F_EC_GROUP_NEW				 108
#define EC_F_EC_GROUP_NEW_BY_CURVE_NAME			 174
#define EC_F_EC_GROUP_NEW_FROM_DATA			 175
#define EC_F_EC_GROUP_PRECOMPUTE_MULT			 142
#define EC_F_EC_GROUP_SET_CURVE_GF2M			 176
#define EC_F_EC_GROUP_SET_CURVE_GFP			 109
#define EC_F_EC_GROUP_SET_EXTRA_DATA			 110
#define EC_F_EC_GROUP_SET_GENERATOR			 111
#define EC_F_EC_KEY_CHECK_KEY				 177
#define EC_F_EC_KEY_COPY				 178
#define EC_F_EC_KEY_GENERATE_KEY			 179
#define EC_F_EC_KEY_NEW					 182
#define EC_F_EC_KEY_PRINT				 180
#define EC_F_EC_KEY_PRINT_FP				 181
#define EC_F_EC_KEY_SET_PUBLIC_KEY_AFFINE_COORDINATES	 229
#define EC_F_EC_POINTS_MAKE_AFFINE			 136
#define EC_F_EC_POINT_ADD				 112
#define EC_F_EC_POINT_CMP				 113
#define EC_F_EC_POINT_COPY				 114
#define EC_F_EC_POINT_DBL				 115
#define EC_F_EC_POINT_GET_AFFINE_COORDINATES_GF2M	 183
#define EC_F_EC_POINT_GET_AFFINE_COORDINATES_GFP	 116
#define EC_F_EC_POINT_GET_JPROJECTIVE_COORDINATES_GFP	 117
#define EC_F_EC_POINT_INVERT				 210
#define EC_F_EC_POINT_IS_AT_INFINITY			 118
#define EC_F_EC_POINT_IS_ON_CURVE			 119
#define EC_F_EC_POINT_MAKE_AFFINE			 120
#define EC_F_EC_POINT_MUL				 184
#define EC_F_EC_POINT_NEW				 121
#define EC_F_EC_POINT_OCT2POINT				 122
#define EC_F_EC_POINT_POINT2OCT				 123
#define EC_F_EC_POINT_SET_AFFINE_COORDINATES_GF2M	 185
#define EC_F_EC_POINT_SET_AFFINE_COORDINATES_GFP	 124
#define EC_F_EC_POINT_SET_COMPRESSED_COORDINATES_GF2M	 186
#define EC_F_EC_POINT_SET_COMPRESSED_COORDINATES_GFP	 125
#define EC_F_EC_POINT_SET_JPROJECTIVE_COORDINATES_GFP	 126
#define EC_F_EC_POINT_SET_TO_INFINITY			 127
#define EC_F_EC_PRE_COMP_DUP				 207
#define EC_F_EC_PRE_COMP_NEW				 196
#define EC_F_EC_WNAF_MUL				 187
#define EC_F_EC_WNAF_PRECOMPUTE_MULT			 188
#define EC_F_I2D_ECPARAMETERS				 190
#define EC_F_I2D_ECPKPARAMETERS				 191
#define EC_F_I2D_ECPRIVATEKEY				 192
#define EC_F_I2O_ECPUBLICKEY				 151
#define EC_F_NISTP224_PRE_COMP_NEW			 227
#define EC_F_NISTP256_PRE_COMP_NEW			 236
#define EC_F_NISTP521_PRE_COMP_NEW			 237
#define EC_F_O2I_ECPUBLICKEY				 152
#define EC_F_OLD_EC_PRIV_DECODE				 222
#define EC_F_PKEY_EC_CTRL				 197
#define EC_F_PKEY_EC_CTRL_STR				 198
#define EC_F_PKEY_EC_DERIVE				 217
#define EC_F_PKEY_EC_KEYGEN				 199
#define EC_F_PKEY_EC_PARAMGEN				 219
#define EC_F_PKEY_EC_SIGN				 218

/* Reason codes. */
#define EC_R_ASN1_ERROR					 115
#define EC_R_ASN1_UNKNOWN_FIELD				 116
#define EC_R_BIGNUM_OUT_OF_RANGE			 144
#define EC_R_BUFFER_TOO_SMALL				 100
#define EC_R_COORDINATES_OUT_OF_RANGE			 146
#define EC_R_D2I_ECPKPARAMETERS_FAILURE			 117
#define EC_R_DECODE_ERROR				 142
#define EC_R_DISCRIMINANT_IS_ZERO			 118
#define EC_R_EC_GROUP_NEW_BY_NAME_FAILURE		 119
#define EC_R_FIELD_TOO_LARGE				 143
#define EC_R_GF2M_NOT_SUPPORTED				 147
#define EC_R_GROUP2PKPARAMETERS_FAILURE			 120
#define EC_R_I2D_ECPKPARAMETERS_FAILURE			 121
#define EC_R_INCOMPATIBLE_OBJECTS			 101
#define EC_R_INVALID_ARGUMENT				 112
#define EC_R_INVALID_COMPRESSED_POINT			 110
#define EC_R_INVALID_COMPRESSION_BIT			 109
#define EC_R_INVALID_CURVE				 141
#define EC_R_INVALID_DIGEST_TYPE			 138
#define EC_R_INVALID_ENCODING				 102
#define EC_R_INVALID_FIELD				 103
#define EC_R_INVALID_FORM				 104
#define EC_R_INVALID_GROUP_ORDER			 122
#define EC_R_INVALID_PENTANOMIAL_BASIS			 132
#define EC_R_INVALID_PRIVATE_KEY			 123
#define EC_R_INVALID_TRINOMIAL_BASIS			 137
#define EC_R_KEYS_NOT_SET				 140
#define EC_R_MISSING_PARAMETERS				 124
#define EC_R_MISSING_PRIVATE_KEY			 125
#define EC_R_NOT_A_NIST_PRIME				 135
#define EC_R_NOT_A_SUPPORTED_NIST_PRIME			 136
#define EC_R_NOT_IMPLEMENTED				 126
#define EC_R_NOT_INITIALIZED				 111
#define EC_R_NO_FIELD_MOD				 133
#define EC_R_NO_PARAMETERS_SET				 139
#define EC_R_PASSED_NULL_PARAMETER			 134
#define EC_R_PKPARAMETERS2GROUP_FAILURE			 127
#define EC_R_POINT_AT_INFINITY				 106
#define EC_R_POINT_IS_NOT_ON_CURVE			 107
#define EC_R_SLOT_FULL					 108
#define EC_R_UNDEFINED_GENERATOR			 113
#define EC_R_UNDEFINED_ORDER				 128
#define EC_R_UNKNOWN_GROUP				 129
#define EC_R_UNKNOWN_ORDER				 114
#define EC_R_UNSUPPORTED_FIELD				 131
#define EC_R_WRONG_CURVE_PARAMETERS			 145
#define EC_R_WRONG_ORDER				 130

#ifdef  __cplusplus
}
#endif
#endif
