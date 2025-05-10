// serial_reader.h

#ifndef SERIAL_READER_H
#define SERIAL_READER_H

#include <wiringPi.h>
#include <wiringSerial.h>
#include <thread>  // Para el uso de hilos
#include <atomic>

// Variables de tiempo
extern std::string sessionTimestamp;         
extern std::atomic<bool> sessionReady;

// Perfil actual de sonda
extern std::atomic<uint8_t> currentProfile;

// Función para leer datos desde el puerto serial y actualizar las variables
void leerSerial(int &st, int &sp, int &tem, float& pr, int& tm, int& ft, int& home);

// Función para iniciar la lectura en un hilo
void iniciarLecturaEnHilo(int &st, int &sp, int &tem, float& pr, int& tm, int& ft,  int& home);

#endif


