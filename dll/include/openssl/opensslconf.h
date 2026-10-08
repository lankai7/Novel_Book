/*
 * Copyright 1998-2019 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef OPENSSL_OPENSSLCONF_H
#define OPENSSL_OPENSSLCONF_H
#pragma once

#ifdef  __cplusplus
extern "C" {
#endif

/* Declaration deprecated */
# if defined(__GNUC__) && __GNUC__ >= 4
#  define DECLARE_DEPRECATED(f)    f __attribute__ ((deprecated));
# elif defined(_MSC_VER)
#  define DECLARE_DEPRECATED(f)    __declspec(deprecated) f;
# else
#  define DECLARE_DEPRECATED(f)    f;
# endif

/* Deprecation macros */
#define DEPRECATEDIN_0_9_8(f)   DECLARE_DEPRECATED(f)
#define DEPRECATEDIN_1_0_0(f)   DECLARE_DEPRECATED(f)
#define DEPRECATEDIN_1_0_1(f)   DECLARE_DEPRECATED(f)
#define DEPRECATEDIN_1_0_2(f)   DECLARE_DEPRECATED(f)
#define DEPRECATEDIN_1_1_0(f)   DECLARE_DEPRECATED(f)
#define DEPRECATEDIN_1_1_1(f)   DECLARE_DEPRECATED(f)
#define DEPRECATEDIN_1_2_0(f)   DECLARE_DEPRECATED(f)
#define DEPRECATEDIN_3_0_0(f)   DECLARE_DEPRECATED(f)

#define OPENSSL_NO_ASM
#define OPENSSL_THREADS
#define OPENSSL_NO_STATIC_ENGINE

#ifdef BN_LONGLONG
#undef BN_LONGLONG
#endif
#define BN_LONGLONG
#define SIXTY_FOUR_BIT_LONG
#define SIXTY_FOUR_BIT

#define OPENSSL_NO_CMS
#define OPENSSL_NO_JPAKE
#define OPENSSL_NO_MD2
#define OPENSSL_NO_RMD160
#define OPENSSL_NO_SEED
#define OPENSSL_NO_WHRLPOOL
#define OPENSSL_NO_EC2M
#define OPENSSL_NO_SHA0

#ifdef  __cplusplus
}
#endif

#endif /* OPENSSL_OPENSSLCONF_H */
