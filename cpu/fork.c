#include <unistd.h>
#include <stdio.h>

#define ITERS 100000

static inline unsigned long rdtscp()
{
    unsigned long rax, rdx;
    unsigned aux;
    asm volatile ("rdtscp\n" : "=a" (rax), "=d" (rdx), "=c" (aux) : :);
    return (rdx << 32) + rax;
}

int main() {
    unsigned long t = rdtscp();  
    for (long i = 0; i < ITERS; i++) {        
        if (fork() == 0) {
            return 0;
        }
    }
    printf("%.4lf\n", (rdtscp() - t) / (double)ITERS);
    return 0;
}
