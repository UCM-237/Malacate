#include <wiringPi.h>
#include <iostream>
#include <thread>
#include <chrono>
#include "ambulanceSiren.h"
#include <unistd.h> // para usleep

#define BUZZER_PIN 21  // GPIO 21





// WiringPi pin number correspondiente a GPIO21 (BCM 21)
//#define BUZZER_PIN 25



void playTone(int pin, int frequency, int duration_ms) {
    int delay_us = 1000000 / (2 * frequency); // medio período

    int cycles = (frequency * duration_ms) / 1000;
    for (int i = 0; i < cycles; ++i) {
        digitalWrite(pin, HIGH);
        delayMicroseconds(delay_us);
        digitalWrite(pin, LOW);
        delayMicroseconds(delay_us);
    }
}


void ambulanceSiren() {
    int low_freq = 600;
    int high_freq = 1200;
    int step = 20;
    int step_duration = 10; // ms por paso
    int j=0;
    pinMode(BUZZER_PIN, OUTPUT);
    
    while (j<=5) {
        // Subida de frecuencia
        for (int freq = low_freq; freq <= high_freq; freq += step) {
            playTone(BUZZER_PIN, freq, step_duration);
        }
        // Bajada de frecuencia
        for (int freq = high_freq; freq >= low_freq; freq -= step) {
            playTone(BUZZER_PIN, freq, step_duration);
        }
        j++;
    }
}


/*
int main() {
    if (wiringPiSetup() == -1) {
        std::cerr << "Error inicializando wiringPi\n";
        return 1;
    }

    pinMode(BUZZER_PIN, OUTPUT);
    std::cout << "Sirena de ambulancia en pin wiringPi #" << BUZZER_PIN << "\n";
    ambulanceSiren(BUZZER_PIN);

    return 0;
}

*/
