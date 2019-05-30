#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>

typedef struct thread {
	long id ;
	int mutex_count ;
	long mutexs[100] ;
} Thread ;

int thread_count = 0 ;
Thread threads[10] ;

typedef struct edge {
	long start ;
	long end ;
} Edge ;

int e_count = 0 ;
Edge edges[55] ;

int cyclic() ;
long find(long start) ;
Thread * find_thread(long tid) ;
int _cyclic(long start, long end) ;
void draw(Thread* thread,long mid) ;
void add_edge(long start, long end) ;
void mremove(long m,Thread* thread) ;

int pthread_mutex_lock(pthread_mutex_t *mutex) {

	void * (* m_lock)(pthread_mutex_t * mutex) ;
	m_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock") ;

	long tid = pthread_self() ;
	long mid = (long) mutex ;

	Thread * thread = find_thread(tid) ;
	draw(thread, mid) ;
	

	if (cyclic()) {
                fputs("🐞 DEAD LOCK 🐞\n", stderr) ;
                // exit(1) ;
        }
	
	m_lock(mutex) ;
	thread->mutexs[thread->mutex_count] = mid ;
        thread->mutex_count++ ;
}

int pthread_mutex_unlock(pthread_mutex_t * mutex) {

	void * (* m_unlock)(pthread_mutex_t * mutex) ;
	m_unlock = dlsym(RTLD_NEXT, "pthread_mutex_unlock") ;

	long tid = pthread_self() ;
	long mid = (long) mutex ;

	Thread * thread = find_thread(tid) ;
	mremove(mid, thread) ;
	m_unlock(mutex) ;
}

int cyclic() {
	int sum = 0 ;

	for (int i = 0; i<e_count; i++)
		sum += _cyclic(edges[i].start, edges[i].end) ;
	
	return sum ;
}

int _cyclic(long start, long end) {
	long in = start ;
	long temp = end ;
	
	while (temp != -1 && temp != in)
		temp = find(temp) ;
	
	if (temp == in) return 1 ;
	else return 0 ;
}

long find(long start) {
	for (int i = 0; i<e_count; i++)
		if(edges[i].start == start) return edges[i].end ;
	return -1 ;
}

void mremove(long m, Thread * thread) {
	for (int i = 0; i < thread->mutex_count; i++) {
		if (thread->mutexs[i] == m) {
			for(int j = i + 1; j < thread->mutex_count; j++)
                                thread->mutexs[j-1] = thread->mutexs[j] ;
                        i-- ;
                        thread->mutex_count-- ;
		}
	}
	for (int i = 0; i < e_count; i++) {
		if (edges[i].start == m || edges[i].end == m) {
//			fprintf(stderr,"remove: %ld -> %ld\n",edges[i].start,edges[i].end);
			for (int j = i+1; j<e_count; j++)
				edges[j-1] = edges[j] ;
			i-- ;
			e_count-- ;
		}
	}
}

Thread * find_thread(long tid) {
	for (int i = 0; i < thread_count; i++)
		if (threads[i].id == tid) return &threads[i] ;

	Thread t = { (long)pthread_self(), 0, 0x0 } ;
	threads[thread_count] = t ;
	thread_count++ ;

	return &threads[thread_count-1] ;
}

void draw(Thread * thread, long mid) {
	for (int i = 0; i < thread->mutex_count; i++) {
		int check = 0 ;
		for(int j = 0; j < e_count; j++) {
			Edge edge = edges[j] ;
		
			long start = thread->mutexs[i] ;
			long end = mid ;
			
			if (edge.start == start && edge.end == end) {
				check = 1 ;
                        	break ;
			}
        	}
		if (check || thread->mutexs[i] == mid) continue ;
		add_edge(thread->mutexs[i], mid) ;
	}
}

void add_edge(long start, long end) {
//	fprintf(stderr,"add edge: %ld -> %ld, %\d\n",start,end,pthread_self());
	Edge edge = {start, end} ;
	edges[e_count] = edge ;
	e_count++ ;
}
