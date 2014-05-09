bool cfg_parse_connspec(CONN_SPEC* spec, char* input){
	unsigned off;

	for(off=0;!isspace(input[off]);off++){
	}

	if(input[off]==0||off==0){
		return false;
	}

	spec->hostname=calloc(off+1, sizeof(char));
	if(!spec->hostname){
		return false;
	}
	
	strncpy(spec->hostname, input, off);

	spec->port=(uint16_t)strtoul(input+off, NULL, 10);

	if(spec->port==0){
		return false;
	}

	return true;
}

bool cfg_store_data_connspec(CONFIG* cfg, CONN_SPEC* conn){
	unsigned num;
	CONN_SPEC new_conn=*conn;
	//TODO error checking
	
	if(!cfg->inputs){
		cfg->inputs=realloc(cfg->inputs, 2*sizeof(DATA_CONNECTION*));
		cfg->inputs[1]=NULL;
		num=0;
	}
	
	else{
		for(num=0;cfg->inputs[num];num++){
		}
		cfg->inputs=realloc(cfg->inputs, (num+2)*sizeof(DATA_CONNECTION*));
		cfg->inputs[num+1]=NULL;
	}
	
	cfg->inputs[num]=malloc(sizeof(DATA_CONNECTION));
	cfg->inputs[num]->conn.spec=new_conn;
	cfg->inputs[num]->conn.type=CONN_OUTGOING;

	return true;
}

bool cfg_store_listen_connspec(CONFIG* cfg, CONN_SPEC* conn){
	unsigned num;
	CONN_SPEC new_conn=*conn;
	//TODO error checking
	
	if(!cfg->listen_socks){
		cfg->listen_socks=realloc(cfg->listen_socks, 2*sizeof(CONNECTION*));
		cfg->listen_socks[1]=NULL;
		num=0;
	}
	
	else{
		for(num=0;cfg->listen_socks[num];num++){
		}
		cfg->listen_socks=realloc(cfg->listen_socks, (num+2)*sizeof(CONNECTION*));
		cfg->listen_socks[num+1]=NULL;
	}
	
	cfg->listen_socks[num]=malloc(sizeof(CONNECTION));
	cfg->listen_socks[num]->spec=new_conn;
	cfg->listen_socks[num]->type=CONN_LISTEN;

	return true;
}

bool cfg_sane(ARGUMENTS* args, CONFIG* cfg){
	//at least one exec or do
	//FIXME prefix-freeness
	unsigned listen_socks=0, client_socks=0, tokens=0;

	if(cfg->listen_socks){
		for(;cfg->listen_socks[listen_socks];listen_socks++){
		}
	}
	
	if(cfg->inputs){
		for(;cfg->inputs[client_socks];client_socks++){
		}
	}

	if(cfg->tokens){
		for(;cfg->tokens[tokens];tokens++){
		}
	}

	if(listen_socks+client_socks<1){
		fprintf(stderr, "No connections defined\n");
		return false;
	}

	if(tokens<1){
		fprintf(stderr, "No tokens defined\n");
		return false;
	}

	if(args->verbosity>0){
		fprintf(stderr, "Configuration details:\n");
		fprintf(stderr, "\t%d outgoing connections\n", client_socks);
		fprintf(stderr, "\t%d listening sockets\n", listen_socks);
		fprintf(stderr, "\t%d defined tokens\n", tokens);
		fprintf(stderr, "\tConnection timeout %d seconds\n", cfg->conn_timeout);
	}

	return true;
}

bool cfg_free(CONFIG* cfg){
	unsigned i;
	//free listen connection data
	if(cfg->listen_socks){
		for(i=0;cfg->listen_socks[i];i++){
			if(cfg->listen_socks[i]->fd>0){
				//FIXME use cross-platform calls here
				close(cfg->listen_socks[i]->fd);
				cfg->listen_socks[i]->fd=-1;
			}
			free(cfg->listen_socks[i]->spec.hostname);
			free(cfg->listen_socks[i]);
		}
		free(cfg->listen_socks);
	}

	//free client connection data
	if(cfg->inputs){
		for(i=0;cfg->inputs[i];i++){
			if(cfg->inputs[i]->conn.fd>0){
				//FIXME use cross-platform calls here
				close(cfg->inputs[i]->conn.fd);
				cfg->inputs[i]->conn.fd=-1;
			}
			free(cfg->inputs[i]->conn.spec.hostname);
			free(cfg->inputs[i]);
		}
		free(cfg->inputs);
	}

	return token_free(cfg);
}
