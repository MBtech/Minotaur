import sys
import json
import re
import subprocess

f= sys.argv[1]
data = json.load(open(f))
components =data["components"]
parallelism = data["parallelism"]

for component in components:
    for i in range(0, parallelism[component]):
        f = open(component+"log"+str(i), "r")
        lat = list()
        for line in f:
            if re.search('Latency:', line) and line.split(':')[1].strip(' ').strip('\n').isdigit():
                 lat.append(line.split(':')[1].strip(' ').strip('\n'))
       
        print len(lat)
        f.close()
 	f = open("Latency_"+component, "a")
       
        for line in lat[int(len(lat)/2):]:
	    f.write(line+"\n")
        f.close()
