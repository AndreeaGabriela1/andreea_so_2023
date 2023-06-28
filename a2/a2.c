#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include "a2_helper.h"
#include <stdlib.h>
#include <unistd.h>

pthread_barrier_t barrier;

void *thread_func(void *thread_id)
{
    int tid = *(int *)thread_id;
	if(tid == 2)
	{
		info(BEGIN, 2, tid);
		pthread_barrier_wait(&barrier);
    	info(END, 2, tid);
	}
	else
	{
		pthread_barrier_wait(&barrier);
        info(BEGIN, 2, tid);
        info(END, 2, tid);
	}
    pthread_exit(NULL);
}

int main()
{
    init();
	//incepe P1
    info(BEGIN, 1, 0);
	pid_t P2, P3, P4, P5, P6, P7;
	P2 = fork();
	if(P2 == -1)
	{
		perror("Could not create process P2");
		return -1;
	}
	else if(P2 == 0)
	{
		pthread_t threads[4]; // Array pentru a stoca ID-urile threadurilor
    	int thread_ids[4] = {1, 2, 3, 4}; // ID-uri pentru fiecare thread
		//incepe P2
		info(BEGIN, 2, 0);
		pthread_barrier_init(&barrier, NULL, 4);
    	// Crearea threadurilor
		for(int i = 0; i < 4; i++)
		{
			pthread_create(&threads[i], NULL, thread_func, (void *)&thread_ids[i]);
		}
		for(int i = 0; i < 4; i++)
		{
			pthread_join(threads[i], NULL);
		}
		pthread_barrier_destroy(&barrier);
		//se termina P2
		info(END, 2, 0);
	}
	else
	{
		//suntem in P1
		P3 = fork();
		if(P3 == -1)
		{
			perror("Could not create process P3");
			return -1;
		}
		else if(P3 == 0)
		{
			//incepe P3
			info(BEGIN, 3, 0);
			P4 = fork();
			if(P4 == -1)
			{
				perror("Could not create process P4");
				return -1;
			}
			else if(P4 == 0)
			{
				//incepe P4
				info(BEGIN, 4, 0);
				P6 = fork();
				if(P6 == -1)
				{
					perror("Could not create process P7");
					return -1;
				}
				else if(P6 == 0)
				{
					//incepe P6
					info(BEGIN, 6, 0);
					//se termina P6
					info(END, 6, 0);
				}
				else
				{
					//suntem in P4
					waitpid(P6, NULL, 0);
					//se termina P4
					info(END, 4, 0);
				}
			}
			else
			{
				///suntem in P3
				P5 = fork();
				if(P5 == -1)
				{
					perror("Could not create process P5");
					return -1;
				}
				else if(P5 == 0)
				{
					//incepe P5
					info(BEGIN, 5, 0);
					//se termina P5
					info(END, 5, 0);
				}
				else 
				{
					//suntem in P3
					P7 = fork();
					if(P7 == -1)
					{
						perror("Could not create process P6");
						return -1;
					}
					else if(P7 == 0)
					{
						//incepe P7
						info(BEGIN, 7, 0);
						//se termina P7
						info(END, 7, 0);
					}
					else 
					{
						waitpid(P4, NULL, 0);
						waitpid(P5, NULL, 0);
						waitpid(P7, NULL, 0);
						//se termina P3
						info(END, 3, 0);
					}
				}
			}
		}
		else
		{
			//suntem in P1
			waitpid(P2, NULL, 0);
			waitpid(P3, NULL, 0);
			//se termina P1
			info(END, 1, 0);
		}
	}
    return 0;
}
