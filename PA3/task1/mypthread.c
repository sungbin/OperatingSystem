#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>

typedef struct _Edge {
	long start;
	long  end;
} Edge;

Edge edges[1000];
int count = 0;

int cyclic(long start, long end);
long find(long start);
void reverse(long start, long end);
void mremove(long start, long end);

int
pthread_mutex_lock(pthread_mutex_t *mutex) {

	void *(*lockp)(pthread_mutex_t *mutex);
	lockp = dlsym(RTLD_NEXT, "pthread_mutex_lock");

	long tid = pthread_self();
	long mid = (long) mutex;

//	char buf[500];
//	sprintf(buf,"\nnew: tid:%ld, mid:%ld\n\n",tid,mid);
//	fputs(buf,stderr);
	
	Edge edge;
	edge.start = tid;
	edge.end = mid;
	edges[count] = edge;
	count++;

	if(cyclic(edge.start,edge.end)) {
		fputs("DEAD LOCK!\n",stderr);
		exit(1);
	}
/*
	sprintf(buf,"\nLCOK!\n");
	fputs(buf,stderr);
*/	

	lockp(mutex);

	reverse(edge.start,edge.end);
	if(cyclic(edge.end,edge.start)) {
		fputs("DEAD LOCK!\n",stderr);
		exit(1);
	}
}

int
pthread_mutex_unlock(pthread_mutex_t *mutex) {

	void *(*unlockp)(pthread_mutex_t *mutex);
	unlockp = dlsym(RTLD_NEXT, "pthread_mutex_unlock");

	long tid = pthread_self();
	long mid = (long) mutex;

	mremove(mid,tid); //revered data
	unlockp(mutex);
}


int cyclic(long start, long end) {
/* for check
	for(int i = 0; i<count; i++) {
		char buf[50];
		sprintf(buf,"start:%ld, end:%ld\n",edges[i].start,edges[i].end);
		fputs(buf,stderr);
	}
*/
	long in = start;
	long temp = end;
	while(temp != -1 && temp != in) {
		temp = find(temp);
	}
	if(temp == in)
		return 1; //cyclic
	else
		return 0; //no cyclle

}
long find(long start) {
	int i;
	for(i = 0; i<count; i++) {
		if(edges[i].start == start)
			return edges[i].end;
	}
	return -1; //NULL
}
void reverse(long start, long end) {
	int i;
	for(int i = 0; i<count; i++) {
		if(edges[i].start == start && edges[i].end == end) {
			edges[i].start = end;
			edges[i].end = start;
			return;
		}
	}

}
void mremove(long start, long end) {
	int i;
	for(i = 0; i<count; i++) {
		if(edges[i].start == start && edges[i].end == end) {
		
			int j;
			for(j = i+1; j<count; j++) {
				edges[j-1] = edges[j];
			}
			count--;
			return;
		}
	}

}
