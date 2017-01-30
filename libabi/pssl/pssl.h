/* This file is released to public domain, no copyright claimed */
#ifndef HEADER_PSSL_H
#define HEADER_PSSL_H

#include <openssl/opensslconf.h>
#include "ec.h"
#include "ecdsa.h"

#define ECerr(a,b) {}
#define ECDSAerr(a,b) {}

#include "ec_key.c"
#include "ec_lib.c"
#include "ec_mult.c"
#include "ecp_mont.c"
#include "ecp_smpl.c"
#include "ecs_ossl.c"
#include "ecs_sign.c"
#include "ecs_vrf.c"

#endif /* HEADER_PSSL_H */
