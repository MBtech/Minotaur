#!/bin/bash
dir=~/Minotaur/Minotaur-SGX/$1
timeout=(0 200 400 800 1000)
#timeout=(0)

mkdir Metrics_"$1"_"$4"
for i in "${timeout[@]}"
do

cat hosts | xargs -I {} ssh {} "sed -i \"s/#define BUFFER_TIMEOUT.*/#define BUFFER_TIMEOUT $i/g\" $dir/Include/user_types.h"

./$2 $1 $3
#./preprocess.sh "$1"-"$4" $i

python summarize.py $3 $i
mv Latency_* Metrics_"$1"_"$4"/
#python percentile.py "$1"-"$4" "$1"-"$4"_metrics_small_"$i"
done



#tar -czf "$1"_5_metrics.tar.gz *metrics_* 
