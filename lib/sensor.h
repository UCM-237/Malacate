#ifndef SENSOR_H
#define SENSOR_H

#include <atomic>

// Variables globales
extern std::atomic<int> contador;       // Contador de flancos de subida en GPIO 17
extern std::atomic<int> h;              // Indicador de estado de GPIO 22
extern std::atomic<int> j;              // Indicador de estado de GPIO 27
extern std::atomic<int> fc;             // Indicador de estado de GPIO 17


// Declaración de la función sensor3
void sensor(int& control);

#endif  // SENSOR_H
