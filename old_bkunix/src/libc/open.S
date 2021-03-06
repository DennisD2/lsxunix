/*
 * C library --
 *	file = open(string, mode)
 *
 * file == -1 means error
 *
 * This file is part of BKUNIX project, which is distributed
 * under the terms of the GNU General Public License (GPL).
 * See the accompanying file "COPYING" for more details.
 */
#include "syscall.h"

	.globl	_open, cerror
_open:
	mov	r5,-(sp)
	mov	sp,r5
	mov	4(r5),2f
	mov	6(r5),3f
	sys	0
	.word	9f
	bcc	1f
	jmp	cerror
1:
	mov	(sp)+,r5
	rts	pc

	.data
9:	sys	SYS_open
2:	.word	0
3:	.word	0
