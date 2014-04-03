void setLED(int fd, int led, int value) {
	struct input_event event;
	event.type=EV_LED;
	event.code=led;
	event.value=value;

  	if(write(fd, &event, sizeof(struct input_event))<0){
		perror("setLED");
	}
}
