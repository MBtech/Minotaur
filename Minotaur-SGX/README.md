## Setup
You can setup Intel SGX development and execution environment using the ansible playbook available at [ansible-sgx repo](https://github.com/mbtech/ansible-sgx.git). 

## Library
The work on the library is underprogress

## Applications
There are currently three SGX based streaming applications available 
1. Rolling Word count
2. New york taxi routes application
3. Spike Detection application

## Usage
Information about the topologies can be specified using the json file. See the examples such as `wordcount.json` and `spike_agg.json`

The application can be executed using the following command:
```
./run_tests.sh <App_Dir> run_single.sh <json_file> <App_version>
```

App Dir refers to the directory where the stream application exists
Json file is the path to the json file that provides the configurable information about the application
App version is used to specify whether to run the application in `SGX`, `NATIVE` or `NOENCRYPT` modes.

Here is an example 
```
./run_tests.sh WordCount run_single.sh wordcount_agg.json SGX
```

Note that the json file that have `_agg` are the ones that specify topology information when we decide to use the power of n choice algorithm 
