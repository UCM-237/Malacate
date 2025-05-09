#include <wiringPi.h>
#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringSerial.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#define MAX_BUFFER_SIZE 256

class SerialHandler {
public:
    SerialHandler(const char* port = "/dev/serial0", int baud_rate = 115200) {
        wiringPiSetup();
        fd = serialOpen(port, baud_rate);
        if (fd < 0) {
            std::cerr << "No se pudo abrir el puerto\n";
            exit(EXIT_FAILURE);
        }
    }

    void processSerialData() {
        char dat;
        char buffer[MAX_BUFFER_SIZE]; // Buffer para acumular datos
        int buffer_index = 0; // Índice del buffer

        for (;;) {
            if (serialDataAvail(fd)) {
                dat = serialGetchar(fd);

                // Verificar si es el fin de un mensaje
                if (dat == '\n') {
                    buffer[buffer_index] = '\0'; // Finalizar cadena

                    // Procesar mensaje completo
                    if (strncmp(buffer, "PU", 2) == 0) {
                        std::cout << "SONDA UP\n";
                    } else if (strncmp(buffer, "PD", 2) == 0) {
                        std::cout << "SONDA DOWN\n";
                    } else if (strncmp(buffer, "PC", 2) == 0) {
                        std::cout << "SONDA CENTER\n";
                    } else if (strncmp(buffer, "PA", 2) == 0) {
                        std::cout << "MODE AUTO\n\n";
                    } else if (strncmp(buffer, "PB", 2) == 0) {
                        std::cout << "MODE MANUAL\n\n";
                    }

                    // Reiniciar el buffer para el siguiente mensaje
                    buffer_index = 0;
                } else {
                    // Acumular caracteres en el buffer
                    if (buffer_index < MAX_BUFFER_SIZE - 1) {
                        buffer[buffer_index++] = dat;
                    }
                }

                // Respuesta al dispositivo
                serialPutchar(fd, 'o');
            }
        }
    }

private:
    int fd;
};

int main(int argc, char *argv[]) {
    const char* port = (argc > 1) ? argv[1] : "/dev/serial0";
    SerialHandler serialHandler(port);
    serialHandler.processSerialData();
    return 0;
}
