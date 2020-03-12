#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

#define ITERS 10000000

static inline unsigned long rdtscp()
{
    unsigned long rax, rdx;
    unsigned aux;
    asm volatile ("rdtscp\n" : "=a" (rax), "=d" (rdx), "=c" (aux) : :);
    return (rdx << 32) + rax;
}

int main() {
    uid_t p;
    unsigned long t = rdtscp();
    for (long i = 0; i < ITERS; i++) {
        p = getpid();
    }
    printf("%.4lf\n", (rdtscp() - t) / (double)ITERS);
    return 0;
}
