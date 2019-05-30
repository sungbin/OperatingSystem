#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t mutex[3] ;

void 
noise()
{
	usleep(rand() % 1000) ;
}

void *
thread(void *arg) 
{
		pthread_mutex_lock(&mutex[0]);	 noise() ;
		pthread_mutex_lock(&mutex[2]); noise() ;
		pthread_mutex_lock(&mutex[1]); noise() ;
                pthread_mutex_unlock(&mutex[1]); noise() ;
		pthread_mutex_unlock(&mutex[2]); noise() ;
		pthread_mutex_unlock(&mutex[0]); noise() ;

		return NULL;
}

int
main(int argc, char *argv[])
{
	pthread_t tid;
	srand(time(0x0)) ;

	pthread_create(&tid, NULL, thread, NULL);

	pthread_mutex_lock(&mutex[0]); noise() ; 
	pthread_mutex_lock(&mutex[1]);	noise() ;
	pthread_mutex_lock(&mutex[2]);  noise() ; 
	pthread_mutex_unlock(&mutex[2]); noise() ;
	pthread_mutex_unlock(&mutex[1]); noise() ;
	pthread_mutex_unlock(&mutex[0]); noise() ;
	
	pthread_join(tid, NULL);
	return 0;
}
