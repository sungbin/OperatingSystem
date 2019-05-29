#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Log{
	int mutex;
	int thread;
	char info[10][255];
	int info_count;
} Log;

Log logs[1000];
int log_count = 0;

int main() {

	FILE *f = fopen("example.trace","r"); //trace file name
	if(f == NULL) {
		exit(1);	
	}
	char str[255];
	Log log = {0,0,0x0,0};
	while(!feof(f)) {

		fgets(str,sizeof(str),f);
		str[strlen(str)-1] = '\0';

		switch(str[0]) {

			case 'm' : {
				
				if(log.mutex != 0) {
					logs[log_count] = log;
					log_count++;
					Log new_log = {1,0,0x0,0};
					log = new_log;
				}

				str[strlen(str)-1] = '\0'; // ')'
				char* p;
				p = str + 6;
				int mutex = atoi(p);
				log.mutex = mutex;

				break;
			}

			case 't' : {

				str[strlen(str)-1] = '\0'; // ')'
                                char* p;
                                p = str + 7;
                                int thread = atoi(p);
                                log.thread = thread;

                                break;
                        }

			default : { 

				strcpy(log.info[log.info_count],str);
				log.info_count++;
				break;
			}
		}		

	}

	int i;
	for(i = 0; i<log_count; i++) {
		printf("mutex: %d, thread: %d\n",logs[i].mutex,logs[i].thread);
		int j;
		for(j = 0; j<logs[i].info_count; j++)
			printf("%s\n",logs[i].info[j]);
	}


	fclose(f);
	return 0;
}
