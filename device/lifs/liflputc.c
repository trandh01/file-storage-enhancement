/* liflputc.c - lifputc */

#include <xinu.h>

/*------------------------------------------------------------------------
 * liflputc  -  Write a single byte to an open local file
 *------------------------------------------------------------------------
 */
devcall	liflputc (
	  struct dentry *devptr,	/* Entry in device switch table */
	  char		ch		/* Character (byte) to write	*/
	)
{
	struct	liflcblk	*lifptr;		/* Ptr to open file table entry	*/
	struct	ldentry	*ldptr;		/* Ptr to file's entry in the	*/
					/*  in-memory directory		*/

	/* Obtain exclusive use of the file */

	lifptr = &lifltab[devptr->dvminor];
	wait(lifptr->lifmutex);

	/* If file is not open, return an error */

	if (lifptr->lifstate != LF_USED) {
		signal(lifptr->lifmutex);
		return SYSERR;
	}

	/* Return SYSERR for an attempt to skip bytes beyond the byte	*/
	/* 	that is currently the end of the file		 	*/

	ldptr = lifptr->lifdirptr;
	if (lifptr->lifpos > ldptr->ld_size) {
		signal(lifptr->lifmutex);
		return SYSERR;
	}

	/* If pointer is outside current block, set up new block */

	if (lifptr->lifbyte >= &lifptr->lifdblock[LF_BLKSIZ]) {

		/* Set up block for current file position */

		lifsetup(lifptr);
	}

	/* If appending a byte to the file, increment the file size.	*/
	/*   Note: comparison might be equal, but should not be greater.*/

	if (lifptr->lifpos >= ldptr->ld_size) {
		ldptr->ld_size++;
		Lif_data.lif_dirdirty = TRUE;
	}

	/* Place byte in buffer and mark buffer "dirty" */

	*lifptr->lifbyte++ = ch;
	lifptr->lifpos++;
	lifptr->lifdbdirty = TRUE;

	signal(lifptr->lifmutex);
	return OK;
}
