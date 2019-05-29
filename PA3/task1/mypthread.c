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

Edge edges[1000];

int edge_count = 0;

int cyclic();
int _cyclic(long start, long end);
long find(long start);
void mremove(long start, long end);
Thread find_thread(long tid);
void draw(Thread thread,long mid);
void add_edge(long start, long end);

int
pthread_mutex_lock(pthread_mutex_t *mutex) {

	void *(*lockp)(pthread_mutex_t *mutex);
	lockp = dlsym(RTLD_NEXT, "pthread_mutex_lock");

	long tid = pthread_self();
	long mid = (long) mutex;

	Thread thread = find_thread(tid);
	draw(thread,mid);
	

	if(cyclic()) {
                fputs("DEAD LOCK!\n",stderr);
                exit(1);
        }
	lockp(mutex);

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

int cyclic() {
	int sum = 0, i;
	for(i = 0; i<edge_count; i++)
		sum += _cyclic(edges[i].start,edges[i].end);
	return sum;
} 
int _cyclic(long start, long end) {
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
void mremove(long start, long end) {
	int i;
	for(i = 0; i<edge_count; i++) {
		if(edges[i].start == start && edges[i].end == end) {
		
			int j;
			for(j = i+1; j<edge_count; j++) {
				edges[j-1] = edges[j];
			}
			edge_count--;
			return;
		}
	}

}
Thread find_thread(long tid) {
	int i;
	for(i = 0; i<thread_count; i++) {
		if(threads[i].id == tid)
			return threads[i];
	}
	Thread t = {(long)pthread_self(),0,0x0};
	threads[thread_count] = t;
	thread_count++;
	return t;
}
void draw(Thread thread,long mid) {
	fprintf(stderr,"draw %ld, %d, %d\n",thread.id,thread.mutex_count,edge_count);
	int i;
	for(i = 0; i<thread.mutex_count; i++) { // make edges
		int j;
		int check = 0;
        	for(j = 0; j<edge_count; j++) {
			Edge edge = edges[j];
		
			long start = thread.mutexs[i];
			long end = mid;
			
                	if(edge.start == start && edge.end == end) {
				check = 1;
                        	break;
			}
        	}
		if(check) continue;
		add_edge(thread.mutexs[i],mid);
	}
	thread.mutexs[thread.mutex_count] = mid;
	thread.mutex_count++;
}
void add_edge(long start, long end) {
	fprintf(stderr,"add edge\n");
	Edge edge = {start, end};
	edges[edge_count] = edge;
	edge_count++;
}
