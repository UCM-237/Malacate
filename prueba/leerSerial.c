#include <wiringPi.h>
#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringSerial.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 256

int main(int argc, char *argv[]) {
    int fd;
    char dat;
    char buffer[MAX_BUFFER_SIZE]; // Buffer para acumular datos
    int buffer_index = 0; // Índice del buffer
    char default_port[] = "/dev/serial0"; // Puerto serie predeterminado
    char *port = default_port; // Asignar el puerto por defecto

    if (argc > 1) {
        port = argv[1];
        printf("Usando puerto: %s\n", port);
    } else {
        printf("Usando puerto predeterminado: %s\n", port);
    }

    wiringPiSetup();
    fd = serialOpen(port, 115200); // Puerto serie
    if (fd < 0) {
        printf("No se pudo abrir el puerto\n");
        return -1;
    }

    for (;;) {
        
        if (serialDataAvail(fd)) {
            dat = serialGetchar(fd);
			//printf("dat = %c \n", dat);
            

            // Acumular los datos en el buffer
            if (buffer_index < MAX_BUFFER_SIZE - 1) {
                buffer[buffer_index++] = dat;
                buffer[buffer_index] = '\0'; // Mantener buffer como cadena
            }

            // Verificar si ya tenemos al menos dos bytes
            if (buffer_index >= 2) {
                if (buffer[0] == 'P' && buffer[1] == 'U') {
                    printf("SONDA UP'\n");
                }
				else if(buffer[0] == 'P' && buffer[1] == 'D'){
					printf("SONDA DOWN'\n");
				}

		/*else{
		printf("WAITING \n");
		sleep(1);
		}*/
		// Reiniciar buffer (opcional: ajusta según la lógica que desees para más datos)
                buffer_index = 0;
            }

            // Respuesta al dispositivo
            serialPutchar(fd, 'o');
        }
    }

    return 0;
}