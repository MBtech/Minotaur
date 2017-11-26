#!/bin/bash

cat countlog* | grep "Latency" >> total_metrics
cat total_metrics |  awk '{ if($2>=0) {print $2}}' >> metrics
cat total_metrics | wc -l >> TP
#split -n 2 metrics 
#cat xab >> metrics_small
#rm xaa xab
tail -n 5000000 metrics >> metrics_small
