#include<stdio.h>
#include<unistd.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<signal.h>
#define LOOPN 100

void* test_read(void* args);
void* test_write(void* args);
void* test_read_halfjmp(void*args);
// void test_read_halfjmp(int*buf, int*last, int n);
void* test_write_halfjmp(void*args);

static inline unsigned long rdtscp(){    unsigned long rax, rdx;    unsigned aux;    asm volatile ("rdtscp\nlfence" : "=a" (rax), "=d" (rdx), "=c" (aux) : :);    return (rdx << 32) + rax;}

struct arg_struct{
	int* buf;
	long long* record;
	long n;
	int id;
};

static void handler(int signum)
{
	pthread_exit(NULL);
}

int main(int argc, char **argv){
	if(argc != 3){
		printf("usage: ./bandwidth [thread_number] [read/write] \n");
		exit(0);
	}
	int tn = atoi(argv[1]);
	long long n = 16LL * 1024 / 4 / tn * 1024 * 1024 ;

	int** buf = (int **)malloc(tn * sizeof(int *));
	for (int i = 0; i < tn; i++) {
		buf[i] = (int*)valloc(n * sizeof(int));
		memset(buf[i], 1, n * sizeof(int));
	}
	
	pthread_t* pt = (pthread_t*)malloc(tn * sizeof(pthread_t));
	long long* record = (long long*)malloc(tn * sizeof(long long));
	memset((void*)record,0,tn * sizeof(long long));

	struct arg_struct* args = (struct arg_struct*)malloc(tn * sizeof(struct arg_struct));
	for(int i = 0 ; i < tn; i++){
		args[i].buf = buf[i];
		args[i].record = record;
		args[i].n = n;
		args[i].id = i;
	}

	signal(SIGUSR1, handler);
	uint64_t start, end;
	start = rdtscp();
	if(strcmp(argv[2],"read") == 0){
		for(int i = 0; i < tn; i ++){
			pthread_create(&pt[i], NULL, test_read, (void*)&args[i]);
		}
	}else if(strcmp(argv[2],"write") == 0){
		for(int i = 0; i < tn; i ++){
			pthread_create(&pt[i], NULL, test_write, (void*)&args[i]);
		}
	}
	sleep(10);
	for(int i = 0; i < tn; i ++){
		pthread_kill(pt[i],SIGUSR1);
	}
	for(int i = 0; i < tn; i ++){
		pthread_join(pt[i],NULL);
	}
	end = rdtscp();
	double time = (end-start) / 2600000000.0;
	double size = 0;
	for(int i = 0 ; i < tn; i ++){
		size += record[i];
	}
	printf("Time is %lf s,  Bandwidth is %.6lf MB/s\n", time, size * sizeof(int) / 1000 / 1000 / time);

	free(buf);
	return 0;
}

void *test_read(void* args){
#define FUNC(i) p[i]+

	static sigset_t mask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);

	int* buf = ((struct arg_struct*)args)->buf;
	long long* record = ((struct arg_struct*)args)->record;
	long n = ((struct arg_struct*)args)->n;
	int id = ((struct arg_struct*)args)->id;
	int* last = buf + n - 128;

	// uint64_t start, end;
	int sum = 0;
	// start = rdtscp();
	while(1){
		int* p = buf;
		while(p <= last){
			sum += FUNC(0) FUNC(1) FUNC(2) FUNC(3) FUNC(4) FUNC(5) FUNC(6)
		FUNC(7) FUNC(8) FUNC(9) FUNC(10) FUNC(11) FUNC(12)
		FUNC(13) FUNC(14) FUNC(15) FUNC(16) FUNC(17) FUNC(18)
		FUNC(19) FUNC(20) FUNC(21) FUNC(22) FUNC(23) FUNC(24)
		FUNC(25) FUNC(26) FUNC(27) FUNC(28) FUNC(29) FUNC(30)
		FUNC(31) FUNC(32) FUNC(33) FUNC(34) FUNC(35) FUNC(36)
		FUNC(37) FUNC(38) FUNC(39) FUNC(40) FUNC(41) FUNC(42)
		FUNC(43) FUNC(44) FUNC(45) FUNC(46) FUNC(47) FUNC(48)
		FUNC(49) FUNC(50) FUNC(51) FUNC(52) FUNC(53) FUNC(54)
		FUNC(55) FUNC(56) FUNC(57) FUNC(58) FUNC(59) FUNC(60)
		FUNC(61) FUNC(62) FUNC(63) FUNC(64) FUNC(65) FUNC(66)
		FUNC(67) FUNC(68) FUNC(69) FUNC(70) FUNC(71) FUNC(72)
		FUNC(73) FUNC(74) FUNC(75) FUNC(76) FUNC(77) FUNC(78)
		FUNC(79) FUNC(80) FUNC(81) FUNC(82) FUNC(83) FUNC(84)
		FUNC(85) FUNC(86) FUNC(87) FUNC(88) FUNC(89) FUNC(90)
		FUNC(91) FUNC(92) FUNC(93) FUNC(94) FUNC(95) FUNC(96)
		FUNC(97) FUNC(98) FUNC(99) FUNC(100) FUNC(101) FUNC(102)
		FUNC(103) FUNC(104) FUNC(105) FUNC(106) FUNC(107)
		FUNC(108) FUNC(109) FUNC(110) FUNC(111) FUNC(112)
		FUNC(113) FUNC(114) FUNC(115) FUNC(116) FUNC(117)
		FUNC(118) FUNC(119) FUNC(120) FUNC(121) FUNC(122)
		FUNC(123) FUNC(124) FUNC(125) FUNC(126) 
			p[127];
			p += 128;
			record[id] += 128;
		}
	}
	// end = rdtscp();
	// double ovh = 5.6+LOOPN*5.6*n/128;
	// double time = ((end-start-ovh)/(double)LOOPN) / 2600000000;
	// printf(" Bandwidth is %.6lf MB/s\n", n * sizeof(int) / 1000 / 1000 / time);
	return NULL;
#undef FUNC
}

void *test_write(void* args){
#define FUNC(i) p[i] = 1;

	static sigset_t mask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);

	int* buf = ((struct arg_struct*)args)->buf;
	long long* record = ((struct arg_struct*)args)->record;
	long n = ((struct arg_struct*)args)->n;
	int id = ((struct arg_struct*)args)->id;
	int* last = buf + n - 128;

	// uint64_t start, end;
	int sum = 0;
	// start = rdtscp();
	while(1){
		int* p = buf;
		while(p <= last){
			FUNC(0) FUNC(1) FUNC(2) FUNC(3) FUNC(4) FUNC(5) FUNC(6)
		FUNC(7) FUNC(8) FUNC(9) FUNC(10) FUNC(11) FUNC(12)
		FUNC(13) FUNC(14) FUNC(15) FUNC(16) FUNC(17) FUNC(18)
		FUNC(19) FUNC(20) FUNC(21) FUNC(22) FUNC(23) FUNC(24)
		FUNC(25) FUNC(26) FUNC(27) FUNC(28) FUNC(29) FUNC(30)
		FUNC(31) FUNC(32) FUNC(33) FUNC(34) FUNC(35) FUNC(36)
		FUNC(37) FUNC(38) FUNC(39) FUNC(40) FUNC(41) FUNC(42)
		FUNC(43) FUNC(44) FUNC(45) FUNC(46) FUNC(47) FUNC(48)
		FUNC(49) FUNC(50) FUNC(51) FUNC(52) FUNC(53) FUNC(54)
		FUNC(55) FUNC(56) FUNC(57) FUNC(58) FUNC(59) FUNC(60)
		FUNC(61) FUNC(62) FUNC(63) FUNC(64) FUNC(65) FUNC(66)
		FUNC(67) FUNC(68) FUNC(69) FUNC(70) FUNC(71) FUNC(72)
		FUNC(73) FUNC(74) FUNC(75) FUNC(76) FUNC(77) FUNC(78)
		FUNC(79) FUNC(80) FUNC(81) FUNC(82) FUNC(83) FUNC(84)
		FUNC(85) FUNC(86) FUNC(87) FUNC(88) FUNC(89) FUNC(90)
		FUNC(91) FUNC(92) FUNC(93) FUNC(94) FUNC(95) FUNC(96)
		FUNC(97) FUNC(98) FUNC(99) FUNC(100) FUNC(101) FUNC(102)
		FUNC(103) FUNC(104) FUNC(105) FUNC(106) FUNC(107)
		FUNC(108) FUNC(109) FUNC(110) FUNC(111) FUNC(112)
		FUNC(113) FUNC(114) FUNC(115) FUNC(116) FUNC(117)
		FUNC(118) FUNC(119) FUNC(120) FUNC(121) FUNC(122)
		FUNC(123) FUNC(124) FUNC(125) FUNC(126) FUNC(127);
			p += 128;
			record[id] += 128;
		}
	}
	// end = rdtscp();
	// double ovh = 5.6+LOOPN*5.6*n/128;
	// double time = (end-start-ovh)/(double)LOOPN / 2600000000;
	// printf(" Bandwidth is %.6lf MB/s\n", n * sizeof(int) / 1000 / 1000 / time);
	return NULL;
#undef FUNC
}
