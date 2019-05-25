#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>

typedef struct Edge {   // 연결 리스트의 노드 구조체
        struct Edge *next;    // 다음 노드의 주소를 저장할 포인터
	int start;
	int end;
} Edge, *pEdge;

pEdge head = NULL;

int
pthread_mutex_lock(pthread_mutex_t *mutex) {

	void *(*lockp)(pthread_mutex_t *mutex);
	lockp = dlsym(RTLD_NEXT, "pthread_mutex_lock");


	int tid = pthread_self();
	int mid = (int) mutex;

	make_edge(tid,mid);
	if(cyclic(tid,mid)) {
		char buf[500];
		sprintf(buf,"DEAD LOCK\n");	
		fputs(buf,stderr);
		exit(1);
	}
	lockp(mutex);
	revere_edge(tid,mid);
	if(cyclic()) {
		char buf[500];
		sprintf(buf,"DEAD LOCK\n");	
		fputs(buf,stderr);
		exit(1);
	}
}

int
pthread_mutex_unlock(pthread_mutex_t *mutex) {

	int tid = pthread_self();
	int mid = (int) mutex;
	void *(*unlockp)(pthread_mutex_t *mutex);
	unlockp = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
	
	delete_edge(mid,tid); // because reversed mutex_lock

	fputs("my unlock!!\n",stderr);
	unlockp(mutex);
}

int cyclic(int start, int end) {
	pEdge node;
	find_node(node, start);

	pEdge next_node;
	find_node(next_node, end);
	while(next_node != NULL) {
		if(node == next_node)
			return 1;
	}
	return 0;
}

void find_node(pEdge node,int start) {
	pEdge pre;
	for(pre = head; pre != NULL; pre = pre->next) {
		if(pre->start == start) {
			break;
		}
	}
	node = pre;
}

void
delete_edge(int start,int end) {
	pEdge target_edge;
	pEdge pre = head;
	while(pre->next != NULL) {
		if(pre->next->start == start && pre->next->end == end) {
			target_edge = pre->next;
			break;
		}
		pre = pre->next;
	}
	if(pre == head) {
		head = NULL;
	} else {
		if(target_edge->next != NULL) {
			pre->next = tartget_edge->next;
		} else {
			pre->next = NULL;
		}
		free(target_edge);
	}
}

void
add_edge(int tid,int mid) {
	pEdge new_edge;
	init_edge(new_edge,tid,mid);
	if(head == NULL) {
		head = new_edge;
	} else {
		pEdge pre;
		for(pre = head; pre != NULL; pre = pre->next) {  }
		pre->next = new_edge;
	}
}
void
init_edge(pEdge edge,int tid, int mid) {
	edge = (pEdge)malloc(sizeof(Edge));
	edge->next = NULL;
	edge->start = tid;
	edge->end = mid;
}
void
revere_edge(int start,int end) {
	pEdge target_edge;
	pEdge pre;
	for(pre = head; pre != NULL; pre = pre->next) {
		if(pre->start == start && pre->end == end) {
			pre->end = start;
			pre->start = end;
			break;
		}
	}
}
