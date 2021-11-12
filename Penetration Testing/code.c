#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
sem_t mutex;
int64_t result;
struct parameters { int32_t threadId, start, end; };
void usage() { }
void displayError() { }
bool validateInput(char *string) {
	int32_t len = 0, i = 0;
	// ASSUME: string != NULL
	len = strlen(string); // COMMENT: unsafe cast from size_t to int32_t
	// WARNING: If input was a long string >4GB, the check len > 6 would
	// overflow, causing a security bug and the invariant [1] on line 23 
	// would no longer hold. However, since the argument is a string
	// from the stack, we know it will be small and the length will fit
	// safely in 32 bits.
	if (len > 6)
		return false;
	// MUST: len <= 6 <= strlen(string) [1]
	for (i = 0; i < len; i++) {
		// ASSUME: 0 <= i <= strlen(string)
		if (!(string[i] >= '0' && string[i] <= '9'))
			return false;
	}
	// MUST: string is numeric
	return true;
}
void* calcSum(void *param) {
	int32_t i = 0;
	int64_t sum = 0;
	// ASSUME: param != NULL
	struct parameters realParam = *((struct parameters *) param);
	// ASSUME sum += i fits in 64 bits (OK)
	for (i = realParam.start; i < realParam.end; i++) sum += i;
	// ASSUME: mutex initialized
	sem_wait(&mutex); // MUST: mutex locked
	result += sum; // MUST: result protected by mutex
	sem_post(&mutex); // ASSUME: mutex locked
	// COMMENT: return NULL here to avoid compiler warning
	return NULL;
}
int main(int argc, char *argv[]) {
	int32_t threadAmount, increment, i, err;
	pthread_t *threads;
	struct parameters *param;
	if (argc != 3) {
		displayError();
		return 1;
	}
	// MUST: argc == 3
	// MUST: argv[1] != NULL
	// MUST: argv[2] != NULL
	// ASSUME: argc >= 3
	if (!(validateInput(argv[1]) && validateInput(argv[2]))) {
		displayError();
		return 1;
	}
	// MUST: argv[1] and argv[2] is numeric
	// MUST: strlen(argv[1]) <= 6
	// MUST: strlen(argv[2]) <= 6
	threadAmount = atoi(argv[1]), increment = atoi(argv[2]), i = 0;
	if (threadAmount > 1000 || threadAmount < 1 || increment < 1 || increment > 999999) {
		displayError();
		return 1;
	}
	// MUST: 0 < threadAmount < 1000
	// MUST 0 < increment < 1000000
	// ASSUME: threadAmount is small
	threads = malloc(sizeof(pthread_t) * threadAmount);
	// MUST: length(threads) == threadAmount
	sem_init(&mutex, 0, 1);
	// MUST: mutex initialized
	for (i = 0; i < threadAmount; i++) {
		param = malloc(sizeof(struct parameters));
		// ASSUME: param != NULL
		// WARNING: malloc can return NULL if memory is unavailable
		param->threadId = i + 1; // ASSUME: i+1 fits in 32 bits (OK)
		param->start = i * increment; // ASSUME: i*increment fits in 32 bits (OK)
		param->end = (i + 1) * increment; // (i+1)*increment fits in 32 bits (OK)
		// ASSUME: mutex initialized
		err = pthread_create(threads + i, NULL, calcSum, param); 
		if (err) return 1; // WARNING: Memory leak
	}
	// ASSUME: 0 <= i <= length(threads)
	for(i = 0; i < threadAmount; i++) pthread_join(threads[i], NULL);
	// ASSUME: mutex initialized
	sem_destroy(&mutex);
	printf("The result is %" PRId64 ".\n", result);
	// WARNING: Memory leak
	return 0;
}
