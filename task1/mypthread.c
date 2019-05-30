#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>

typedef struct _Thread {
	long id;
	int mutex_count; //holding mutex count
	long mutexs[100];
} Thread;

Thread threads[10];
int thread_count = 0;

typedef struct _Edge {
	long start;
	long  end;
} Edge;

Edge edges[55];

int edge_count = 0;

int cyclic();
int _cyclic(long start, long end);
long find(long start);
void mremove(long m,Thread* thread);
Thread* find_thread(long tid);
void draw(Thread* thread,long mid);
void add_edge(long start, long end);

int
pthread_mutex_lock(pthread_mutex_t *mutex) {

	void *(*lockp)(pthread_mutex_t *mutex);
	lockp = dlsym(RTLD_NEXT, "pthread_mutex_lock");

	long tid = pthread_self();
	long mid = (long) mutex;

	Thread* thread = find_thread(tid);
	draw(thread,mid);
	

	if(cyclic()) {
                fputs("DEAD LOCK!\n",stderr);
                exit(1);
        }
	lockp(mutex);
	thread->mutexs[thread->mutex_count] = mid;
        thread->mutex_count++;
}

int
pthread_mutex_unlock(pthread_mutex_t *mutex) {

	void *(*unlockp)(pthread_mutex_t *mutex);
	unlockp = dlsym(RTLD_NEXT, "pthread_mutex_unlock");

	long tid = pthread_self();
	long mid = (long) mutex;

	Thread* thread = find_thread(tid);
	mremove(mid,thread); //revered data
	unlockp(mutex);
}

int cyclic() {
	int sum = 0, i;
	for(i = 0; i<edge_count; i++)
		sum += _cyclic(edges[i].start,edges[i].end);
	return sum;
} 
int _cyclic(long start, long end) {
	long in = start;
	long temp = end;
	while(temp != -1 && temp != in) {
		temp = find(temp);
	}
	if(temp == in)
		return 1; //_cyclic
	else
		return 0; //no cyclle

}
long find(long start) {
	int i;
	for(i = 0; i<edge_count; i++) {
		if(edges[i].start == start)
			return edges[i].end;
	}
	return -1; //NULL
}
void mremove(long m,Thread* thread) {
	int i;
	for(i = 0; i<thread->mutex_count;i++) {
		if(thread->mutexs[i] == m) {
			int j;
                        for(j = i+1; j<thread->mutex_count; j++) {
                                thread->mutexs[j-1] = thread->mutexs[j];
                        }
                        i--;
                        thread->mutex_count--;
		}

	}
	for(i = 0; i<edge_count; i++) {
		if(edges[i].start == m || edges[i].end == m) {
//			fprintf(stderr,"remove: %ld -> %ld\n",edges[i].start,edges[i].end);
			int j;
			for(j = i+1; j<edge_count; j++) {
				edges[j-1] = edges[j];
			}
			i--;
			edge_count--;
		}
	}
}
Thread* find_thread(long tid) {
	int i;
	for(i = 0; i<thread_count; i++) {
		if(threads[i].id == tid)
			return &threads[i];
	}
	Thread t = {(long)pthread_self(),0,0x0};
	threads[thread_count] = t;
	thread_count++;
	return &threads[thread_count-1];
}
void draw(Thread* thread,long mid) {
	int i;
	for(i = 0; i<thread->mutex_count; i++) { // make edges
		int j;
		int check = 0;
        	for(j = 0; j<edge_count; j++) {
			Edge edge = edges[j];
		
			long start = thread->mutexs[i];
			long end = mid;
			
                	if(edge.start == start && edge.end == end) {
				check = 1;
                        	break;
			}
        	}
		if(check || thread->mutexs[i] == mid) continue;
		add_edge(thread->mutexs[i],mid);
	}
}
void add_edge(long start, long end) {
//	fprintf(stderr,"add edge: %ld -> %ld, %\d\n",start,end,pthread_self());
	Edge edge = {start, end};
	edges[edge_count] = edge;
	edge_count++;
}
