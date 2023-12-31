/*	$NetBSD: t_swab.c,v 1.2.52.1 2023/02/22 18:46:59 martin Exp $ */

/*-
 * Copyright (c) 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code was contributed to The NetBSD Foundation by Christos Zoulas.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <atf-c.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <err.h>

#define MAXCHK	100

static void
build(char *a, char *b, size_t n)
{
	size_t i;

	for (i = 0; i < n; i += 2) {
		b[i + 1] = a[i] = (char)i;
		b[i] = a[i + 1] = (char)(i + 1);
	}
	if (n & 1)
		a[n - 1] = b[n - 1] = (char)~1;
	for (; n < MAXCHK; n++)
		a[n] = b[n] = (char)~0;
}

static void
dump(const char *f, char *b, size_t l)
{

	printf("%s ", f);
	while (l--)
		printf("%.2x ", (unsigned char)*b++);
	printf("\n");
}

ATF_TC(swab_basic);
ATF_TC_HEAD(swab_basic, tc)
{

	atf_tc_set_md_var(tc, "descr", "Test swab results");
}

ATF_TC_BODY(swab_basic, tc)
{
	char a[MAXCHK], b[MAXCHK], r[MAXCHK];
	size_t i;
	bool failed = false;

	for (i = 0; i < MAXCHK; i++) {
		build(a, b, i);
		(void)memset(r, ~0, MAXCHK);
		swab(a, r, i);
		if (i & 1)	/* last byte unspecified if odd length */
			r[i - 1] = (char )~1;
		if (memcmp(b, r, MAXCHK) != 0) {
			printf("pattern mismatch at %zu bytes\n", i);
			dump("expect:", b, MAXCHK);
			dump("result:", r, MAXCHK);
			failed = true;
		}
	}
	if (failed)
		atf_tc_fail_nonfatal("Check stdout for details");
}

ATF_TP_ADD_TCS(tp)
{
	ATF_TP_ADD_TC(tp, swab_basic);

	return atf_no_error();
}
