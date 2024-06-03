/* lifsopen.c - lifsopen */

#include <xinu.h>

/*------------------------------------------------------------------------
 * lifsopen - Open a file and allocate a local file pseudo-device
 *------------------------------------------------------------------------
 */
devcall	lifsopen (
	 struct	dentry	*devptr,	/* Entry in device switch table	*/
	 char	*name,			/* Name of file to open		*/
	 char	*mode			/* Mode chars: 'r' 'w' 'o' 'n'	*/
	)
{
	struct	lifdir	*dirptr;	/* Ptr to in-memory directory	*/
	char		*from, *to;	/* Ptrs used during copy	*/
	char		*nam, *cmp;	/* Ptrs used during comparison	*/
	int32		i;		/* General loop index		*/
	did32		lifnext;		/* Minor number of an unused	*/
					/*    file pseudo-device	*/
	struct	ldentry	*ldptr;		/* Ptr to an entry in directory	*/
	struct	liflcblk	*lifptr;		/* Ptr to open file table entry	*/
	bool8		found;		/* Was the name found?		*/
	int32	retval;			/* Value returned from function	*/
	int32	mbits;			/* Mode bits			*/

	/* Check length of name file (leaving space for NULLCH */

	from = name;
	for (i=0; i< LF_NAME_LEN; i++) {
		if (*from++ == NULLCH) {
			break;
		}
	}
	if (i >= LF_NAME_LEN) {		/* Name is too long */
		return SYSERR;
	}

	/* Parse mode argument and convert to binary */

	mbits = lfgetmode(mode);
	if (mbits == SYSERR) {
		return SYSERR;
	}

	/* If named file is already open, return SYSERR */

	lifnext = SYSERR;
	for (i=0; i<Nlifl; i++) {	/* Search file pseudo-devices	*/
		lifptr = &lifltab[i];
		if (lifptr->lifstate == LF_FREE) {
			if (lifnext == SYSERR) {
				lifnext = i; /* Record index */
			}
			continue;
		}

		/* Compare requested name to name of open file */

		nam = name;
		cmp = lifptr->lifname;
		while(*nam != NULLCH) {
			if (*nam != *cmp) {
				break;
			}
			nam++;
			cmp++;
		}

		/* See if comparison succeeded */

		if ( (*nam==NULLCH) && (*cmp == NULLCH) ) {
			return SYSERR;
		}
	}
	if (lifnext == SYSERR) {	/* No slave file devices are available	*/
		return SYSERR;
	}

	/* Obtain copy of directory if not already present in memory	*/

	dirptr = &Lif_data.lif_dir;
	wait(Lif_data.lif_mutex);
	if (! Lif_data.lif_dirpresent) {
	    retval = read(Lif_data.lif_dskdev,(char *)dirptr,LF_AREA_DIR);
	    if (retval == SYSERR ) {
		signal(Lif_data.lif_mutex);
		return SYSERR;
	    }
	    if (lifscheck(dirptr) == SYSERR ) {
		kprintf("Disk does not contain a Xinu file system\n");
		signal(Lif_data.lif_mutex);
		return SYSERR;
	    }
	    Lif_data.lif_dirpresent = TRUE;
	}

	/* Search directory to see if file exists */

	found = FALSE;
	for (i=0; i<dirptr->lifd_nfiles; i++) {
		ldptr = &dirptr->lifd_files[i];
		nam = name;
		cmp = ldptr->ld_name;
		while(*nam != NULLCH) {
			if (*nam != *cmp) {
				break;
			}
			nam++;
			cmp++;
		}
		if ( (*nam==NULLCH) && (*cmp==NULLCH) ) { /* Name found	*/
			found = TRUE;
			break;
		}
	}

	/* Case #1 - file is not in directory (i.e., does not exist)	*/

	if (! found) {
		if (mbits & LF_MODE_O) {	/* File *must* exist	*/
			signal(Lif_data.lif_mutex);
			return SYSERR;
		}

		/* Take steps to create new file and add to directory	*/

		/* Verify that space remains in the directory */

		if (dirptr->lifd_nfiles >= LF_NUM_DIR_ENT) {
			signal(Lif_data.lif_mutex);
			return SYSERR;
		}

		/* Allocate next dir. entry & initialize to empty file	*/

		ldptr = &dirptr->lifd_files[dirptr->lifd_nfiles++];
		ldptr->ld_size = 0;
		from = name;
		to = ldptr->ld_name;
		while ( (*to++ = *from++) != NULLCH ) {
			;
		}
		ldptr->ld_ilist = LF_INULL;

	/* Case #2 - file is in directory (i.e., already exists)	*/

	} else if (mbits & LF_MODE_N) {		/* File must not exist	*/
			signal(Lif_data.lif_mutex);
			return SYSERR;
	}

	/* Initialize the local file pseudo-device */

	lifptr = &lifltab[lifnext];
	lifptr->lifstate = LF_USED;
	lifptr->lifdirptr = ldptr;	/* Point to directory entry	*/
	lifptr->lifmode = mbits & LF_MODE_RW;

	/* File starts at position 0 */

	lifptr->lifpos     = 0;

	to = lifptr->lifname;
	from = name;
	while ( (*to++ = *from++) != NULLCH ) {
		;
	}

	/* Neither index block nor data block are initially valid	*/

	lifptr->lifinum    = LF_INULL;
	lifptr->lifdnum    = LF_DNULL;
	lifptr->lifindnum  = LF_DNULL;
	lifptr->lif2indnum = LF_DNULL;
	lifptr->lif3indnum = LF_DNULL;

	/* Initialize byte pointer to address beyond the end of the	*/
	/*	buffer (i.e., invalid pointer triggers setup)		*/

	lifptr->lifbyte = &lifptr->lifdblock[LF_BLKSIZ];
	lifptr->lifibdirty = FALSE;
	lifptr->lifdbdirty = FALSE;
	lifptr->lifindbdirty = FALSE;
	lifptr->lif2indbdirty = FALSE;
	lifptr->lif3indbdirty = FALSE;

	signal(Lif_data.lif_mutex);

	return lifptr->lifdev;
}
