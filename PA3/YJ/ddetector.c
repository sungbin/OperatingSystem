#define _GNU_SOURCE
#define MAX_NODE 100
#define MAX_EDGE 5000
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>

typedef struct node {
    unsigned long m ;
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

int is_cycle(int idx) ;
int is_dup(int out, int in) ;
int is_already(unsigned long m_id) ;
int insert_node(unsigned long m_id) ;
int insert_edge() ;
void remove_graph(unsigned long m_id) ;

int pthread_mutex_lock(pthread_mutex_t * mutex) {
    void *(* m_lock)(pthread_mutex_t * mutex) ;
	m_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock") ;

    // pthread_t tid = pthread_self();
    unsigned long m_id = (unsigned long)mutex ;
    
    int idx = is_already(m_id) ;
    if (idx == -1)
        idx = insert_node(m_id) ;
    int count = G.n_count ;

    // char buf[500] ;
    // sprintf(buf, "\nt: %ld, mutex: %ld, idx : %d, count : %d\n", (unsigned long)tid, m_id, idx, count) ;
    // fputs(buf, stderr) ;

    // sprintf(buf, "t: %ld, <Nodes In Graph>\n", (unsigned long)tid) ;
    // fputs(buf, stderr) ;
    // for (int i = 0 ; i < count; i++) {
    //     sprintf(buf, "t: %ld, i : %d, mutex: %ld\n", (unsigned long)tid, i, G.nodes[i].m) ;
    //     fputs(buf, stderr) ;
    // }
    
    if (count > 1) {
        int idx2 ;
        for (int i = 0; i < count; i++) {
            if (!is_dup(i, idx)) {
                // sprintf(buf, "\nt: %ld, create edge\n", (unsigned long)tid) ;
                // fputs(buf, stderr) ;
                
                idx2 = insert_edge(idx, i) ;
                // sprintf(buf, "\nt: %ld, e_idx : %d\n", (unsigned long)tid, idx2) ;
                // fputs(buf, stderr) ;
                if (is_cycle(idx2)) {
                    fputs("🐞 DEAD LOCK 🐞\n", stderr) ;
		            exit(1);
                }
            } else {
                // sprintf(buf, "t: %ld, DUP!!!!!!!\n", (unsigned long)tid) ;
                // fputs(buf, stderr) ;
            }
        }
    }

    m_lock(mutex) ;
}

int pthread_mutex_unlock(pthread_mutex_t * mutex) {
    void *(* m_unlock)(pthread_mutex_t * mutex) ;
    m_unlock = dlsym(RTLD_NEXT, "pthread_mutex_unlock") ;

    // pthread_t tid = pthread_self();
    unsigned long m_id = (unsigned long)mutex ;

    // char buf[500] ;
    // sprintf(buf, "\n*Unlock* t: %ld, mutex: %ld\n", (unsigned long)tid, m_id) ;
    // fputs(buf, stderr) ;

    remove_graph(m_id) ;

    m_unlock(mutex) ;
}

int is_already(unsigned long m_id) {
    for (int i = 0; i < G.n_count; i++)
        if (G.nodes[i].m == m_id) return i ;
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

int is_cycle(int idx) {
    unsigned long in = G.edges[idx].in.m ;
    unsigned long out = G.edges[idx].out.m ;

    int flag = 0 ;
    while(flag == 0 && in != out) {
        unsigned long temp = in ;
        for (int i = 0; i < G.e_count; i++) {
            if (in == G.edges[i].out.m) {
                in = G.edges[i].in.m ;
                break ;
            }
        }
        if (temp == in) flag = 1 ;
    }

    if (flag == 0) return 1 ;
    else return 0 ;
}

int insert_node(unsigned long m_id) {
    Node node ;
    int idx = G.n_count ;

    node.m = m_id ;
    G.nodes[idx] = node ;
    G.n_count++ ;

    return idx ;
}

int insert_edge(int in, int out) {
    Edge edge ;
    int idx = G.e_count ;

    edge.in = G.nodes[in] ;
    edge.out = G.nodes[out] ;
    G.edges[idx] = edge ;
    G.e_count++ ;

    return idx ;
}

void remove_graph(unsigned long m_id) {
    // fputs("remove graph", stderr) ;

    for (int i = 0; i < G.n_count; i++) {
        if (G.nodes[i].m == m_id) {
            for (int j = i + 1; j < G.n_count; j++)
                G.nodes[j - 1] = G.nodes[j] ;
            G.n_count-- ;
            break ;
        }
    }

    for (int i = 0; i < G.e_count; i++) {
        if (G.edges[i].in.m == m_id || G.edges[i].out.m == m_id) {
            for (int j = i + 1; j < G.e_count; j++)
                G.edges[j - 1] = G.edges[j] ;
            G.e_count-- ;
            i-- ;
        }
    }
}