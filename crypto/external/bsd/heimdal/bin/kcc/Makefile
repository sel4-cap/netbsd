# $NetBSD: Makefile,v 1.3.22.1 2023/08/11 13:39:45 martin Exp $

.include <bsd.own.mk>
.include <${.CURDIR}/../../Makefile.inc>

USE_FORT?= yes	# cryptographic software

.PATH: ${HEIMDIST}/kuser

PROG= kcc
LINKS+= ${BINDIR}/kcc ${BINDIR}/klist

MAN= klist.1

HEIMSRCS= heimtools-commands.in

SRCS=	copy_cred_cache.c	\
	heimtools.c		\
	klist.c			\
	kswitch.c

CPPFLAGS+= -I${DESTDIR}/usr/include/krb5
CPPFLAGS+= -I${HEIMDIST}/kuser

PROGDPLIBS+= ${DPLIBKAFS} ${DPLIBSL}

.include <${HEIMBASE}/Makefile.rules.inc>
.include <bsd.prog.mk>

LDADD+= -ledit -lterminfo ${KRB5LDADD}
DPADD+= ${LIBEDIT} ${LIBTERMINFO} ${KRB5DPADD}
