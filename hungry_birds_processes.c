/*
 * hungry birds - an example of multi-process application using the shared 
 *                memory model. A producer/consumer problem.
 *
 * author:        Paolo Rovelli <paolo.rovelli@ericsson.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define NUM_BIRDS 7
#define NUM_WORMS 13

#define SHM_NAME "hungry-birds"
#define SEM_NAME_LOCK "hungry-birds-lock"
#define SEM_NAME_EMPTY "hungry-birds-empty"
#define SEM_NAME_ANY "hungry-birds-any"

static int shm;
static int *worms;
static sem_t *empty, *any, *lock;

void *bird_parent(void *arg);
void *bird_child(void *arg);

static void sigchld_handler(int signal)
{
	pid_t pid;
	int status;
    
	/* handle child termination */
	do {
		pid = waitpid(-1, &status, WNOHANG);
	} while (pid > 0);
}

static void sigterm_handler(int signal)
{
	int retval;
	
	/* remove everything */
	retval = shm_unlink(SHM_NAME) || munmap(worms, sizeof(*worms)) ||
		sem_unlink(SEM_NAME_EMPTY) ||
		sem_unlink(SEM_NAME_ANY) ||
		sem_unlink(SEM_NAME_LOCK);
        
        exit(retval ? EXIT_FAILURE : EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	int i;
	pid_t pid, cid[NUM_BIRDS];

	/* Initialize shared memory */
	shm = shm_open(SHM_NAME, O_CREAT | O_RDWR, 644);
	if (shm == -1)
		goto err1;
	if (ftruncate(shm, sizeof(*worms)) == -1)
        	goto err2;       		
    	worms = mmap(NULL, sizeof(*worms), PROT_READ | PROT_WRITE,
    		MAP_SHARED, shm, 0);
	if (worms == MAP_FAILED)
        	goto err2;            	
	*worms = NUM_WORMS;
	printf("\n + intitial dish %d worms\n\n", *worms);
	
	/* Initialize shared semaphores */
	empty = sem_open(SEM_NAME_EMPTY, O_CREAT | O_RDWR, 644, 0);
	if (empty == SEM_FAILED)
		goto err3;
	any = sem_open(SEM_NAME_ANY, O_CREAT | O_RDWR, 644, NUM_WORMS);
	if (any == SEM_FAILED)
		goto err4;
	lock = sem_open(SEM_NAME_LOCK, O_CREAT | O_RDWR, 644, 1);
	if (lock == SEM_FAILED)
		goto err5;

	/* Register signal handlers */
	signal(SIGINT, sigterm_handler);
	signal(SIGTERM, sigterm_handler);
	signal(SIGCHLD, sigchld_handler);

	/* Process creation */
	pid = getpid();
	for (i = 0; i < NUM_BIRDS; i++) { 
		switch (cid[i] = fork()) {
		case -1: /* error */
			goto err6;
		case 0: /* childs */
			bird_child((void *)i);
		default: /* parent */
			break;
		}
	}
	bird_parent((void *)0) ;
    
err6:
	while(--i)
		kill(cid[i], SIGTERM);
	sem_unlink(SEM_NAME_LOCK);
err5:
	sem_unlink(SEM_NAME_ANY);
err4:
	sem_unlink(SEM_NAME_EMPTY);
err3:
	munmap(worms, sizeof(*worms));
err2:
	shm_unlink(SHM_NAME);
err1:
	exit(EXIT_FAILURE);
}

void *bird_parent(void *arg)
{
	int i;

	while (1) {
		sem_wait(empty);
		sem_wait(lock);
		(*worms) = NUM_WORMS;
		printf(" + bird parent brings %d worms\n\n", *worms);
		sem_post(lock);
		for (i = 0; i < NUM_WORMS; i++)
			 sem_post(any);
	}
}

void *bird_child(void *arg)
{
	while (1) {
		sleep(rand() % ((10*NUM_BIRDS)/NUM_WORMS));
		sem_wait(any);
		sem_wait(lock);
		(*worms)--;
		if(*worms != 0) {
			printf(" - bird child %d eats (remain worms %d)\n",
				(int)arg, *worms);
			sem_post(lock);
		} else {
			printf(" - bird child %d eats (remain worms %d)"
				" and screams\n\n", (int)arg, *worms);
			sem_post(lock);
			sem_post(empty);
		}
	}
}

