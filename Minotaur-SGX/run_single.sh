#!/bin/bash
dir=~/bilal/Minotaur/Minotaur-SGX/$1

IPs=(192.168.10.1 192.168.10.2)

cp $dir/Enclave/Enclave.cpp $dir/App/Enclave.cpp
cp $dir/Enclave/Enclave.h $dir/App/Enclave.h
scp -r $dir 192.168.10.1:~/bilal/Minotaur/Minotaur-SGX/

python create_files.py $2

for IP in ${IPs[@]}
do
scp kill.sh sgx@$IP:$dir/
ssh $IP "sh -c 'cd $dir; ./cleanup.sh; ./kill.sh' "
scp *IP sgx@$IP:$dir/
ssh $IP "cd $dir; make clean; make"
done

python deploy.py $dir $2

sleep 100

for IP in ${IPs[@]}
do
ssh $IP "sh -c $dir/kill.sh"
scp $IP:$dir/*log* .
done
