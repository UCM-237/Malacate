
#ifndef CASA_H
#define CASA_H
#include <atomic>

extern std::atomic<int> h;        // Indicador de estado de GPIO 22
extern std::atomic<int> j;        // Indicador de estado de GPIO 22
extern std::atomic<int> contador;  // Contador de flancos de subida en GPIO 11
//extern std::atomic<int> control;  // Contador de flancos de subida en GPIO 11

//void motorForward(bool& st1, bool& n1, bool& n2);
void motorForward();
//void motorBackward(bool& st1, bool& n1, bool& n2);
void motorBackward();
//void stopMotor(bool& st1, bool& n1, bool& n2);
void stopMotor();

//bool isHomePosition();



// Declaración de la función power para ejecutarse en un hilo
void casa(int& control1, bool& st1, bool& n1, bool& n2, std::atomic<int>& control, std::atomic<int>& contador , std::atomic<int>& j);

#endif  // POWER_H