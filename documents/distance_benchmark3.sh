#!/bin/bash 

max_distance=100
host_dir=`hostname`_splatdist_`date +%F`
results_dir=../documents/results
test_num=1

if [ ! -d $results_dir/$host_dir ]
then
   echo "Creating Results directory"
   mkdir $results_dir/$host_dir
fi

while [ -d $results_dir/$host_dir/$test_num ]; do
   test_num=`expr $test_num + 1`
done

test_dir=$results_dir/$host_dir/$test_num 
echo "Creating test dir $test_dir"
mkdir $test_dir 

echo "Dumping general info"
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

echo "Starting Tests"
output_dir=$test_dir/opencl
run_dir=../psplat/
for((i = 1; i<= $max_distance; i = i+1)) do
   cd $run_dir
   echo "Running $i miles of $max_distance in OpenCL"
   echo "Run with $i points:" > $output_dir/time_$i.out
   (time ../psplat/psplat -d ../../splat/eledata/sdf/ -L 10 -R $i -t ../documents/W6BHZ.qth) 1>$output_dir/stdout_$i.out 2>> $output_dir/time_$i.out
   echo "Run with $i miles:" > $output_dir/gprof_$i.out
   gprof ../psplat/psplat >> $output_dir/gprof_$i.out
done

output_dir=$test_dir/serial
run_dir=../splat-1.4.0/
for((i = 1; i<= $max_distance; i = i+1)) do
   cd $run_dir
   echo "Running $i miles of $max_distance in Serial"
   echo "Run with $i points:" > $output_dir/time_$i.out
   (time ../splat-1.4.0/splat-hd -d ../../splat/eledata/sdf/ -L 10 -R $i -t ../documents/W6BHZ.qth) 1>$output_dir/stdout_$i.out 2>> $output_dir/time_$i.out
   echo "Run with $i miles:" > $output_dir/gprof_$i.out
   gprof ../splat-1.4.0/splat-hd >> $output_dir/gprof_$i.out
done
