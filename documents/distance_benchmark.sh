#!/bin/bash 

max_distance=100
for((i=1; i<=100; i++)) do
    echo "Run with $i miles:" > ./data/time_$i.out
    (time ../splat-1.4.0/splat-hd -d ../eledata/sdf/ -L 10 -R $i -t W6BHZ.qth) 1>./data/stdout_$i.out 2>> ./data/time_$i.out

    echo "Run with $i miles:" > ./data/gprof_$i.out
    gprof ../splat-1.4.0/splat-hd >> ./data/gprof_$i.out
done
