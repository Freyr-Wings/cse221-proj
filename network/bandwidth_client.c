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
#define BUF_SIZE 1024*1024*10


static inline unsigned long rdtscp(){
    unsigned long rax, rdx;
    unsigned aux;
    __asm__ __volatile__ ("rdtscp\nlfence" : "=a" (rax), "=d" (rdx), "=c" (aux) : :);
    return (rdx << 32) + rax;
}


int main(int argc, char *argv[]) { 
    if (argc < 3) {
        fprintf(stderr, "usage: ./client hostname port [iter=5]\n");
        exit(1);
    }

    int port = atoi(argv[2]);

    printf("Starting client\n");
    int sockfd = 0, valread;
    struct sockaddr_in serv_addr;
    char* send_buf = (char*)malloc(BUF_SIZE);
    memset(send_buf, '1', BUF_SIZE+1);
    char* recv_buf = (char*)malloc(BUF_SIZE);
    memset(recv_buf, '2', BUF_SIZE+1);

    int iteration = 5;

    if (argc == 4) {
        iteration = atoi(argv[3]);
    }

    unsigned long start, end, records = 0;
    double results = 0;

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
        int n = send(sockfd, send_buf, BUF_SIZE, 0);
        end = rdtscp();
        printf("send %d\n", n);
        records += (end - start);
    }
    
    results = (BUF_SIZE * (double)iteration) / records;

    printf("bandwidth: %f bytes/cycle, %f MB/sec\n", results, results * (2.6e9 / (1 << 20)));

    close(sockfd);

    return 0; 
} 