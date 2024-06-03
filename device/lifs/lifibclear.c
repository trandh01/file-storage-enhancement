/* lifibclear.c - lifibclear */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  lifibclear  --  Clear an in-core copy of an index block
 *------------------------------------------------------------------------
 */
void	lifibclear(
	  struct lifiblk	*ibptr,		/* Address of i-block in memory	*/
	  int32		offset		/* File offset for this i-block	*/
	)
{
	int32	i;			/* Index for data block array 	*/

	for (i=0 ; i<LIF_DIBLEN ; i++) {	/* Clear each data block pointer*/
		ibptr->ib_dba[i] = LF_DNULL;
	}
	ibptr->ind = LF_DNULL;
	ibptr->ind2 = LF_DNULL;
	ibptr->ind3 = LF_DNULL;
	return;
}
