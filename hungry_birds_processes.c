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

static struct shm {
	int worms;
} *shm;
static sem_t *empty, *any, *lock;

static void sigchld_handler(int signal)
{
	pid_t pid;
	int status;
    
	/* Handle child termination */
	do {
		pid = waitpid(-1, &status, WNOHANG);
	} while (pid > 0);
}

static void sigterm_handler(int signal)
{
	int retval;
	
	/* Remove everything */
	retval = shm_unlink(SHM_NAME) || munmap(shm, sizeof(*shm)) ||
		sem_unlink(SEM_NAME_EMPTY) ||
		sem_unlink(SEM_NAME_ANY) ||
		sem_unlink(SEM_NAME_LOCK);
        
        exit(retval ? EXIT_FAILURE : EXIT_SUCCESS);
}

void *parent_bird(void *arg)
{
	int i;

	setbuf(stdout, NULL);
	while (1) {
		sem_wait(empty);
		sem_wait(lock);
		shm->worms = NUM_WORMS;
		printf(" + parent bird brings %d worms\n\n", shm->worms);
		sem_post(lock);
		for (i = 0; i < NUM_WORMS; i++)
			 sem_post(any);
	}
}

void *baby_bird(void *arg)
{
	setbuf(stdout, NULL);
	while (1) {
		usleep(rand() % ((1000000*NUM_BIRDS)/NUM_WORMS + 1));
		sem_wait(any);
		sem_wait(lock);
		(shm->worms)--;
		if (shm->worms != 0) {
			printf(" - baby bird %d eats (dish: %d worms)\n",
				(int)arg, shm->worms);
			sem_post(lock);
		} else {
			printf(" - baby bird %d eats (dish: %d worms)"
				" and screams\n\n", (int)arg, shm->worms);
			sem_post(lock);
			sem_post(empty);
		}
	}
}

int main(int argc, char *argv[])
{
	int i, fd;
	pid_t parent_pid;
	pid_t baby_pid[NUM_BIRDS];

	/* Initialize shared memory */
	fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 644);
	if (fd == -1)
		goto err1;
	if (ftruncate(fd, sizeof(*shm)) == -1)
        	goto err2;       		
    	shm = mmap(NULL, sizeof(*shm), PROT_READ | PROT_WRITE,
    		MAP_SHARED, fd, 0);
	if (shm == MAP_FAILED)
        	goto err2;            	
	shm->worms = NUM_WORMS;
	printf("\n + intitial dish: %d worms\n\n", shm->worms);
	
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
	parent_pid = getpid();
	for (i = 0; i < NUM_BIRDS; i++) { 
		switch (baby_pid[i] = fork()) {
		case -1: /* error */
			goto err6;
		case 0: /* childs */
			baby_bird((void *)i);
		default: /* parent */
			break;
		}
	}
	parent_bird((void *)0) ;
    
err6:
	while(--i)
		kill(baby_pid[i], SIGTERM);
	sem_unlink(SEM_NAME_LOCK);
err5:
	sem_unlink(SEM_NAME_ANY);
err4:
	sem_unlink(SEM_NAME_EMPTY);
err3:
	munmap(shm, sizeof(*shm));
err2:
	shm_unlink(SHM_NAME);
err1:
	exit(EXIT_FAILURE);
}

