/*	$NetBSD: crypto-evp.c,v 1.3.14.1 2023/08/11 13:40:00 martin Exp $	*/

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

void
_krb5_evp_schedule(krb5_context context,
		   struct _krb5_key_type *kt,
		   struct _krb5_key_data *kd)
{
    struct _krb5_evp_schedule *key = kd->schedule->data;
    const EVP_CIPHER *c = (*kt->evp)();

#if OPENSSL_VERSION_NUMBER < 0x10100000UL
    key->ectx = malloc(sizeof(*key->ectx));
    key->dctx = malloc(sizeof(*key->dctx));
    EVP_CIPHER_CTX_init(key->ectx);
    EVP_CIPHER_CTX_init(key->dctx);
#else
    key->ectx = EVP_CIPHER_CTX_new();
    key->dctx = EVP_CIPHER_CTX_new();
#endif

    if (!EVP_CipherInit_ex(key->ectx, c, NULL, kd->key->keyvalue.data, NULL, 1))
	krb5_abortx(context, "can't initialize cipher");
    if (!EVP_CipherInit_ex(key->dctx, c, NULL, kd->key->keyvalue.data, NULL, 0))
	krb5_abortx(context, "can't initialize cipher");
}

void
_krb5_evp_cleanup(krb5_context context, struct _krb5_key_data *kd)
{
    struct _krb5_evp_schedule *key = kd->schedule->data;
#if OPENSSL_VERSION_NUMBER < 0x10100000UL
    EVP_CIPHER_CTX_cleanup(key->ectx);
    EVP_CIPHER_CTX_cleanup(key->dctx);
    free(key->ectx);
    free(key->dctx);
#else
    EVP_CIPHER_CTX_free(key->ectx);
    EVP_CIPHER_CTX_free(key->dctx);
#endif
}

krb5_error_code
_krb5_evp_encrypt(krb5_context context,
		struct _krb5_key_data *key,
		void *data,
		size_t len,
		krb5_boolean encryptp,
		int usage,
		void *ivec)
{
    struct _krb5_evp_schedule *ctx = key->schedule->data;
    EVP_CIPHER_CTX *c;
    c = encryptp ? ctx->ectx : ctx->dctx;
    if (ivec == NULL) {
	/* alloca ? */
	size_t len2 = EVP_CIPHER_CTX_iv_length(c);
	void *loiv = malloc(len2);
	if (loiv == NULL)
	    return krb5_enomem(context);
	memset(loiv, 0, len2);
	if (!EVP_CipherInit_ex(c, NULL, NULL, NULL, loiv, -1))
	    krb5_abortx(context, "can't initialize cipher");
	free(loiv);
    } else if (!EVP_CipherInit_ex(c, NULL, NULL, NULL, ivec, -1))
	krb5_abortx(context, "can't initialize cipher");
	
    EVP_Cipher(c, data, data, len);
    return 0;
}

static const unsigned char zero_ivec[EVP_MAX_BLOCK_LENGTH] = { 0 };

krb5_error_code
_krb5_evp_encrypt_cts(krb5_context context,
		      struct _krb5_key_data *key,
		      void *data,
		      size_t len,
		      krb5_boolean encryptp,
		      int usage,
		      void *ivec)
{
    size_t i, blocksize;
    int ret;
    struct _krb5_evp_schedule *ctx = key->schedule->data;
    unsigned char tmp[EVP_MAX_BLOCK_LENGTH], ivec2[EVP_MAX_BLOCK_LENGTH];
    EVP_CIPHER_CTX *c;
    unsigned char *p;

    c = encryptp ? ctx->ectx : ctx->dctx;

    blocksize = EVP_CIPHER_CTX_block_size(c);

    if (len < blocksize) {
	krb5_set_error_message(context, EINVAL,
			       "message block too short");
	return EINVAL;
    } else if (len == blocksize) {
	if (!EVP_CipherInit_ex(c, NULL, NULL, NULL, zero_ivec, -1))
	    krb5_abortx(context, "can't initialize cipher");
	EVP_Cipher(c, data, data, len);
	return 0;
    }

    if (ivec)
	ret = EVP_CipherInit_ex(c, NULL, NULL, NULL, ivec, -1);
    else
	ret = EVP_CipherInit_ex(c, NULL, NULL, NULL, zero_ivec, -1);
    if (!ret)
	krb5_abortx(context, "can't initialize cipher");

    if (encryptp) {

	p = data;
	i = ((len - 1) / blocksize) * blocksize;
	EVP_Cipher(c, p, p, i);
	p += i - blocksize;
	len -= i;
	memcpy(ivec2, p, blocksize);

	for (i = 0; i < len; i++)
	    tmp[i] = p[i + blocksize] ^ ivec2[i];
	for (; i < blocksize; i++)
	    tmp[i] = 0 ^ ivec2[i];

	if (!EVP_CipherInit_ex(c, NULL, NULL, NULL, zero_ivec, -1))
	    krb5_abortx(context, "can't initialize cipher");
	EVP_Cipher(c, p, tmp, blocksize);

	memcpy(p + blocksize, ivec2, len);
	if (ivec)
	    memcpy(ivec, p, blocksize);
    } else {
	unsigned char tmp2[EVP_MAX_BLOCK_LENGTH], tmp3[EVP_MAX_BLOCK_LENGTH];

	p = data;
	if (len > blocksize * 2) {
	    /* remove last two blocks and round up, decrypt this with cbc, then do cts dance */
	    i = ((((len - blocksize * 2) + blocksize - 1) / blocksize) * blocksize);
	    memcpy(ivec2, p + i - blocksize, blocksize);
	    EVP_Cipher(c, p, p, i);
	    p += i;
	    len -= i + blocksize;
	} else {
	    if (ivec)
		memcpy(ivec2, ivec, blocksize);
	    else
		memcpy(ivec2, zero_ivec, blocksize);
	    len -= blocksize;
	}

	memcpy(tmp, p, blocksize);
	if (!EVP_CipherInit_ex(c, NULL, NULL, NULL, zero_ivec, -1))
	    krb5_abortx(context, "can't initialize cipher");
	EVP_Cipher(c, tmp2, p, blocksize);

	memcpy(tmp3, p + blocksize, len);
	memcpy(tmp3 + len, tmp2 + len, blocksize - len); /* xor 0 */

	for (i = 0; i < len; i++)
	    p[i + blocksize] = tmp2[i] ^ tmp3[i];

	if (!EVP_CipherInit_ex(c, NULL, NULL, NULL, zero_ivec, -1))
	    krb5_abortx(context, "can't initialize cipher");
	EVP_Cipher(c, p, tmp3, blocksize);

	for (i = 0; i < blocksize; i++)
	    p[i] ^= ivec2[i];
	if (ivec)
	    memcpy(ivec, tmp, blocksize);
    }
    return 0;
}
