#!/bin/bash
dir=~/bilal/Minotaur/Minotaur-SGX/SpikeDetection

IP1=192.168.10.1
IP2=192.168.10.2

ssh $IP1 "sh -c 'cd $dir; ./cleanup.sh' "
ssh $IP2 "sh -c 'cd $dir; ./cleanup.sh' "

scp kill.sh sgx@$IP1:$dir/
scp kill.sh sgx@$IP2:$dir/

ssh $IP1 "sh -c 'cd $dir; ./kill.sh' "
ssh $IP2 "sh -c 'cd $dir; ./kill.sh' "

scp splitIP sgx@$IP1:$dir/
scp spikeIP sgx@$IP1:$dir/
scp splitIP sgx@$IP2:$dir/
scp spikeIP sgx@$IP2:$dir/

ssh $IP1 "cd $dir; make clean; make"
ssh $IP2 "cd $dir; make clean; make"

for i in `seq 0 2 0`
do
ssh $IP1 "sh -c 'cd $dir; nohup ./app spout  $i > spoutlog$i 2>&1 &' " 
done

for i in `seq 1 2 1`
do
ssh $IP2 "sh -c 'cd $dir; nohup ./app spout $i > spoutlog$i 2>&1 &' "
done


for i in `seq 0 2 2`
do
ssh $IP2 "sh -c 'cd $dir; nohup ./app splitter  $i $IP2 5000 $IP2 6000> splitterlog$i 2>&1 &' "
done
for i in `seq 1 2 3`
do
ssh $IP1 "sh -c 'cd $dir; nohup ./app splitter  $i $IP1 5000 $IP1 6000> splitterlog$i 2>&1 &' "
done

for i in `seq 1 2 5`
do
ssh $IP1 "sh -c 'cd $dir; nohup ./app count  $i > countlog$i 2>&1 &' "
done

for i in `seq 0 2 4`
do
ssh $IP2 "sh -c 'cd $dir; nohup ./app count  $i > countlog$i 2>&1 &' "
done

sleep 100

ssh $IP2 "sh -c $dir/kill.sh"
ssh $IP1 "sh -c $dir/kill.sh"


scp $IP2:$dir/*log* .
scp $IP1:$dir/*log* .
