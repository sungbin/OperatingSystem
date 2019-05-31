ddetector: ddetector.c
	gcc -shared -fPIC -o ddetector.so ddetector.c -ldl

dmonitor: dmonitor.c dpredictor.c
	gcc -shared -fPIC -o dmonitor.so dmonitor.c -ldl
	gcc -o dpredictor.out dpredictor.c

abba: ./test/abba.c
	gcc -pthread ./test/abba.c -o abba.out

dinning: ./test/dinning.c
	gcc -pthread ./test/dinning.c -o dinning.out

dinning_deadlock: ./test/dinning_deadlock.c
	gcc -pthread ./test/dinning_deadlock.c -o dinning_deadlock.out

gate_lock: ./test/gate_lock.c
	gcc -pthread ./test/gate_lock.c -o gate_lock.out

thread_creation: ./test/thread_creation.c
	gcc -pthread ./test/thread_creation.c -o thread_creation.out

clean:
	rm -rf *.so *.out dmonitor.trace
