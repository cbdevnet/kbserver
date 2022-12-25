#include <sys/socket.h>
#include <sys/un.h>

int conn_handle_read(ARGUMENTS* args, CONFIG* cfg, unsigned connection){
	int bytes, bytes_left;

	bytes_left=sizeof(cfg->inputs[connection]->data_buf)-1-cfg->inputs[connection]->data_offset;
	
	if(bytes_left<1){
		return -1;
	}

	if(args->verbosity>3){
		fprintf(stderr, "Reading at max %d bytes from connection %d (Offset %d)\n", bytes_left, connection, cfg->inputs[connection]->data_offset);
	}

	bytes=recv(cfg->inputs[connection]->conn.fd, ((char*)&(cfg->inputs[connection]->data_buf))+cfg->inputs[connection]->data_offset, bytes_left, 0);

	if(bytes<=0){
		if(bytes<0){
			perror("read");
		}
		close(cfg->inputs[connection]->conn.fd);
		cfg->inputs[connection]->conn.fd=-1;
		cfg->inputs[connection]->active=false;

		if(args->verbosity>2){
			fprintf(stderr, "Disconnected client %d\n", connection);
		}
		return 0;
	}

	cfg->inputs[connection]->data_offset+=bytes;
	cfg->inputs[connection]->data_buf[cfg->inputs[connection]->data_offset]=0;

	return 0;
}

int conn_handle_new(ARGUMENTS* args, CONFIG* cfg, unsigned connection){
	unsigned pos;
	
	if(cfg->inputs){
		//find slot
		for(pos=0;cfg->inputs[pos];pos++){
			if(!cfg->inputs[pos]->active&&cfg->inputs[pos]->conn.type==CONN_INCOMING){
				if(args->verbosity>2){
					fprintf(stderr, "Accepting new client into existing slot %d\n", pos);
				}
				break;
			}
		}

		if(!cfg->inputs[pos]){
			//realloc
			cfg->inputs=realloc(cfg->inputs, (pos+2)*sizeof(DATA_CONNECTION*));
			if(!cfg->inputs){
				//TODO errmsg
				return -2;
			}
			cfg->inputs[pos+1]=NULL;
			cfg->inputs[pos]=malloc(sizeof(DATA_CONNECTION));
			if(!cfg->inputs[pos]){
				//TODO errmsg
				return -2;
			}
			cfg->inputs[pos]->conn.type=CONN_INCOMING;
			cfg->inputs[pos]->conn.spec.hostname=NULL;
			if(args->verbosity>2){
				fprintf(stderr, "Accepting new client into created slot %d\n", pos);
			}
		}
	}
	else{
		//allocate initial
		cfg->inputs=malloc(2*sizeof(DATA_CONNECTION*));
		if(!cfg->inputs){
			//TODO errmsg
			return -2;
		}
		cfg->inputs[1]=NULL;
		pos=0;
		cfg->inputs[pos]=malloc(sizeof(DATA_CONNECTION));
		if(!cfg->inputs[pos]){
			//TODO errmsg
			return -2;
		}
		cfg->inputs[pos]->conn.type=CONN_INCOMING;
		cfg->inputs[pos]->conn.spec.hostname=NULL;
		if(args->verbosity>2){
			fprintf(stderr, "Accepting new client into inital slot %d\n", pos);
		}
	}

	//insert connection
	cfg->inputs[pos]->active=true;
	cfg->inputs[pos]->data_offset=0;
	cfg->inputs[pos]->last_event=0;
	cfg->inputs[pos]->conn.fd=accept(cfg->listen_socks[connection]->fd, NULL, NULL);	
	return 0;
}

int conn_process_blocking(ARGUMENTS* args, CONFIG* cfg){
	fd_set read_fds;
	struct timeval tv;
	int maxfd=-1, error;
	unsigned i;

	FD_ZERO(&read_fds);
	
	//add all listen sockets
	if(cfg->listen_socks){
		for(i=0;cfg->listen_socks[i];i++){
			FD_SET(cfg->listen_socks[i]->fd, &read_fds);
			maxfd=(maxfd>cfg->listen_socks[i]->fd)?maxfd:cfg->listen_socks[i]->fd;
		}
	}

	//add active client sockets
	if(cfg->inputs){
		for(i=0;cfg->inputs[i];i++){
			if(cfg->inputs[i]->active){
				FD_SET(cfg->inputs[i]->conn.fd, &read_fds);
				maxfd=(maxfd>cfg->inputs[i]->conn.fd)?maxfd:cfg->inputs[i]->conn.fd;
			}
		}
	}

	//prepare timeouts
	tv.tv_sec=SELECT_TIMEOUT;
	tv.tv_usec=0;

	//select
	error=select(maxfd+1, &read_fds, NULL, NULL, &tv);
	if(error<0){
		perror("select");
		return -1;
	}
	if(args->verbosity>3){
		printf("Data on %d sockets\n", error);
	}
	
	//test listening sockets
	if(cfg->listen_socks){
		for(i=0;cfg->listen_socks[i];i++){
			if(FD_ISSET(cfg->listen_socks[i]->fd, &read_fds)){
				error=conn_handle_new(args, cfg, i);
				//TODO handle return value
			}
		}
	}

	//handle client reads
	if(cfg->inputs){
		for(i=0;cfg->inputs[i];i++){
			if(cfg->inputs[i]->active&&FD_ISSET(cfg->inputs[i]->conn.fd, &read_fds)){
				error=conn_handle_read(args, cfg, i);
				//TODO handle return value
			}
		}
	}

	return 0;
}

bool conn_open_tcp(CONNECTION* conn){
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

bool conn_open_unix(CONNECTION* conn){
	int status;

	struct sockaddr_un addr;


	if(conn->type!=CONN_LISTEN&&conn->type!=CONN_OUTGOING){
		return false;
	}

	if ((conn->fd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Could not create UNIX-socket!\n");
		return -1;
	}


	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, conn->spec.hostname);

	if(conn->type==CONN_LISTEN){
		unlink(conn->spec.hostname);

		if (bind(conn->fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			fprintf(stderr, "Could not bind to socket!\n");
			close(conn->fd);
			conn->fd=-1;
			return false;
		}

		status=listen(conn->fd, LISTEN_QUEUE_LENGTH);
		if (status<0){
			perror("conn_open/listen");
			close(conn->fd);
			conn->fd=-1;
			return false;
		}

	}
	else if(conn->type==CONN_OUTGOING){
		if (connect(conn->fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
			perror("conn_open/connect");
			close(conn->fd);
			conn->fd=-1;
			return -1;
		}
	}

	return true;
}


bool conn_open(CONNECTION* conn){
	if (conn->spec.socket_type == TCP_SOCKET) {
		return conn_open_tcp(conn);
	} else {
		return conn_open_unix(conn);
	}
}

bool conn_reconnect(ARGUMENTS* args, CONFIG* cfg){
	unsigned i;
	if(cfg->inputs){
		for(i=0;cfg->inputs[i];i++){
			if(!cfg->inputs[i]->active&&cfg->inputs[i]->conn.type==CONN_OUTGOING){
				if(args->verbosity>1){
					fprintf(stderr, "Reestablishing connection to %s Port %d\n", cfg->inputs[i]->conn.spec.hostname, cfg->inputs[i]->conn.spec.port);
				}

				if(!conn_open(&(cfg->inputs[i]->conn))){
					fprintf(stderr, "Failed to reconnect %s Port %d\n", cfg->inputs[i]->conn.spec.hostname, cfg->inputs[i]->conn.spec.port);
					return false;
				}

				cfg->inputs[i]->last_event=0;
				cfg->inputs[i]->data_offset=0;
				cfg->inputs[i]->active=true;
			}
		}
	}
	return true;
}

bool conn_init(ARGUMENTS* args, CONFIG* cfg){
	unsigned pos;
	
	//open listen connections
	if(cfg->listen_socks){
		for(pos=0;cfg->listen_socks[pos];pos++){
			if(args->verbosity>1){
				if (cfg->listen_socks[pos]->spec.socket_type == TCP_SOCKET) {
					fprintf(stderr, "Opening listening socket on %s Port %d\n", cfg->listen_socks[pos]->spec.hostname, cfg->listen_socks[pos]->spec.port);
				} else {
					fprintf(stderr, "Opening listening unix-socket on %s\n", cfg->listen_socks[pos]->spec.hostname);
				}
			}
			if(!conn_open(cfg->listen_socks[pos])){
				if (cfg->listen_socks[pos]->spec.socket_type == TCP_SOCKET) {
					fprintf(stderr, "Failed to open listening socket on %s Port %d\n", cfg->listen_socks[pos]->spec.hostname, cfg->listen_socks[pos]->spec.port);
				} else {
					fprintf(stderr, "Failed to open listening unix-socket on %s\n", cfg->listen_socks[pos]->spec.hostname);
				}
				return false;
			}
		}
	}
	
	//open client connections
	if(cfg->inputs){
		for(pos=0;cfg->inputs[pos];pos++){
			if(cfg->inputs[pos]->conn.type==CONN_OUTGOING){
				if(args->verbosity>1){
					if (cfg->inputs[pos]->conn.spec.socket_type == TCP_SOCKET) {
						fprintf(stderr, "Opening connection to %s Port %d\n", cfg->inputs[pos]->conn.spec.hostname, cfg->inputs[pos]->conn.spec.port);
					} else {
						fprintf(stderr, "Opening unix-socket on %s\n", cfg->inputs[pos]->conn.spec.hostname);
					}

				}

				if(!conn_open(&(cfg->inputs[pos]->conn))){
					if (cfg->inputs[pos]->conn.spec.socket_type == TCP_SOCKET) {
						fprintf(stderr, "Failed to connect to %s Port %d\n", cfg->inputs[pos]->conn.spec.hostname, cfg->inputs[pos]->conn.spec.port);
					} else {
						fprintf(stderr, "Failed to open unix-socket on %s\n", cfg->inputs[pos]->conn.spec.hostname);
					}

					return false;
				}

				cfg->inputs[pos]->data_offset=0;
				cfg->inputs[pos]->last_event=0;
				cfg->inputs[pos]->active=true;
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
