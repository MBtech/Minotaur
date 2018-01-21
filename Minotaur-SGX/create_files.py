import sys
import json

f= sys.argv[1]
data = json.load(open(f))
servers=data["servers"]
components =data["components"]
ports = dict()
parallelism = data["parallelism"]

port =5000
for component in components:
    if(component not in data["topology"]):    
        continue
    child = data["topology"][component]
    k = 0
    for c in range(0, len(child), 2):
        filename_out = component+"_out_"+str(k)+"_IP"
        filename_in = child[c]+"_in_0_IP"
        fd = open(filename_out, "w")
	fd1 = open(filename_in, "w")
	count = 0
        j = 0
        for server in servers:
          
            for i in range(j, int(parallelism[component]), len(servers)):
                fd.write(server + " " + str(port+i)+"\n")
                fd1.write(server + " " + str(port+i)+"\n")
    		count+=1
            
            j+=1 
        port = port +count
        fd.close()
	fd1.close()
        k +=1
            
