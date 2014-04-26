#define _VERSION "0.1"
#define COMMAND_BUFFER_LENGTH 512


typedef struct /*_ARGUMENTS*/ {
	unsigned verbosity;
	char* cfgfile;
} ARGUMENTS;

typedef struct /*_CLIENT_INFO*/ {
	int fd;
	char command_buffer[COMMAND_BUFFER_LENGTH];
} CLIENT;
