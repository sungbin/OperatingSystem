all: ddetector

ddetector: ddetector.c
	gcc -shared -fPIC -o ddetector.so ddetector.c -ldl

abba: ddetector.c abba.c
	gcc -shared -fPIC -o ddetector.so ddetector.c -ldl	
	gcc -pthread abba.c -o exec.out

dinning: ddetector.c dinning.c
	gcc -shared -fPIC -o ddetector.so ddetector.c -ldl
	gcc -pthread dinning.c -o exec.out

dinning_deadlock: ddetector.c dinning_deadlock.c
	gcc -shared -fPIC -o ddetector.so ddetector.c -ldl
        gcc -pthread dinning_deadlock.c -o exec.out

clean:
	rm -rf ddetector.so exec.out
