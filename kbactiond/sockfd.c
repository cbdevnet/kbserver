int conn_process_blocking(ARGUMENTS* args, CONFIG* cfg){
	//TODO
	return -1;
}

bool conn_open(CONNECTION* conn){
	int status;
	char port[10];

	struct addrinfo hints;
	struct addrinfo* list;
	struct addrinfo* list_it;

	if(conn->type!=CONN_LISTEN&&conn->type!=CONN_OUTGOING){
		return false;
	}

	memset(&hints, 0, sizeof(hints));
	snprintf(port, sizeof(port), "%d", conn->spec.port);

	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	if(conn->type==CONN_LISTEN){
		hints.ai_flags=AI_PASSIVE;
	}

	status=getaddrinfo(conn->spec.hostname, port, &hints, &list);
	if(status!=0){
		fprintf(stderr, "conn_open/gai: %s\n", gai_strerror(status));
		return false;
	}

	for(list_it=list;list_it!=NULL;list_it=list_it->ai_next){
		conn->fd=socket(list_it->ai_family, list_it->ai_socktype, list_it->ai_protocol);
		
		if(conn->fd<0){
			continue;
		}

		if(conn->type==CONN_LISTEN){
			status=bind(conn->fd, list_it->ai_addr, list_it->ai_addrlen);
			if(status<0){
				perror("conn_open/bind");
				//FIXME portability
				close(conn->fd);
				conn->fd=-1;
				continue;
			}

			status=listen(conn->fd, LISTEN_QUEUE_LENGTH);
			if(status<0){
				perror("conn_open/listen");
				close(conn->fd);
				conn->fd=-1;
				continue;
			}
		}
		else if(conn->type==CONN_OUTGOING){
			status=connect(conn->fd, list_it->ai_addr, list_it->ai_addrlen);
			if(status<0){
				perror("conn_open/connect");
				//FIXME portability
				close(conn->fd);
				conn->fd=-1;
				continue;
			}
		}
		break;
	}

	freeaddrinfo(list);
	
	if(!list_it){
		return false;
	}
	
	return true;
}

bool conn_init(ARGUMENTS* args, CONFIG* cfg){
	unsigned pos;
	
	//open listen connections
	if(cfg->listen_socks){
		for(pos=0;cfg->listen_socks[pos];pos++){
			if(args->verbosity>1){
				fprintf(stderr, "Opening listening socket on %s Port %d\n", cfg->listen_socks[pos]->spec.hostname, cfg->listen_socks[pos]->spec.port);
			}
			if(!conn_open(cfg->listen_socks[pos])){
				fprintf(stderr, "Failed to open listening socket on %s Port %d\n", cfg->listen_socks[pos]->spec.hostname, cfg->listen_socks[pos]->spec.port);
				return false;
			}
		}
	}
	
	//open client connections
	if(cfg->inputs){
		for(pos=0;cfg->inputs[pos];pos++){
			if(cfg->inputs[pos]->conn.type==CONN_OUTGOING){
				if(args->verbosity>1){
					fprintf(stderr, "Opening connection to %s Port %d\n", cfg->inputs[pos]->conn.spec.hostname, cfg->inputs[pos]->conn.spec.port);
				}

				if(!conn_open(&(cfg->inputs[pos]->conn))){
					fprintf(stderr, "Failed to connect to %s Port %d\n", cfg->inputs[pos]->conn.spec.hostname, cfg->inputs[pos]->conn.spec.port);
					return false;
				}
			}
		}
	}
	

	return true;
}

bool conn_close(CONFIG* cfg){
	unsigned pos;

	if(cfg->listen_socks){
		for(pos=0;cfg->listen_socks[pos];pos++){
			if(cfg->listen_socks[pos]->fd>0){
				close(cfg->listen_socks[pos]->fd);
				cfg->listen_socks[pos]->fd=-1;
			}
		}
	}

	if(cfg->inputs){
		for(pos=0;cfg->inputs[pos];pos++){
			if(cfg->inputs[pos]->conn.fd>0){
				close(cfg->inputs[pos]->conn.fd);
				cfg->inputs[pos]->conn.fd=-1;
			}
		}
	}
	return true;
}
