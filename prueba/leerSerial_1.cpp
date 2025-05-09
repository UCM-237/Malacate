#include <wiringPi.h>
#include <wiringSerial.h>
#include <iostream>
#include <string>
#include <unistd.h>

#define MAX_BUFFER_SIZE 256

void readSerial(int fd) {
    char dat;
    char buffer[MAX_BUFFER_SIZE]; // Buffer para acumular datos
    int buffer_index = 0; // Índice del buffer

    while (true) {
        if (serialDataAvail(fd)) {
            dat = serialGetchar(fd);
            //std::cout << dat << std::endl;
            
            
            // Acumular los datos en el buffer
            if (buffer_index < MAX_BUFFER_SIZE - 1) {
                buffer[buffer_index++] = dat;
                buffer[buffer_index] = '\0'; // Mantener buffer como cadena
            }

            // Verificar si ya tenemos al menos dos bytes
            if (buffer_index >= 2) {
                if (buffer[0] == 'P' && buffer[1] == 'U') {
                    std::cout << "SONDA UP" << std::endl;
                } else if (buffer[0] == 'P' && buffer[1] == 'D') {
                    std::cout << "SONDA DOWN" << std::endl;
                }
                else if (buffer[0] == 'P' && buffer[1] == 'C') {
                    std::cout << "CENTRO" << std::endl;
                }
                // Reiniciar buffer
                buffer_index = 0;
            }

            // Respuesta al dispositivo
            serialPutchar(fd, 'o');
        }
    }
}

int main(int argc, char *argv[]) {
    int fd;
    std::string default_port = "/dev/serial0";
    std::string port = default_port;

    if (argc > 1) {
        port = argv[1];
        std::cout << "Usando puerto: " << port << std::endl;
    } else {
        std::cout << "Usando puerto predeterminado: " << port << std::endl;
    }

    wiringPiSetup();
    fd = serialOpen(port.c_str(), 115200);
    if (fd < 0) {
        std::cerr << "No se pudo abrir el puerto" << std::endl;
        return -1;
    }

    readSerial(fd);

    return 0;
}
