#	$NetBSD: Makefile,v 1.6.14.1 2023/08/11 13:41:59 martin Exp $

.include "bsd.own.mk"

CRYPTODIST=${NETBSDSRCDIR}/crypto
.include "${NETBSDSRCDIR}/crypto/Makefile.openssl"

regen:
	for i in $$(find ${OPENSSLSRC} -name \*sparcv9\*.pl) \
		 ${OPENSSLSRC}/crypto/bn/asm/sparct4-mont.pl \
		 ${OPENSSLSRC}/crypto/bn/asm/vis3-mont.pl; do \
		j=$$(basename $$i .pl).S; \
		case $$j in \
		sparc*_modes.pl|sha1-*) perl $$i $$j;; \
		sha512-*) perl $$i $$j; perl $$i $${j:S/512/256/};; \
		*) perl $$i > $$j;; \
		esac; \
	done
	#cp ${OPENSSLSRC}/crypto/bn/asm/sparcv8plus.S bn-sparcv9.S
	m4 ${OPENSSLSRC}/crypto/des/asm/des_enc.m4 | \
		sed 's,OPENSSL_SYSNAME_ULTRASPARC,__sparc_v9__,g' | \
		sed 's,\.PIC\.DES_SPtrans,_PIC_DES_SPtrans,g' > des_enc-sparc.S
foo:
