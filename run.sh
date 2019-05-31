# make $1 $2 > /dev/null 2>&1
LD_PRELOAD=./$1.so ./$2.out
# if [ "$1" == "dmonitor" ]; then
# 	./dpredictor.out dmonitor.trace
# fi

