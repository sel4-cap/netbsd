# $NetBSD: Makefile,v 1.2.56.1 2023/08/11 13:40:09 martin Exp $

.include <bsd.own.mk>
.include <${.CURDIR}/../../Makefile.inc>

USE_FORT?= yes	# network server

.PATH: ${HEIMDIST}/kdc

PROG= kdc

# XXXrcd: ??? should I include this?
#	announce.c		\

SRCS= connect.c config.c main.c

CPPFLAGS+= 				\
	-I${HEIMDIST}/lib/krb5		\
	-I${HEIMBASE}/include/krb5	\
	-I${HEIMDIST}/lib

PROGDPLIBS+=	${DPLIBKDC} ${DPLIBHDB} ${DPLIBHEIMNTLM}

MAN=	kdc.8

.include <${HEIMBASE}/Makefile.rules.inc>
.include <bsd.prog.mk>

LDADD+=	-lutil ${KRB5LDADD}
DPADD+=	${LIBUTIL} ${KRB5DPADD}
