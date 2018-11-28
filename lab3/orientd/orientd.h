#ifndef _ORIENTD_H
#define _ORIENTD_H

#include <unistd.h>

#define __NR_hello 323
#define __NR_set_orientation 325
#define __NR_create_orientation 324

struct dev_orientation {
	int azimuth; /* rotation around
		      * the X axis (-180<=azimuth<180)
		      */
	int pitch;   /* rotation around the Y-axis: -90<=pitch<=90 */
	int roll;    /* rotation around Z-axis: +Y == -roll, -180<=roll<=180 */
};

/* syscall wrapper */
static inline int set_orientation(struct dev_orientation *orient)
{
	return syscall(__NR_set_orientation, orient);
}

#endif /* _ORIENTD_H */
