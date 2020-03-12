#include<stdio.h>
#include<unistd.h>
#include<stdint.h>
#include<stdlib.h>
#include<pthread.h>
#define LOOPN 100000

uint64_t rdtsc() {
  uint64_t a, d;
  __asm__ volatile ("rdtscp" : "=a" (a), "=d" (d));
  return (d<<32) | a;
}

void* foo(){
	return NULL;
}

int main(){
	uint64_t start, end, sum = 0;
	double ovh;
	pthread_t pt;

	
	for(long i = 0; i < LOOPN; i++){
start = rdtsc();
		pthread_create(&pt, NULL, foo, NULL);
end = rdtsc();
		pthread_join(pt, 0);
// printf(" %ld, execution time is %lu clock cycles\n", i, end - start);
sum += end - start;

	}
	printf(" execution time is %.4lf clock cycles\n", sum / (double)(LOOPN));
	

	return 0;
}

