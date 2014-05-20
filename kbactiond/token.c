const char* dbg_token_type(TOKEN_TYPE type){
	switch(type){
		case T_NOMATCH:
			return "T_NOMATCH";
		case T_INCOMPLETE:
			return "T_INCOMPLETE";
		case T_START:
			return "T_START";
		case T_APPEND:
			return "T_APPEND";
		case T_PARAM:
			return "T_PARAM";
		case T_DO:
			return "T_DO";
		case T_EXEC:
			return "T_EXEC";
	}
	return "UNKNOWN";
}

TOKEN* token_resolve(ARGUMENTS* args, CONFIG* cfg, TOKEN_TYPE* out_type, char* input){
	unsigned tok, i;
	TOKEN_TYPE rv=T_NOMATCH;
	TOKEN* resolved=NULL;

	for(tok=0;cfg->tokens[tok];tok++){
		//full match -> set rv, resolved
		
		for(i=0;cfg->tokens[tok]->token[i]==input[i]&&cfg->tokens[tok]->token[i]!=0;i++){
		}

		if(input[i]==0&&cfg->tokens[tok]->token[i]==input[i]){
			//complete match
			resolved=cfg->tokens[tok];
			rv=cfg->tokens[tok]->type;
			break;
		}
		else if(input[i]==0&&i>0){
			//partial match
			rv=T_INCOMPLETE;		
		}
		else{
			//no match
		}
	}


	if(out_type){
		*out_type=rv;
	}
	return resolved;
}

bool token_add_mapping(CONFIG* cfg, char* name, char* action, TOKEN_TYPE type){
	int insert_pos=0;
	if(!cfg->tokens){
		cfg->tokens=malloc(2*sizeof(TOKEN*));
		if(!cfg->tokens){
			perror("token_add_mapping/alloc");
			return false;
		}
		cfg->tokens[1]=NULL;
	}
	else{
		for(;cfg->tokens[insert_pos];insert_pos++){
		}
		cfg->tokens=realloc(cfg->tokens, (insert_pos+2)*sizeof(TOKEN*));
		if(!cfg->tokens){
			perror("token_add_mapping/realloc");
			return false;
		}
		cfg->tokens[insert_pos+1]=NULL;
	}

	cfg->tokens[insert_pos]=calloc(1, sizeof(TOKEN));
	if(!cfg->tokens[insert_pos]){
		perror("token_add_mapping/calloc");
		return false;
	}

	//FIXME check for prefix-free properties

	//copy data
	strncpy(cfg->tokens[insert_pos]->token, name, MAX_TOKEN_LENGTH);
	cfg->tokens[insert_pos]->type=type;
	
	if(action){
		strncpy(cfg->tokens[insert_pos]->command, action, MAX_PART_LENGTH);
	}

	return true;
}

bool token_free(CONFIG* cfg){
	unsigned i;
	if(cfg->tokens){
		for(i=0;cfg->tokens[i];i++){
			free(cfg->tokens[i]);
		}
		free(cfg->tokens);
	}
	return true;
}

TOKEN_TYPE token_type_from_string(char* in){
	if(!strncmp(in, "START", 5)){
		return T_START;
	}
	else if(!strncmp(in, "APPEND", 6)){
		return T_APPEND;
	}
	else if(!strncmp(in, "PARAM", 5)){
		return T_PARAM;
	}
	else if(!strncmp(in, "DO", 2)){
		return T_DO;
	}
	else if(!strncmp(in, "EXEC", 4)){
		return T_EXEC;
	}
	return T_NOMATCH;
}
