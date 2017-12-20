#!/bin/bash

cat countlog* | grep "Latency" >> "$1"_total_metrics_"$2"
#cat total_metrics |  awk '{ if($2>=0) {print $2}}' >> metrics
cat "$1"_total_metrics_"$2" | wc -l >> "$1"_TP
for filename in countlog*; do
   cat $filename | grep "Latency" | awk '{ if($2>=0) {print $2}}' > temp
   split -n 2 temp
   cat xab >> "$1"_metrics_small_"$2"
   rm xaa xab
done
#split -n 2 metrics 
#cat xab >> metrics_small
#rm xaa xab
#tail -n 5000000 metrics >> metrics_small
