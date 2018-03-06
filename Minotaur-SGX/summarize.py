import sys
import json
import re
import subprocess

f= sys.argv[1]
buf = sys.argv[2]
data = json.load(open(f))
components =data["components"]
parallelism = data["parallelism"]

for component in components:
    for i in range(0, parallelism[component]):
        cmd = "cat "+ component+"log"+str(i)+ " | grep 'L:' | awk -F: '$2 ~ /^[0-9]+$/ {if($2>0) {print $2}}' > temp ; ./split.sh " + "Latency_"+component+"_"+buf
        #cmd = "cat "+ component+"log"+str(i)+ " | grep 'Latency:' | awk -F: "
 # ; ./split.sh " + "Latency_"+component+"_"+buf
        ps = subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
        output = ps.communicate()[0]
        print output
