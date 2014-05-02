bool cfg_read(ARGUMENTS* args, CONFIG* cfg){
	FILE* cfg;

	if(!args->cfgfile){
		if(args->verbosity>0){
			printf("No config file supplied, aborting\n");
		}
		return false;
	}

	cfg=fopen(args->cfgfile, "r");
	if(!cfg){
		if(args->verbosity>0){
			perror("read_config");
		}
		return false;
	}

	//TODO read config file

	fclose(cfg);

	return false;
}
