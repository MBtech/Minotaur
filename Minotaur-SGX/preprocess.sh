#!/bin/bash

cat countlog* | grep "Latency" >> total_metrics
cat total_metrics |  awk '{ if($2>=0) {print $2}}' >> metrics
tail -n 10000000 metrics >> metrics_small
