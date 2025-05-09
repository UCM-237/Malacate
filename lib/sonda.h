#ifndef SONDA_H
#define SONDA_H


#include <atomic>


extern std::atomic<float> prof;


void sonda();
//extern volatile bool keepRunning;
//void signalHandler(int);

#endif  // POWER_H