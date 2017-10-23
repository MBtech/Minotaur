#!/bin/bash
dir=~/bilal/Minotaur/Minotaur-SGX/WordCount-Native

ssh 192.168.10.2 "sh -c 'cd $dir; ./cleanup.sh' "
ssh 192.168.10.1 "sh -c 'cd $dir; ./cleanup.sh' "

scp kill.sh sgx@192.168.10.1:$dir/
scp kill.sh sgx@192.168.10.2:$dir/

ssh 192.168.10.1 "sh -c 'cd $dir; ./kill.sh' "
ssh 192.168.10.2 "sh -c 'cd $dir; ./kill.sh' "

scp spoutIP sgx@192.168.10.1:$dir/
scp countIP sgx@192.168.10.1:$dir/
scp spoutIP sgx@192.168.10.2:$dir/
scp countIP sgx@192.168.10.2:$dir/

for i in `seq 0 2 0`
do
ssh 192.168.10.1 "sh -c 'cd $dir; nohup ./app spout  $i 192.168.10.1 5000 > spoutlog$i 2>&1 &' " 
done

for i in `seq 1 2 1`
do
ssh 192.168.10.2 "sh -c 'cd $dir; nohup ./app spout $i 192.168.10.2 5000 > spoutlog$i 2>&1 &' "
done


for i in `seq 0 2 2`
do
ssh 192.168.10.1 "sh -c 'cd $dir; nohup ./app splitter  $i > splitterlog$i 2>&1 &' "
done
for i in `seq 1 2 3`
do
ssh 192.168.10.2 "sh -c 'cd $dir; nohup ./app splitter  $i > splitterlog$i 2>&1 &' "
done

for i in `seq 1 2 5`
do
ssh 192.168.10.1 "sh -c 'cd $dir; nohup ./app count  $i 192.168.10.1 6000 > countlog$i 2>&1 &' "
done

for i in `seq 0 2 4`
do
ssh 192.168.10.2 "sh -c 'cd $dir; nohup ./app count  $i 192.168.10.2 6000 > countlog$i 2>&1 &' "
done

sleep 200

ssh 192.168.10.2 "sh -c $dir/kill.sh"
ssh 192.168.10.1 "sh -c $dir/kill.sh"


scp 192.168.10.2:$dir/*log* .
scp 192.168.10.1:$dir/*log* .
