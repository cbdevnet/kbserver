#define _VERSION "0.1"
#define COMMAND_BUFFER_LENGTH 512
#define MAX_TOKEN_LENGTH 128
#define MAX_PART_LENGTH 64
#define MAX_CFGLINE_LENGTH (MAX_TOKEN_LENGTH+MAX_PART_LENGTH+128)

typedef struct /*_ARGUMENTS*/ {
	unsigned verbosity;
	char* cfgfile;
} ARGUMENTS;

typedef enum /*_CONN_TYPE*/ {
	CONN_LISTEN,
	CONN_INCOMING,
	CONN_OUTGOING
} CONN_TYPE;

typedef enum /*_TTYPE*/ {
	T_INVALID,
	T_INCOMPLETE,
	T_START,
	T_APPEND,
	T_PARAM,
	T_DO,
	T_EXEC
} TOKEN_TYPE;

typedef struct /*_CONN*/ {
	CONN_TYPE type;
	int fd;
} CONNECTION;

typedef enum /*_CFG_LINE_STATUS*/ {
	LINE_OK,
	LINE_WARN,
	LINE_FAIL
} CFG_LINE_STATUS;

typedef struct /*_DATA_CONN*/ {
	CONNECTION conn;
	char cmd_buf[COMMAND_BUFFER_LENGTH];
} DATA_CONNECTION;

typedef struct /*_TOKEN*/ {
	TOKEN_TYPE type;
	char token[MAX_TOKEN_LENGTH];
	char command[MAX_PART_LENGTH];
} TOKEN;

typedef struct /*_CONFIG*/ {
	CONNECTION* listen_socks;
	DATA_CONNECTION* inputs;
	TOKEN* tokens;
} CONFIG;
