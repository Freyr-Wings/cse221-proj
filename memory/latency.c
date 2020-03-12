#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#define STRIDE 256
#define PAGESIZE 4096

#define	ONE	p = (char **)*p;
#define	FIVE	ONE ONE ONE ONE ONE
#define	TEN	    FIVE FIVE
#define	FIFTY	TEN TEN TEN TEN TEN
#define	HUNDRED	FIFTY FIFTY

size_t length[27] = {512, 1 << 10, 2 << 10, 4 << 10, 8 << 10, 16 << 10, 32 << 10,
48 << 10, 64 << 10, 96 << 10, 128 << 10, 192 << 10, 256 << 10, 384 << 10, 512 << 10,
768 << 10, 1 << 20, 1536 << 10, 2 << 20, 3 << 20, 4 << 20, 6 << 20, 8 << 20, 12 << 20, 
16 << 20, 24 << 20, 32 << 20};

static volatile long dummy;
void use(long result) { 
    dummy += result; 
}

static inline unsigned long rdtscp()
{
    unsigned long rax, rdx;
    unsigned aux;
    asm volatile ("rdtscp\nlfence" : "=a" (rax), "=d" (rdx), "=c" (aux) : :);
    return (rdx << 32) + rax;
}

size_t* indice_initialize(size_t max, int scale) {
	size_t	i, j, len = 0;
	size_t*	words = (size_t*)malloc(max * sizeof(size_t));

	assert(words);

	bzero(words, max * sizeof(size_t));
	for (i = max >> 1; i != 0; i >>= 1) {
        len++;
    }

	for (i = 0; i < max; ++i) {
		for (j = 0; j < len; j++) {
			if (i & (1 << j)) {
				words[i] |= (1 << (len - j - 1));
			}
		}
		words[i] *= scale;
	}
	return words;
}

char *buf_initialize(size_t len)
{
	char *buf = (char*)malloc(len + 2 * PAGESIZE);
	assert(buf);

	if ((unsigned long)buf % PAGESIZE) {
		buf += PAGESIZE - (unsigned long)buf % PAGESIZE;
	}
	
    return buf;
}

static inline int randint(int n) {
    return (int)(rand() / RAND_MAX * n);
}

static inline void swap(size_t *a, size_t *b) {
    size_t t = *a;
    *a = *b;
    *b = t;
}

size_t *pages_initialize(size_t npages) {
    size_t *pages = (size_t *)malloc(npages * sizeof(size_t));
    for (int i = 0; i < npages; i++) {
        pages[i] = i;
    }
    for (int i = npages - 1; i >= 0; i--) {
        int j = randint(i);
        swap(&pages[j], &pages[i]);
    }
    return pages;
}

char *initialize(size_t len) {    
	char *buf = buf_initialize(len);    
	
    if (len % PAGESIZE) {
        size_t nwords = len / STRIDE;
        size_t *indice = indice_initialize(nwords, STRIDE);
        for (size_t i = 0; i < nwords - 1; ++i) {
            *(char **)&buf[indice[i]] = (char*)&buf[indice[i + 1]];
        }
        *(char **)&buf[indice[nwords - 1]] = buf;
    } else {
        size_t nwords = PAGESIZE / STRIDE, npages = len / PAGESIZE;
        size_t *indice = indice_initialize(nwords, STRIDE);
        size_t *pages = pages_initialize(npages);
        for (size_t i = 0; i < npages - 1; i++) {
            char *cpage = buf + pages[i] * PAGESIZE;
            char *npage = buf + pages[i + 1] * PAGESIZE;
            for (size_t j = 0; j < nwords; j++) {
                *(char **)&cpage[indice[(i + j) % nwords]] = (char*)&npage[indice[(i + j + 1) % nwords]];
            }
        }
        char *cpage = buf + pages[npages] * PAGESIZE;
        char *npage = buf + pages[0] * PAGESIZE;
        for (size_t j = 0; j < nwords; j++) {
            *(char **)&cpage[indice[(j + npages - 1) % nwords]] = (char*)&npage[indice[(j + 1) % nwords]];
        }
    }
	
    return buf;
}

void run(size_t iterations, char *buf) {
	register char **p = (char**)buf;

	while (iterations--) {
		HUNDRED;
	}

	use((long)p);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s iter\n", argv[0]);
        return 1;
    }
    
    int iters = atoi(argv[1]);
    for (int i = 0; i < 27; i++) {
        char *buf = initialize(length[i]);
        unsigned long t = rdtscp();
        run(iters, buf);
        printf("%lu %.4lf\n", length[i], (rdtscp() - t) / (iters * 100.0) / 2.6);
    }
    return 0;
}
