#ifndef AUTOMANUAL_H
#define AUTOMANUAL_H
#include <atomic>

extern std::atomic<int> h;          // Indicador de estado de GPIO 22
extern std::atomic<int> j;          // Indicador de estado de GPIO 22
extern std::atomic<int> contador;   // Contador de flancos de subida en GPIO 11
extern std::atomic<int> control;    // Contador de flancos de subida en GPIO 11
extern std::atomic<int> fc;         // Contador de flancos de subida en GPIO 11


// Declaración de la función power para ejecutarse en un hilo
void autoManual(int t,int nv, int &s, int& control1, bool& st1, bool& n1, bool& n2, std::atomic<int>& control, std::atomic<int>& contador, std::atomic<int>& j, std::atomic<int>& f, cint &ft );

#endif

