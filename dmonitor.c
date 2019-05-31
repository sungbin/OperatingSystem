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

Thread * find_thread(long tid) ;
void draw(Thread* thread,long mid) ;
void add_edge(long start, long end, Thread * thread) ;
void mremove(long m,Thread* thread) ;

int pthread_mutex_lock(pthread_mutex_t *mutex) {

	void * (* m_lock)(pthread_mutex_t * mutex) ;
	m_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock") ;

	long tid = pthread_self() ;
	long mid = (long) mutex ;

	Thread * thread = find_thread(tid) ;
	draw(thread, mid) ;
	
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


void mremove(long m, Thread * thread) {
	for (int i = 0; i < thread->mutex_count; i++) {
		if (thread->mutexs[i] == m) {
			for(int j = i + 1; j < thread->mutex_count; j++)
                                thread->mutexs[j-1] = thread->mutexs[j] ;
                        i-- ;
                        thread->mutex_count-- ;
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
		if (thread->mutexs[i] == mid) continue ;
		add_edge(thread->mutexs[i], mid,thread) ;
	}
}

void add_edge(long start, long end, Thread * thread) {

	char buf[500] = "";
	char g[100] = "[";
	
	int i;
	for(i = 0; i<thread->mutex_count; i++) {

		char temp[50];
		sprintf(temp,"%d,",thread->mutexs[i]);
		strcat(g,temp);
	}
	g[strlen(g)-1] = '\0'; // remove: ,
	strcat(g,"]");

	sprintf(buf,"%d,0,%d,%s,0,%d\n",start,thread->id,g,end);

	FILE *f;
	f = fopen("dmonitor.trace","a");
	fputs(buf,f);
	fclose(f);
//	fputs(buf,stderr);

}
