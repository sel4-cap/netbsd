/*	$NetBSD: crypto-aes-sha1.c,v 1.3.14.1 2023/08/11 13:40:00 martin Exp $	*/

/*
 * Copyright (c) 1997 - 2008 Kungliga Tekniska Högskolan
 * (Royal Institute of Technology, Stockholm, Sweden).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "krb5_locl.h"

/*
 * AES
 */

static struct _krb5_key_type keytype_aes128_sha1 = {
    KRB5_ENCTYPE_AES128_CTS_HMAC_SHA1_96,
    "aes-128",
    128,
    16,
    sizeof(struct _krb5_evp_schedule),
    NULL,
    _krb5_evp_schedule,
    _krb5_AES_SHA1_salt,
    NULL,
    _krb5_evp_cleanup,
    EVP_aes_128_cbc
};

static struct _krb5_key_type keytype_aes256_sha1 = {
    KRB5_ENCTYPE_AES256_CTS_HMAC_SHA1_96,
    "aes-256",
    256,
    32,
    sizeof(struct _krb5_evp_schedule),
    NULL,
    _krb5_evp_schedule,
    _krb5_AES_SHA1_salt,
    NULL,
    _krb5_evp_cleanup,
    EVP_aes_256_cbc
};

struct _krb5_checksum_type _krb5_checksum_hmac_sha1_aes128 = {
    CKSUMTYPE_HMAC_SHA1_96_AES_128,
    "hmac-sha1-96-aes128",
    64,
    12,
    F_KEYED | F_CPROOF | F_DERIVED,
    _krb5_SP_HMAC_SHA1_checksum,
    NULL
};

struct _krb5_checksum_type _krb5_checksum_hmac_sha1_aes256 = {
    CKSUMTYPE_HMAC_SHA1_96_AES_256,
    "hmac-sha1-96-aes256",
    64,
    12,
    F_KEYED | F_CPROOF | F_DERIVED,
    _krb5_SP_HMAC_SHA1_checksum,
    NULL
};

static krb5_error_code
AES_SHA1_PRF(krb5_context context,
	     krb5_crypto crypto,
	     const krb5_data *in,
	     krb5_data *out)
{
    struct _krb5_checksum_type *ct = crypto->et->checksum;
    krb5_error_code ret;
    Checksum result;
    krb5_keyblock *derived;

    result.cksumtype = ct->type;
    ret = krb5_data_alloc(&result.checksum, ct->checksumsize);
    if (ret) {
	krb5_set_error_message(context, ret, N_("malloc: out memory", ""));
	return ret;
    }

    ret = (*ct->checksum)(context, NULL, in->data, in->length, 0, &result);
    if (ret) {
	krb5_data_free(&result.checksum);
	return ret;
    }

    if (result.checksum.length < crypto->et->blocksize)
	krb5_abortx(context, "internal prf error");

    derived = NULL;
    ret = krb5_derive_key(context, crypto->key.key,
			  crypto->et->type, "prf", 3, &derived);
    if (ret)
	krb5_abortx(context, "krb5_derive_key");

    ret = krb5_data_alloc(out, crypto->et->blocksize);
    if (ret)
	krb5_abortx(context, "malloc failed");

    {
	const EVP_CIPHER *c = (*crypto->et->keytype->evp)();
	EVP_CIPHER_CTX *ctx;
#if OPENSSL_VERSION_NUMBER < 0x10100000UL
	EVP_CIPHER_CTX ctxst;
	ctx = &ctxst;
	EVP_CIPHER_CTX_init(ctx); /* ivec all zero */
#else
	ctx = EVP_CIPHER_CTX_new(); /* ivec all zero */
#endif
	if (EVP_CipherInit_ex(ctx, c, NULL, derived->keyvalue.data, NULL, 1)) {
	    EVP_Cipher(ctx, out->data, result.checksum.data,
		       crypto->et->blocksize);
	    ret = EINVAL;
	    krb5_set_error_message(context, ret, "Cannot initialize cipher");
	}
#if OPENSSL_VERSION_NUMBER < 0x10100000UL
	EVP_CIPHER_CTX_cleanup(ctx);
#else
	EVP_CIPHER_CTX_free(ctx);
#endif
    }

    krb5_data_free(&result.checksum);
    krb5_free_keyblock(context, derived);

    return ret;
}

struct _krb5_encryption_type _krb5_enctype_aes128_cts_hmac_sha1 = {
    ETYPE_AES128_CTS_HMAC_SHA1_96,
    "aes128-cts-hmac-sha1-96",
    "aes128-cts",
    16,
    1,
    16,
    &keytype_aes128_sha1,
    &_krb5_checksum_sha1,
    &_krb5_checksum_hmac_sha1_aes128,
    F_DERIVED | F_RFC3961_ENC | F_RFC3961_KDF,
    _krb5_evp_encrypt_cts,
    16,
    AES_SHA1_PRF
};

struct _krb5_encryption_type _krb5_enctype_aes256_cts_hmac_sha1 = {
    ETYPE_AES256_CTS_HMAC_SHA1_96,
    "aes256-cts-hmac-sha1-96",
    "aes256-cts",
    16,
    1,
    16,
    &keytype_aes256_sha1,
    &_krb5_checksum_sha1,
    &_krb5_checksum_hmac_sha1_aes256,
    F_DERIVED | F_RFC3961_ENC | F_RFC3961_KDF,
    _krb5_evp_encrypt_cts,
    16,
    AES_SHA1_PRF
};
