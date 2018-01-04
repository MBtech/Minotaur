import sys
import json

f= sys.argv[1]
data = json.load(open(f))
servers=data["servers"]
components =data["components"]
ports = dict()
base_port = 5000
k = 0
for c in components:
    ports[c] =  str(base_port + (k*500))
    k +=1

parallelism = data["parallelism"]

for component in components:
    j = 0
    fd = open(component+"IP", "w")
    for server in servers:
        for i in range(j, int(parallelism[component]), len(servers)):
            fd.write(server + " " + str(int(ports[component])+i)+"\n")
        j+=1 
    fd.close()
            
