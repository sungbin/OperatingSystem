make $1
rm dmonitor.trace
LD_PRELOAD=./ddetector.so ./exec.out
./parser.out dmonitor.trace
