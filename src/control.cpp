#include "../lib/control.h"

void alternarControl(int time,std::atomic<int>& control) {
    while (true) {
        control = 1; // Activar
        std::this_thread::sleep_for(std::chrono::seconds(time));
        control = 0; // Desactivar
        std::this_thread::sleep_for(std::chrono::seconds(time));
    }
}
