# $NetBSD: Makefile.inc,v 1.12.6.1 2023/02/14 15:47:01 martin Exp $

USE_LIBEDIT=no
USE_TUI=yes
BINDIR=/usr/bin
WARNS?=		0
CPPFLAGS+=	-D_KERNTYPES

.include "Makefile.gdb_arch"

CWARNFLAGS.clang+=	-Wno-unused-value -Wno-conversion \
			-Wno-switch-enum -Wno-parentheses -Wno-comment \
			-Wno-format-security \
			-Wno-tautological-compare -Wno-format-extra-args \
			-Wno-string-plus-int

COPTS+=			-fcommon

DIST:=		${.PARSEDIR}/dist

# Reduce debugging for these extremely large objects.
.if ${MKDEBUG:Uno} != "no"
CXXFLAGS+=      -g1
.endif
