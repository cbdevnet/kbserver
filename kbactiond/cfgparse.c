char* string_trim_lead(char* in){
	unsigned i;
	for(i=0;isspace(in[i]);i++){
	}
	return in+i;
}

//returns number of chars read
int string_parse_expression(const char* in, char** out, const char parse_delim, const char invalid_marker){
	unsigned out_pos=0, in_pos=0;
	uint8_t conv_byte;
	char* translation=calloc(strlen(in), sizeof(char));
	char* conv_end;
	bool verbatim=false, invalid=false;

	if(!translation){
		perror("parse_expression/alloc");
		*out=NULL;
		return -1;
	}

	for(;in[in_pos];){
		if(in[in_pos]=='\''){
			verbatim=!verbatim;
			in_pos++;
		}
		else if(verbatim){
			translation[out_pos++]=in[in_pos];
			in_pos++;
		}
		else{
			for(;isspace(in[in_pos]);in_pos++){
			}

			if(in[in_pos]==parse_delim){
				if(invalid){
					free(translation);
					*out=NULL;
				}
				else{
					*out=translation;
				}
				return in_pos;
			}

			if(in[in_pos]==invalid_marker){
				invalid=true;
				in_pos++;
			}

			else if(in[in_pos]!=0&&in[in_pos]!='\''){
				conv_byte=(uint8_t)strtoul(in+in_pos, &conv_end, 0);
				if(conv_byte==0||in+in_pos==conv_end){
					free(translation);
					fprintf(stderr, "Invalid numeral at %s\n", in);
					*out=NULL;
					return -1;
				}
				translation[out_pos++]=conv_byte;
				in_pos+=(conv_end-(in+in_pos));
			}
		}
	}

	if(invalid){
		free(translation);
		*out=NULL;
	}
	else{
		*out=translation;
	}
	return in_pos;
}

CFG_LINE_STATUS cfg_handle_line(char* line, ARGUMENTS* args, CONFIG* cfg){
	unsigned off;
	char* token_name;
	char* token_command;
	TOKEN_TYPE token_type;
	CFG_LINE_STATUS rv=LINE_OK;


	//skip comments & blank lines
	if(line[0]=='#'||line[0]==0){
		return rv;
	}

	if(!strncmp(line, "token", 5)){
		//handle token stanza
		line=string_trim_lead(line+5);
		//read stanza parts (NAME, COMMAND, TYPE)
			//parse until first separator (name)
			line+=string_parse_expression(line, &token_name, ',', '-')+1;
			if(!token_name){
				return LINE_FAIL;
			}

			//parse until second separator (command)
			line+=string_parse_expression(line, &token_command, ',', '-')+1;
			
			//parse from last separator (type)
			token_type=token_type_from_string(line);
		
		printf("Token: \"%s\" Command: \"%s\" Type %s\n", token_name, token_command, dbg_token_type(token_type));

		free(token_name);
		if(token_command){
			free(token_command);
		}
		return rv;
	}
	else if(!strncmp(line, "connect", 7)){
		//handle connect stanza
		line=string_trim_lead(line+7);
		printf("Connect: %s\n", line);
		return LINE_OK;
	}
	else if(!strncmp(line, "listen", 6)){
		//handle listen stanza
		line=string_trim_lead(line+6);
		printf("Listen: %s\n", line);
		return LINE_OK;
	}

	fprintf(stderr, "Unknown token: %s\n", line);
	return LINE_FAIL;
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
