#	$NetBSD: encode_decode.inc,v 1.1.2.2 2023/08/11 13:41:55 martin Exp $

.PATH:	${OPENSSLSRC}/crypto/encode_decode \
	${OPENSSLSRC}/providers/implementations/encode_decode

ENCODE_DECODE_SRCS += \
decoder_err.c \
decoder_lib.c \
decoder_meth.c \
decoder_pkey.c \
encoder_err.c \
encoder_lib.c \
encoder_meth.c \
encoder_pkey.c \

ENCODE_DECODE_SRCS += \
decode_der2key.c

SRCS += ${ENCODE_DECODE_SRCS}

.for cryptosrc in ${ENCODE_DECODE_SRCS}
CPPFLAGS.${cryptosrc} += -I${OPENSSLSRC}/crypto/encode_decode ${ENCODE_DECODECPPFLAGS}
.endfor
