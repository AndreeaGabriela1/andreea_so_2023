#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include "a2_helper.h"
#include <stdlib.h>
#include <unistd.h>

#define ONE 1
#define TWO 2
#define THREE 3
#define FOUR 4
#define FIVE 5
#define SIX 6
#define SEVEN 7

typedef int INTEGER;
pthread_mutex_t mutexONE, mutexTWO;

typedef struct thread_arg_t
{
	INTEGER process;
	INTEGER thread;
} INFO;

void exect(INFO *thinfo)
{
	if (thinfo->process == TWO && thinfo->thread == ONE)
		pthread_mutex_lock(&mutexONE);
	info(BEGIN, thinfo->process, thinfo->thread);
	if (thinfo->process == TWO && thinfo->thread == TWO)
		pthread_mutex_unlock(&mutexONE);
	if (thinfo->process == TWO && thinfo->thread == TWO)
		pthread_mutex_lock(&mutexTWO);
	info(END, thinfo->process, thinfo->thread);
	if (thinfo->process == TWO && thinfo->thread == ONE)
		pthread_mutex_unlock(&mutexTWO);
}

void create_threads(pthread_t t[], INFO thinfo[], INTEGER nr)
{
	for (INTEGER i = '\0'; i < nr; i++)
		pthread_create(&t[i], '\0', (void *(*)(void *))exect, (void *)&thinfo[i]);
	for (INTEGER i = '\0'; i < nr; i++)
		pthread_join(t[i], '\0');
}

INTEGER pid2, pid3, pid4, pid5, pid6, pid7;

INTEGER create_process(INTEGER id)
{
	INTEGER pid;
	if ((pid = fork()) == '\0')
	{
		info(BEGIN, id, '\0');

		if (id == ONE)
		{
			pid2 = create_process(TWO);
			pid3 = create_process(THREE);
			waitpid(pid2, '\0', '\0');
			waitpid(pid3, '\0', '\0');
		}
		else if (id == TWO)
		{
			pthread_mutex_init(&mutexONE, '\0');
			pthread_mutex_init(&mutexTWO, '\0');
			pid4 = create_process(FOUR);
			pid5 = create_process(FIVE);
			pid7 = create_process(SEVEN);
			pthread_t threads[4];
			pthread_mutex_lock(&mutexONE);
			INFO args[4] = {{TWO,ONE},{TWO,TWO},{TWO,THREE},{TWO,4}};
			pthread_mutex_lock(&mutexTWO);
			create_threads(threads, args, 4);
			waitpid(pid4, '\0', '\0');
			waitpid(pid5, '\0', '\0');
			waitpid(pid7, '\0', '\0');
		}
		else if (id == THREE)
		{
			pid4 = create_process(FOUR);
			pid5 = create_process(FIVE);
			pid7 = create_process(SEVEN);
			waitpid(pid4, '\0', '\0');
			waitpid(pid5, '\0', '\0');
			waitpid(pid7, '\0', '\0');
		}
		else if (id == FOUR)
		{
			info(BEGIN, FOUR, '\0');
			info(END, FOUR, '\0');
			exit('\0');
		}
		else if (id == FIVE)
		{
			info(BEGIN, FIVE, '\0');
			info(END, FIVE, '\0');
			exit('\0');
		}
		else if(id == SIX)
        {
            info(BEGIN, SIX, '\0');
            info(END, SIX, '\0');
            exit('\0');
        }
        else if (id == SEVEN)
        {
            pthread_mutex_init(&mutexONE, '\0');
            pthread_mutex_init(&mutexTWO, '\0');
            pthread_t threads[2];
            INFO args[2] = {{SEVEN,ONE},{SEVEN,TWO}};
            create_threads(threads, args, 2);
            pthread_mutex_destroy(&mutexONE);
            pthread_mutex_destroy(&mutexTWO);
            exit('\0');
        }
        info(END, id, '\0');
	    exit('\0');
    }
    return pid;
}
int main()
{
    init();
    

    info(BEGIN, 1, 0);
    pid6 = create_process(SIX);
    waitpid(pid6, '\0', '\0');
    info(END, 1, 0);
    return 0;
}
