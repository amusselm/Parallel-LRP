#!/bin/bash 

max_points=90000
host_dir=`hostname`_singlepath_`date +%F`
results_dir=./results
test_num=1

profile=1
altitude=250
txheight=20
rxheight=10
distance=75
frequency=144.39

if [ ! -d $results_dir/$host_dir ]
then
   mkdir $results_dir/$host_dir
fi

while [ -d $results_dir/$host_dir/$test_num ]; do
   test_num=`expr $test_num + 1`
done

test_dir=$results_dir/$host_dir/$test_num 
mkdir $test_dir 

cat /proc/cpuinfo > $test_dir/cpuinfo.out
../tools/devicelister > $test_dir/deviceinfo.out
free -m > $test_dir/memfree.out

if [ ! -d $test_dir/opencl ]
then
   mkdir $test_dir/opencl
fi
if [ ! -d $test_dir/serial ]
then
   mkdir $test_dir/serial
fi

output_dir=$test_dir/opencl
for((i = 10; i<= $max_points; i = i+10)) do
   echo "Running $i points of $max_points of OpenCL"
   echo "Run with $i points:" > $output_dir/time_$i.out
   (time ../psplat/pathtest -m 2 -v 1 -p $profile -a $altitude -t $txheight -r $rxheight -f $frequendy -d $distance) 1> $output_dir/stdout_$i.out 2>> $output_dir/time_$i.out
    
   echo "Run with $i points:" > $output_dir/gprof_$i.out
   gprof ../psplat/pathtest -m 2 -v 1 -p $profile -a $altitude -t $txheight -r $rxheight -f $frequendy -d $distance >> $output_dir/gprof_$i.out 
done

output_dir=$test_dir/serial
for((i = 10; i<= $max_points; i = i+10)) do
   echo "Running $i points of $max_points of Serial"
   echo "Run with $i points:" > $output_dir/time_$i.out
   (time ../psplat/pathtest -m 3 -v 1 -p $profile -a $altitude -t $txheight -r $rxheight -f $frequendy -d $distance) 1> $output_dir/stdout_$i.out 2>> $output_dir/time_$i.out
    
   echo "Run with $i points:" > $output_dir/gprof_$i.out
   gprof ../psplat/pathtest -m 3 -v 1 -p $profile -a $altitude -t $txheight -r $rxheight -f $frequendy -d $distance >> $output_dir/gprof_$i.out 
done

