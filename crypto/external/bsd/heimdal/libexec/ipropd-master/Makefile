# $NetBSD: Makefile,v 1.2.56.1 2023/08/11 13:40:08 martin Exp $

.include <bsd.own.mk>
.include <${.CURDIR}/../../Makefile.inc>

.PATH:		${HEIMDIST}/lib/kadm5

PROG=		ipropd-master

MAN=		iprop.8
MLINKS=	\
	iprop.8		ipropd-master.8		\
	iprop.8		ipropd-slave.8

SRCS+= 	ipropd_master.c				\
	ipropd_common.c

CPPFLAGS+= 				\
	-I${HEIMDIST}/lib/krb5		\
	-I${HEIMDIST}/lib/asn1		\
	-I${DESTDIR}/usr/include/kadm5

PROGDPLIBS+= ${DPLIBKADM5SRV} ${DPLIBHDB}

.include <${HEIMBASE}/Makefile.rules.inc>
.include <bsd.prog.mk>

LDADD+= -lutil ${KRB5LDADD}
DPADD+= ${LIBUTIL} ${KRB5DPADD}
