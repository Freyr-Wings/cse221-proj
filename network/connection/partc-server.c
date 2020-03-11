#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 

int main(int argc, char const *argv[]){
	if(argc !=2){
		printf("usage: ./server port\n");
		exit(0);
	}

	int port = atoi(argv[1]);

	int server_sd, client_sd;
	struct sockaddr_in server_addr,client_addr;

	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	server_sd = socket(AF_INET,SOCK_STREAM,0);
	if(server_sd < 0){
		perror("socket() failed!\n");
		exit(0);
	}

	if(bind(server_sd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		perror("bind() failed\n");
		exit(0);
	}

	if(listen(server_sd, 3) < 0){
		perror("listen() failed!\n");
		exit(0);
	}

	socklen_t addrlen = sizeof(client_addr);

	while(1){
		if((client_sd = accept(server_sd, (struct sockaddr*)&client_addr, &addrlen)) < 0){
			perror("accept() failed!\n");
			exit(0);
		}

	}

	return 0;


}