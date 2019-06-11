#include <unistd.h>
#include <stdio.h>
#include "smalloc.h" 

sm_container_ptr sm_first = 0x0 ;
sm_container_ptr sm_last = 0x0 ;
sm_container_ptr sm_unused_containers = 0x0 ;

void sm_container_split(sm_container_ptr hole, size_t size)
{
	sm_container_ptr remainder = hole->data + size ;

	remainder->data = ((void *)remainder) + sizeof(sm_container_t) ;
	remainder->dsize = hole->dsize - size - sizeof(sm_container_t) ;
	remainder->status = Unused ;
	remainder->next = hole->next ;
	hole->next = remainder ;

	if (hole == sm_last)
		sm_last = remainder ;
}

void * sm_retain_more_memory(int size)
{
	sm_container_ptr hole ;
	int pagesize = getpagesize() ;
	int n_pages = 0 ;

	n_pages = (sizeof(sm_container_t) + size + sizeof(sm_container_t)) / pagesize  + 1 ;
	hole = (sm_container_ptr) sbrk(n_pages * pagesize) ;
	if (hole == 0x0)
		return 0x0 ;

	hole->data = ((void *) hole) + sizeof(sm_container_t) ;
	hole->dsize = n_pages * getpagesize() - sizeof(sm_container_t) ;
	hole->status = Unused ;

	return hole ;
}

void * smalloc(size_t size) 
{
	sm_container_ptr hole = 0x0 ;

	sm_container_ptr itr = 0x0 ;
	for (itr = sm_unused_containers ; itr != 0x0 ; itr = itr->next_unused) {
		if (itr->status == Busy)
			continue ;

		if (size == itr->dsize) {
			// a hole of the exact size
			itr->status = Busy ;
			return itr->data ;
		}
		else if (size + sizeof(sm_container_t) < itr->dsize) {

			if(hole == 0x0)
				hole = itr ;
			else if(hole->dsize > itr->dsize)
				hole = itr ;
			
			fprintf(stderr,"traverse : %d!!\n",itr->dsize);
		}
	}

	if (hole == 0x0) {
		hole = sm_retain_more_memory(size) ;

		if (hole == 0x0)
			return 0x0 ;

		if (sm_first == 0x0) {
			// sm_unused_containers = hole;
			sm_first = hole ;
			sm_last = hole ;
			hole->next = 0x0 ;
			hole->next_unused = 0x0;
		}
		else {
			//for (itr = sm_unused_containers ; itr->next_unused != 0x0 ; itr = itr->next_unused) {   }
			//itr->next_unused = hole;

			sm_last->next = hole ;
			sm_last = hole ;
			hole->next = 0x0 ;
			hole->next_unused = 0x0;
		}
	}
	sm_container_split(hole, size) ;
	hole->dsize = size ;
	hole->status = Busy ;

	sm_container_ptr remain = hole->next;
	if(sm_unused_containers == 0x0) sm_unused_containers = remain;
	else {
		if(hole == sm_unused_containers) {
			remain->next_unused = hole->next_unused;
			sm_unused_containers = remain;
			fprintf(stderr,"HOW ARE YOU!!\n");
		} else {
			for(itr = sm_unused_containers ; itr->next_unused != 0x0 ; itr = itr->next_unused) {
				if(itr->next_unused == hole)	 {
					break;
				}
			}
			if(itr->next_unused == 0x0) {
				itr->next_unused = remain; 
				fprintf(stderr,"Wow!!\n");
			} else {
				fprintf(stderr,"THIS!\n");
				remain->next_unused = hole->next_unused;
				itr->next_unused = remain;

			}
		}
	}


	return hole->data ;
}


void sfree(void * p)
{
	sm_container_ptr itr, pre,cur = 0x0,pos;
	for (itr = sm_first ; itr != 0x0 ; itr = itr->next) {
		if (itr->data == p) {
			itr->status = Unused ;
			cur = itr;
			break ;
		}
	}

	if(cur == 0x0) {
		fprintf(stderr,"free(): long address!\n");
		exit(1);
	}
	
	if(cur != sm_first) {
		for (itr = sm_first ; itr->next != 0x0 ; itr = itr->next) {
			if(itr->next == cur) {
				pre = itr;
			}
		}
		if(pre->status == Unused) {
                	pre->dsize += cur->dsize;
                	pre->next = cur->next;
                	cur = pre;
        	}
	}

	pos = cur->next;
	if(pos != 0x0 && pos->status == Unused) {
		cur->dsize += pos->dsize;
		cur->next = pos->next;
	}
}

void print_sm_containers()
{
	sm_container_ptr itr ;
	int i = 0 ;

	printf("==================== sm_containers ====================\n") ;
	for (itr = sm_first ; itr != 0x0 ; itr = itr->next, i++) {
	//for (itr = sm_unused_containers ; itr != 0x0 ; itr = itr->next_unused, i++) {
		char * s ;
		printf("%3d:%p:%s:", i, itr->data, itr->status == Unused ? "Unused" : "  Busy") ;
		printf("%8d:", (int) itr->dsize) ;

		for (s = (char *) itr->data ;
			 s < (char *) itr->data + (itr->dsize > 8 ? 8 : itr->dsize) ;
			 s++) 
			printf("%02x ", *s) ;
		printf("\n") ;
	}
	printf("=======================================================\n") ;

}
void print_sm_uses() {

        sm_container_ptr itr ;
        size_t m1=0, m2=0, m3=0; // retained, alloced retained, not alloced retained

        printf("==================== sm_uses ====================\n") ;
        for (itr = sm_first ; itr != 0x0 ; itr = itr->next) {
                 //printf("%3d:%p:%s:", i, itr->data, itr->status == Unused ? "Unused" : "  Busy") ;
                int data = itr->dsize;
                m1 += data;
                if(itr->status == Unused)
                        m3 += data;
                else
                        m2 += data;
        }
        printf("All retained memnory size: %d\n",m1);
        printf("retained momery allocated size: %d\n",m2);
        printf("retained momery not allocated size: %d\n",m3);
        printf("=======================================================\n") ;   

}
