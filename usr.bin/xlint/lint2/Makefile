#	$NetBSD: Makefile,v 1.22 2021/08/22 15:06:49 rillig Exp $

NOMAN=		# defined

PROG=		lint2
SRCS=		main2.c hash.c read.c mem.c mem2.c chk.c msg.c emit.c emit2.c \
		inittyp.c tyname.c
BINDIR=		/usr/libexec
CPPFLAGS+=	-I${.CURDIR}
CPPFLAGS+=	-DIS_LINT2
LINTFLAGS+=	-T		# strict bool mode

COPTS.msg.c+=	${${ACTIVE_CC} == "clang":? -Wno-format-nonliteral :}

.include <bsd.prog.mk>
