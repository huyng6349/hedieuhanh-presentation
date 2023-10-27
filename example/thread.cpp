#include <stdio.h>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

sem_t chopstick[5];

void* philosophers(void* n);

void eat(int n);

int main()
{
	int i, p[5] = {0, 1, 2, 3, 4};
	pthread_t T[5];
		for (i = 0; i < 5; i++)
			sem_init(&chopstick[i], 0, 1);
		for (i = 0; i < 5; i++)
		{
			p[i] = i;
			pthread_create(&T[i], NULL, philosophers, (void*)&p[i]);
		}
		for (i = 0; i < 5; i++)
		{
			pthread_join(T[i], NULL);
		}
}

void* philosophers(void* n) //normal
{
	int self = *(int*)n;
	int next = (self + 1) % 5;
	printf("\nPhilosopher %d wants to eat\n", self);
	printf("\nPhilosopher %d tries to pick up the left chopstick\n", self);
	sem_wait(&chopstick[self]);
	printf("\nPhilosopher %d picks up the left chopstick\n", self);
	printf("\nPhilosopher %d tries to pick up the right chopstick\n", self);
	sem_wait(&chopstick[next]);
	printf("\nPhilosopher %d picks up the right chopstick\n", self);
	eat(self);
	usleep(2000);
	printf("\nPhilosopher %d finished eating\n", self);
	sem_post(&chopstick[next]);
	printf("\nPhilosopher %d puts down the right chopstick\n", self);
	sem_post(&chopstick[self]);
	printf("\nPhilosopher %d puts down the left chopstick\n", next);
	return NULL;
}

//void* philosophers(void* n) //seft-next solution; 
//{
//	int self = *(int*)n;
//	int next = (self + 1) % 5;
//	int flag = self;
//	if (self > next)
//	{
//		int temp = self;
//		self = next;
//		next = temp;
//	}
//	printf("\nPhilosopher %d wants to eat\n", flag);
//	printf("\nPhilosopher %d tries to pick up the left chopstick\n", flag);
//	sem_wait(&chopstick[self]);
//	printf("\nPhilosopher %d picks up the left chopstick\n", flag);
//	printf("\nPhilosopher %d tries to pick up the right chopstick\n", flag);
//	sem_wait(&chopstick[next]);
//	printf("\nPhilosopher %d picks up the right chopstick\n", flag);
//	eat(flag);
//	printf("\nPhilosopher %d finished eating\n", flag);
//	/*Sleep(1);*/
//	sem_post(&chopstick[next]);
//	printf("\nPhilosopher %d puts down the right chopstick\n", flag);
//	sem_post(&chopstick[self]);
//	printf("\nPhilosopher %d puts down the left chopstick\n", flag);
//	return NULL;
//}

void eat(int n)
{
	printf("\nPhilosopher %d is eating\n", n);
	
}