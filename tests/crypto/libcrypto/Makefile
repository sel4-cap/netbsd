# $NetBSD: Makefile,v 1.14.10.1 2023/08/11 13:43:42 martin Exp $

.include <bsd.own.mk>

SUBDIR+=bf bn cast conf des dh dsa ec engine evp hmac \
	rc2 rc4 rsa srp threads

.if ${HAVE_OPENSSL} <= 11
SUBDIR+= ecdh ecdsa md2
.endif

SUBDIR+=idea mdc2
SUBDIR+=rc5

.if ${HAVE_OPENSSL} == 10
SUBDIR += lhash sha x509v3
.endif

TESTSDIR=	${TESTSBASE}/crypto/libcrypto

.if ${HAVE_OPENSSL} == 10
TESTS_SH=	t_certs
.endif
TESTS_SH+=	t_ciphers
TESTS_SH+=	t_hashes
TESTS_SH+=	t_libcrypto
TESTS_SH+=	t_pubkey

.include <bsd.test.mk>
