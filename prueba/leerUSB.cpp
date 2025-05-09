#include <fstream>
#include <thread>
#include <string>
#include <sstream>
#include <atomic>
#include <unistd.h>
#include <termios.h>
#include <iostream>

using namespace std;

void leerUSB(int& st, int& sp, int& pr, int& tm, int& tem, atomic<bool>& modoTransmision) {
    while (true) {
        // Modo transmisión (enviar secuencia)
        if (modoTransmision.load()) {
            // Enviar la secuencia 0,0,0,0,0
            std::ofstream usb("/dev/ttyUSB0");  // Abrimos el puerto USB en modo escritura
            if (!usb.is_open()) {
                std::cerr << "Error: No se pudo abrir el puerto USB para transmitir" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
            usb << "0,0,0,0,0" << endl;  // Transmitir la secuencia
            cout  << "Imprimiendo datos: 0,0,0,0,0" << endl;  // Imprimir la secuencia enviada
            usb.close();
            std::this_thread::sleep_for(std::chrono::seconds(1));  // Esperar antes de transmitir nuevamente
        } 
        else {
            // Modo recepción (leer los datos)
            std::ifstream usb("/dev/ttyUSB0");  // Abrir puerto USB en modo lectura
            if (!usb.is_open()) {
                std::cerr << "Error: No se pudo abrir el puerto USB" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }

            std::string line;
            if (std::getline(usb, line)) {
                std::istringstream iss(line);
                std::string campo;
                int campo_num = 0;
                while (std::getline(iss, campo, ',')) {
                    switch (campo_num) {
                        case 0: st = std::stoi(campo); break;
                        case 1: sp = std::stoi(campo); break;
                        case 2: pr = std::stoi(campo); break;
                        case 3: tm = std::stoi(campo); break;
                        case 4: tem = std::stoi(campo); break;
                    }
                    campo_num++;
                }
                // Imprimir los datos recibidos en tiempo real
                cout << "Datos recibidos: " << st << ", " << sp << ", " << pr << ", " << tm << ", " << tem << endl;
            }
            usb.close();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Leer cada 100 ms
        }
    }
}

int main() {
    int st, sp, pr, tm, tem;
    int modo=1;
    atomic<bool> modoTransmision(false);  // Empezamos en modo recepción (cambiar a true para modo transmisión)
    
    // Decidir el modo: transmisión o recepción
    if (modo==1) {
        modoTransmision.store(true);
        cout << "Modo Transmisión activado. Enviando secuencia 0,0,0,0,0..." << endl;
    } else {
        modoTransmision.store(false);
        cout << "Modo Recepción activado. Recibiendo datos en tiempo real..." << endl;
    }

    // Iniciamos un hilo para leer el puerto USB
    std::thread tLeerUSB(leerUSB, std::ref(st), std::ref(sp), std::ref(pr), std::ref(tm), std::ref(tem), std::ref(modoTransmision));

    // Esperamos a que el hilo termine (en este caso no termina, ya que es un ciclo infinito)
    tLeerUSB.join();
    
    return 0;
} 