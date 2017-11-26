#!/bin/bash
dir=~/bilal/Minotaur/Minotaur-SGX/$1
timeout=(0 100 200 400 800)
#timeout=(0)

for i in "${timeout[@]}"
do

ssh 192.168.10.1 "sed -i \"s/#define BUFFER_TIMEOUT.*/#define BUFFER_TIMEOUT $i/g\" $dir/Include/user_types.h"
ssh 192.168.10.2 "sed -i \"s/#define BUFFER_TIMEOUT.*/#define BUFFER_TIMEOUT $i/g\" $dir/Include/user_types.h"

./$2 $1
cat countlog* | grep "Latency" >> "$1"_total_metrics_$i
cat "$1"_total_metrics_$i |  awk '{ if($2>=0) {print $2}}' >> "$1"_metrics_$i
cat "$1"_metrics_$i | wc -l >> "$1"_TP
split -n 2 "$1"_metrics_$i
cat xab >> "$1"_metrics_"$i"_small
rm xaa xab

#tail -n 1000000 "$1"_metrics_$i >> "$1"_metrics_"$i"_small

python percentile.py $1 "$1"_metrics_"$i"_small
done

#tar -czf "$1"_5_metrics.tar.gz *metrics_* 
