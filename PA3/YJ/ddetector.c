#define _GNU_SOURCE
#define MAX_NODE 100
#define MAX_EDGE 5000
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>

typedef struct node {
    pthread_mutex_t * m ;
} Node ;

typedef struct edge {
    Node in ;
    Node out ;
} Edge ;

typedef struct graph {
    int n_count ;
    int e_count ;
    Node nodes[MAX_NODE] ;
    Edge edges[MAX_EDGE] ;
} Graph ;

Graph G = {0, 0, {0x0, }, {0x0, }} ;

int is_cycle() ;
int is_dup(int out, int in) ;
int is_already(pthread_mutex_t * mutex) ;
void insert_graph(pthread_mutex_t * mutex) ;
void remove_graph(pthread_mutex_t * mutex) ;

int pthread_mutex_lock(pthread_mutex_t * mutex) {
    void *(* m_lock)(pthread_mutex_t * mutex) ;
	m_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock") ;

    int idx = is_already(mutex) ;
    if (idx != -1) {
        idx = G.n_count ;
        
        insert_graph(mutex) ;
    }

    m_lock(mutex) ;

    if (G.n_count > 1) {
        for (int i = 0; i < G.n_count; i++) {
            if (!is_dup(i, idx)) {
                Edge edge ;
                edge.in = G.nodes[idx] ;
                edge.out = G.nodes[i] ;
                G.edges[G.e_count] = edge ;
                G.e_count++ ;
                if (is_cycle()) {
                    fputs("!!🐞 DEAD LOCK 🐞!!\n", stderr) ;
		            exit(1);
                }
            }
        }
    }
}

int pthread_mutex_unlock(pthread_mutex_t * mutex) {
    void *(* m_unlock)(pthread_mutex_t * mutex) ;
    m_unlock = dlsym(RTLD_NEXT, "pthread_mutex_unlock") ;

    remove_graph(mutex) ;

    m_unlock(mutex) ;
}

int is_already(pthread_mutex_t * mutex) {
    for (int i = 0; i < G.n_count; i++)
        if (G.nodes[i].m == mutex) return i ;
    return -1 ;
}

int is_dup(int out, int in) {
    if (out == in) return 1 ;
    for (int i = 0; i < G.e_count; i++) {
        if (G.edges[i].out.m == G.nodes[out].m && G.edges[i].in.m == G.nodes[in].m)
            return 1 ;
    }
    return 0 ;
}

int is_cycle() {
    pthread_mutex_t * in = G.edges[G.e_count - 1].in.m ;
    pthread_mutex_t * out = G.edges[G.e_count - 1].out.m ;

    while(in != NULL && in != out) {
        pthread_mutex_t * temp = in ;
        for (int i = 0; i < G.e_count - 1; i++) {
            if (in == G.edges[i].out.m) {
                in = G.edges[i].out.m ;
                break ;
            }
        }
        if (temp == in) in = NULL ;
    }

    if (in == out) return 1 ;
    else return 0 ;
}

void insert_graph(pthread_mutex_t * mutex) {
    Node node ;

    node.m = mutex ;
    G.nodes[G.n_count] = node ;
    G.n_count++ ;
}

void remove_graph(pthread_mutex_t * mutex) {
    for (int i = 0; i < G.n_count; i++) {
        if (G.nodes[i].m == mutex) {
            for (int j = i + 1; j < G.n_count; j++)
                G.nodes[j - 1] = G.nodes[j] ;
            G.n_count-- ;
            break ;
        }
    }

    for (int i = 0; i < G.e_count; i++) {
        if (G.edges[i].in.m == mutex || G.edges[i].out.m == mutex) {
            for (int j = i + 1; j < G.e_count; j++)
                G.edges[j - 1] = G.edges[j] ;
            G.e_count-- ;
            i-- ;
        }
    }
}