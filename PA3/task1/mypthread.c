#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>

typedef struct _Edge {
	int start;
	int end;
} Edge;

Edge edges[1000];
int count = 0;

int cyclic(int start, int end);
int find(int start);

int
pthread_mutex_lock(pthread_mutex_t *mutex) {

	void *(*lockp)(pthread_mutex_t *mutex);
	lockp = dlsym(RTLD_NEXT, "pthread_mutex_lock");

	int tid = pthread_self();
	int mid = (int) mutex;
	
	
	Edge edge;
	edge.start = tid;
	edge.end = mid;
	edges[count] = edge;
	count++;
	
	cyclic

	lockp(mutex);

}

int
pthread_mutex_unlock(pthread_mutex_t *mutex) {

	void *(*unlockp)(pthread_mutex_t *mutex);
	unlockp = dlsym(RTLD_NEXT, "pthread_mutex_unlock");

	fputs("my unlock!!\n",stderr);
	unlockp(mutex);
}


int cyclic(int start, int end) {

}
int find(int start) {

}
