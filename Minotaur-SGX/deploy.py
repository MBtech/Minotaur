import paramiko
import sys


servers=['192.168.10.1','192.168.10.2']
components = ['spout', 'splitter', 'count']
ports = {'spout':'5000', 'splitter': '','count':'6000'}
parallelism = {'spout':2, 'splitter':4, 'count':6}
parent_directory = sys.argv[1]
ssh = paramiko.SSHClient()
ssh.load_system_host_keys()

for component in components:
    j = 0
    for server in servers:
        ssh.connect(server)
        for i in range(j, parallelism[component], len(servers)):
            cmd = "cd " + parent_directory+ "; nohup ./app " + component+" " + str(i) + " " + server+ " " + ports[component] + " > " + component+"log"+str(i)+ " 2>&1 &" 
            print cmd 
            ssh_stdin, ssh_stdout, ssh_stderr = ssh.exec_command(cmd)
        j+=1 
            
