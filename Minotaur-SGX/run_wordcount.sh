#!/bin/bash
dir=~/bilal/Minotaur/Minotaur-SGX/$1

IPs=(192.168.10.1 192.168.10.2)

for IP in ${IPs[@]}
do
scp kill.sh sgx@$IP:$dir/
ssh $IP "sh -c 'cd $dir; ./cleanup.sh; ./kill.sh' "
scp spoutIP sgx@$IP:$dir/
scp countIP sgx@$IP:$dir/
scp aggregateIP sgx@$IP:$dir/
ssh $IP "cd $dir; make clean; make"
done

python deploy.py $dir $2

sleep 100

for IP in ${IPs[@]}
do
ssh $IP "sh -c $dir/kill.sh"
scp $IP:$dir/*log* .
done
