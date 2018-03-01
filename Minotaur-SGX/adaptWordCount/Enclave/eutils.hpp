#include <string>
#include <vector>
#include <algorithm>
#include <set>

#ifndef _EUTILS_H_
#define _EUTILS_H_
//int ROUTE_LEN = 7;
//int ROUTE_ALGO = 4;
int* get_route(std::string key, int n, int algo, int nroutes, int* valid);
std::string append_valid(std::string word, int valid);

#endif
