#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "kbactiond.h"
bool stop_processing=false;

#include "token.c"
#include "config.c"
#include "sockfd.c"
#include "argparse.c"
#include "cfgparse.c"
#include "sighandle.c"
#include "logic.c"

int usage(char* fn){
	printf("kbactiond v%s\n",_VERSION);
	return 1;
}

int main(int argc, char** argv){
	ARGUMENTS args;
	CONFIG cfg;
	int status;
	
	memset(&args, 0, sizeof(args));
	memset(&cfg, 0, sizeof(cfg));

	//parse arguments
	if(!args_parse(&args, argc-1, argv+1)){
		exit(usage(argv[0]));
	}
	
	//parse config file
	if(!cfg_read(&args, &cfg)){
		fprintf(stderr, "Failed to read configuration file\n");
		cfg_free(&cfg);
		exit(1);
	}

	//check config sanity
	if(!cfg_sane(&args, &cfg)){
		fprintf(stderr, "Configuration failed the sanity test\n");
		cfg_free(&cfg);
		exit(1);
	}

	//open connections
	if(!conn_init(&args, &cfg)){
		fprintf(stderr, "Failed to initialize network connections\n");
		conn_close(&cfg);
		cfg_free(&cfg);
		exit(1);
	}

	//set up signal handlers
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, sig_interrupt);

	//begin execution
	while(!stop_processing){
		status=conn_process_blocking(&args, &cfg);
		if(status<0){
			//TODO errhandling
		}

		status=logic_process_incoming(&args, &cfg);
		//iterate over active data connections
			//if timeout -> clear buffer
			//match data to token
			//resolve token to command/action
			//if necessary, execute
			//update last action timestamp

		if(status<0){
			//TODO errhandling
		}

		conn_reconnect(&args, &cfg);
	}

	//clean up
	conn_close(&cfg);
	cfg_free(&cfg);
	return 0;
}
