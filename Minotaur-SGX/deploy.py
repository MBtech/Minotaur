import paramiko
import sys
import json

parent_directory = sys.argv[1]
f= sys.argv[2]
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
ssh = paramiko.SSHClient()
ssh.load_system_host_keys()

for component in components:
    j = 0
    for server in servers:
        ssh.connect(server)
        for i in range(j, int(parallelism[component]), len(servers)):
            cmd = "cd " + parent_directory+ "; nohup ./app " + component+" " + str(i) + " " + server+ " " + ports[component] +" "+ server +" "+ ports[components[(components.index(component)+1)%len(components)]] + " > " + component+"log"+str(i)+ " 2>&1 &" 
            print cmd 
            ssh_stdin, ssh_stdout, ssh_stderr = ssh.exec_command(cmd)
        j+=1 
            
