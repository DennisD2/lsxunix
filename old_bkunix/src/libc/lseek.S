/*
 * C library --
 *	error = lseek(file, loffset, ptr);
 *
 * This file is part of BKUNIX project, which is distributed
 * under the terms of the GNU General Public License (GPL).
 * See the accompanying file "COPYING" for more details.
 */
#include "syscall.h"

	.globl	_lseek, cerror
_lseek:
	mov	r5,-(sp)
	mov	sp,r5
	mov	4(r5),r0
	mov	6(r5),2f
	mov	8(r5),3f
	mov	10(r5),4f
	sys	0
	.word	9f
	bcc	1f
	jmp	cerror
1:
	clr	r0
	mov	(sp)+,r5
	rts	pc

	.data
9:	sys	SYS_seek
2:	.word	0
3:	.word	0
4:	.word	0
