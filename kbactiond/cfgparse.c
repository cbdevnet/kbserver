char* string_trim_lead(char* in){
	unsigned i;
	for(i=0;isspace(in[i]);i++){
	}
	return in+i;
}

char* string_parse_expression(char* in){
	char* translation=calloc(strlen(input), sizeof(char));
i	unsigned pos;

}

CFG_LINE_STATUS cfg_handle_line(char* line, ARGUMENTS* args, CONFIG* cfg){
	TOKEN new_token;

	//skip comments & blank lines
	if(line[0]=='#'||line[0]==0){
		return LINE_OK;
	}

	if(!strncmp(line, "token", 5)){
		//handle token stanza
		line=string_trim_lead(line+5);
		printf("Token: %s\n", line);
		//read stanza parts (NAME, COMMAND, TYPE)
			//find first separator, replace by 0, parse
			//find second separator, replace by 0, parse
			//parse from last separator
	}
	else if(!strncmp(line, "connect", 7)){
		//handle connect stanza
		line=string_trim_lead(line+7);
		printf("Connect: %s\n", line);
	}
	else if(!strncmp(line, "listen", 6)){
		//handle listen stanza
		line=string_trim_lead(line+6);
		printf("Listen: %s\n", line);
	}
	else{
		fprintf(stderr, "Unknown token: %s\n", line);
		return LINE_FAIL;
	}
}

bool cfg_read(ARGUMENTS* args, CONFIG* cfg){
	FILE* cfg_handle;
	//FIXME null contents
	char cfg_line[MAX_CFGLINE_LENGTH+1];
	int err, line_count=0, offset;
	CFG_LINE_STATUS line_status;

	if(!args->cfgfile){
		fprintf(stderr, "No config file supplied, aborting\n");
		return false;
	}

	cfg_handle=fopen(args->cfgfile, "r");
	if(!cfg_handle){
		perror("read_config");
		return false;
	}

	while(fgets(cfg_line, MAX_CFGLINE_LENGTH, cfg_handle)!=NULL){
		line_count++;
		
		//skip trailing newline and spaces
		for(offset=strlen(cfg_line)-1;offset>=0;offset--){
			if(isspace(cfg_line[offset])){
				cfg_line[offset]=0;
			}
			else{
				break;
			}
		}

		//skip leading spaces
		for(offset=0;isspace(cfg_line[offset]);offset++){
		}

		line_status=cfg_handle_line(cfg_line+offset, args, cfg);

		switch(line_status){
			case LINE_OK:
				break;

			case LINE_WARN:
				fprintf(stderr, "Warning emitted on configuration file line %d\n", line_count);
				break;

			case LINE_FAIL:
				fprintf(stderr, "Failed to parse configuration file line %d, aborting\n", line_count);
				fclose(cfg_handle);
				return false;
		}
	}

	err=0;

	if(errno!=0){
		perror("cfg_read");
		err=-1;
	}

	fclose(cfg_handle);

	return err==0;
}
