#include "sensor.h"
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <mutex>

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include <wiringPi.h>

#define CHAN_CONFIG_SINGLE  8
#define CHAN_CONFIG_DIFF    0

using namespace std;

// Variables globales atómicas
std::atomic<int> contador(0);
std::atomic<int> j(0);
std::atomic<int> h(0);
std::atomic<int> fc(0);

// Pines GPIO
const int GPIO_FLANCO = 27;  // GPIO 27
const int GPIO_HOME = 22;    // GPIO 22
const int GPIO_FC = 17;      // GPIO 17

// Tiempo mínimo entre interrupciones (en microsegundos)
constexpr int DEBOUNCE_TIME_US = 2000;  // 2 ms

// Variables para control de rebote
std::atomic<uint64_t> last_time_27(0);
std::atomic<uint64_t> last_time_22(0);

// Función auxiliar para obtener timestamp en microsegundos
uint64_t micros64() {
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return static_cast<uint64_t>(ts.tv_sec) * 1000000ULL + ts.tv_nsec / 1000;
}

// ISR GPIO 22
void gpio22_isr() {
    uint64_t now = micros64();
    if (now - last_time_22.load() > DEBOUNCE_TIME_US) {
        // Doble lectura para validación
        if (digitalRead(GPIO_HOME) == HIGH) {
            delayMicroseconds(100);  // Pequeña espera
            if (digitalRead(GPIO_HOME) == HIGH) {
                contador++;
                last_time_22.store(now);
            }
        }
    }
}

// ISR GPIO 27
void gpio27_isr() {
    uint64_t now = micros64();
    if (now - last_time_27.load() > DEBOUNCE_TIME_US) {
        if (digitalRead(GPIO_FLANCO) == HIGH) {
            delayMicroseconds(100);  // Pequeña espera
            if (digitalRead(GPIO_FLANCO) == HIGH) {
                j++;
                last_time_27.store(now);
            }
        }
    }
}

void sensor(int& control) {
    // Configuración de pines
    pinMode(GPIO_FLANCO, INPUT);
    pinMode(GPIO_FC, INPUT);
    pinMode(GPIO_HOME, INPUT);

    // Activación de interrupciones
    if ((wiringPiISR(GPIO_FLANCO, INT_EDGE_RISING, &gpio27_isr) < 0) && control == 1) {
        std::cerr << "No se pudo configurar la interrupción para GPIO 27" << std::endl;
        return;
    }

    if ((wiringPiISR(GPIO_HOME, INT_EDGE_RISING, &gpio22_isr) < 0) && control == 1) {
        std::cerr << "No se pudo configurar la interrupción para GPIO 22" << std::endl;
        return;
    }

    // Bucle principal
    while (true) {
        //delayMicroseconds(100);
        if (digitalRead(GPIO_FC) == HIGH){
            fc=1;
        }else
            fc=0;


        /**
        fc = digitalRead(GPIO_FC) == HIGH ? 1 : 0;
        int cont=0;
        while (fc==1 && digitalRead(GPIO_FC) == HIGH){
            cont++;
            if (cont>100)
                break;
        }
        if (cont<100)
            fc=0;
        */
        // Puedes agregar aquí lógica adicional o impresión controlada si control == 1
    }
}


/*
#include "sensor.h"
#include <iostream>
#include <atomic>
#include <thread>
#include <errno.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>


#include <wiringPi.h>
//#include <wiringPiSPI.h>

//#define	TRUE	            (1==1)
//#define	FALSE	            (!TRUE)
#define CHAN_CONFIG_SINGLE  8
#define CHAN_CONFIG_DIFF    0

using namespace std;
//Definición de variables globales
std::atomic<int> contador(0);
std::atomic<int> j(0);
std::atomic<int> h(0);
std::atomic<int> fc(0);

// Pines GPIO
const int GPIO_FLANCO = 27;  // GPIO 17 - para contar flancos de subida
const int GPIO_HOME = 22;    // GPIO 22 - para home
const int GPIO_FC = 17;      // GPIO 0 - para indicar estado de alto


void gpio22_isr() {
    int currentState = digitalRead(22); // Leer el estado actual del GPIO 22  
    if (currentState == HIGH ){
        contador++ ;  // Incrementar j por cada flanco ascendente en GPIO 22
    }   
}


// Función para manejar la interrupción del GPIO 27
void gpio27_isr() {
    //static int lastState = LOW;  // Variable para guardar el estado anterior de GPIO 17
    int currentState = digitalRead(27); // Leer el estado actual del GPIO 27

    if (currentState == HIGH ) {
        j++;  // Incrementar contador por cada flanco ascendente en GPIO 27
    }
   
}



void sensor(int& control) {
   
    
    pinMode(GPIO_FLANCO, INPUT);
    pinMode(GPIO_FC, INPUT);
    pinMode(GPIO_HOME, INPUT);


    if ((wiringPiISR(27, INT_EDGE_RISING, &gpio27_isr) < 0) && control==1) {
        std::cerr << "No se pudo configurar la interrupción para GPIO 27" << std::endl;
        return ;
    }

    
    if ((wiringPiISR(22, INT_EDGE_RISING, &gpio22_isr) < 0) && control==1){
        std::cerr << "No se pudo configurar la interrupción para GPIO 22" << std::endl;
        return ;
    }
    
    while (true) {
    
        // Verificar si GPIO 26 está en alto
        
        if (digitalRead(17) == HIGH) {
            fc = 1;
        } else {
            fc = 0;
        }

        
        if(control==1){
            cout << " Contador j "<< j << " Contador con "<< contador << endl;
        } 
         
        
    }
    
}

*/
