#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "kbactiond.h"

#include "token.c"
#include "config.c"
#include "sockfd.c"
#include "argparse.c"
#include "cfgparse.c"
#include "sighandle.c"

bool stop_processing=false;

int usage(char* fn){
	printf("kbactiond v%s\n",_VERSION);
	return 1;
}

int main(int argc, char** argv){
	ARGUMENTS args;
	CONFIG cfg;
	
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
	//add all fds to read set
	//select over set
	//iterate over results
		//listen socket -> accept client
		//use or create CONN_INCOMING entry

		//data socket -> read data
		//if closed, mark data inactive
		//if timeout -> clear buffer
		//match data to token
		//resolve token to command/action
		//if necessary, execute
		//update last action timestamp
	}

	//clean up
	conn_close(&cfg);
	cfg_free(&cfg);
	return 0;
}
