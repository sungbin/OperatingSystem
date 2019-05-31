#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct edge {
    int lock_m[100] ;
    int lock_count ;
    int seg1, seg2 ;
    int thread ;
    int out_m ;
    int in_m ;
} Edge ;

typedef struct edge_set {
    int count ;
    Edge edge[100] ;
} Edge_set ;

typedef struct cycle {
    int count ;
    Edge cycle[100] ;
} Cycle ;

typedef struct cycle_set {
    int count ;
    Cycle sets[100] ;
} Cycle_set ;

Edge_set edges = {0, {0x0, }} ;
Cycle_set c_sets = {0, {0x0, }} ;
Cycle_set tp_sets = {0, {0x0, }} ;

void parsing(char f_name[]) ;
void collect_cycle() ;
void skip_FP() ;

int main(int argc, char * argv[]) {
    parsing(argv[1]) ;

    // for (int i = 0; i < edges.count; i++) {
    //     printf("out: %d, seg1: %d, thread: %d, G[", edges.edge[i].out_m, edges.edge[i].seg1, edges.edge[i].thread) ;
    //     for (int j = 0; j < edges.edge[i].lock_count; j++)
    //         printf("%d,", edges.edge[i].lock_m[j]) ;
    //     printf("], seg2: %d, in: %d\n", edges.edge[i].seg2, edges.edge[i].in_m) ;
    // }

    collect_cycle() ;
    skip_FP() ;
    print_TP() ;
}

void parsing(char f_name[]) {
    FILE * f = fopen(f_name, "r") ;
    
    if (f == NULL) exit(1) ;
    
    char line[512] ;
    while(!feof(f)) {
        fgets(line, sizeof(line), f) ;
        line[strlen(line) - 1] = '\0' ;

        Edge e = {{0x0, }, 0, 0, 0, 0, 0, 0} ;
        char gate[128] ;
        char * token = NULL ;
        
        sscanf(line, "%d,%d,%d,/%[^/]/,%d,%d", &e.out_m, &e.seg1, &e.thread, gate, &e.seg2, &e.in_m) ;
        
        token = strtok(gate, ",") ;
        for (int i = 0; token != NULL; i++) {
            e.lock_m[i] = atoi(token) ;
            token = strtok(NULL, ",") ;
            e.lock_count++ ;
        }

        edges.edge[edges.count] = e ;
        edges.count++ ;
    }
    edges.count-- ;

    fclose(f) ;
}

void collect_cycle() {
    for (int i = 0; i < edges.count; i++) {
        Cycle c = {0, {0x0, }} ;
        c.cycle[c.count++] = edges.edge[i] ;
        for (int j = i + 1; j < edges.count; j++) {
            if (edges.edge[i].out_m == edges.edge[j].in_m && edges.edge[i].in_m == edges.edge[j].out_m) {
                Cycle _c = c ;
                _c.cycle[_c.count++] = edges.edge[j] ;
                c_sets.sets[c_sets.count++] = _c ;
            }
            // if (edges.edge[i].in_m == edges.edge[j].out_m && edges.edge[j].in_m != edges.edge[i].out_m) {

            // }
        }
    }
}

void skip_FP() {
    for (int i = 0; i < c_sets.count; i++) {
        int flag = 1 ;
        for (int j = 0; j < c_sets.sets[i].count; j++) {
            int t1 = c_sets.sets[i].cycle[j].thread ;
            for (int k = j + 1; k < c_sets.sets[i].count; k++) {
                int t2 = c_sets.sets[i].cycle[k].thread ;
                if (t1 == t2) {
                    flag = 0 ;
                    break ;
                }
            }
        }
        if (flag)
            tp_sets.sets[tp_sets.count++] = c_sets.sets[i];
    }
}

void print_TP() {
    for (int i = 0; i < tp_sets.count; i++) {
        for (int j = 0; j < tp_sets.sets[i].count; j++) {
            printf("out: %d, seg1: %d, thread: %d, G[", tp_sets.sets[i].cycle[j].out_m, tp_sets.sets[i].cycle[j].seg1, tp_sets.sets[i].cycle[j].thread) ;
            for (int k = 0; k < tp_sets.sets[i].cycle[j].lock_count; k++)
                printf("%d,", tp_sets.sets[i].cycle[j].lock_m[k]) ;
            printf("], seg2: %d, in: %d\n", tp_sets.sets[i].cycle[j].seg2, tp_sets.sets[i].cycle[j].in_m) ;
        }
    }
}
