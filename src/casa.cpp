#include "casa.h"
#include <wiringPi.h>
#include <iostream>
#include <atomic>
#include <chrono>
#include <thread>

using namespace std;

// Pines GPIO en modo BCM
const int GPIO_ST = 6;
const int GPIO_N1 = 0;
const int GPIO_N2 = 5;

// Función para encender el motor adelante

void motorForward() {
        bool st = true;
        bool n1 = false ;
        bool n2 = true;

        // Configurar los pines como salida (esto se hace una sola vez)
        pinMode(GPIO_ST, OUTPUT);
        pinMode(GPIO_N1, OUTPUT);
        pinMode(GPIO_N2, OUTPUT);

        // Configurar cada GPIO en alto si el valor es true, y en bajo si es false
        digitalWrite(GPIO_ST, st ? HIGH : LOW);
        digitalWrite(GPIO_N1, n1 ? HIGH : LOW);
        digitalWrite(GPIO_N2, n2 ? HIGH : LOW);
}







// Función para encender el motor en dirección contraria


void motorBackward() {
        bool st = true;
        bool n1 = true;
        bool n2 = false; 
        // Configurar los pines como salida (esto se hace una sola vez)
        pinMode(GPIO_ST, OUTPUT);
        pinMode(GPIO_N1, OUTPUT);
        pinMode(GPIO_N2, OUTPUT);

        // Configurar cada GPIO en alto si el valor es true, y en bajo si es false
        digitalWrite(GPIO_ST, st ? HIGH : LOW);
        digitalWrite(GPIO_N1, n1 ? HIGH : LOW);
        digitalWrite(GPIO_N2, n2 ? HIGH : LOW);
}




// Función para detener el motor

void stopMotor() {
    bool st = false;
    bool n1 =  true;
    bool n2 =  true;
    // Configurar los pines como salida (esto se hace una sola vez)
    pinMode(GPIO_ST, OUTPUT);
    pinMode(GPIO_N1, OUTPUT);
    pinMode(GPIO_N2, OUTPUT);

    // Configurar cada GPIO en alto si el valor es true, y en bajo si es false
    digitalWrite(GPIO_ST, st ? HIGH : LOW);
    digitalWrite(GPIO_N1, n1 ? HIGH : LOW);
    digitalWrite(GPIO_N2, n2 ? HIGH : LOW);   
}








/*
// Función para verificar si el sensor de posición está activado
bool isHomePosition() {
    return contador == HIGH;  // Devuelve true si el sensor detecta el "home"
}





// Función para buscar el home
void searchHome(bool forward, bool& st1, bool& n1, bool& n2, atomic<int>& control) {

    if (forward) {
        motorForward(st1,n1,n2);  // Mueve el motor hacia adelante
        while (!isHomePosition()) {  // Mientras no se detecte el "home"
            delay(100);  // Retraso para permitir que el motor se mueva
        }
    } else {
        
        motorBackward(st1,n1,n2);  // Mueve el motor hacia atrás


        while (!isHomePosition()) {  // Mientras no se detecte el "home"
            delay(100);  // Retraso para permitir que el motor se mueva
        }
    }
    stopMotor(st1,n1,n2);  // Detiene el motor cuando se llega al "home"

}


// Función principal para controlar el motor y el sensor
void controlMotorf(bool& st1, bool& n1, bool& n2, atomic<int>& control) {
    if (control == 1) {
        // Intentar buscar el "home" hacia adelante primero
        searchHome(true, st1, n1,n2,  control);

        // Si no se encuentra el "home" adelante, buscar hacia atrás
        if (!isHomePosition()) {
            std::cout << "No encontrado el home adelante, buscando hacia atrás..." << std::endl;
            searchHome(false, st1, n1,n2,  control);
        }
        
        // El motor se detiene una vez que se llega al "home"
        stopMotor(st1,n1,n2);
    } 
    else {
        stopMotor(st1,n1,n2);  // Detiene el motor si control es 0
    }
    this_thread::sleep_for(chrono::milliseconds(1000));   
}



void controlMotorb(int &control1,bool& st1, bool& n1, bool& n2, atomic<int>& control){  
    while (control==1){
        //std::cout << "Contador  "<< contador << std::endl;
        motorBackward(st1, n1, n2);
        if(contador==2){
            control=0;
            control1=0;
            stopMotor(st1,n1,n2);
        }        
    }        
}




void casa(int& control1, bool& st1, bool& n1, bool& n2, atomic<int>& control, atomic<int>& contador, atomic<int>& j  ){
    controlMotorf(st1, n1,  n2, control);  // Controlar el motor basado en el valor de 'control'
    //controlMotorb(control1,st1, n1,  n2, control);
    control1=0;
    control=0;
    contador=0;
    j=0;
    return ;        
}

*/