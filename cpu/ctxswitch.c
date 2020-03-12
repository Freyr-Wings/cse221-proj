#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

#include <linux/futex.h>

static inline unsigned long rdtscp()
{
    unsigned long rax, rdx;
    asm volatile ("rdtscp\n" : "=a" (rax), "=d" (rdx) : :);
    return (rdx << 32) + rax;
}

int main(int argc, char **argv) {
    if (argc != 2 && argc != 4) {
        printf("Usage: %s iter [CPUA] [CPUB]\n", argv[0]);
        return 1;
    }
    const int iterations = atoi(argv[1]);
    struct timespec ts;
    const int shm_id = shmget(IPC_PRIVATE, sizeof (int), IPC_CREAT | 0666);
    const pid_t other = fork();
    if (argc == 4) {
        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(other ? atoi(argv[2]) : atoi(argv[3]), &mask);
        int result = sched_setaffinity(0, sizeof(mask), &mask);
    }
    int* futex = shmat(shm_id, NULL, 0);
    *futex = 0xA;
    if (other == 0) {
        for (int i = 0; i < iterations; i++) {
            sched_yield();
            while (syscall(SYS_futex, futex, FUTEX_WAIT, 0xA, NULL, NULL, 42)) {
                sched_yield();
            }
            *futex = 0xB;
            while (!syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 42)) {
                sched_yield();
            }
        }
        return 0;
    }

    unsigned long start = rdtscp();
    for (int i = 0; i < iterations; i++) {
        *futex = 0xA;
        while (!syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 42)) {
            sched_yield();
        }
        sched_yield();
        while (syscall(SYS_futex, futex, FUTEX_WAIT, 0xB, NULL, NULL, 42)) {
            sched_yield();
        }
    }
    unsigned long delta = rdtscp() - start;

    const int nswitches = iterations << 2;
    printf("Average time of context switches is %.2lfns\n", (delta / (float) nswitches / 2.6));
    wait(futex);
    return 0;
}
