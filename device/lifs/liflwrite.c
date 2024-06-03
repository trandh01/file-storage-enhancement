/* liflwrite.c - liflwrite */

#include <xinu.h>

/*------------------------------------------------------------------------
 * liflwrite  --  Write data to a previously opened local disk file
 *------------------------------------------------------------------------
 */
devcall	liflwrite (
	  struct dentry *devptr,	/* Entry in device switch table */
	  char	*buff,			/* Buffer holding data to write	*/
	  int32	count			/* Number of bytes to write	*/
	)
{
	int32		i;		/* Number of bytes written	*/

	if (count < 0) {
		return SYSERR;
	}

	/* Iteratate and write one byte at a time */

	for (i=0; i<count; i++) {
		if (liflputc(devptr, *buff++) == SYSERR) {
			return SYSERR;
		}
	}
	return count;
}
