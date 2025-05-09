#ifndef MONITORIV_H
#define MONITORIV_H

#include <atomic>

extern std::atomic<float> vm;  
extern std::atomic<float> im;        
extern std::atomic<int> fd;        

void monitorIV();

#endif