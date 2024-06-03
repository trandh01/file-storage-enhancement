/* lifilesys.h */

#ifndef	Nlifl
#define	Nlifl	1
#endif

/* Use the remote disk device if no disk is defined (file system  */
/*  *assumes* the underlying disk has a block size of 512 bytes)  */

#ifndef	LIF_DISK_DEV
#define	LIF_DISK_DEV	SYSERR
#endif

#define	LIF_BLKSIZ	512		/* Assumes 512-byte disk blocks	*/
#define	LIF_NAME_LEN	16		/* Length of name plus null	*/
#define	LIF_NUM_DIR_ENT	20		/* Num. of files in a directory	*/

#define	LIF_FREE		0		/* Slave device is available	*/
#define	LIF_USED		1		/* Slave device is in use	*/

#define	LIF_INULL	(ibid32) -1	/* Index block null pointer	*/
#define	LIF_DNULL	(dbid32) -1	/* Data block null pointer	*/
#define LIF_DIBLEN	15			/* the number of direct blocks in an inode */
#define LIF_NUM_ENT_PER_BLK (LIF_BLKSIZ / sizeof (dbid32)) /* the number of block pointers in a disk block */

#define LIF_AREA_DIRECT	(LIF_DIBLEN * LIF_BLKSIZ)
#define LIF_AREA_INDIR	(LIF_NUM_ENT_PER_BLK * LIF_BLKSIZ)
#define LIF_AREA_2INDIR	(LIF_NUM_ENT_PER_BLK * LIF_NUM_ENT_PER_BLK * LIF_BLKSIZ)
#define LIF_AREA_3INDIR	(LIF_NUM_ENT_PER_BLK * LIF_NUM_ENT_PER_BLK * LIF_NUM_ENT_PER_BLK * LIF_BLKSIZ)

#define	LIF_IDATA	8192		/* Bytes of data indexed by a	*/
					/*   single index block		*/
#define	LIF_IMASK	0x00001fff	/* Mask for the data indexed by	*/
					/*   one index block (i.e.,	*/
					/*   bytes 0 through 8191).	*/
#define	LIF_DMASK	0x000001ff	/* Mask for the data in a data	*/
					/*   block (0 through 511)	*/

#define	LIF_AREA_IB	1		/* First sector of i-blocks	*/
#define	LIF_AREA_DIR	0		/* First sector of directory	*/

/* Structure of an index block on disk */

// 512 bytes, containing 128 pointers, each of size 4 bytes
struct	lifindblk		{		/* Format of indirect block	*/
	dbid32		ib_dba[LIF_NUM_ENT_PER_BLK]; /* Ptrs to data blocks indexed	*/
};

struct	lifiblk		{		/* Format of index block	*/
	dbid32		ib_dba[LIF_DIBLEN];/* Ptrs to data blocks indexed	*/
	dbid32		ind;
	dbid32		ind2;
	dbid32		ind3;
};

/* Structure of a data block when on the free list on disk */

struct	lifdbfree {
	dbid32	lif_nextdb;		/* Next data block on the list	*/
	char	lif_unused[LIF_BLKSIZ - sizeof(dbid32)];
};

/* Format of the file system directory, either on disk or in memory */

#pragma pack(2)
struct	lifdir	{			/* Entire directory on disk	*/
	uint32	lifd_fsysid;		/* File system ID		*/
	int16	lifd_vers;		/* File system version		*/
	int16	lifd_subvers;		/* File system subversion	*/
	uint32	lifd_allzeros;		/* All 0 bits			*/
	uint32	lifd_allones;		/* All 1 bits			*/
	dbid32	lifd_dfree;		/* List of free d-blocks on disk*/
	ibid32	lifd_ifree;		/* List of free i-blocks on disk*/
	int32	lifd_nfiles;		/* Current number of files	*/
	struct	ldentry lifd_files[LIF_NUM_DIR_ENT]; /* Set of files	*/
	uint32	lifd_revid;		/* fsysid in reverse byte order	*/
};
#pragma pack()

/* Global data used by local file system */

struct	lifdata	{			/* Local file system data	*/
	did32	lif_dskdev;		/* Device ID of disk to use	*/
	sid32	lif_mutex;		/* Mutex for the directory and	*/
					/*   index/data free lists	*/
	struct	lifdir	lif_dir;		/* In-memory copy of directory	*/
	bool8	lif_dirpresent;		/* True when directory is in	*/
					/*   memory (1st file is open)	*/
	bool8	lif_dirdirty;		/* Has the directory changed?	*/
};

/* Control block for local file pseudo-device */

struct	liflcblk	{			/* Local file control block	*/
					/*   (one for each open file)	*/
	byte	lifstate;		/* Is entry free or used	*/
	did32	lifdev;			/* Device ID of this device	*/
	sid32	lifmutex;		/* Mutex for this file		*/
	struct	ldentry	*lifdirptr;	/* Ptr to file's entry in the	*/
					/*   in-memory directory	*/
	int32	lifmode;			/* Mode (read/write/both)	*/
	uint32	lifpos;			/* Byte position of next byte	*/
					/*   to read or write		*/
	char	lifname[LIF_NAME_LEN];	/* Name of the file		*/
	ibid32	lifinum;			/* ID of current index block in	*/
					/*   lifiblock or LIF_INULL	*/
	struct	lifiblk	lifiblock;	/* In-mem copy of current index	*/
					/*   block			*/
	dbid32	lifdnum;			/* Number of current data block	*/
					/*   in lifdblock or LIF_DNULL	*/
	char	lifdblock[LIF_BLKSIZ];	/* In-mem copy of current data	*/
					/*   block			*/
	char	*lifbyte;		/* Ptr to byte in lifdblock or	*/
					/*   address one beyond lifdblock*/
					/*   if current file pos lies	*/
					/*   outside lifdblock		*/

	// the indirect block number and content
	dbid32	lifindnum;
	dbid32	lifindblock[LIF_NUM_ENT_PER_BLK];

	// the doubly indirect block number and content
	dbid32	lif2indnum;
	dbid32	lif2indblock[LIF_NUM_ENT_PER_BLK];

	// the triply indirect block number and content
	dbid32	lif3indnum;
	dbid32	lif3indblock[LIF_NUM_ENT_PER_BLK];

	bool8	lifibdirty;			/* Has lifiblock changed?	*/
	bool8	lifdbdirty;			/* Has lifdblock changed?	*/
	bool8	lifindbdirty;		/* Has lifindblock changed?	*/
	bool8	lif2indbdirty;		/* Has lif2indblock changed?	*/
	bool8	lif3indbdirty;		/* Has lif3indblock changed?	*/
};

extern	struct	lifdata	Lif_data;
extern	struct	liflcblk	lifltab[];

/* Control functions */

#define	LIF_CTL_DEL	F_CTL_DEL	/* Delete a file		*/
#define	LIF_CTL_TRUNC	F_CTL_TRUNC	/* Truncate a file		*/
#define LIF_CTL_SIZE	F_CTL_SIZE	/* Obtain the size of a file	*/
