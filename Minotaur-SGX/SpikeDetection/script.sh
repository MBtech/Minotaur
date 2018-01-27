#!/bin/bash
killbg() {
	for p in "${pids[@]}" ; do
		kill "$p"
	done
}

trap killbg EXIT
pids=()
for i in `seq 0 0`
do
./app spout  $i 192.168.10.2 5000 > spoutlog$i & 
pids+=($!)
done
#sleep 1
for i in `seq 0 2`
do
./app splitter $i 2 192.168.10.2 6000 192.168.10.2 6001 2 192.168.10.2 5000 192.168.10.2 5001 > splitterlog$i & 
pids+=($!)
done
#sleep 1 
for i in `seq 0 2`
do
echo "Calling count"
./app count $i 192.168.10.2 6000 > countlog$i 2>&1 & 
pids+=($!)
done

sleep 100
