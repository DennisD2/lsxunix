/* Copyright (C) 1991, 1992, 1996, 1998 Free Software Foundation, Inc.
   This file is derived from mkstemp.c from the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "stdlib.h"
#include "string.h"

/* Generate a unique temporary file name from TEMPLATE.

   TEMPLATE has the form:

   <path>/ccXXXXXX<suffix>

   SUFFIX_LEN tells us how long <suffix> is (it can be zero length).

   The last six characters of TEMPLATE before <suffix> must be "XXXXXX";
   they are replaced with a string that makes the filename unique.

   Returns a file descriptor open on the file for reading and writing.  */
int
mkstemp (template)
	char *template;
{
	static int value;
	char *XXXXXX;
	int len, count, now[2];

	len = strlen (template);
	if (len < 6 || strncmp (&template[len - 6], "XXXXXX", 6)) {
		return -1;
	}
	XXXXXX = &template[len - 6];
	time (now);
	value += getpid () + now[0] + now[1];

	for (count = 0; count < 100; ++count) {
		register int v = value & 077777;
		int fd;

		/* Fill in the random bits.  */
		XXXXXX[0] = (v & 31) + 'A';
		v >>= 5;
		XXXXXX[1] = (v & 31) + 'A';
		v >>= 5;
		XXXXXX[2] = (v & 31) + 'A';
		fd = creat (template, 0600);
		if (fd >= 0) {
			/* The file does not exist.  */
			return fd;
		}

		value += 7777;
	}

	/* We return the null string if we can't find a unique file name.  */
	template[0] = '\0';
	return -1;
}
