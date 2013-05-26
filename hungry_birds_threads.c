/*
 * hungry birds - an example of multi-thread application using the shared memory
 *                model. A producer/consumer problem.
 *
 * author:        Paolo Rovelli <paolo.rovelli@ericsson.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_BIRDS 7
#define NUM_WORMS 13

#define SEM_SHARED 1

static int worms;
static sem_t empty, any;
static pthread_mutex_t lock;

void *parent_bird(void *arg)
{
	int i;

	while (1) {
		sem_wait(&empty);
		pthread_mutex_lock(&lock);
		worms = NUM_WORMS;
		printf(" + parent bird brings %d worms\n\n", worms);
		pthread_mutex_unlock(&lock);
		for (i = 0; i < NUM_WORMS; i++)
			 sem_post(&any);
	}
}

void *baby_bird(void *arg)
{
	while (1) {
		usleep(rand() % ((1000000*NUM_BIRDS)/NUM_WORMS + 1));
		sem_wait(&any);
		pthread_mutex_lock(&lock);
		worms--;
		if(worms != 0) {
			printf(" - baby bird %d eats (dish: %d worms)\n",
				(int)arg, worms);
			pthread_mutex_unlock(&lock);
		} else {
			printf(" - baby bird %d eats (dish: %d worms)"
				" and screams\n\n", (int)arg, worms);
			pthread_mutex_unlock(&lock);
			sem_post(&empty);
		}
	}
}

int main(int argc, char *argv[])
{
	int i;
	pthread_attr_t attr;
	pthread_t parent_tid;
	pthread_t baby_tid[NUM_WORMS];

	worms = NUM_WORMS;
	printf("\n + intitial dish: %d worms\n\n", worms);
	sem_init(&empty, SEM_SHARED, 0);
	sem_init(&any, SEM_SHARED, NUM_WORMS);
	pthread_mutex_init(&lock, NULL);

	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	pthread_create(&parent_tid, &attr, parent_bird, (void *) 0);
	for (i = 0; i < NUM_BIRDS; i++)
		pthread_create(&baby_tid[i], &attr, baby_bird, (void *) i);

	pthread_join(parent_tid, NULL);
	for (i = 0; i < NUM_BIRDS; i++)
		pthread_join(baby_tid[i], NULL);

	pthread_exit(NULL);
}

