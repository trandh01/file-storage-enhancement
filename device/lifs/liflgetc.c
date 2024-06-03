/* liflgetc.c - lifgetc */

#include <xinu.h>

/*------------------------------------------------------------------------
 * liflgetc  -  Read the next byte from an open local file
 *------------------------------------------------------------------------
 */
devcall	liflgetc (
	  struct dentry *devptr		/* Entry in device switch table */
	)
{
	struct	liflcblk	*lifptr;		/* Ptr to open file table entry	*/
	struct	ldentry	*ldptr;		/* Ptr to file's entry in the	*/
					/*   in-memory directory	*/
	int32	onebyte;		/* Next data byte in the file	*/

	/* Obtain exclusive use of the file */

	lifptr = &lifltab[devptr->dvminor];
	wait(lifptr->lifmutex);

	/* If file is not open, return an error */

	if (lifptr->lifstate != LF_USED) {
		signal(lifptr->lifmutex);
		return SYSERR;
	}

	/* Return EOF for any attempt to read beyond the end-of-file */

	ldptr = lifptr->lifdirptr;
	if (lifptr->lifpos >= ldptr->ld_size) {
		signal(lifptr->lifmutex);
		return EOF;
	}

	/* If byte pointer is beyond the current data block, set up	*/
	/*	a new data block					*/

	if (lifptr->lifbyte >= &lifptr->lifdblock[LF_BLKSIZ]) {
		lifsetup(lifptr);
	}


	/* Extract the next byte from block, update file position, and	*/
	/*	return the byte to the caller				*/

	onebyte = 0xff & *lifptr->lifbyte++;
	lifptr->lifpos++;
	signal(lifptr->lifmutex);
	return onebyte;
}
