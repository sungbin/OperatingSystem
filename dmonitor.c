#define _GNU_SOURCE
#define MAX_NODE 100
#define MAX_EDGE 5000
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>

int pthread_mutex_lock(pthread_mutex_t * mutex) {

	static __thread int n_malloc = 0 ;
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

//		sprintf(temp, "Stack trace\n") ;
//		strcat(buf,temp);
		
//		sprintf(temp, "============\n") ;
//		strcat(buf,temp);
		
		for (i = 0 ; i < sz ; i++) {
//			sprintf(temp, "[%d] %s\n", i, stack[i]) ;
			sprintf(temp, "%s\n", stack[i]) ;
			strcat(buf,temp);
		}

//		sprintf(temp, "============\n\n") ;
//		strcat(buf,temp);
		FILE *f;
		f=fopen("dmonitor.trace","a");
		fputs(buf,f);
	}
    n_malloc -= 1;

    void *(* m_lock)(pthread_mutex_t * mutex) ;
	m_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock") ;
    m_lock(mutex) ;
}

