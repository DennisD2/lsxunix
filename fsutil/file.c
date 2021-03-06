/*
 * File i/o routines for LSX filesystem.
 *
 */
#include <stdio.h>
#include <string.h>
#include "lsxfs.h"

extern int verbose;

int lsxfs_file_create (lsxfs_t *fs, lsxfs_file_t *file, char *name, int mode)
{
	if (! lsxfs_inode_by_name (fs, &file->inode, name, 1, mode)) {
		fprintf (stderr, "%s: inode open failed\n", name);
		return 0;
	}
	if ((file->inode.mode & INODE_MODE_FMT) == INODE_MODE_FDIR) {
		/* Cannot open directory on write. */
		return 0;
	}
	lsxfs_inode_truncate (&file->inode);
	lsxfs_inode_save (&file->inode /*, 0*/);
	file->writable = 1;
	file->offset = 0;
	return 1;
}

int lsxfs_file_write (lsxfs_file_t *file, unsigned char *data, unsigned long bytes)
{
	if (! file->writable)
		return 0;
    if (verbose > 3) {
        printf("write %ld bytes to %s\n", bytes, "file");
    }

    if (! lsxfs_inode_write (&file->inode, file->offset, data, bytes)) {
		fprintf (stderr, "inode %d: file write failed, %ld bytes @ offset=%ld\n",
			file->inode.number, bytes, file->offset);
		return 0;
	}
	file->offset += bytes;
	return 1;
}

int lsxfs_file_close (lsxfs_file_t *file)
{
	if (file->writable) {
		if (! lsxfs_inode_save (&file->inode /*, 0*/)) {
			fprintf (stderr, "inode %d: file close failed\n",
				file->inode.number);
			return 0;
		}
	}
	return 1;
}