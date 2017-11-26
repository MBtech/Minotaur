#!/bin/bash
dir=~/bilal/Minotaur/Minotaur-SGX/$1

IP1=192.168.10.1
IP2=192.168.10.2

ssh $IP1 "sh -c 'cd $dir; ./cleanup.sh' "
ssh $IP2 "sh -c 'cd $dir; ./cleanup.sh' "

#scp kill.sh sgx@$IP1:$dir/
#scp kill.sh sgx@$IP2:$dir/

ssh $IP1 "sh -c 'cd $dir; ./kill.sh' "
ssh $IP2 "sh -c 'cd $dir; ./kill.sh' "

scp spoutIP sgx@$IP1:$dir/
scp countIP sgx@$IP1:$dir/
scp spoutIP sgx@$IP2:$dir/
scp countIP sgx@$IP2:$dir/
scp aggregateIP sgx@$IP1:$dir/
scp aggregateIP sgx@$IP2:$dir/

ssh $IP1 "cd $dir; make clean; make"
ssh $IP2 "cd $dir; make clean; make"

for i in `seq 0 2 0`
do
ssh $IP1 "sh -c 'cd $dir; nohup ./app spout  $i $IP1 5000 > spoutlog$i 2>&1 &' " 
done

for i in `seq 1 2 1`
do
ssh $IP2 "sh -c 'cd $dir; nohup ./app spout $i $IP2 5000 > spoutlog$i 2>&1 &' "
done


for i in `seq 0 2 4`
do
ssh $IP1 "sh -c 'cd $dir; nohup ./app splitter  $i > splitterlog$i 2>&1 &' "
done
for i in `seq 1 2 5`
do
ssh $IP2 "sh -c 'cd $dir; nohup ./app splitter  $i > splitterlog$i 2>&1 &' "
done

for i in `seq 1 2 5`
do
ssh $IP1 "sh -c 'cd $dir; nohup ./app count  $i $IP1 6000 > countlog$i 2>&1 &' "
done

for i in `seq 0 2 4`
do
ssh $IP2 "sh -c 'cd $dir; nohup ./app count  $i $IP2 6000 > countlog$i 2>&1 &' "
done

ssh $IP2 "sh -c 'cd $dir; nohup ./app aggregate 0 $IP2 7000 > aggregatelog0 2>&1 &' "

sleep 100

ssh $IP2 "sh -c $dir/kill.sh"
ssh $IP1 "sh -c $dir/kill.sh"


scp $IP2:$dir/*log* .
scp $IP1:$dir/*log* .
