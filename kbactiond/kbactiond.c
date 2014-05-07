#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>

#include "kbactiond.h"

#include "token.c"
#include "config.c"
#include "argparse.c"
#include "cfgparse.c"

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
	//begin execution
	//
	
	cfg_free(&cfg);
	return 0;
}
