#include <wiringPi.h>
#include <iostream>
#include <unistd.h>
#include <atomic>
#include <thread>
#include "../lib/generarOndaCuadrada.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <wiringPiSPI.h>

#define CHAN_CONFIG_SINGLE  8
#define CHAN_CONFIG_DIFF    0


using namespace std;

std::atomic<double> vm(0);
std::atomic<double> im(0);
//std::atomic<int> fc(0);

static int myFd ;

void loadSpiDriver()
{
    if (system("gpio load spi") == -1)
    {
        fprintf (stderr, "Can't load the SPI driver: %s\n", strerror (errno)) ;
        exit (EXIT_FAILURE) ;
    }
}

void spiSetup (int spiChannel)
{
    if ((myFd = wiringPiSPISetup (spiChannel, 1000000)) < 0)
    {
        fprintf (stderr, "Can't open the SPI bus: %s\n", strerror (errno)) ;
        exit (EXIT_FAILURE) ;
    }
}

int myAnalogRead(int spiChannel,int channelConfig,int analogChannel)
{
    if(analogChannel<0 || analogChannel>7)
        return -1;
    unsigned char buffer[3] = {1}; // start bit
    buffer[1] = (channelConfig+analogChannel) << 4;
    wiringPiSPIDataRW(spiChannel, buffer, 3);
    return ( (buffer[1] & 3 ) << 8 ) + buffer[2]; // get last 10 bits
}





void generarOndaCuadrada(int frecuencia, int duty, atomic<int>& control) {
    int gpioPin = 13; // Pin GPIO 13

    int loadSpi=FALSE;
    //int analogChannel=0;
    int spiChannel=0;
    int channelConfig=CHAN_CONFIG_SINGLE;
    float r1 = (29.4)*1000; // 
    float r2 = (7.4)*1000; // 
    float SENSIBILITY = 0.185;
    float vc;
    float ic;
   
    if(loadSpi==TRUE)
        loadSpiDriver();

    //wiringPiSetup () ;
    spiSetup(spiChannel);
    pinMode(gpioPin, PWM_OUTPUT); // Configura el pin como salida
    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(1024); // Rango del PWM
    pwmSetClock(20); // Establecer el reloj. Esta configuración afecta la frecuencia total


    
    
    
    while (true) {
        if (control == 0) {
            pwmWrite(gpioPin, 0);
            sleep(0.2);
        } else {
            pwmWrite(gpioPin, (int)(duty*10.24));
            

            vc = myAnalogRead(spiChannel,channelConfig, 0);
	        ic = myAnalogRead(spiChannel,channelConfig, 1);
            //fc = myAnalogRead(spiChannel,channelConfig, 2);
            vc=(vc/1023.0);
            vc=3.3*vc*(r1+r2)/r2;
            ic= (ic*(5.0/1023.0)-2.5)/SENSIBILITY;
            //cout << " V:  " << vc << endl;
            //cout << " I:  " << ic << endl;
            vm=vc;
            im=ic;
            

            

        }
           
    }
    digitalWrite(gpioPin, LOW); // Apaga el pin al salir
}
