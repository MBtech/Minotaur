#include "eutils.hpp"
#include <map>

std::map <int, long> load; 
int j=0;

int* get_route(std::string key, int n, int algo, int nroutes) {
    int * routes = (int*)malloc(sizeof(int) * nroutes);
    // Shuffle grouping
    if(algo==0){
	j++;
        j = j% n;
        routes[0] = j;
     }
    // Key based grouping
    else if(algo == 1) {
        std::hash<std::string> hasher;
        long hashed = hasher(key);
        int route  = abs(hashed % n);
        routes[0]=route;
    }
    // Broadcast
    else if(algo==2) {
        for(int i =0; i < n; i++) {
            routes[i]=i;
        }
    }
    //Multicast
    else if (algo == 3) {
        for(int i =0; i<nroutes; i++) {
            //TODO: This should really be another hash function
            std::hash<std::string> hasher;
            long hashed = hasher(key);
            int route  = abs((hashed+i) % n);
            routes[i]=route;
        }
    }
    // Power of n-choice
    else if(algo == 4 ){
	std::vector<int> pr;
	for(int i =0; i<nroutes; i++) {
            //TODO: This should really be another hash function
            std::hash<std::string> hasher;
            long hashed = hasher(key);
            int route  = abs((hashed+i) % n);
            pr.push_back(route);
        }
	int min=pr[0];
	int min_load = load[pr[0]];
	for(int i =1; i<pr.size();i++){
	    if(load[pr[i]]<min_load){
	    	min = pr[i];
		min_load = load[pr[i]];
	    }
	}	
	load[min] +=1;
        routes[0]= min;
    }

    return routes;
}

