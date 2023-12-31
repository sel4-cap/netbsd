/*	$NetBSD: aes-test.c,v 1.2.22.1 2023/08/11 13:40:00 martin Exp $	*/

/*
 * Copyright (c) 2003-2016 Kungliga Tekniska Högskolan
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
 * 3. Neither the name of KTH nor the names of its contributors may be
 *    used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY KTH AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL KTH OR ITS CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#include "krb5_locl.h"
#include <krb5/hex.h>
#include <err.h>
#include <assert.h>

static int verbose = 0;

static void
hex_dump_data(const void *data, size_t length)
{
    char *p;

    hex_encode(data, length, &p);
    printf("%s\n", p);
    free(p);
}

struct {
    char *password;
    char *salt;
    int saltlen;
    int iterations;
    krb5_enctype enctype;
    size_t keylen;
    char *pbkdf2;
    char *key;
} keys[] = {
    {
	"password",
	"\x10\xDF\x9D\xD7\x83\xE5\xBC\x8A\xCE\xA1\x73\x0E\x74\x35\x5F\x61"
	"ATHENA.MIT.EDUraeburn",
	37,
	32768,
	KRB5_ENCTYPE_AES128_CTS_HMAC_SHA256_128,
	16,
	NULL,
	"\x08\x9B\xCA\x48\xB1\x05\xEA\x6E\xA7\x7C\xA5\xD2\xF3\x9D\xC5\xE7"
    },
    {
	"password",
	"\x10\xDF\x9D\xD7\x83\xE5\xBC\x8A\xCE\xA1\x73\x0E\x74\x35\x5F\x61"
	"ATHENA.MIT.EDUraeburn",
	37,
	32768,
	KRB5_ENCTYPE_AES256_CTS_HMAC_SHA384_192,
	32,
	NULL,
	"\x45\xBD\x80\x6D\xBF\x6A\x83\x3A\x9C\xFF\xC1\xC9\x45\x89\xA2\x22"
	"\x36\x7A\x79\xBC\x21\xC4\x13\x71\x89\x06\xE9\xF5\x78\xA7\x84\x67"
    },
    {
	"password", "ATHENA.MIT.EDUraeburn", -1,
	1,
	ETYPE_AES128_CTS_HMAC_SHA1_96, 16,
	"\xcd\xed\xb5\x28\x1b\xb2\xf8\x01\x56\x5a\x11\x22\xb2\x56\x35\x15",
	"\x42\x26\x3c\x6e\x89\xf4\xfc\x28\xb8\xdf\x68\xee\x09\x79\x9f\x15"
    },
    {
	"password", "ATHENA.MIT.EDUraeburn", -1,
	1,
	ETYPE_AES256_CTS_HMAC_SHA1_96, 32,
	"\xcd\xed\xb5\x28\x1b\xb2\xf8\x01\x56\x5a\x11\x22\xb2\x56\x35\x15"
	"\x0a\xd1\xf7\xa0\x4b\xb9\xf3\xa3\x33\xec\xc0\xe2\xe1\xf7\x08\x37",
	"\xfe\x69\x7b\x52\xbc\x0d\x3c\xe1\x44\x32\xba\x03\x6a\x92\xe6\x5b"
	"\xbb\x52\x28\x09\x90\xa2\xfa\x27\x88\x39\x98\xd7\x2a\xf3\x01\x61"
    },
    {
	"password", "ATHENA.MIT.EDUraeburn", -1,
	2,
	ETYPE_AES128_CTS_HMAC_SHA1_96, 16,
	"\x01\xdb\xee\x7f\x4a\x9e\x24\x3e\x98\x8b\x62\xc7\x3c\xda\x93\x5d",
	"\xc6\x51\xbf\x29\xe2\x30\x0a\xc2\x7f\xa4\x69\xd6\x93\xbd\xda\x13"
    },
    {
	"password", "ATHENA.MIT.EDUraeburn", -1,
	2,
	ETYPE_AES256_CTS_HMAC_SHA1_96, 32,
	"\x01\xdb\xee\x7f\x4a\x9e\x24\x3e\x98\x8b\x62\xc7\x3c\xda\x93\x5d"
	"\xa0\x53\x78\xb9\x32\x44\xec\x8f\x48\xa9\x9e\x61\xad\x79\x9d\x86",
	"\xa2\xe1\x6d\x16\xb3\x60\x69\xc1\x35\xd5\xe9\xd2\xe2\x5f\x89\x61"
	"\x02\x68\x56\x18\xb9\x59\x14\xb4\x67\xc6\x76\x22\x22\x58\x24\xff"
    },
    {
	"password", "ATHENA.MIT.EDUraeburn", -1,
	1200,
	ETYPE_AES128_CTS_HMAC_SHA1_96, 16,
	"\x5c\x08\xeb\x61\xfd\xf7\x1e\x4e\x4e\xc3\xcf\x6b\xa1\xf5\x51\x2b",
	"\x4c\x01\xcd\x46\xd6\x32\xd0\x1e\x6d\xbe\x23\x0a\x01\xed\x64\x2a"
    },
    {
	"password", "ATHENA.MIT.EDUraeburn", -1,
	1200,
	ETYPE_AES256_CTS_HMAC_SHA1_96, 32,
	"\x5c\x08\xeb\x61\xfd\xf7\x1e\x4e\x4e\xc3\xcf\x6b\xa1\xf5\x51\x2b"
	"\xa7\xe5\x2d\xdb\xc5\xe5\x14\x2f\x70\x8a\x31\xe2\xe6\x2b\x1e\x13",
	"\x55\xa6\xac\x74\x0a\xd1\x7b\x48\x46\x94\x10\x51\xe1\xe8\xb0\xa7"
	"\x54\x8d\x93\xb0\xab\x30\xa8\xbc\x3f\xf1\x62\x80\x38\x2b\x8c\x2a"
    },
    {
	"password", "\x12\x34\x56\x78\x78\x56\x34\x12", 8,
	5,
	ETYPE_AES128_CTS_HMAC_SHA1_96, 16,
	"\xd1\xda\xa7\x86\x15\xf2\x87\xe6\xa1\xc8\xb1\x20\xd7\x06\x2a\x49",
	"\xe9\xb2\x3d\x52\x27\x37\x47\xdd\x5c\x35\xcb\x55\xbe\x61\x9d\x8e"
    },
    {
	"password", "\x12\x34\x56\x78\x78\x56\x34\x12", 8,
	5,
	ETYPE_AES256_CTS_HMAC_SHA1_96, 32,
	"\xd1\xda\xa7\x86\x15\xf2\x87\xe6\xa1\xc8\xb1\x20\xd7\x06\x2a\x49"
	"\x3f\x98\xd2\x03\xe6\xbe\x49\xa6\xad\xf4\xfa\x57\x4b\x6e\x64\xee",
	"\x97\xa4\xe7\x86\xbe\x20\xd8\x1a\x38\x2d\x5e\xbc\x96\xd5\x90\x9c"
	"\xab\xcd\xad\xc8\x7c\xa4\x8f\x57\x45\x04\x15\x9f\x16\xc3\x6e\x31"
    },
    {
	"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
	"pass phrase equals block size", -1,
	1200,
	ETYPE_AES128_CTS_HMAC_SHA1_96, 16,
	"\x13\x9c\x30\xc0\x96\x6b\xc3\x2b\xa5\x5f\xdb\xf2\x12\x53\x0a\xc9",
	"\x59\xd1\xbb\x78\x9a\x82\x8b\x1a\xa5\x4e\xf9\xc2\x88\x3f\x69\xed"
    },
    {
	"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
	"pass phrase equals block size", -1,
	1200,
	ETYPE_AES256_CTS_HMAC_SHA1_96, 32,
	"\x13\x9c\x30\xc0\x96\x6b\xc3\x2b\xa5\x5f\xdb\xf2\x12\x53\x0a\xc9"
	"\xc5\xec\x59\xf1\xa4\x52\xf5\xcc\x9a\xd9\x40\xfe\xa0\x59\x8e\xd1",
	"\x89\xad\xee\x36\x08\xdb\x8b\xc7\x1f\x1b\xfb\xfe\x45\x94\x86\xb0"
	"\x56\x18\xb7\x0c\xba\xe2\x20\x92\x53\x4e\x56\xc5\x53\xba\x4b\x34"
    },
    {
	"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
	"pass phrase exceeds block size", -1,
	1200,
	ETYPE_AES128_CTS_HMAC_SHA1_96, 16,
	"\x9c\xca\xd6\xd4\x68\x77\x0c\xd5\x1b\x10\xe6\xa6\x87\x21\xbe\x61",
	"\xcb\x80\x05\xdc\x5f\x90\x17\x9a\x7f\x02\x10\x4c\x00\x18\x75\x1d"
    },
    {
	"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
	"pass phrase exceeds block size", -1,
	1200,
	ETYPE_AES256_CTS_HMAC_SHA1_96, 32,
	"\x9c\xca\xd6\xd4\x68\x77\x0c\xd5\x1b\x10\xe6\xa6\x87\x21\xbe\x61"
	"\x1a\x8b\x4d\x28\x26\x01\xdb\x3b\x36\xbe\x92\x46\x91\x5e\xc8\x2a",
	"\xd7\x8c\x5c\x9c\xb8\x72\xa8\xc9\xda\xd4\x69\x7f\x0b\xb5\xb2\xd2"
	"\x14\x96\xc8\x2b\xeb\x2c\xae\xda\x21\x12\xfc\xee\xa0\x57\x40\x1b"
    },
    {
	"\xf0\x9d\x84\x9e" /* g-clef */, "EXAMPLE.COMpianist", -1,
	50,
	ETYPE_AES128_CTS_HMAC_SHA1_96, 16,
	"\x6b\x9c\xf2\x6d\x45\x45\x5a\x43\xa5\xb8\xbb\x27\x6a\x40\x3b\x39",
	"\xf1\x49\xc1\xf2\xe1\x54\xa7\x34\x52\xd4\x3e\x7f\xe6\x2a\x56\xe5"
    },
    {
	"\xf0\x9d\x84\x9e" /* g-clef */, "EXAMPLE.COMpianist", -1,
	50,
	ETYPE_AES256_CTS_HMAC_SHA1_96, 32,
	"\x6b\x9c\xf2\x6d\x45\x45\x5a\x43\xa5\xb8\xbb\x27\x6a\x40\x3b\x39"
	"\xe7\xfe\x37\xa0\xc4\x1e\x02\xc2\x81\xff\x30\x69\xe1\xe9\x4f\x52",
	"\x4b\x6d\x98\x39\xf8\x44\x06\xdf\x1f\x09\xcc\x16\x6d\xb4\xb8\x3c"
	"\x57\x18\x48\xb7\x84\xa3\xd6\xbd\xc3\x46\x58\x9a\x3e\x39\x3f\x9e"
    },
    {
	"foo", "", -1,
	0,
	ETYPE_ARCFOUR_HMAC_MD5, 16,
	NULL,
	"\xac\x8e\x65\x7f\x83\xdf\x82\xbe\xea\x5d\x43\xbd\xaf\x78\x00\xcc"
    },
    {
	"test", "", -1,
	0,
	ETYPE_ARCFOUR_HMAC_MD5, 16,
	NULL,
	"\x0c\xb6\x94\x88\x05\xf7\x97\xbf\x2a\x82\x80\x79\x73\xb8\x95\x37"
    }
};

static int
string_to_key_test(krb5_context context)
{
    krb5_data password, opaque;
    krb5_error_code ret;
    krb5_salt salt;
    int i, val = 0;
    char iter[4];

    for (i = 0; i < sizeof(keys)/sizeof(keys[0]); i++) {

	password.data = keys[i].password;
	password.length = strlen(password.data);

	salt.salttype = KRB5_PW_SALT;
	salt.saltvalue.data = keys[i].salt;
	if (keys[i].saltlen == -1)
	    salt.saltvalue.length = strlen(salt.saltvalue.data);
	else
	    salt.saltvalue.length = keys[i].saltlen;

	opaque.data = iter;
	opaque.length = sizeof(iter);
	_krb5_put_int(iter, keys[i].iterations, 4);

	if (keys[i].pbkdf2) {
	    unsigned char keyout[32];

	    if (keys[i].keylen > sizeof(keyout))
		abort();

	    PKCS5_PBKDF2_HMAC(password.data, password.length,
			      salt.saltvalue.data, salt.saltvalue.length,
			      keys[i].iterations, EVP_sha1(),
			      keys[i].keylen, keyout);

	    if (memcmp(keyout, keys[i].pbkdf2, keys[i].keylen) != 0) {
		krb5_warnx(context, "%d: pbkdf2", i);
		val = 1;
		hex_dump_data(keyout, keys[i].keylen);
		continue;
	    }

	    if (verbose) {
		printf("PBKDF2:\n");
		hex_dump_data(keyout, keys[i].keylen);
	    }
	}

	{
	    krb5_keyblock key;

	    ret = krb5_string_to_key_data_salt_opaque (context,
						       keys[i].enctype,
						       password,
						       salt,
						       opaque,
						       &key);
	    if (ret) {
		krb5_warn(context, ret, "%d: string_to_key_data_salt_opaque",
			  i);
		val = 1;
		continue;
	    }

	    if (key.keyvalue.length != keys[i].keylen) {
		krb5_warnx(context, "%d: key wrong length (%lu/%lu)",
			   i, (unsigned long)key.keyvalue.length,
			   (unsigned long)keys[i].keylen);
		val = 1;
		continue;
	    }

	    if (memcmp(key.keyvalue.data, keys[i].key, keys[i].keylen) != 0) {
		krb5_warnx(context, "%d: key wrong", i);
		val = 1;
		hex_dump_data(key.keyvalue.data, key.keyvalue.length);
		hex_dump_data(keys[i].key, keys[i].keylen);
		continue;
	    }

	    if (verbose) {
		printf("key:\n");
		hex_dump_data(key.keyvalue.data, key.keyvalue.length);
	    }
	    krb5_free_keyblock_contents(context, &key);
	}
    }
    return val;
}

static int
krb_enc(krb5_context context,
	krb5_crypto crypto,
	unsigned usage,
	krb5_data *cipher,
	krb5_data *clear)
{
    krb5_data decrypt;
    krb5_error_code ret;

    krb5_data_zero(&decrypt);

    ret = krb5_decrypt(context,
		       crypto,
		       usage,
		       cipher->data,
		       cipher->length,
		       &decrypt);

    if (ret) {
	krb5_warn(context, ret, "krb5_decrypt");
	return ret;
    }

    if (decrypt.length != clear->length ||
        (decrypt.length &&
	 memcmp(decrypt.data, clear->data, decrypt.length) != 0)) {
	krb5_warnx(context, "clear text not same");
	return EINVAL;
    }

    krb5_data_free(&decrypt);

    return 0;
}

static int
krb_enc_iov2(krb5_context context,
	     krb5_crypto crypto,
	     unsigned usage,
	     size_t cipher_len,
	     krb5_data *clear)
{
    krb5_crypto_iov iov[4];
    krb5_data decrypt;
    int ret;
    char *p, *q;
    size_t len, i;

    p = clear->data;
    len = clear->length;

    iov[0].flags = KRB5_CRYPTO_TYPE_HEADER;
    krb5_crypto_length(context, crypto, iov[0].flags, &iov[0].data.length);
    iov[0].data.data = emalloc(iov[0].data.length);

    iov[1].flags = KRB5_CRYPTO_TYPE_DATA;
    iov[1].data.length = len;
    iov[1].data.data = emalloc(iov[1].data.length);
    memcpy(iov[1].data.data, p, iov[1].data.length);

    /* padding buffer */
    iov[2].flags = KRB5_CRYPTO_TYPE_PADDING;
    krb5_crypto_length(context, crypto, KRB5_CRYPTO_TYPE_PADDING, &iov[2].data.length);
    iov[2].data.data = emalloc(iov[2].data.length);

    iov[3].flags = KRB5_CRYPTO_TYPE_TRAILER;
    krb5_crypto_length(context, crypto, iov[3].flags, &iov[3].data.length);
    iov[3].data.data = emalloc(iov[3].data.length);

    ret = krb5_encrypt_iov_ivec(context, crypto, usage,
				iov, sizeof(iov)/sizeof(iov[0]), NULL);
    if (ret)
	errx(1, "encrypt iov failed: %d", ret);

    /* check len */
    for (i = 0, len = 0; i < sizeof(iov)/sizeof(iov[0]); i++)
	len += iov[i].data.length;
    if (len != cipher_len)
	errx(1, "cipher len wrong");

    /*
     * Plain decrypt
     */

    p = q = emalloc(len);
    for (i = 0; i < sizeof(iov)/sizeof(iov[0]); i++) {
	memcpy(q, iov[i].data.data, iov[i].data.length);
	q += iov[i].data.length;
    }

    ret = krb5_decrypt(context, crypto, usage, p, len, &decrypt);
    if (ret)
	krb5_err(context, 1, ret, "krb5_decrypt");
    else
	krb5_data_free(&decrypt);

    free(p);

    /*
     * Now decrypt use iov
     */

    /* padding turn into data */
    p = q = emalloc(iov[1].data.length + iov[2].data.length);

    memcpy(q, iov[1].data.data, iov[1].data.length);
    q += iov[1].data.length;
    memcpy(q, iov[2].data.data, iov[2].data.length);

    free(iov[1].data.data);
    free(iov[2].data.data);

    iov[1].data.data = p;
    iov[1].data.length += iov[2].data.length;

    iov[2].flags = KRB5_CRYPTO_TYPE_EMPTY;
    iov[2].data.length = 0;

    ret = krb5_decrypt_iov_ivec(context, crypto, usage,
				iov, sizeof(iov)/sizeof(iov[0]), NULL);
    free(iov[0].data.data);
    free(iov[3].data.data);

    if (ret)
	krb5_err(context, 1, ret, "decrypt iov failed: %d", ret);

    if (clear->length != iov[1].data.length)
	errx(1, "length incorrect");

    p = clear->data;
    if (memcmp(iov[1].data.data, p, iov[1].data.length) != 0)
	errx(1, "iov[1] incorrect");

    free(iov[1].data.data);

    return 0;
}


static int
krb_enc_iov(krb5_context context,
	    krb5_crypto crypto,
	    unsigned usage,
	    krb5_data *cipher,
	    krb5_data *clear)
{
    krb5_crypto_iov iov[3];
    int ret;
    char *p;
    size_t len;

    p = cipher->data;
    len = cipher->length;

    iov[0].flags = KRB5_CRYPTO_TYPE_HEADER;
    krb5_crypto_length(context, crypto, iov[0].flags, &iov[0].data.length);
    iov[0].data.data = emalloc(iov[0].data.length);
    memcpy(iov[0].data.data, p, iov[0].data.length);
    p += iov[0].data.length;
    len -= iov[0].data.length;

    iov[1].flags = KRB5_CRYPTO_TYPE_TRAILER;
    krb5_crypto_length(context, crypto, iov[1].flags, &iov[1].data.length);
    iov[1].data.data = emalloc(iov[1].data.length);
    memcpy(iov[1].data.data, p + len - iov[1].data.length, iov[1].data.length);
    len -= iov[1].data.length;

    iov[2].flags = KRB5_CRYPTO_TYPE_DATA;
    iov[2].data.length = len;
    iov[2].data.data = emalloc(len);
    memcpy(iov[2].data.data, p, len);

    ret = krb5_decrypt_iov_ivec(context, crypto, usage,
				iov, sizeof(iov)/sizeof(iov[0]), NULL);
    if (ret)
	krb5_err(context, 1, ret, "krb_enc_iov decrypt iov failed: %d", ret);

    if (clear->length != iov[2].data.length)
	errx(1, "length incorrect");

    p = clear->data;
    if (memcmp(iov[2].data.data, p, iov[2].data.length) != 0)
	errx(1, "iov[2] incorrect");

    free(iov[0].data.data);
    free(iov[1].data.data);
    free(iov[2].data.data);


    return 0;
}

static int
krb_checksum_iov(krb5_context context,
		 krb5_crypto crypto,
		 unsigned usage,
		 krb5_data *plain,
		 krb5_data *verify)
{
    krb5_crypto_iov iov[3];
    int ret;
    char *p;
    size_t len;

    p = plain->data;
    len = plain->length;

    iov[0].flags = KRB5_CRYPTO_TYPE_CHECKSUM;
    if (verify) {
	iov[0].data = *verify;
    } else {
	krb5_crypto_length(context, crypto, iov[0].flags, &iov[0].data.length);
	iov[0].data.data = emalloc(iov[0].data.length);
    }

    iov[1].flags = KRB5_CRYPTO_TYPE_DATA;
    iov[1].data.length = len;
    iov[1].data.data = p;

    iov[2].flags = KRB5_CRYPTO_TYPE_TRAILER;
    krb5_crypto_length(context, crypto, iov[0].flags, &iov[2].data.length);
    iov[2].data.data = malloc(iov[2].data.length);

    if (verify == NULL) {
	ret = krb5_create_checksum_iov(context, crypto, usage,
				       iov, sizeof(iov)/sizeof(iov[0]), NULL);
	if (ret)
	    krb5_err(context, 1, ret, "krb5_create_checksum_iov failed");
    }

    ret = krb5_verify_checksum_iov(context, crypto, usage, iov, sizeof(iov)/sizeof(iov[0]), NULL);
    if (ret)
	krb5_err(context, 1, ret, "krb5_verify_checksum_iov");

    if (verify == NULL)
	free(iov[0].data.data);
    free(iov[2].data.data);

    return 0;
}


static int
krb_enc_mit(krb5_context context,
	    krb5_enctype enctype,
	    krb5_keyblock *key,
	    unsigned usage,
	    krb5_data *cipher,
	    krb5_data *clear)
{
#ifndef HEIMDAL_SMALLER
    krb5_error_code ret;
    krb5_enc_data e;
    krb5_data decrypt;
    size_t len;

    e.kvno = 0;
    e.enctype = enctype;
    e.ciphertext = *cipher;

    ret = krb5_c_decrypt(context, *key, usage, NULL, &e, &decrypt);
    if (ret)
	return ret;

    if (decrypt.length != clear->length ||
        (decrypt.length &&
         memcmp(decrypt.data, clear->data, decrypt.length) != 0)) {
	krb5_warnx(context, "clear text not same");
	return EINVAL;
    }

    krb5_data_free(&decrypt);

    ret = krb5_c_encrypt_length(context, enctype, clear->length, &len);
    if (ret)
	return ret;

    if (len != cipher->length) {
	krb5_warnx(context, "c_encrypt_length wrong %lu != %lu",
		   (unsigned long)len, (unsigned long)cipher->length);
	return EINVAL;
    }
#endif /* HEIMDAL_SMALLER */
    return 0;
}

struct {
    krb5_enctype enctype;
    unsigned usage;
    size_t keylen;
    void *key;
    size_t elen;
    void* edata;
    size_t plen;
    void *pdata;
    size_t clen; /* checksum length */
    void *cdata; /* checksum data */
} krbencs[] =  {
    {
	ETYPE_AES256_CTS_HMAC_SHA1_96,
	7,
	32,
	"\x47\x75\x69\x64\x65\x6c\x69\x6e\x65\x73\x20\x74\x6f\x20\x41\x75"
	"\x74\x68\x6f\x72\x73\x20\x6f\x66\x20\x49\x6e\x74\x65\x72\x6e\x65",
	44,
	"\xcf\x79\x8f\x0d\x76\xf3\xe0\xbe\x8e\x66\x94\x70\xfa\xcc\x9e\x91"
	"\xa9\xec\x1c\x5c\x21\xfb\x6e\xef\x1a\x7a\xc8\xc1\xcc\x5a\x95\x24"
	"\x6f\x9f\xf4\xd5\xbe\x5d\x59\x97\x44\xd8\x47\xcd",
	16,
	"\x54\x68\x69\x73\x20\x69\x73\x20\x61\x20\x74\x65\x73\x74\x2e\x0a",
	0,
	NULL
    },
    {
	KRB5_ENCTYPE_AES128_CTS_HMAC_SHA256_128,
	2,
	16,
	"\x37\x05\xD9\x60\x80\xC1\x77\x28\xA0\xE8\x00\xEA\xB6\xE0\xD2\x3C",
	32,
	"\xEF\x85\xFB\x89\x0B\xB8\x47\x2F\x4D\xAB\x20\x39\x4D\xCA\x78\x1D"
	"\xAD\x87\x7E\xDA\x39\xD5\x0C\x87\x0C\x0D\x5A\x0A\x8E\x48\xC7\x18",
	0,
	"",
	0,
	NULL
    },
    {
	KRB5_ENCTYPE_AES128_CTS_HMAC_SHA256_128,
	2,
	16,
	"\x37\x05\xD9\x60\x80\xC1\x77\x28\xA0\xE8\x00\xEA\xB6\xE0\xD2\x3C",
	38,
	"\x84\xD7\xF3\x07\x54\xED\x98\x7B\xAB\x0B\xF3\x50\x6B\xEB\x09\xCF"
	"\xB5\x54\x02\xCE\xF7\xE6\x87\x7C\xE9\x9E\x24\x7E\x52\xD1\x6E\xD4"
	"\x42\x1D\xFD\xF8\x97\x6C",
	6,
	"\x00\x01\x02\x03\x04\x05",
	0,
	NULL
    },
    {
	KRB5_ENCTYPE_AES128_CTS_HMAC_SHA256_128,
	2,
	16,
	"\x37\x05\xD9\x60\x80\xC1\x77\x28\xA0\xE8\x00\xEA\xB6\xE0\xD2\x3C",
	48,
	"\x35\x17\xD6\x40\xF5\x0D\xDC\x8A\xD3\x62\x87\x22\xB3\x56\x9D\x2A"
	"\xE0\x74\x93\xFA\x82\x63\x25\x40\x80\xEA\x65\xC1\x00\x8E\x8F\xC2"
	"\x95\xFB\x48\x52\xE7\xD8\x3E\x1E\x7C\x48\xC3\x7E\xEB\xE6\xB0\xD3",
	16,
	"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F",
	0,
	NULL
    },
    {
	KRB5_ENCTYPE_AES128_CTS_HMAC_SHA256_128,
	2,
	16,
	"\x37\x05\xD9\x60\x80\xC1\x77\x28\xA0\xE8\x00\xEA\xB6\xE0\xD2\x3C",
	53,
	"\x72\x0F\x73\xB1\x8D\x98\x59\xCD\x6C\xCB\x43\x46\x11\x5C\xD3\x36"
	"\xC7\x0F\x58\xED\xC0\xC4\x43\x7C\x55\x73\x54\x4C\x31\xC8\x13\xBC"
	"\xE1\xE6\xD0\x72\xC1\x86\xB3\x9A\x41\x3C\x2F\x92\xCA\x9B\x83\x34"
	"\xA2\x87\xFF\xCB\xFC",
	21,
	"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
	"\x10\x11\x12\x13\x14",
	16,
	"\xD7\x83\x67\x18\x66\x43\xD6\x7B\x41\x1C\xBA\x91\x39\xFC\x1D\xEE"
    },
    {
	KRB5_ENCTYPE_AES256_CTS_HMAC_SHA384_192,
	2,
	32,
	"\x6D\x40\x4D\x37\xFA\xF7\x9F\x9D\xF0\xD3\x35\x68\xD3\x20\x66\x98"
	"\x00\xEB\x48\x36\x47\x2E\xA8\xA0\x26\xD1\x6B\x71\x82\x46\x0C\x52",
	40,
	"\x41\xF5\x3F\xA5\xBF\xE7\x02\x6D\x91\xFA\xF9\xBE\x95\x91\x95\xA0"
	"\x58\x70\x72\x73\xA9\x6A\x40\xF0\xA0\x19\x60\x62\x1A\xC6\x12\x74"
	"\x8B\x9B\xBF\xBE\x7E\xB4\xCE\x3C",
	0,
	"",
	0,
	NULL
    },
    {
	KRB5_ENCTYPE_AES256_CTS_HMAC_SHA384_192,
	2,
	32,
	"\x6D\x40\x4D\x37\xFA\xF7\x9F\x9D\xF0\xD3\x35\x68\xD3\x20\x66\x98"
	"\x00\xEB\x48\x36\x47\x2E\xA8\xA0\x26\xD1\x6B\x71\x82\x46\x0C\x52",
	46,
	"\x4E\xD7\xB3\x7C\x2B\xCA\xC8\xF7\x4F\x23\xC1\xCF\x07\xE6\x2B\xC7"
	"\xB7\x5F\xB3\xF6\x37\xB9\xF5\x59\xC7\xF6\x64\xF6\x9E\xAB\x7B\x60"
	"\x92\x23\x75\x26\xEA\x0D\x1F\x61\xCB\x20\xD6\x9D\x10\xF2",
	6,
	"\x00\x01\x02\x03\x04\x05",
	0,
	NULL
    },
    {
	KRB5_ENCTYPE_AES256_CTS_HMAC_SHA384_192,
	2,
	32,
	"\x6D\x40\x4D\x37\xFA\xF7\x9F\x9D\xF0\xD3\x35\x68\xD3\x20\x66\x98"
	"\x00\xEB\x48\x36\x47\x2E\xA8\xA0\x26\xD1\x6B\x71\x82\x46\x0C\x52",
	56,
	"\xBC\x47\xFF\xEC\x79\x98\xEB\x91\xE8\x11\x5C\xF8\xD1\x9D\xAC\x4B"
	"\xBB\xE2\xE1\x63\xE8\x7D\xD3\x7F\x49\xBE\xCA\x92\x02\x77\x64\xF6"
	"\x8C\xF5\x1F\x14\xD7\x98\xC2\x27\x3F\x35\xDF\x57\x4D\x1F\x93\x2E"
	"\x40\xC4\xFF\x25\x5B\x36\xA2\x66",
	16,
	"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F",
	0,
	NULL
    },
    {
	KRB5_ENCTYPE_AES256_CTS_HMAC_SHA384_192,
	2,
	32,
	"\x6D\x40\x4D\x37\xFA\xF7\x9F\x9D\xF0\xD3\x35\x68\xD3\x20\x66\x98"
	"\x00\xEB\x48\x36\x47\x2E\xA8\xA0\x26\xD1\x6B\x71\x82\x46\x0C\x52",
	61,
	"\x40\x01\x3E\x2D\xF5\x8E\x87\x51\x95\x7D\x28\x78\xBC\xD2\xD6\xFE"
	"\x10\x1C\xCF\xD5\x56\xCB\x1E\xAE\x79\xDB\x3C\x3E\xE8\x64\x29\xF2"
	"\xB2\xA6\x02\xAC\x86\xFE\xF6\xEC\xB6\x47\xD6\x29\x5F\xAE\x07\x7A"
	"\x1F\xEB\x51\x75\x08\xD2\xC1\x6B\x41\x92\xE0\x1F\x62",
	21,
	"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
	"\x10\x11\x12\x13\x14",
	24,
	"\x45\xEE\x79\x15\x67\xEE\xFC\xA3\x7F\x4A\xC1\xE0\x22\x2D\xE8\x0D"
	"\x43\xC3\xBF\xA0\x66\x99\x67\x2A"
    }
};

static int
krb_enc_test(krb5_context context)
{
    krb5_error_code ret;
    krb5_crypto crypto;
    krb5_keyblock kb;
    krb5_data cipher, plain;
    int i;

    for (i = 0; i < sizeof(krbencs)/sizeof(krbencs[0]); i++) {

	kb.keytype = krbencs[i].enctype;
	kb.keyvalue.length = krbencs[i].keylen;
	kb.keyvalue.data = krbencs[i].key;

	ret = krb5_crypto_init(context, &kb, krbencs[i].enctype, &crypto);

	cipher.length = krbencs[i].elen;
	cipher.data = krbencs[i].edata;
	plain.length = krbencs[i].plen;
	plain.data = krbencs[i].pdata;

	ret = krb_enc(context, crypto, krbencs[i].usage, &cipher, &plain);

	if (ret)
	    errx(1, "krb_enc failed with %d for test %d", ret, i);

	ret = krb_enc_iov(context, crypto, krbencs[i].usage, &cipher, &plain);
	if (ret)
	    errx(1, "krb_enc_iov failed with %d for test %d", ret, i);

	ret = krb_enc_iov2(context, crypto, krbencs[i].usage,
			   cipher.length, &plain);
	if (ret)
	    errx(1, "krb_enc_iov2 failed with %d for test %d", ret, i);

	ret = krb_checksum_iov(context, crypto, krbencs[i].usage, &plain, NULL);
	if (ret)
	    errx(1, "krb_checksum_iov failed with %d for test %d", ret, i);

	if (krbencs[i].cdata) {
	    krb5_data checksum;

	    checksum.length = krbencs[i].clen;
	    checksum.data = krbencs[i].cdata;

	    ret = krb_checksum_iov(context, crypto, krbencs[i].usage,
				   &plain, &checksum);
	    if (ret)
		errx(1, "krb_checksum_iov(2) failed with %d for test %d", ret, i);
	}

	krb5_crypto_destroy(context, crypto);

	ret = krb_enc_mit(context, krbencs[i].enctype, &kb,
			  krbencs[i].usage, &cipher, &plain);
	if (ret)
	    errx(1, "krb_enc_mit failed with %d for test %d", ret, i);
    }

    return 0;
}

static int
iov_test(krb5_context context, krb5_enctype enctype)
{
    krb5_error_code ret;
    krb5_crypto crypto;
    krb5_keyblock key;
    krb5_data signonly, in, in2;
    krb5_crypto_iov iov[6];
    size_t len, i;
    unsigned char *base, *p;

    ret = krb5_generate_random_keyblock(context, enctype, &key);
    if (ret)
	krb5_err(context, 1, ret, "krb5_generate_random_keyblock");

    ret = krb5_crypto_init(context, &key, 0, &crypto);
    if (ret)
	krb5_err(context, 1, ret, "krb5_crypto_init");


    ret = krb5_crypto_length(context, crypto, KRB5_CRYPTO_TYPE_HEADER, &len);
    if (ret)
	krb5_err(context, 1, ret, "krb5_crypto_length");

    signonly.data = "This should be signed";
    signonly.length = strlen(signonly.data);
    in.data = "inputdata";
    in.length = strlen(in.data);

    in2.data = "INPUTDATA";
    in2.length = strlen(in2.data);


    memset(iov, 0, sizeof(iov));

    iov[0].flags = KRB5_CRYPTO_TYPE_HEADER;
    iov[1].flags = KRB5_CRYPTO_TYPE_DATA;
    iov[1].data = in;
    iov[2].flags = KRB5_CRYPTO_TYPE_SIGN_ONLY;
    iov[2].data = signonly;
    iov[3].flags = KRB5_CRYPTO_TYPE_EMPTY;
    iov[4].flags = KRB5_CRYPTO_TYPE_PADDING;
    iov[5].flags = KRB5_CRYPTO_TYPE_TRAILER;

    ret = krb5_crypto_length_iov(context, crypto, iov,
				 sizeof(iov)/sizeof(iov[0]));
    if (ret)
	krb5_err(context, 1, ret, "krb5_crypto_length_iov");

    for (len = 0, i = 0; i < sizeof(iov)/sizeof(iov[0]); i++) {
	if (iov[i].flags == KRB5_CRYPTO_TYPE_SIGN_ONLY)
	    continue;
	len += iov[i].data.length;
    }

    base = emalloc(len);

    /*
     * Allocate data for the fields
     */

    for (p = base, i = 0; i < sizeof(iov)/sizeof(iov[0]); i++) {
	if (iov[i].flags == KRB5_CRYPTO_TYPE_SIGN_ONLY)
	    continue;;
	iov[i].data.data = p;
	p += iov[i].data.length;
    }
    assert(iov[1].data.length == in.length);
    memcpy(iov[1].data.data, in.data, iov[1].data.length);

    /*
     * Encrypt
     */

    ret = krb5_encrypt_iov_ivec(context, crypto, 7, iov,
				sizeof(iov)/sizeof(iov[0]), NULL);
    if (ret)
	krb5_err(context, 1, ret, "krb5_encrypt_iov_ivec");

    /*
     * Decrypt
     */

    ret = krb5_decrypt_iov_ivec(context, crypto, 7,
				iov, sizeof(iov)/sizeof(iov[0]), NULL);
    if (ret)
	krb5_err(context, 1, ret, "krb5_decrypt_iov_ivec");

    /*
     * Verify data
     */

    if (krb5_data_cmp(&iov[1].data, &in) != 0)
	krb5_errx(context, 1, "decrypted data not same");

    /*
     * Free memory
     */

    free(base);

    /* Set up for second try */

    iov[3].flags = KRB5_CRYPTO_TYPE_DATA;
    iov[3].data = in;

    ret = krb5_crypto_length_iov(context, crypto,
				 iov, sizeof(iov)/sizeof(iov[0]));
    if (ret)
	krb5_err(context, 1, ret, "krb5_crypto_length_iov");

    for (len = 0, i = 0; i < sizeof(iov)/sizeof(iov[0]); i++) {
	if (iov[i].flags == KRB5_CRYPTO_TYPE_SIGN_ONLY)
	    continue;
	len += iov[i].data.length;
    }

    base = emalloc(len);

    /*
     * Allocate data for the fields
     */

    for (p = base, i = 0; i < sizeof(iov)/sizeof(iov[0]); i++) {
	if (iov[i].flags == KRB5_CRYPTO_TYPE_SIGN_ONLY)
	    continue;;
	iov[i].data.data = p;
	p += iov[i].data.length;
    }
    assert(iov[1].data.length == in.length);
    memcpy(iov[1].data.data, in.data, iov[1].data.length);

    assert(iov[3].data.length == in2.length);
    memcpy(iov[3].data.data, in2.data, iov[3].data.length);



    /*
     * Encrypt
     */

    ret = krb5_encrypt_iov_ivec(context, crypto, 7,
				iov, sizeof(iov)/sizeof(iov[0]), NULL);
    if (ret)
	krb5_err(context, 1, ret, "krb5_encrypt_iov_ivec");

    /*
     * Decrypt
     */

    ret = krb5_decrypt_iov_ivec(context, crypto, 7,
				iov, sizeof(iov)/sizeof(iov[0]), NULL);
    if (ret)
	krb5_err(context, 1, ret, "krb5_decrypt_iov_ivec");

    /*
     * Verify data
     */

    if (krb5_data_cmp(&iov[1].data, &in) != 0)
	krb5_errx(context, 1, "decrypted data 2.1 not same");

    if (krb5_data_cmp(&iov[3].data, &in2) != 0)
	krb5_errx(context, 1, "decrypted data 2.2 not same");

    /*
     * Free memory
     */

    free(base);

    krb5_crypto_destroy(context, crypto);

    krb5_free_keyblock_contents(context, &key);

    return 0;
}



static int
random_to_key(krb5_context context)
{
    krb5_error_code ret;
    krb5_keyblock key;

    ret = krb5_random_to_key(context,
			     ETYPE_DES3_CBC_SHA1,
			     "\x21\x39\x04\x58\x6A\xBD\x7F"
			     "\x21\x39\x04\x58\x6A\xBD\x7F"
			     "\x21\x39\x04\x58\x6A\xBD\x7F",
			     21,
			     &key);
    if (ret){
	krb5_warn(context, ret, "random_to_key");
	return 1;
    }
    if (key.keyvalue.length != 24)
	return 1;

    if (memcmp(key.keyvalue.data,
	       "\x20\x38\x04\x58\x6b\xbc\x7f\xc7"
	       "\x20\x38\x04\x58\x6b\xbc\x7f\xc7"
	       "\x20\x38\x04\x58\x6b\xbc\x7f\xc7",
	       24) != 0)
	return 1;

    krb5_free_keyblock_contents(context, &key);

    return 0;
}

int
main(int argc, char **argv)
{
    krb5_error_code ret;
    krb5_context context;
    int val = 0;

    if (argc > 1 && strcmp(argv[1], "-v") == 0)
        verbose = 1;

    ret = krb5_init_context (&context);
    if (ret)
	errx (1, "krb5_init_context failed: %d", ret);

    val |= string_to_key_test(context);

    val |= krb_enc_test(context);
    val |= random_to_key(context);
    val |= iov_test(context, KRB5_ENCTYPE_AES256_CTS_HMAC_SHA1_96);
    val |= iov_test(context, KRB5_ENCTYPE_AES128_CTS_HMAC_SHA256_128);
    val |= iov_test(context, KRB5_ENCTYPE_AES256_CTS_HMAC_SHA384_192);

    if (verbose && val == 0)
	printf("all ok\n");
    if (val)
	printf("tests failed\n");

    krb5_free_context(context);

    return val;
}
