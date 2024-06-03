/* lifdballoc.c - lifdballoc */

#include <xinu.h>

#define  DFILL  '+'		/* character used to fill a disk block	*/

/*------------------------------------------------------------------------
 * lifdballoc  -  Allocate a new data block from free list on disk
 *			(assumes directory mutex held)
 *------------------------------------------------------------------------
 */
dbid32	lifdballoc (
	  struct lifdbfree *dbuff /* Addr. of buffer to hold data block	*/
	)
{
	dbid32	dnum;		/* ID of next d-block on the free list	*/
	int32	retval;		/* Return value				*/

	/* Get the ID of first data block on the free list */

	dnum = Lif_data.lif_dir.lifd_dfree;
	if (dnum == LF_DNULL) {	/* Ran out of free data blocks */
		panic("out of data blocks");
	}
	retval = read(Lif_data.lif_dskdev, (char *)dbuff, dnum);
	if (retval == SYSERR) {
		panic("lifdballoc cannot read disk block\n\r");
	}

	/* Unlink d-block from in-memory directory */

	Lif_data.lif_dir.lifd_dfree = dbuff->lif_nextdb;
	write(Lif_data.lif_dskdev, (char *)&Lif_data.lif_dir, LF_AREA_DIR);
	Lif_data.lif_dirdirty = FALSE;

	/* Fill data block to erase old data */

	memset((char *)dbuff, DFILL, LF_BLKSIZ);
	return dnum;
}

