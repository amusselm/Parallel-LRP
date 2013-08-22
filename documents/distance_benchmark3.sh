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
   run_num=`printf %03d $i`
   echo "Running $run_num miles of $max_distance in OpenCL"
   echo "Run with $run_num points:" > $output_dir/time_$run_num.out
   (time ../psplat/psplat -d ../../splat/eledata/sdf/ -L 10 -R $run_num -t ../documents/W6BHZ.qth) 1>$output_dir/stdout_$run_num.out 2>> $output_dir/time_$run_num.out
   echo "Run with $run_num miles:" > $output_dir/gprof_$run_num.out
   gprof ../psplat/psplat >> $output_dir/gprof_$run_num.out
done

output_dir=$test_dir/serial
run_dir=../splat-1.4.0/
for((i = 1; i<= $max_distance; i = i+1)) do
   cd $run_dir
   run_num=`printf %03d $i`
   echo "Running $run_num miles of $max_distance in Serial"
   echo "Run with $run_num points:" > $output_dir/time_$run_num.out
   (time ../splat-1.4.0/splat-hd -d ../../splat/eledata/sdf/ -L 10 -R $run_num -t ../documents/W6BHZ.qth) 1>$output_dir/stdout_$run_num.out 2>> $output_dir/time_$run_num.out
   echo "Run with $run_num miles:" > $output_dir/gprof_$run_num.out
   gprof ../splat-1.4.0/splat-hd >> $output_dir/gprof_$run_num.out
done
