#!/bin/bash
dir=~/bilal/Minotaur/Minotaur-SGX/$1
fileItemString=$(cat hosts |tr "\n" " ")

IPs=($fileItemString)

cp $dir/Enclave/Enclave.cpp $dir/App/Enclave.cpp
cp $dir/Enclave/Enclave.h $dir/App/Enclave.h

python create_files.py $2

for IP in ${IPs[@]}
do
scp kill.sh $IP:$dir/
ssh $IP "sh -c 'cd $dir; ./cleanup.sh; ./kill.sh' "
scp *IP $IP:$dir/
ssh $IP "cd $dir; make clean; make"
done

python deploy.py $dir $2

sleep 100

for IP in ${IPs[@]}
do
ssh $IP "sh -c $dir/kill.sh"
scp $IP:$dir/*log* .
done
