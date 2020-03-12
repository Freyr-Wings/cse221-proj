#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define BLKSIZE (1LL << 12)

unsigned long s[10] = {1UL << 24, 1UL << 25, 1UL << 26, 1UL << 27, 1UL << 28, 1UL << 29, 1UL << 30, 1UL << 31, 1UL << 32, 1UL << 33};
char *names[10] = {"t-16MB","t-32MB","t-64MB","t-128MB","t-256MB","t-512MB", "t-1GB", "t-2GB", "t-4GB", "t-8GB"};

static inline unsigned long rdtscp()
{
    unsigned long rax, rdx;
    unsigned aux;
    asm volatile ("rdtscp\nlfence" : "=a" (rax), "=d" (rdx), "=c" (aux) : :);
    return (rdx << 32) + rax;
}

unsigned randint(unsigned a) {
    return (unsigned)(rand() / 2147483648.0 * a);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: %s fileid iter\n", argv[0]);
        return 1;
    }
    int i = atoi(argv[1]);
    int iter = atoi(argv[2]);
    unsigned long size = s[i];   
    int fd = open(names[i], O_RDONLY);
    char *buf = (char *)malloc(BLKSIZE);
    
    unsigned long curPoint = 0;
    if (iter == 0) {
        unsigned long start = rdtscp();
        while (1) {
            // unsigned long s = rdtscp();
            int r = read(fd, buf, BLKSIZE);
            // printf("%lu, Access time = %.4lf ms\n", curPoint >> 12, ((rdtscp() - s) / 2.4e6));
            curPoint += BLKSIZE;
            if (curPoint >= size) {
                break;
            }
        }
        unsigned long end = rdtscp();
        printf("Seq: Access time = %.4lf ms\n", ((end - start) / 2.4e6) / (size / BLKSIZE));
    } else {
        unsigned long start = rdtscp();
        srand(time(NULL));
        unsigned nblocks = size >> 12;
        for (int j = 0; j < iter; j++) {
            lseek(fd, randint(nblocks) << 12, SEEK_SET);
            int r = read(fd, buf, BLKSIZE);
        }
        unsigned long end = rdtscp();
        printf("Rnd: Access time = %.4lf ms\n", ((end - start) / 2.4e6) / iter);
    }
    
    close(fd);
}