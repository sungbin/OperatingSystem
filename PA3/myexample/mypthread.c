#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>

int
pthread_mutex_lock(pthread_mutex_t *mutex) {

	void *(*lockp)(pthread_mutex_t *mutex);
	lockp = dlsym(RTLD_NEXT, "pthread_mutex_lock");

	char buf[500];
	sprintf(buf,"my lock! %d\n",mutex);	
	fputs(buf,stderr);

	lockp(mutex);

}

int
pthread_mutex_unlock(pthread_mutex_t *mutex) {

	void *(*unlockp)(pthread_mutex_t *mutex);
	unlockp = dlsym(RTLD_NEXT, "pthread_mutex_unlock");

	fputs("my unlock!!\n",stderr);
	unlockp(mutex);
}