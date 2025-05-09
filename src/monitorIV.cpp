//#define _GNU_SOURCE

#include "monitorIV.h"
#include <wiringPi.h>
#include <iostream>
#include <atomic>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <wiringPiSPI.h>

#define SPI_CHANNEL     0   // Canal SPI (0 o 1)
#define SPI_SPEED       1000000 // Velocidad SPI en Hz
#define CHAN_CONFIG_SINGLE 8 // Configuración para entrada de un solo extremo

std::atomic<float> vm(0);
std::atomic<float> im(0);
std::atomic<int> fd(0);

using namespace std;

// Configuración SPI
void setupSPI() {
    if (wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) < 0) {
        cerr << "Error al configurar SPI" << endl;
        exit(EXIT_FAILURE);
    }
}


// Función para leer un canal del MCP3008
int readADC(int channel) {
    if (channel < 0 || channel > 7) {
        cerr << "Canal fuera de rango (0-7)" << endl;
        return -1;
    }

    unsigned char buffer[3] = {1}; // Start bit
    buffer[1] = (CHAN_CONFIG_SINGLE + channel) << 4;
    buffer[2] = 0;

    wiringPiSPIDataRW(SPI_CHANNEL, buffer, 3);

    return ((buffer[1] & 3) << 8) + buffer[2]; // Últimos 10 bits
}




void monitorIV(){
    setupSPI(); // Configura el SPI
    
    double r1 = (30.2)*1000;  
    double r2 = (7.2)*1000;  
    double SENSIBILITY = 0.185;
    double vaux;
    double iaux;

    while (true) {
       
        int va = readADC(0);
        int ia = readADC(1);
        int fd = readADC(2);
        

        //cout << "Canal 0: " << va << "  |  Canal 1: " << ia << endl;
        vaux=va/1024.0;
        iaux=ia/1024.0;
        vm = 3.3*vaux*(r1+r2)/r2;                
        im = ((5*iaux)-2.5)/SENSIBILITY;

        //fdc=fdc*1;
        cout << "dentro " << vm << "\t" << im << "\t"<< fd << endl;
        
        delay(500); // Espera 500 ms
    }
    


}



