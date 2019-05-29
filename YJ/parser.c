#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Info {
	char fname[75];
	char func[20];
	char addr[50];

} Info;

typedef struct _Log{
	int mutex;
	int thread;
	Info info[10];
	int info_count;
} Log;

Log logs[1000];
int log_count = 0;
Info getInfo(char line[]);
void parse(char _fname[]);
int main() {

	parse("example.trace");

	int i;
        for(i = 0; i<log_count; i++) {
                printf("mutex: %d, thread: %d\n",logs[i].mutex,logs[i].thread);
                int j;
                for(j = 0; j<logs[i].info_count; j++) {
                        printf("fname: %s, func: %s, addr: %s\n",logs[i].info[j].fname,logs[i].info[j].func,logs[i].info[j].addr);
                }
        }

	return 0;
}
void parse(char _fname[]) {

	FILE *f = fopen(_fname,"r"); //trace file name
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

                                Info info;
                                info = getInfo(str);
                                log.info[log.info_count] = info;
                                log.info_count++;
                                break;
                        }
                }

        }
        fclose(f);	
}

Info getInfo(char line[]) {
	Info info = {0x0,0x0,0x0};
	char fname[75];
	char func[20];
	char addr[50];

	char ch = line[0];
	int i = 0;
	while(ch != '(') {
		fname[i] = ch;
		i++;
		ch = line[i];
	}
	i++; //func start
	int j = 0;
	ch = line[i];
	while(ch != '+') {
                func[j] = ch;
		j++;
                i++;
		ch = line[i];
        }
	while(ch != '[') {
		i++;
		ch = line[i];
	}
	i++;
	j = 0;
	ch = line[i];
	while(ch != ']') {
		addr[j] = ch;
		i++;
		j++;
                ch = line[i];
        }
	strcpy(info.fname,fname);
	strcpy(info.func,func);
	strcpy(info.addr,addr);

	return info;
}

