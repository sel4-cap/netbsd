#	$NetBSD: modes.inc,v 1.6.14.1 2023/08/11 13:41:55 martin Exp $

.PATH:	${OPENSSLSRC}/crypto/modes

MODES_SRCS += \
cbc128.c \
ccm128.c \
cfb128.c \
ctr128.c \
cts128.c \
gcm128.c \
ocb128.c \
ofb128.c \
siv128.c \
wrap128.c \
xts128.c \

SRCS += ${MODES_SRCS}

.for cryptosrc in ${MODES_SRCS}
CPPFLAGS.${cryptosrc} += -I${OPENSSLSRC}/crypto/modes ${MODESCPPFLAGS}
.endfor
