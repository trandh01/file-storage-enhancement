/* lifiballoc.c - lifiballoc */

#include <xinu.h>

/*------------------------------------------------------------------------
 * lifiballoc  -  Allocate a new index block from free list on disk
 *			(assumes directory mutex held)
 *------------------------------------------------------------------------
 */
ibid32	lifiballoc (void)
{
	ibid32	ibnum;		/* ID of next block on the free list	*/
	struct	lfiblk	iblock;	/* Buffer to hold an index block	*/

	/* Get ID of first index block on free list */

	ibnum = Lif_data.lif_dir.lifd_ifree;
	if (ibnum == LF_INULL) {	/* Ran out of free index blocks */
		panic("out of index blocks");
	}
	lifibget(Lif_data.lif_dskdev, ibnum, &iblock);

	/* Unlink index block from the directory free list */

	Lif_data.lif_dir.lifd_ifree = iblock.ib_next;

	/* Write a copy of the directory to disk after the change */

	write(Lif_data.lif_dskdev, (char *) &Lif_data.lif_dir, LF_AREA_DIR);
	Lif_data.lif_dirdirty = FALSE;

	return ibnum;
}
