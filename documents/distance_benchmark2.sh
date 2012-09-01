#!/bin/bash 

program=../psplat/psplat
max_distance=50
machine=`hostname`
data_dir="results/$machine/"
for((i=1; i<=$max_distance; i++)) do
    num=`printf %3d $i`
    echo "Run with $i miles:" > ./$data_dir/time_$i.out
    (time $program -d ../../sdf -L 10 -R $i -t W6BHZ.qth) 1>./$data_dir/stdout_$i.out 2>> ./$data_dir/time_$i.out

    echo "Run with $i miles:" > ./$data_dir/gprof_$i.out
    gprof $program >> ./$data_dir/gprof_$i.out
done
