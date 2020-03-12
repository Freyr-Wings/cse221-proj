#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define BLKSIZE (1LL << 12)
#define SIZE (1LL << 26)

char *buf[128];
int fds[128];
pthread_t threads[128];
double times[128];

static inline unsigned long rdtscp()
{
    unsigned long rax, rdx;
    unsigned aux;
    asm volatile ("rdtscp\nlfence" : "=a" (rax), "=d" (rdx), "=c" (aux) : :);
    return (rdx << 32) + rax;
}

void *readdata(void *args) {
    int id = (int)args;
    unsigned long start = rdtscp();
    for (int i = 0; i < SIZE / BLKSIZE; i++) {
        read(fds[id], buf[id], BLKSIZE);
    }
    unsigned long end = rdtscp();
    times[id] = (end - start) / 2.6e6 / (SIZE / BLKSIZE);
    printf("Thread %d, access time = %.4lfms\n", id, times[id]);
    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s nthreads\n", argv[0]);
        return 1;
    }
    
    int nthreads = atoi(argv[1]);   

    for (int i = 0; i < nthreads; i++) {
        buf[i] = (char *)malloc(BLKSIZE);
        char filename[5];
        sprintf(filename, "t%d", i);
        fds[i] = open(filename, O_RDONLY);
    }

    for (int i = 0; i < nthreads; i++) {
        pthread_create(&threads[i], NULL, readdata, (void *)i);
    }

    for (int i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    double sum = 0;
    for (int i = 0; i < nthreads; i++) {
        sum += times[i];
    }

    printf("Average access time = %.4lfms\n", sum / nthreads);
    return 0;
}