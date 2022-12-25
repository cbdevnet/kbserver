#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>

int sock_open_tcp(CONFIG_PARAMS* cfg){
	int fd=-1, status;
	char port[10];
	struct addrinfo hints;
	struct addrinfo* info;
	struct addrinfo* addr_it;

	memset(&hints, 0, sizeof(hints));
	snprintf(port, 9, "%d", cfg->port);

	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;

	status=getaddrinfo(cfg->bind_host, port, &hints, &info);
	if(status!=0){
		fprintf(stderr, "sock_open/gai: %s\n", gai_strerror(status));
		return -1;
	}

	for(addr_it=info;addr_it!=NULL;addr_it=addr_it->ai_next){
		fd=socket(addr_it->ai_family, addr_it->ai_socktype, addr_it->ai_protocol);
		if(fd<0){
			continue;
		}

		status=bind(fd, addr_it->ai_addr, addr_it->ai_addrlen);
		if(status<0){
			close(fd);
			continue;
		}

		break;
	}

	freeaddrinfo(info);

	if(!addr_it){
		fprintf(stderr, "Failed to create listening socket\n");
		if (fd >= 0) {
			close(fd);
		}
		return -1;
	}

	status=listen(fd, LISTEN_QUEUE_LENGTH);
	if(status<0){
		perror("sock_open/listen");
		close(fd);
		return -1;
	}

	return fd;
}

int sock_open_unix(CONFIG_PARAMS* cfg){
	int fd;
	struct sockaddr_un addr;

	if ((fd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Could not create UNIX-socket!\n");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, cfg->unix_socket_location);
	unlink(cfg->unix_socket_location);

	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		fprintf(stderr, "Could not bind to socket!\n");
		if (fd >= 0) {
			close(fd);
		}
		return -1;
	}

	int status=listen(fd, LISTEN_QUEUE_LENGTH);
	if(status<0){
		perror("sock_open/listen");
		close(fd);
		return -1;
	}

	return fd;
}

int sock_open(CONFIG_PARAMS* cfg) {
	if (cfg->socket_type == TCP_SOCKET) {
		return sock_open_tcp(cfg);
	} else {
		return sock_open_unix(cfg);
	}
}

int sock_close(int fd){
	return close(fd);
}
