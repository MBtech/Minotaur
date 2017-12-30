#include <iostream>
#include <map>
#include <time.h>
#include "utils.hpp"
using namespace std;

class TumblingWindow{
	std::map<std::string, double> results;
	unsigned long beginTime;
	int windowSize; 
public:
	TumblingWindow(int window_size);
	void TumblingWindow::add_value();
	void TumblingWindow::get_values();	
};
