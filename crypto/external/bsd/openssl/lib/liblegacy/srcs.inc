#	$NetBSD: srcs.inc,v 1.1.2.2 2023/08/11 13:42:15 martin Exp $

LEGACYINCS= \
crypto.inc \
crypto_des.inc \
crypto_md5.inc \
providers.inc \
providers_common.inc \
providers_implementations_ciphers.inc \
providers_implementations_digests.inc \
providers_implementations_kdfs.inc \

.for legacyinc in ${LEGACYINCS}
.if exists(${.CURDIR}/arch/${LEGACY_MACHINE_CPU}/${legacyinc})
.include "${.CURDIR}/arch/${LEGACY_MACHINE_CPU}/${legacyinc}"
.else
.include "${legacyinc}"
.endif
.endfor
