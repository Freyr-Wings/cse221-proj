#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 

static inline unsigned long rdtscp()
{
    unsigned long rax, rdx;
    asm volatile ("rdtscp\n" : "=a" (rax), "=d" (rdx) : :);
    return (rdx << 32) + rax;
}

int main(int argc, char const *argv[]){

	if(argc !=4 ){
		printf("usage: ./client ip port n\n");
		exit(0);
	}

	int port = atoi(argv[2]);
	int n = atoi(argv[3]);

	int socket_fd;
	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	if(inet_pton(AF_INET, argv[1], &server_addr.sin_addr)<=0)
    {
		perror("inet_pton() failed!\n");
		exit(0);
    }

	unsigned start,end;
	double con_time = 0;
	double clo_time = 0;
	double time;

	for(int i = 0 ; i < n; i++){

		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if(socket_fd < 0){
			perror("socket() failed!\n");
			exit(0);
		}

		start = rdtscp();
		if(connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
			perror("connect() failed!\n");
			exit(0);
		}
		end = rdtscp();

		// time = (end-start); // cycles
		time = (end-start) / 2600000000.0 * 1000000; // us
		// printf("connect time is %lf us \n", time);
		con_time += time;

		start = rdtscp();
		close(socket_fd);
		end = rdtscp();
		// time = (end-start); // cycles
		time = (end-start) / 2600000000.0 * 1000000; // us
		// printf("close time is %lf us\n", time);
		clo_time += time;
		usleep(10000);
	}
		printf("connect time is %lf us \n", con_time/n);
		printf("close time is %lf us\n", clo_time/n);

	return 0;
}
