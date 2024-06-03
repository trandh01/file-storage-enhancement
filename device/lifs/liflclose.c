/* liflclose.c - liflclose.c */

#include <xinu.h>

/*------------------------------------------------------------------------
 * liflclose  -  Close a file by flushing output and freeing device entry
 *------------------------------------------------------------------------
 */
devcall	liflclose (
	  struct dentry *devptr		/* Entry in device switch table */
	)
{
	struct	liflcblk	*lifptr;		/* Ptr to open file table entry	*/

	/* Obtain exclusive use of the file */

	lifptr = &lifltab[devptr->dvminor];
	wait(lifptr->lifmutex);

	/* If file is not open, return an error */

	if (lifptr->lifstate != LF_USED) {
		signal(lifptr->lifmutex);
		return SYSERR;
	}

	/* Write index or data blocks to disk if they have changed */

	if (Lif_data.lif_dirdirty || lifptr->lifdbdirty || lifptr->lifibdirty || 
			lifptr->lifindbdirty || lifptr->lif2indbdirty || lifptr->lif3indbdirty ) {
		lifflush(lifptr);
	}

	/* Set device state to FREE and return to caller */

	lifptr->lifstate = LF_FREE;
	signal(lifptr->lifmutex);
	return OK;
}
