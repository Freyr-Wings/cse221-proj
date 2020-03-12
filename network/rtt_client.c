#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>


static inline unsigned long rdtscp(){
    unsigned long rax, rdx;
    unsigned aux;
    asm volatile ("rdtscp\nlfence" : "=a" (rax), "=d" (rdx), "=c" (aux) : :);
    return (rdx << 32) + rax;
}


int main(int argc, char *argv[]) { 
    if (argc < 3) {
        fprintf(stderr, "usage: ./client hostname port [iter=1000]\n");
        exit(1);
    }

    int port = atoi(argv[2]);
    printf("Starting client\n");
    int sockfd = 0, valread;
    struct sockaddr_in serv_addr;
    char send_buf = 'a';
    char recv_buf = 'a';

    int iteration = 1000;
    if (argc == 4) {
        iteration = atoi(argv[3]);
    }

    unsigned long start, end;
    double records = 0;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(port); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) {
        perror("inet_pton");
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        exit(1);
    }

    for(int i = 0; i < iteration; i++) {
        start = rdtscp();
        send(sockfd, &send_buf, 1, 0);
        recv(sockfd, &recv_buf, 1, 0);
        end = rdtscp();
        records += (end - start);
        printf("RTT: %lu\n", (end - start));
    }

    close(sockfd);


    printf("Average RTT: %f cycles, %f ms\n", records / (double)iteration, records / (double)iteration / 2.6e6);

    return 0; 
} 