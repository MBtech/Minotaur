#include "TumblingWindow.hpp"
#include "user_types.h"

TumblingWindow::TumblingWindow(int window_size){
      windowSize = window_size;
      timeval startTime;
      gettimeofday(&startTime, NULL);
      beginTime = startTime.tv_sec;

}

void TumblingWindow::add_value(std::string key, double value){
      timeval t;
      gettimeofday(&t, NULL);
      unsigned long currentTime = t.tv_sec;
}

void TumblingWindow::get_values(){

}
