/* lifscreate.c - lifscreate */

#include <xinu.h>
#include <ramdisk.h>

/*------------------------------------------------------------------------
 * lifscreate  -  Create an initially-empty file system on a disk
 *------------------------------------------------------------------------
 */
status	lifscreate (
	  did32		disk,		/* ID of an open disk device	*/
	  ibid32	lifiblks,	/* Num. of index blocks on disk	*/
	  uint32	dsiz		/* Total size of disk in bytes	*/
	)
{
	uint32	sectors;		/* Number of sectors to use	*/
	uint32	ibsectors;		/* Number of sectors of i-blocks*/
	uint32	ibpersector;		/* Number of i-blocks per sector*/
	struct	lifdir	dir;		/* Buffer to hold the directory	*/
	uint32	dblks;			/* Total free data blocks	*/
	struct	lfiblk	iblock;		/* Space for one i-block	*/
	struct	lifdbfree dblock;	/* Data block on the free list	*/
	dbid32	dbindex;		/* Index for data blocks	*/
	int32	retval;			/* Return value from func call	*/
	int32	i;			/* Loop index			*/

	/* Compute total sectors on disk */

	sectors = dsiz	/ LF_BLKSIZ;	/* Truncate to full sector */

	/* Compute number of sectors comprising i-blocks */

	ibpersector = LF_BLKSIZ / sizeof(struct lifiblk);
	ibsectors = (lifiblks+(ibpersector-1)) / ibpersector;/* Round up	*/
	lifiblks = ibsectors * ibpersector;
	if (ibsectors > sectors/2) {	/* Invalid arguments */
		return SYSERR;
	}

	/* Create an initial directory */

	memset((char *)&dir, NULLCH, sizeof(struct lifdir));

	dir.lifd_allones = 0xffffffff;
	dir.lifd_fsysid = LFS_ID;
	dir.lifd_revid = ((LFS_ID>>24) & 0x000000ff) | 
		  ((LFS_ID>> 8) & 0x0000ff00) |
		  ((LFS_ID<< 8) & 0x00ff0000) |
		  ((LFS_ID<<24) & 0xff000000) ;

	dir.lifd_nfiles = 0;
	dbindex= (dbid32)(ibsectors + 1);
	dir.lifd_dfree = dbindex;
	dblks = sectors - ibsectors - 1;
	retval = write(disk,(char *)&dir, LF_AREA_DIR);
	if (retval == SYSERR) {
		return SYSERR;
	}

	/* Create list of free i-blocks on disk */

	lifibclear((struct lifiblk *)&iblock, 0);
	for (i=0; i<lifiblks-1; i++) {
		iblock.ib_next = (ibid32)(i + 1);
		lifibput(disk, i, (struct lifiblk *)&iblock);
	}
	iblock.ib_next = LF_INULL;
	lifibput(disk, i, (struct lifiblk *)&iblock);

	/* Create list of free data blocks on disk */

	memset((char*)&dblock, NULLCH, LF_BLKSIZ);
	for (i=0; i<dblks-1; i++) {
		dblock.lif_nextdb = dbindex + 1;
		write(disk, (char *)&dblock, dbindex);
		dbindex++;
	}
	dblock.lif_nextdb = LF_DNULL;
	write(disk, (char *)&dblock, dbindex);
	close(disk);
	return OK;
}
