/*	$NetBSD: msg_001_c90.c,v 1.2 2022/10/01 09:42:40 rillig Exp $	*/
# 3 "msg_001_c90.c"

/*
 * Test for message: old-style declaration; add 'int' [1]
 *
 * In strict C90 mode, an old-style declaration is an error, not merely a
 * warning.
 */

/* lint1-flags: -s */

/* expect+1: error: old-style declaration; add 'int' [1] */
implicit_global_variable;
