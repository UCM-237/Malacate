#ifndef CONTROL_H
#define CONTROL_H

#include <atomic>
#include <thread>
#include <chrono>

void alternarControl(int time, std::atomic<int>& control);

#endif // CONTROL_H
