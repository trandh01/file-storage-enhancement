/* liftruncate.c - liftruncate */

#include <xinu.h>

/*------------------------------------------------------------------------
 * liftruncate  -  Truncate a file by freeing its index and data blocks
 *			(assumes directory mutex held)
 *------------------------------------------------------------------------
 */
status	liftruncate (
	  struct liflcblk *lifptr		/* Ptr to file's cntl blk entry	*/
	)
{
	kprintf("unimplemented\n");
	return SYSERR;
}
