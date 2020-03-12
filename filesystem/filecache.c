#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define BLKSIZE (1LL << 27)

unsigned long s[8] = {1UL << 30, 1UL << 31, 1UL << 32, 1UL << 33, 3 * (1UL << 32), 1UL << 34, 3 * (1UL << 33), 1UL << 35};
char *names[8] = {"t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8"};

static inline unsigned long rdtscp()
{
    unsigned long rax, rdx;
    unsigned aux;
    asm volatile ("rdtscp\nlfence" : "=a" (rax), "=d" (rdx), "=c" (aux) : :);
    return (rdx << 32) + rax;
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
    unsigned long start = 0;
    unsigned long curPoint = 0;
    for (int j = 0; j < iter; j++) {
        int r = read(fd, buf, BLKSIZE);
        curPoint += BLKSIZE;
        if (curPoint >= size) {
            if (start == 0) {
                start = rdtscp();
            }
            curPoint = 0;
            lseek(fd, 0, SEEK_SET);
        }
    }
    unsigned long end = rdtscp();
    printf("128MB Blocks = %d, Size = %.2lf, Speed = %.2lf MiB/s\n", iter, size / 1048576.0, (iter * BLKSIZE - size) / ((end - start) / 2.6e9) / 1048576.0);
    close(fd);
}