#ifndef GENERAR_ONDA_CUADRADA_H
#define GENERAR_ONDA_CUADRADA_H

extern std::atomic<double> vm;  
extern std::atomic<double> im;        
//extern std::atomic<int> fc;

void generarOndaCuadrada(int frecuencia, int duty, std::atomic<int>& control);
//void detenerOnda(); // Función para detener el hilo

#endif // GENERAR_ONDA_CUADRADA_H
