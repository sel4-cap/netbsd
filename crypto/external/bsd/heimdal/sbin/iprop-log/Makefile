# $NetBSD: Makefile,v 1.2.56.1 2023/08/11 13:40:09 martin Exp $

.include <bsd.own.mk>
.include <${.CURDIR}/../../Makefile.inc>

.PATH: ${HEIMDIST}/lib/kadm5

PROG= iprop-log
MAN=  iprop-log.8

HEIMSRCS= iprop-log.c iprop-commands.in

CPPFLAGS+= -I${DESTDIR}/usr/include/kadm5 -I${HEIMDIST}/lib/krb5

PROGDPLIBS += ${DPLIBKADM5SRV} ${DPLIBHDB} ${DPLIBSL}

.include <${HEIMBASE}/Makefile.rules.inc>
.include <bsd.prog.mk>

LDADD+= -ledit -lterminfo ${KRB5LDADD}
DPADD+= ${LIBEDIT} ${LIBTERMINFO} ${KRB5DPADD}
