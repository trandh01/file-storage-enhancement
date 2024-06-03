/* liflseek.c - liflseek */

#include <xinu.h>

/*------------------------------------------------------------------------
 * liflseek  -  Seek to a specified position in a file
 *------------------------------------------------------------------------
 */
devcall	liflseek (
	  struct dentry *devptr,	/* Entry in device switch table */
	  uint32	offset		/* Byte position in the file	*/
	)
{
	struct	liflcblk	*lifptr;		/* Ptr to open file table entry	*/

	/* If file is not open, return an error */

	lifptr = &lifltab[devptr->dvminor];
	wait(lifptr->lifmutex);
	if (lifptr->lifstate != LF_USED) {
		signal(lifptr->lifmutex);
		return SYSERR;
	}

	/* Verify offset is within current file size */

	if (offset > lifptr->lifdirptr->ld_size) {
		signal(lifptr->lifmutex);
		return SYSERR;
	}

	/* Record new offset and invalidate byte pointer (i.e., force	*/
	/*   the index and data blocks to be replaced if a successive	*/
	/*   call is made to read or write)				*/

	lifptr->lifpos = offset;
	lifptr->lifbyte = &lifptr->lifdblock[LF_BLKSIZ];

	signal(lifptr->lifmutex);
	return OK;
}
