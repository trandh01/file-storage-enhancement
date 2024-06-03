/*  main.c  - main */

#include <xinu.h>

#define CONTENT "Hello World"
#define LEN 138800

process main() {
    // print some constants
    kprintf("LIF_AREA_DIRECT: %d\n", LIF_AREA_DIRECT);
    kprintf("LIF_AREA_INDIR: %d\n", LIF_AREA_INDIR);
    kprintf("LIF_AREA_2INDIR: %d\n", LIF_AREA_2INDIR);
    kprintf("LIF_AREA_3INDIR: %d\n", LIF_AREA_3INDIR);

    lifscreate(RAM0, 1700, 10052400); // initialize the ramdisk

    did32 fd;
    int status;
    int i;
    char buffer[LEN+1];
    int count;
    int content_len = strlen(CONTENT);

    for (i = 0, count = 0; count < LEN; i = (i + 1) % content_len, count++) {
        buffer[count] = CONTENT[i];
    }

    // buffer[LEN] = '\0'; // Null-terminate the buffer

    // open the file for write
    fd = open(LIFILESYS, "index.txt", "rw");
    kprintf("fd: %d\n", fd);

    // test write
    status = write(fd, buffer, LEN);
    kprintf("status for write: %d\n", status);

    // test seek
    status = seek(fd, 138750);
    kprintf("status for seek: %d\n", status);

    // test putc
    status = putc(fd, 'X');
    kprintf("status for putc: %d\n", status);

    status = close(fd);
    kprintf("status for close: %d\n", status);

    // re-open the file for read
    fd = open(LIFILESYS, "index.txt", "r");
    kprintf("fd: %d\n", fd);

    status = seek(fd, 138240);
    kprintf("status for seek: %d\n", status); 

    char buf[LEN + 1];
    memset(buf, NULLCH, LEN + 1);

    // test read
    status = read(fd, buf, LEN);
    buf[LEN] = '\0';
    kprintf("Content:");
    for (i = 0; i < LEN; ++i) {
        kprintf("%c", buf[i]);
    } 
    kprintf("\n"); 
}
