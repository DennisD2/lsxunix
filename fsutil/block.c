/*
 * Block handling for unix v6 filesystem.
 *
 */
#include <stdio.h>
#include "lsxfs.h"

extern int verbose;

/*
 * Add a block to free list.
 */
int lsxfs_block_free (lsxfs_t *fs, unsigned int bno)
{
	int i;
	unsigned short buf [256];

/*	printf ("free block %d, total %d\n", bno, fs->nfree);*/
	if (fs->nfree >= 100) {
		buf[0] = lsx_short (fs->nfree);
		for (i=0; i<100; i++)
			buf[i+1] = lsx_short (fs->free[i]);
		if (! lsxfs_write_block (fs, bno, (char*) buf)) {
			fprintf (stderr, "block_free: write error at block %d\n", bno);
			return 0;
		}
		fs->nfree = 0;
	}
	fs->free [fs->nfree] = bno;
	fs->nfree++;
	fs->dirty = 1;
	return 1;
}

/*
 * Free an indirect block.
 */
int lsxfs_indirect_block_free (lsxfs_t *fs, unsigned int bno)
{
	unsigned short nb;
	unsigned char data [LSXFS_BSIZE];
	int i;

	if (! lsxfs_read_block (fs, bno, data)) {
		fprintf (stderr, "inode_clear: read error at block %d\n", bno);
		return 0;
	}
	for (i=LSXFS_BSIZE-2; i>=0; i-=2) {
		nb = data [i+1] << 8 | data [i];
		if (nb)
			lsxfs_block_free (fs, nb);
	}
	lsxfs_block_free (fs, bno);
	return 1;
}

/*
 * Free a double indirect block.
 */
int lsxfs_double_indirect_block_free (lsxfs_t *fs, unsigned int bno)
{
	unsigned short nb;
	unsigned char data [LSXFS_BSIZE];
	int i;

	if (! lsxfs_read_block (fs, bno, data)) {
		fprintf (stderr, "inode_clear: read error at block %d\n", bno);
		return 0;
	}
	for (i=LSXFS_BSIZE-2; i>=0; i-=2) {
		nb = data [i+1] << 8 | data [i];
		if (nb)
			lsxfs_indirect_block_free (fs, nb);
	}
	lsxfs_block_free (fs, bno);
	return 1;
}

/*
 * Get a block from free list.
 */
int lsxfs_block_alloc (lsxfs_t *fs, unsigned int *bno)
{
	int i;
	unsigned short buf [256];
again:
	if (fs->nfree == 0)
		return 0;
	fs->nfree--;
	*bno = fs->free [fs->nfree];
	if (verbose > 3) {
        printf("allocate new block %d from slot %d\n", *bno, fs->nfree);
    }
	fs->free [fs->nfree] = 0;
	fs->dirty = 1;
	if (fs->nfree <= 0) {
		if (! lsxfs_read_block (fs, *bno, (char*) buf))
			return 0;
		fs->nfree = lsx_short (buf[0]);
		for (i=0; i<100; i++)
			fs->free[i] = lsx_short (buf[i+1]);
	}
	if (*bno == 0)
		goto again;
	return 1;
}
