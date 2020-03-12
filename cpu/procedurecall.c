#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define ITERS 100000000

static inline unsigned long rdtscp()
{
    unsigned long rax, rdx;
    unsigned aux;
    asm volatile ("rdtscp\nlfence" : "=a" (rax), "=d" (rdx), "=c" (aux) : :);
    return (rdx << 32) + rax;
}


void __attribute__ ((noinline)) foo0() {
    return;
}

void __attribute__ ((noinline)) foo1(unsigned a0) {
    return;
}

void __attribute__ ((noinline)) foo2(unsigned a0, unsigned a1) {
    return;
}

void __attribute__ ((noinline)) foo3(unsigned a0, unsigned a1, unsigned a2) {
    return;
}

void __attribute__ ((noinline)) foo4(unsigned a0, unsigned a1, unsigned a2, unsigned a3) {
    return;
}

void __attribute__ ((noinline)) foo5(unsigned a0, unsigned a1, unsigned a2, unsigned a3, unsigned a4) {
    return;
}

void __attribute__ ((noinline)) foo6(unsigned a0, unsigned a1, unsigned a2, unsigned a3, unsigned a4, unsigned a5) {
    return;
}

void __attribute__ ((noinline)) foo7(unsigned a0, unsigned a1, unsigned a2, unsigned a3, unsigned a4, unsigned a5, unsigned a6) {
    return;
}

void __attribute__ ((noinline)) foo8(unsigned a0, unsigned a1, unsigned a2, unsigned a3, unsigned a4, unsigned a5, unsigned a6, unsigned a7) {
    return;
}

int main(int argc, char **argv) {
    unsigned long t;
    if (argc <= 1) {
        printf("invalid argument\n");
        return 1;
    }
    switch (atoi(argv[1])) {
        case -1:
            t = rdtscp();
            for (long i = 0; i < ITERS; i++) {}
            printf("%.4lf\n", (rdtscp() - t) / (double)ITERS);
            break;
        case 0:
            t = rdtscp();
            for (long i = 0; i < ITERS; i++)
                foo0();
            printf("%.4lf\n", (rdtscp() - t) / (double)ITERS);
            break;
        case 1:
            t = rdtscp();
            for (long i = 0; i < ITERS; i++)
                foo1(0);
            printf("%.4lf\n", (rdtscp() - t) / (double)ITERS); 
            break;
        case 2:
            t = rdtscp();
            for (long i = 0; i < ITERS; i++)
                foo2(0, 1);
            printf("%.4lf\n", (rdtscp() - t) / (double)ITERS);
            break;
        case 3:
            t = rdtscp();
            for (long i = 0; i < ITERS; i++)
                foo3(0, 1, 2);
            printf("%.4lf\n", (rdtscp() - t) / (double)ITERS);
            break;
        case 4:
            t = rdtscp();
            for (long i = 0; i < ITERS; i++)
                foo4(0, 1, 2, 3);
            printf("%.4lf\n", (rdtscp() - t) / (double)ITERS);
            break;
        case 5:
            t = rdtscp();
            for (long i = 0; i < ITERS; i++)
                foo5(0, 1, 2, 3, 4);
            printf("%.4lf\n", (rdtscp() - t) / (double)ITERS);
            break;
        case 6:
            t = rdtscp();
            for (long i = 0; i < ITERS; i++)
                foo6(0, 1, 2, 3, 4, 5);
            printf("%.4lf\n", (rdtscp() - t) / (double)ITERS);
            break;
        case 7:
            t = rdtscp();
            for (long i = 0; i < ITERS; i++)
                foo7(0, 1, 2, 3, 4, 5, 6);
            printf("%.4lf\n", (rdtscp() - t) / (double)ITERS);
            break;
        case 8:
            t = rdtscp();
            for (long i = 0; i < ITERS; i++)
                foo8(0, 1, 2, 3, 4, 5, 6, 7);
            printf("%.4lf\n", (rdtscp() - t) / (double)ITERS);  
            break;
    }        
    // printf("%ld\n", i);
    return 0;
}
