#	$NetBSD: Makefile,v 1.7.6.1 2023/08/11 13:42:56 martin Exp $

.include <bsd.own.mk>

PROG=	ntp-keygen
MAN=	ntp-keygen.8
BINDIR=	/usr/sbin

SRCS=	ntp-keygen-opts.c	\
	ntp-keygen.c

LDADD+=		-L${LIBNTP} -lntp -L${LIBISC} -liscntp -lm
DPADD+=		${LIBISC}/libiscntp.a ${LIBOPTS}/libopts.a  ${LIBNTP}/libntp.a ${LIBM} ${LIBUTIL}

.include "${.CURDIR}/../Makefile.inc"

DIST=${IDIST}/util

CPPFLAGS+=-I${IDIST}/include -I${IDIST}/sntp/libopts

.PATH: ${DIST}

COPTS.ntp-keygen.c+=	${${ACTIVE_CC} == "gcc" && ${HAVE_GCC:U0} == 8:? -Wno-error=restrict :}
COPTS.ntp-keygen.c+=	-Wno-error=deprecated-declarations

CLEANFILES+=${MAN}

.include <bsd.prog.mk>
