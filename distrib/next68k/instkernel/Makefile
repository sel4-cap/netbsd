<<<<<<< HEAD
#	$NetBSD: Makefile,v 1.1.2.2 2023/02/22 12:59:17 martin Exp $
=======
#	$NetBSD: Makefile,v 1.1 2023/02/11 19:12:31 tsutsui Exp $
>>>>>>> trunk

.include <bsd.own.mk>
.include "${NETBSDSRCDIR}/distrib/common/Makefile.distrib"

RAMDISKDIR!=	cd ${.CURDIR}/../ramdisk && ${PRINTOBJDIR}
RAMDISK=	${RAMDISKDIR}/ramdisk.fs

MDSETTARGETS=		RAMDISK ${RAMDISK} -
MDSET_RELEASEDIR=	binary/kernel

.include "${DISTRIBDIR}/common/Makefile.mdset"

.include <bsd.prog.mk>
