#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>

static __thread int n_malloc = 0 ;

int
pthread_mutex_lock(pthread_mutex_t *mutex) {

	void *(*lockp)(pthread_mutex_t *mutex);
	lockp = dlsym(RTLD_NEXT, "pthread_mutex_lock");

	
	n_malloc += 1 ;

	if (n_malloc == 1) {
		int i ;
		void * arr[10] ;
		char ** stack ;
		char buf[10000] = "";
		char temp[100];

		sprintf(temp, "mutex(%d)\nthread(%d)\n", (int) mutex,pthread_self()) ;
		strcat(buf,temp);

		size_t sz = backtrace(arr, 10) ;
		stack = backtrace_symbols(arr, sz) ;

		for (i = 0 ; i < sz ; i++) {
			sprintf(temp, "%s\n", stack[i]) ;
			strcat(buf,temp);
		}

		FILE *f;
		f=fopen("dmonitor.trace","a");
		fputs(buf,stderr);
	}
    	n_malloc -= 1;
	

	lockp(mutex);

}

int
pthread_mutex_unlock(pthread_mutex_t *mutex) {

	void *(*unlockp)(pthread_mutex_t *mutex);
	unlockp = dlsym(RTLD_NEXT, "pthread_mutex_unlock");

	n_malloc += 1 ;

	if (n_malloc == 1) {
		char buf[500];
        	sprintf(buf,"Unlock:%d\n",(int)mutex);
        	fputs(buf,stderr);
	}

	n_malloc -= 1;

	unlockp(mutex);
}
