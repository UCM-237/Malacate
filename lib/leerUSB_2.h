#ifndef LEER_USB_H
#define LEER_USB_H

#include <mutex>
#include <atomic>

using namespace std;
// Declaración de la función de lectura desde USB
void leerUSB(int& st, int& sp, int& pr, int& tm, int& tem, atomic<bool>& modoTransmision);


#endif // LEER_USB_H
