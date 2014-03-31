#include <linux/input.h>
#include <unistd.h>
#include <stdio.h>

void setLED(int fd, int ledCode, int value) {
	struct input_event event;
	event.type  = EV_LED;
	event.code  = ledCode;
	event.value = value;

  	if (write (fd, &event, sizeof (struct input_event)) < 0) {
		printf("failed to send led change event.");	
	}
}
