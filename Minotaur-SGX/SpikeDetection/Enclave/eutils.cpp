#include <string>
#include <vector>
#include <algorithm>
std::vector<int> get_route(std::string key, int n, int algo, int nroutes) {
    std::vector<int> routes;
    // Key based grouping
    if(algo == 1) {
        std::hash<std::string> hasher;
        long hashed = hasher(key);
        int route  = abs(hashed % n);
        routes.push_back(route);
    }
    // Broadcast
    else if(algo==2) {
        for(int i =0, i < n; i++) {
            routes.push_back(i);
        }
    }
    //Multicast
    else if (algo == 3) {
        for(int i =0; i<nroutes; i++) {
            //TODO: This should really be another hash function
            std::hash<std::string> hasher;
            long hashed = hasher(key);
            int route  = abs(hashed % n);
            routes.push_back(route);
            route = abs(hashed+i % n)
                    routes.push_back(route);
        }
    }
    // Power of n-choice
    else if(algo == 4 ){
	
    }

    return routes;
}

