/* lifflush.c - lifflush */

#include <xinu.h>

/*------------------------------------------------------------------------
 * lifflush  -  Flush directory, data block, and index block for an open
 *			file (assumes file mutex is held)
 *------------------------------------------------------------------------
 */
status	lifflush (
	  struct liflcblk  *lifptr	/* Ptr to file pseudo device	*/
	)
{

	if (lifptr->lifstate == LF_FREE) {
		return SYSERR;
	}

	/* Write the directory if it has changed */

	if (Lif_data.lif_dirdirty) {
		write(Lif_data.lif_dskdev, (char *)&Lif_data.lif_dir,
							LF_AREA_DIR);
		Lif_data.lif_dirdirty = FALSE;
	}

	/* Write data block if it has changed */

	if (lifptr->lifdbdirty) {
		write(Lif_data.lif_dskdev, lifptr->lifdblock, lifptr->lifdnum);
		lifptr->lifdbdirty = FALSE;
	}

	/* Write i-block if it has changed */

	if (lifptr->lifibdirty) {
		lifibput(Lif_data.lif_dskdev, lifptr->lifinum, &lifptr->lifiblock);
		lifptr->lifibdirty = FALSE;
	}

	/* Write indirect block */
	if (lifptr->lifindbdirty) {
		write(Lif_data.lif_dskdev, (char *)lifptr->lifindblock, lifptr->lifindnum);
		lifptr->lifindbdirty = FALSE;
	}

	/* Write doubly indirect block */
	if (lifptr->lif2indbdirty) {
		write(Lif_data.lif_dskdev, (char *)lifptr->lif2indblock, lifptr->lif2indnum);
		lifptr->lif2indbdirty = FALSE;
	}

	/* Write triply indirect block */
	if (lifptr->lif3indbdirty) {
		write(Lif_data.lif_dskdev, (char *)lifptr->lif3indblock, lifptr->lif3indnum);
		lifptr->lif3indbdirty = FALSE;
	}

	return OK;
}
