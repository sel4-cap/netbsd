# $NetBSD: Makefile,v 1.9.6.1 2023/08/11 13:42:56 martin Exp $

USE_FORT?= yes	# network client/server

.include <bsd.own.mk>

PROG=	sntp
MAN=	sntp.1
SRCS= \
	crypto.c \
	kod_management.c \
	log.c \
	main.c \
	networking.c \
	sntp-opts.c \
	sntp.c \
	utilities.c

BINDIR=	/usr/sbin

.include "${.CURDIR}/../Makefile.inc"

DIST=${IDIST}/sntp

CPPFLAGS+=-I${DIST} -I${DIST}/libopts

.PATH: ${DIST}

.include <bsd.own.mk>

CPPFLAGS+= -DLOCKNAME=\"/var/run/sntp.pid\"
CPPFLAGS+= -DSAVENAME=\"/var/db/sntp.state\"
LDADD+= -L${LIBISC} -liscntp -levent -lm
DPADD+=	${LIBM} ${LIBEVENT}

COPTS.utilities.c+=	${GCC_NO_FORMAT_TRUNCATION}
COPTS.crypto.c+=	-Wno-error=deprecated-declarations

CLEANFILES+=${MAN}

.include <bsd.prog.mk>
