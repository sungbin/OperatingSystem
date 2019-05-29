all: dmonitor parser

dmonitor: dmonitor.c
	gcc -shared -fPIC -o dmonitor.so dmonitor.c -ldl

parser: parser.c
	gcc -o parser.out parser.c

abba: dmonitor.c ./test/abba.c parser.c
	gcc -shared -fPIC -o dmonitor.so dmonitor.c -ldl
	gcc -rdynamic -g -pthread ./test/abba.c -o exec.out
	gcc -o parser.out parser.c

dinning: dmonitor.c ./test/dinning.c parser.c
	gcc -shared -fPIC -o dmonitor.so dmonitor.c -ldl
	gcc -rdynamic -g -pthread ./test/dinning.c -o exec.out
	gcc -o parser.out parser.c

dinning_deadlock: dmonitor.c ./test/dinning_deadlock.c parser.c
	gcc -shared -fPIC -o dmonitor.so dmonitor.c -ldl
	gcc -rdynamic -g -pthread ./test/dinning_deadlock.c -o exec.out
	gcc -o parser.out parser.c

clean:
	rm -rf dmonitor.so *.out *.trace
