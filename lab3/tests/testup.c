#include <syscall.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>

struct dev_orientation {
	int azimuth; /* rotation around
		      * the X axis (-180<=azimuth<180)
		      */
	int pitch;   /* rotation around the Y-axis: -90<=pitch<=90 */
	int roll;    /* rotation around Z-axis: +Y == -roll, -180<=roll<=180 */
};

struct orientation_range {
	struct dev_orientation orient; /* device orientation */
	unsigned int azimuth_range; /* +/- degrees around X-axis */
	unsigned int pitch_range; /* +/- degrees around Y-axis */
	unsigned int roll_range; /* +/- degrees around Z-axis */
};


int main(){
    //struct dev_orientation dev;
    struct orientation_range range;
    range.orient.azimuth = 0;
    range.orient.pitch = 0;
    range.orient.roll = 0;
    range.azimuth_range = 10;
    range.pitch_range = 10;
    range.roll_range = 10;
    int ret = syscall(324,&range);
    printf("create %d\n",ret);
    if(ret < 0)
        return 0;
    for(int i = 0; i < 5; i++){
        int pid = fork();
        if(pid == 0){
            while(1){
                int ret2 = syscall(322,ret);
                printf("face up %d\n",ret2);
                if(ret2 < 0){
                    exit(0);
                }
                sleep(2);
            }
        }
    }
    sleep(60);
    int ret3 = syscall(321, ret);
    printf("over %d\n",ret3);
    return 0;
}

