/*	$NetBSD: linux_file64.c,v 1.68 2023/07/29 15:04:29 christos Exp $	*/

/*-
 * Copyright (c) 1995, 1998, 2000, 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Frank van der Linden and Eric Haszlakiewicz.
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

/*
 * Linux 64bit filesystem calls. Used on 32bit archs, not used on 64bit ones.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: linux_file64.c,v 1.68 2023/07/29 15:04:29 christos Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/namei.h>
#include <sys/proc.h>
#include <sys/dirent.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/filedesc.h>
#include <sys/ioctl.h>
#include <sys/kernel.h>
#include <sys/mount.h>
#include <sys/malloc.h>
#include <sys/namei.h>
#include <sys/vfs_syscalls.h>
#include <sys/vnode.h>
#include <sys/tty.h>
#include <sys/conf.h>

#include <sys/syscallargs.h>

#include <compat/linux/common/linux_types.h>
#include <compat/linux/common/linux_signal.h>
#include <compat/linux/common/linux_fcntl.h>
#include <compat/linux/common/linux_util.h>
#include <compat/linux/common/linux_machdep.h>
#include <compat/linux/common/linux_dirent.h>
#include <compat/linux/common/linux_ipc.h>
#include <compat/linux/common/linux_sem.h>

#include <compat/linux/linux_syscall.h>
#include <compat/linux/linux_syscallargs.h>

static void bsd_to_linux_stat64(struct stat *, struct linux_stat64 *);

/*
 * Convert a NetBSD stat structure to a Linux stat structure.
 * Only the order of the fields and the padding in the structure
 * is different. linux_fakedev is a machine-dependent function
 * which optionally converts device driver major/minor numbers
 * (XXX horrible, but what can you do against code that compares
 * things against constant major device numbers? sigh)
 */
static void
bsd_to_linux_stat64(struct stat *bsp, struct linux_stat64 *lsp)
{
	memset(lsp, 0, sizeof(*lsp));
	lsp->lst_dev     = linux_fakedev(bsp->st_dev, 0);
	lsp->lst_ino     = bsp->st_ino;
	lsp->lst_mode    = (linux_mode_t)bsp->st_mode;
	if (bsp->st_nlink >= (1 << 15))
		lsp->lst_nlink = (1 << 15) - 1;
	else
		lsp->lst_nlink = (linux_nlink_t)bsp->st_nlink;
	lsp->lst_uid     = bsp->st_uid;
	lsp->lst_gid     = bsp->st_gid;
	lsp->lst_rdev    = linux_fakedev(bsp->st_rdev, 1);
	lsp->lst_size    = bsp->st_size;
	lsp->lst_blksize = bsp->st_blksize;
	lsp->lst_blocks  = bsp->st_blocks;
	lsp->lst_atime   = bsp->st_atime;
	lsp->lst_mtime   = bsp->st_mtime;
	lsp->lst_ctime   = bsp->st_ctime;
#  ifdef LINUX_STAT64_HAS_NSEC
	lsp->lst_atime_nsec   = bsp->st_atimensec;
	lsp->lst_mtime_nsec   = bsp->st_mtimensec;
	lsp->lst_ctime_nsec   = bsp->st_ctimensec;
#  endif
#  if LINUX_STAT64_HAS_BROKEN_ST_INO
	lsp->__lst_ino   = (linux_ino_t) bsp->st_ino;
#  endif
}

int
bsd_to_linux_statx(struct stat *st, struct linux_statx *stx,
    unsigned int mask)
{
	if (mask & STATX__RESERVED)
		return EINVAL;

	/* XXX: STATX_MNT_ID is not supported */
	unsigned int rmask = STATX_TYPE | STATX_MODE | STATX_NLINK |
	    STATX_UID | STATX_GID | STATX_ATIME | STATX_MTIME | STATX_CTIME |
	    STATX_INO | STATX_SIZE | STATX_BLOCKS | STATX_BTIME;

	memset(stx, 0, sizeof(*stx));

	if ((st->st_flags & UF_NODUMP) != 0)
		stx->stx_attributes |= STATX_ATTR_NODUMP;
	if ((st->st_flags & (UF_IMMUTABLE|SF_IMMUTABLE)) != 0)
		stx->stx_attributes |= STATX_ATTR_IMMUTABLE;
	if ((st->st_flags & (UF_APPEND|SF_APPEND)) != 0)
		stx->stx_attributes |= STATX_ATTR_APPEND;

	stx->stx_attributes_mask =
	    STATX_ATTR_NODUMP | STATX_ATTR_IMMUTABLE | STATX_ATTR_APPEND;

	stx->stx_blksize = st->st_blksize;

	stx->stx_nlink = st->st_nlink;
	stx->stx_uid = st->st_uid;
	stx->stx_gid = st->st_gid;
	stx->stx_mode |= st->st_mode & S_IFMT;
	stx->stx_mode |= st->st_mode & ~S_IFMT;
	stx->stx_ino = st->st_ino;
	stx->stx_size = st->st_size;
	stx->stx_blocks = st->st_blocks;

	stx->stx_atime.tv_sec = st->st_atime;
	stx->stx_atime.tv_nsec = st->st_atimensec;

	/* some filesystem has no birthtime returns 0 or -1 */
	if ((st->st_birthtime == 0 && st->st_birthtimensec == 0) ||
	    (st->st_birthtime == (time_t)-1 &&
	    st->st_birthtimensec == (long)-1)) {
		rmask &= ~STATX_BTIME;
	} else {
		stx->stx_btime.tv_sec = st->st_birthtime;
		stx->stx_btime.tv_nsec = st->st_birthtimensec;
	}

	stx->stx_ctime.tv_sec = st->st_ctime;
	stx->stx_ctime.tv_nsec = st->st_ctimensec;

	stx->stx_mtime.tv_sec = st->st_mtime;
	stx->stx_mtime.tv_nsec = st->st_mtimensec;

	if (S_ISCHR(st->st_mode) || S_ISBLK(st->st_mode)) {
		stx->stx_rdev_major = major(st->st_rdev);
		stx->stx_rdev_minor = minor(st->st_rdev);
	} else {
		stx->stx_dev_major = major(st->st_rdev);
		stx->stx_dev_minor = minor(st->st_rdev);
	}

	stx->stx_mask = rmask;

	return 0;
}

/*
 * The stat functions below are plain sailing. stat and lstat are handled
 * by one function to avoid code duplication.
 */
int
linux_sys_fstat64(struct lwp *l, const struct linux_sys_fstat64_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) fd;
		syscallarg(struct linux_stat64 *) sp;
	} */
	struct linux_stat64 tmplst;
	struct stat tmpst;
	int error;

	error = do_sys_fstat(SCARG(uap, fd), &tmpst);
	if (error != 0)
		return error;

	bsd_to_linux_stat64(&tmpst, &tmplst);

	return copyout(&tmplst, SCARG(uap, sp), sizeof tmplst);
}

#if !defined(__aarch64__)
static int
linux_do_stat64(struct lwp *l, const struct linux_sys_stat64_args *uap, register_t *retval, int flags)
{
	struct linux_stat64 tmplst;
	struct stat tmpst;
	int error;

	error = do_sys_stat(SCARG(uap, path), flags, &tmpst);
	if (error != 0)
		return error;

	bsd_to_linux_stat64(&tmpst, &tmplst);

	return copyout(&tmplst, SCARG(uap, sp), sizeof tmplst);
}

int
linux_sys_stat64(struct lwp *l, const struct linux_sys_stat64_args *uap, register_t *retval)
{
	/* {
		syscallarg(const char *) path;
		syscallarg(struct linux_stat64 *) sp;
	} */

	return linux_do_stat64(l, uap, retval, FOLLOW);
}

int
linux_sys_lstat64(struct lwp *l, const struct linux_sys_lstat64_args *uap, register_t *retval)
{
	/* {
		syscallarg(const char *) path;
		syscallarg(struct linux_stat64 *) sp;
	} */

	return linux_do_stat64(l, (const void *)uap, retval, NOFOLLOW);
}
#endif

/*
 * This is an internal function for the *statat() variant of linux,
 * which returns struct stat, but flags and other handling are
 * the same as in linux.
 */
int
linux_statat(struct lwp *l, int fd, const char *path, int lflag,
    struct stat *st)
{
	struct vnode *vp;
	int error, nd_flag;
	uint8_t c;

	if (lflag & ~(LINUX_AT_EMPTY_PATH|LINUX_AT_NO_AUTOMOUNT
            |LINUX_AT_SYMLINK_NOFOLLOW))
		return EINVAL;

	if (lflag & LINUX_AT_EMPTY_PATH) {
		/*
		 * If path is null string:
		 */
		error = ufetch_8(path, &c);
		if (error != 0)
			return error;
		if (c == '\0') {
			if (fd == LINUX_AT_FDCWD) {
				/*
				 * operate on current directory
				 */
				vp = l->l_proc->p_cwdi->cwdi_cdir;
				vn_lock(vp, LK_EXCLUSIVE | LK_RETRY);
				error = vn_stat(vp, st);
				VOP_UNLOCK(vp);
			} else {
				/*
				 * operate on fd
				 */
				error = do_sys_fstat(fd, st);
			}
			return error;
		}
	}

	if (lflag & LINUX_AT_SYMLINK_NOFOLLOW)
		nd_flag = NOFOLLOW;
	else
		nd_flag = FOLLOW;

	return do_sys_statat(l, fd, path, nd_flag, st);
}

int
linux_sys_fstatat64(struct lwp *l, const struct linux_sys_fstatat64_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) fd;
		syscallarg(const char *) path;
		syscallarg(struct linux_stat64 *) sp;
		syscallarg(int) flag;
	} */
	struct linux_stat64 tmplst;
	struct stat tmpst;
	int error;

	error = linux_statat(l, SCARG(uap, fd), SCARG(uap, path),
	    SCARG(uap, flag), &tmpst);
	if (error != 0)
		return error;

	bsd_to_linux_stat64(&tmpst, &tmplst);

	return copyout(&tmplst, SCARG(uap, sp), sizeof tmplst);
}

#ifdef LINUX_SYS_statx
int
linux_sys_statx(struct lwp *l, const struct linux_sys_statx_args *uap,
    register_t *retval)
{
	/* {
		syscallarg(int) fd;
		syscallarg(const char *) path;
		syscallarg(int) flag;
		syscallarg(unsigned int) mask;
		syscallarg(struct linux_statx *) sp;
	} */
	struct linux_statx stx;
	struct stat st;
	int error;

	error = linux_statat(l, SCARG(uap, fd), SCARG(uap, path),
	    SCARG(uap, flag), &st);
	if (error != 0)
		return error;

	error = bsd_to_linux_statx(&st, &stx, SCARG(uap, mask));
	if (error != 0)
		return error;

	return copyout(&stx, SCARG(uap, sp), sizeof stx);
}
#endif /* LINUX_SYS_statx */

#ifndef __alpha__
int
linux_sys_truncate64(struct lwp *l, const struct linux_sys_truncate64_args *uap, register_t *retval)
{
	/* {
		syscallarg(const char *) path;
		syscallarg(off_t) length;
	} */
	struct sys_truncate_args ta;

	/* Linux doesn't have the 'pad' pseudo-parameter */
	SCARG(&ta, path) = SCARG(uap, path);
	SCARG(&ta, PAD) = 0;
	SCARG(&ta, length) = SCARG(uap, length);

	return sys_truncate(l, &ta, retval);
}

int
linux_sys_ftruncate64(struct lwp *l, const struct linux_sys_ftruncate64_args *uap, register_t *retval)
{
	/* {
		syscallarg(unsigned int) fd;
		syscallarg(off_t) length;
	} */
	struct sys_ftruncate_args ta;

	/* Linux doesn't have the 'pad' pseudo-parameter */
	SCARG(&ta, fd) = SCARG(uap, fd);
	SCARG(&ta, PAD) = 0;
	SCARG(&ta, length) = SCARG(uap, length);

	return sys_ftruncate(l, &ta, retval);
}
#endif /* __alpha__ */

/*
 * Linux 'readdir' call. This code is mostly taken from the
 * SunOS getdents call (see compat/sunos/sunos_misc.c), though
 * an attempt has been made to keep it a little cleaner.
 *
 * The d_off field contains the offset of the next valid entry,
 * unless the older Linux getdents(2), which used to have it set
 * to the offset of the entry itself. This function also doesn't
 * need to deal with the old count == 1 glibc problem.
 *
 * Read in BSD-style entries, convert them, and copy them out.
 *
 * Note that this doesn't handle union-mounted filesystems.
 */
int
linux_sys_getdents64(struct lwp *l, const struct linux_sys_getdents64_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) fd;
		syscallarg(struct linux_dirent64 *) dent;
		syscallarg(unsigned int) count;
	} */
	struct dirent *bdp;
	struct vnode *vp;
	char *inp, *tbuf;		/* BSD-format */
	int len, reclen;		/* BSD-format */
	char *outp;			/* Linux-format */
	int resid, linux_reclen = 0;	/* Linux-format */
	file_t *fp;
	struct uio auio;
	struct iovec aiov;
	struct linux_dirent64 idb;
	off_t off;		/* true file offset */
	int buflen, error, eofflag, nbytes;
	struct vattr va;
	off_t *cookiebuf = NULL, *cookie;
	int ncookies;

	/* fd_getvnode() will use the descriptor for us */
	if ((error = fd_getvnode(SCARG(uap, fd), &fp)) != 0)
		return (error);

	if ((fp->f_flag & FREAD) == 0) {
		error = EBADF;
		goto out1;
	}

	vp = (struct vnode *)fp->f_data;
	if (vp->v_type != VDIR) {
		error = ENOTDIR;
		goto out1;
	}

	vn_lock(vp, LK_SHARED | LK_RETRY);
	error = VOP_GETATTR(vp, &va, l->l_cred);
	VOP_UNLOCK(vp);
	if (error)
		goto out1;

	nbytes = SCARG(uap, count);
	buflen = uimin(MAXBSIZE, nbytes);
	if (buflen < va.va_blocksize)
		buflen = va.va_blocksize;
	tbuf = malloc(buflen, M_TEMP, M_WAITOK);

	vn_lock(vp, LK_EXCLUSIVE | LK_RETRY);
	off = fp->f_offset;
again:
	aiov.iov_base = tbuf;
	aiov.iov_len = buflen;
	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_rw = UIO_READ;
	auio.uio_resid = buflen;
	auio.uio_offset = off;
	UIO_SETUP_SYSSPACE(&auio);
	/*
         * First we read into the malloc'ed buffer, then
         * we massage it into user space, one record at a time.
         */
	error = VOP_READDIR(vp, &auio, fp->f_cred, &eofflag, &cookiebuf,
	    &ncookies);
	if (error)
		goto out;

	inp = tbuf;
	outp = (void *)SCARG(uap, dent);
	resid = nbytes;
	if ((len = buflen - auio.uio_resid) == 0)
		goto eof;

	for (cookie = cookiebuf; len > 0; len -= reclen) {
		bdp = (struct dirent *)inp;
		reclen = bdp->d_reclen;
		if (reclen & 3) {
			error = EIO;
			goto out;
		}
		if (bdp->d_fileno == 0) {
			inp += reclen;	/* it is a hole; squish it out */
			if (cookie)
				off = *cookie++;
			else
				off += reclen;
			continue;
		}
		linux_reclen = LINUX_RECLEN(&idb, bdp->d_namlen);
		if (reclen > len || resid < linux_reclen) {
			/* entry too big for buffer, so just stop */
			outp++;
			break;
		}
		if (cookie)
			off = *cookie++;	/* each entry points to next */
		else
			off += reclen;
		/*
		 * Massage in place to make a Linux-shaped dirent (otherwise
		 * we have to worry about touching user memory outside of
		 * the copyout() call).
		 */
		memset(&idb, 0, sizeof(idb));
		idb.d_ino = bdp->d_fileno;
		idb.d_type = bdp->d_type;
		idb.d_off = off;
		idb.d_reclen = (u_short)linux_reclen;
		memcpy(idb.d_name, bdp->d_name, MIN(sizeof(idb.d_name),
		   bdp->d_namlen + 1));
		if ((error = copyout((void *)&idb, outp, linux_reclen)))
			goto out;
		/* advance past this real entry */
		inp += reclen;
		/* advance output past Linux-shaped entry */
		outp += linux_reclen;
		resid -= linux_reclen;
	}

	/* if we squished out the whole block, try again */
	if (outp == (void *)SCARG(uap, dent)) {
		if (cookiebuf)
			free(cookiebuf, M_TEMP);
		cookiebuf = NULL;
		goto again;
	}
	fp->f_offset = off;	/* update the vnode offset */

eof:
	*retval = nbytes - resid;
out:
	VOP_UNLOCK(vp);
	if (cookiebuf)
		free(cookiebuf, M_TEMP);
	free(tbuf, M_TEMP);
out1:
	fd_putfile(SCARG(uap, fd));
	return error;
}
