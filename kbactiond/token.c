bool token_add_mapping(CONFIG* cfg, char* name, char* action, TOKEN_TYPE type){
	//TODO
	return false;
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
	return T_INVALID;
}

const char* dbg_token_type(TOKEN_TYPE type){
	switch(type){
		case T_INVALID:
			return "T_INVALID";
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
