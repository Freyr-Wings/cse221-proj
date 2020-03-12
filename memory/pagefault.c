#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

static unsigned long inline rdtsc_start() {
    unsigned lo, hi;
    __asm__ __volatile__("cpuid\n\t"
                         "rdtsc\n\t"
                         "mov %%edx, %0\n\t"
                         "mov %%eax, %1\n\t"
    : "=r" (hi), "=r" (lo)
    :: "%rax", "%rbx", "%rcx", "%rdx");
    return (((unsigned long)hi << 32) | lo);
}

static unsigned long inline rdtsc_end() {
    unsigned lo, hi;
    __asm__ __volatile__("rdtscp\n\t"
                         "mov %%edx, %0\n\t"
                         "mov %%eax, %1\n\t"
                         "cpuid\n\t"
    : "=r" (hi), "=r" (lo)
    :: "%rax", "%rbx", "%rcx", "%rdx");
    return (((unsigned long)hi << 32) | lo);
}

static inline unsigned long rdtscp(){
    unsigned long rax, rdx;
    unsigned aux;
    __asm__ __volatile__ ("rdtscp\nlfence" : "=a" (rax), "=d" (rdx), "=c" (aux) : :);
    return (rdx << 32) + rax;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr,"usage: ./pagefault file_path [iter=1]\n");
        exit(1);
    }

    int iteration = 1;
    if (argc == 3) {
        iteration = atoi(argv[2]);
    }

    int fd;

    struct stat file_stat;
    
    unsigned long start, end;
    double cycle_page_fault, cycle_mmap;
    char *addr = NULL;

    if ((fd = open(argv[1], O_RDWR)) < 0) {
        exit(1);
    }
    if (fstat(fd, &file_stat) < 0) {
        perror("fstat");
        exit(1);
    }

    int file_size = file_stat.st_size;
    int page_size = 1024*getpagesize();
    int num_pages = file_size / page_size;

    start = rdtscp();
    for (int i = 0; i < iteration; ++i) {

        addr = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED) {
            perror("mmap");
            exit(1);
        }

        int visit = 0;
        for (int j = num_pages-1; j >= 0; j--) {
            visit += *(addr + page_size * j);
        }

        if (msync(addr, file_stat.st_size, MS_INVALIDATE) != 0) {
            perror("msync");
            exit(1);
        }

        munmap(addr, file_size);
    }
    end = rdtscp();
    cycle_page_fault = (end - start) / (double) iteration;

    start = rdtscp();
    for (int i = 0; i < iteration; ++i) {
        addr = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED) {
            perror("mmap");
            exit(1);
        }

        int visit = 0;
        for (int j = num_pages-1; j >= 0; j--) {
            visit += j;
        }

        if (msync(addr, file_stat.st_size, MS_INVALIDATE) != 0) {
            perror("msync");
            exit(1);
        }
        munmap(addr, file_size);
    }
    end = rdtscp();
    cycle_mmap = (end - start) / (double) iteration;

    printf("Page fault: %f cycles, %f ms\n", (cycle_page_fault - cycle_mmap) / num_pages, (cycle_page_fault - cycle_mmap) / num_pages / 2.6e6);
    close(fd);
    return 0;
}