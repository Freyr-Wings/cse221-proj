#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr,"usage: ./server port\n");
        exit(1);
    }
    int port = atoi(argv[1]);

    printf("Starting server\n");
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char send_buf = 'a';
    char recv_buf = 'a';

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(1);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(1);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind");
        exit(1);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(1);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(1);
    }

    while(1) {
        recv(new_socket, &recv_buf, 1, 0);
        send(new_socket, &send_buf, 1, 0);
    }

    return 0;
} 