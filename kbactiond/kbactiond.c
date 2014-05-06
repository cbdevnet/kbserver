#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>
#include <ctype.h>

#include "kbactiond.h"

#include "token.c"
#include "argparse.c"
#include "cfgparse.c"
#include "config.c"

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
		exit(1);
	}

	//check config sanity
	if(!cfg_sane(&args, &cfg)){
		fprintf(stderr, "Configuration failed the sanity test\n");
		exit(1);
	}

	//open connections
	//begin execution
	//
	return 0;
}
