/* liflinit.c - liflinit */

#include <xinu.h>

struct	liflcblk	lifltab[Nlifl];		/* Pseudo-device control blocks	*/

/*------------------------------------------------------------------------
 * liflinit  -  Initialize control blocks for local file pseudo-devices
 *------------------------------------------------------------------------
 */
devcall	liflinit (
	  struct dentry	*devptr		/* Entry in device switch table	*/
	)
{
	struct	liflcblk	*lifptr;		/* Ptr. to control block entry	*/
	int32	i;			/* Walks through name array	*/

	lifptr = &lifltab[ devptr->dvminor ];

	/* Initialize control block entry */

	lifptr->lifstate = LF_FREE;	/* Device is currently unused	*/
	lifptr->lifdev = devptr->dvnum;	/* Set device ID		*/
	lifptr->lifmutex = semcreate(1);	/* Create the mutex semaphore	*/

	/* Initialize the directory and file position */

	lifptr->lifdirptr = (struct  ldentry *) NULL;
	lifptr->lifpos = 0;
	for (i=0; i<LF_NAME_LEN; i++) {
		lifptr->lifname[i] = NULLCH;
	}

	/* Zero the in-memory index block and data block */

	lifptr->lifinum = LF_INULL;
	memset((char *) &lifptr->lifiblock, NULLCH, sizeof(struct lifiblk));
	lifptr->lifdnum = 0;
	memset((char *) &lifptr->lifdblock, NULLCH, LF_BLKSIZ);

	/* Start with the byte beyond the current data block */

	lifptr->lifbyte = &lifptr->lifdblock[LF_BLKSIZ];
	lifptr->lifibdirty = lifptr->lifdbdirty = FALSE;
	lifptr->lifindbdirty = lifptr->lif2indbdirty = lifptr->lif3indbdirty = FALSE;
	return OK;
}
