/* liflcontrol.c - liflcontrol */

#include <xinu.h>

/*------------------------------------------------------------------------
 * liflcontrol  -  Provide control functions for a local file pseudo-device
 *------------------------------------------------------------------------
 */
devcall	liflcontrol (
	 struct dentry	*devptr,	/* Entry in device switch table	*/
	 int32	func,			/* A control function		*/
	 int32	arg1,			/* Argument #1			*/
	 int32	arg2			/* Argument #2			*/
	)
{
	struct	liflcblk	*lifptr;		/* Ptr to open file table entry	*/
	int32	retval;			/* Return value from func. call	*/

	/* Obtain exclusive use of the file */

	lifptr = &lifltab[devptr->dvminor];
	wait(lifptr->lifmutex);

	/* If file is not open, return an error */

	if (lifptr->lifstate != LF_USED) {
		signal(lifptr->lifmutex);
		return SYSERR;
	}

	switch (func) {

	/* Truncate a file */

	case LF_CTL_TRUNC:
		wait(Lif_data.lif_mutex);
		retval = liftruncate(lifptr);
		signal(Lif_data.lif_mutex);
		signal(lifptr->lifmutex);
		return retval;	

	default:
		kprintf("lifcontrol: function %d not valid\n\r", func);
		signal(lifptr->lifmutex);
		return SYSERR;
	}
}
