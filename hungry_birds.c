/*
 * hungry birds - an example of multi-thread application using the shared memory
 *                model. A producer/consumer problem.
 *
 * author:        Paolo Rovelli <paolo.rovelli@ericsson.com>
 *
 * to build:	  gcc -g -Wall -lpthread -o hungry_birds hungry_birds.c
 * to run:	  ./hungry_birds
 */

#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define SHARED 1
#define NUM_BIRDS 50
#define NUM_WORMS 100

void *bird_parent(void *arg);
void *bird_child(void *arg);

static int worms;
static sem_t empty, any;
static pthread_mutex_t lock;

int main(int argc, char *argv[])
{
	pthread_t pid, cid[NUM_WORMS];
	pthread_attr_t attr;
	int i;

	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

	worms = NUM_WORMS;
	printf("\n + intitial dish %d worms\n\n", worms);
	sem_init(&empty, SHARED, 0);
	sem_init(&any, SHARED, NUM_WORMS);
	pthread_mutex_init(&lock, NULL);

	pthread_create(&pid, &attr, bird_parent, (void *) 0);
	for (i = 0; i < NUM_BIRDS; i++)
		pthread_create(&cid[i], &attr, bird_child, (void *) i);

	pthread_join(pid, NULL);
	for (i = 0; i < NUM_BIRDS; i++)
		pthread_join(cid[i], NULL);

	pthread_exit(NULL);
}

void *bird_parent(void *arg)
{
	int i;

	while (1) {
		sem_wait(&empty);
		pthread_mutex_lock(&lock);
		worms = NUM_WORMS;
		printf(" + bird parent brings %d worms\n\n", worms);
		pthread_mutex_unlock(&lock);
		for (i = 0; i < NUM_WORMS; i++)
			 sem_post(&any);
	}
}

void *bird_child(void *arg)
{
	while (1) {
		sleep(rand()%10);
		sem_wait(&any);
		pthread_mutex_lock(&lock);
		worms--;
		if(worms != 0) {
			printf(" - bird child %d eats (remain worms %d)\n",
				(int)arg, worms);
			pthread_mutex_unlock(&lock);
		} else {
			printf(" - bird child %d eats (remain worms %d)"
				" and screams\n\n", (int)arg, worms);
			pthread_mutex_unlock(&lock);
			sem_post(&empty);
		}
	}
}
