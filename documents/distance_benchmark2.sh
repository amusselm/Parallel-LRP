#!/bin/bash 

max_distance=50
machine=`hostname`
data_dir="../results/$machine/"
for((i=1; i<=$max_distance; i++)) do
    echo "Run with $i miles:" > ./$data_dir/time_$i.out
    (time ../psplat -d ../../sdf -L 10 -R $i -t W6BHZ.qth) 1>./stdout_$i.out 2>> ./data/time_$i.out

    echo "Run with $i miles:" > ./$data_dir/gprof_$i.out
    gprof ../splat-1.4.0/splat-hd >> ./$data_dir/gprof_$i.out
done
