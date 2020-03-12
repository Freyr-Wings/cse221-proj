#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

#define ITERS 10000000

static inline unsigned long rdtscp()
{
    unsigned long rax, rdx;
    unsigned aux;
    asm volatile ("rdtscp\nlfence" : "=a" (rax), "=d" (rdx), "=c" (aux) : :);
    return (rdx << 32) + rax;
}


int main() {
    uid_t p;
    unsigned long sum = 0;
    for (long i = 0; i < ITERS; i++) {
	unsigned long t = rdtscp();
	sum += rdtscp() - t;
    }
    printf("%.4lf\n", sum / (double)ITERS);
    return 0;
}
