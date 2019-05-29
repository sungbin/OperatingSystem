# gcc -pthread dinning_deadlock.c -o exec.out
gcc -pthread abba.c -o exec.out
gcc -shared -fPIC -o mypthread.so mypthread.c -ldl
LD_PRELOAD=./mypthread.so ./exec.out
