#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <syslog.h>
#include <signal.h>
#include <string.h>

#include <hardware/hardware.h>
#include <hardware/sensors.h>

#include "orientd.h"

static int open_sensors(struct sensors_module_t **sensors,
			struct sensors_poll_device_t **device);
static int poll_sensor_data(struct sensors_poll_device_t *device,
			    struct dev_orientation *cur_orientation);

void sigExit(int sig)
{
	if(sig == SIGTERM)
	{
		syslog(LOG_INFO, "Terminated.");
		closelog();
		exit(0);
	}
}

char* itoa(int i)
{	
	char* s = malloc(30*sizeof(char));
	sprintf(s, "%d", i);
	return s;
}

int main(int argc, char **argv)
{
	struct sensors_module_t *sensors;
	struct sensors_poll_device_t *device;

	if (open_sensors(&sensors, &device))
		return EXIT_FAILURE;

	/********** Demo code begins **********/
	/* Replace demo code with your daemon */
	struct dev_orientation orientation;
	
	if(-1 == daemon(0, 0))
	{
		printf("Error when creating daemon!\n");
		exit(-1);
	}
	struct sigaction sa;
	sa.sa_handler = sigExit;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if(sigaction(SIGTERM, &sa, NULL))
	{
		printf("Error when setting sigaction!\n");
		exit(0);
	}
	while (true) {
		if (poll_sensor_data(device, &orientation)) {
			printf("No data received!\n");
		} else {
			/*int fd = open("log.txt", O_RDWR | O_CREAT | O_APPEND, 0644);
			if(-1 == fd)
			{
				printf("Error when opening file!\n");
				exit(0);
			}
			char buf[70];
			strcat(buf, "azimuth: ");
			strcat(buf, itoa(orientation.azimuth));
			strcat(buf, "	pitch: ");
			strcat(buf, itoa(orientation.pitch));
			strcat(buf, "	roll: ");
			strcat(buf, itoa(orientation.roll));
			strcat(buf, "\n");
			write(fd, buf, strlen(buf));
			close(fd);*/
			//int q = syscall(__NR_hello);
			int ret = set_orientation(&orientation);
			printf("azimuth = %d, pitch = %d, roll = %d, ret = %d\n",
			       orientation.azimuth, orientation.pitch,
			       orientation.roll, ret);
			
		}
		usleep(100000);
	}
	/*********** Demo code ends ***********/

	return EXIT_SUCCESS;
}

/***************************** DO NOT TOUCH BELOW *****************************/

static inline int open_sensors(struct sensors_module_t **sensors,
			       struct sensors_poll_device_t **device)
{
	int err = hw_get_module("sensors",
				(const struct hw_module_t **)sensors);

	if (err) {
		fprintf(stderr, "Failed to load %s module: %s\n",
			SENSORS_HARDWARE_MODULE_ID, strerror(-err));
		return -1;
	}

	err = sensors_open(&(*sensors)->common, device);

	if (err) {
		fprintf(stderr, "Failed to open device for module %s: %s\n",
			SENSORS_HARDWARE_MODULE_ID, strerror(-err));
		return -1;
	}

	if (!*device)
		return -1;

	const struct sensor_t *list;
	int count = (*sensors)->get_sensors_list(*sensors, &list);

	if (count < 1 || !list) {
		fprintf(stderr, "No sensors!\n");
		return -1;
	}

	for (int i = 0 ; i < count ; ++i) {
		printf("%s (%s) v%d\n", list[i].name, list[i].vendor,
			list[i].version);
		printf("\tHandle:%d, type:%d, max:%0.2f, resolution:%0.2f\n",
			list[i].handle, list[i].type, list[i].maxRange,
			list[i].resolution);
		(*device)->activate(*device, list[i].handle, 1);
	}
	return 0;
}

static int poll_sensor_data(struct sensors_poll_device_t *device,
			    struct dev_orientation *cur_orientation)
{
	static const size_t event_buffer_size = 128;
	sensors_event_t buffer[event_buffer_size];

	int count = device->poll(device, buffer, event_buffer_size);

	for (int i = 0; i < count; ++i) {
		if (buffer[i].type != SENSOR_TYPE_ORIENTATION)
			continue;
		cur_orientation->azimuth =
			(int)(buffer[i].orientation.azimuth / M_PI * 180);
		cur_orientation->pitch =
			(int)(buffer[i].orientation.pitch / M_PI * 180);
		cur_orientation->roll =
			(int)(buffer[i].orientation.roll / M_PI * 180);
		return 0;
	}

	return -1;
}
