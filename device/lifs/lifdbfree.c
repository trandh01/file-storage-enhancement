/* lifdbfree.c - lifdbfree */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  lifdbfree  -  Free a data block given its block number (assumes
 *			directory mutex is held)
 *------------------------------------------------------------------------
 */
status	lifdbfree(
	  did32		diskdev,	/* ID of disk device to use	*/
	  dbid32	dnum		/* ID of data block to free	*/
	)
{
	struct	lifdir	*dirptr;	/* Pointer to directory		*/
	struct	lifdbfree buf;		/* Buffer to hold data block	*/

	dirptr = &Lif_data.lif_dir;
	buf.lif_nextdb = dirptr->lifd_dfree;
	dirptr->lifd_dfree = dnum;
	write(diskdev, (char *)&buf,   dnum);
	write(diskdev, (char *)dirptr, LF_AREA_DIR);

	return OK;
}
