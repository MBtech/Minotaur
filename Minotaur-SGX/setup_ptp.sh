#!/bin/bash
fileItemString=$(cat hosts |tr "\n" " ")

IPs=($fileItemString)


for IP in ${IPs[@]}
do
ssh -t $IP "apt install ptpd -y; ptpd -i bond0:0 -s"
done
