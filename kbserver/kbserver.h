#define VERSION "1.0rc1"

#define MAX_CFGLINE_LENGTH 1024
#define LISTEN_QUEUE_LENGTH 30

typedef enum /*_KEYMODE*/ {
	MODE_UP=0,
	MODE_DOWN=1
} KEYMODE;

typedef enum {
	TCP_SOCKET,
	UNIX_SOCKET
} SOCKET_TYPE;

typedef struct _MAPPING {
	uint16_t scancode;
	KEYMODE mode;
	char* map_target;
	struct _MAPPING* next;
} MAPPING;

typedef struct /*_CFG_PARAMS*/ {
	int verbosity;		//verbosity level
	char* config_file;	//cfg file, allocated by system
	MAPPING* mapping_head;
	char* input_device;	//input device, allocated by cfgparse
	SOCKET_TYPE socket_type;   // the type of the listening-socket
	char* bind_host;	//bindhost, allocated by cfgparse
	uint16_t port;		//local port
	char* unix_socket_location;	// the location of the unix-socket to use
	bool send_raw;		//send raw scancodes
	bool exclusive_access;	//request exclusive access
} CONFIG_PARAMS;
