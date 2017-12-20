#!/bin/bash
dir=~/bilal/Minotaur/Minotaur-SGX/$1
timeout=(0 200 400 800 1000)
#timeout=(0)

for i in "${timeout[@]}"
do

ssh 192.168.10.1 "sed -i \"s/#define BUFFER_TIMEOUT.*/#define BUFFER_TIMEOUT $i/g\" $dir/Include/user_types.h"
ssh 192.168.10.2 "sed -i \"s/#define BUFFER_TIMEOUT.*/#define BUFFER_TIMEOUT $i/g\" $dir/Include/user_types.h"

./$2 $1
./preprocess.sh "$1"-"$3" $i

python percentile.py "$1"-"$3" "$1"-"$3"_metrics_small_"$i"
done

#tar -czf "$1"_5_metrics.tar.gz *metrics_* 
