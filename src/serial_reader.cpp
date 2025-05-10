#include "serial_reader.h"
#include "sonda.h"
#include <iostream>
#include <string.h>
#include <cstring>
#include <thread>  // Para usar hilos
#include <atomic>

#include <fstream>
#include <iomanip>
#include <sstream>
#include <filesystem>

#define MAX_BUFFER_SIZE 256

// Variables de tiempo
std::string sessionTimestamp;          // Variable para almacenar el tiempo global
std::atomic<bool> sessionReady(false); // Indica si ya está disponible (necesita gps fix)

// Perfil actual de sonda
std::atomic<uint8_t> currentProfile(0);

// std::atomic<float> prof(0); // Profundidad actual de sonda


// Estructura del mensaje de Log
#pragma pack(push, 1)
struct LogMessage {
    uint32_t time;
    int32_t  lat;
    int32_t  lon;
    uint16_t Ah;
    uint8_t profile;
    uint16_t time_week;
    uint32_t time_tow;
    uint32_t  orient_raw;
    uint8_t  static_control;
    uint32_t  theta;
    int16_t  throttle_L;
    int16_t  throttle_R;
    int32_t  x;
    int32_t  y;
    uint8_t  utm_zone;
    int32_t  u_raw;
    int32_t  v_raw;
    int32_t  du_raw;
    int32_t  dv_raw;
};
#pragma pack(pop)

// Funcion para calcular la fecha actual con el tiempo del GPS
std::string gpsTimeToString(uint16_t week, uint32_t tow) {
    
    // Epoch GPS: 6 enero 1980
    constexpr time_t gps_epoch = 315964800; // en segundos desde Unix epoch (1970-01-01)
    
    // Segundos totales desde epoch GPS
    uint64_t gps_seconds = static_cast<uint64_t>(week) * 7 * 24 * 3600 + (int)(tow/1000);
    
    // Convertir a epoch Unix
    time_t unix_time = gps_epoch + gps_seconds;

    // Pasar a struct tm (UTC)
    std::tm* tm_ptr = gmtime(&unix_time);

    // Formatear a string YYYYMMDD_HHMMSS
    std::ostringstream oss;
    oss << std::put_time(tm_ptr, "%Y_%m_%d_%H_%M_%S");
    return oss.str();
}


// Crea el CSV para guardar el log local
std::ofstream createCSV_log(std::string time_string) {
    
    // No estoy comprobando si existe la carpeta

    // // Fecha/hora actual
    // auto t = std::time(nullptr);
    // std::tm tm = *std::localtime(&t);

    std::ostringstream filename;
    filename << "logs/nav/log_"
             << time_string
             << ".csv";

    std::ofstream file(filename.str(), std::ios::out | std::ios::app);
    file << std::fixed << std::setprecision(8);
    if (!file.is_open()) {
        throw std::runtime_error("No se pudo crear el archivo CSV");
    }

    // Cabecera
    file << "time,lat,lon,Ah,profile,time_UTC,orient_raw,theta,static_control,"
         << "throttle_L,throttle_R,x,y,utm_zone,u_raw,v_raw,du_raw,dv_raw\n";

    return file;
}


void appendToCSV(std::ofstream& file, const LogMessage& msg) {

    std::string timeUTC = gpsTimeToString(msg.time_week, msg.time_tow);

    file << (float) msg.time/1000 << "," 
        << (double) msg.lat*1e-7 << ","
        << (double) msg.lon*1e-7 << ","
        << (float) msg.Ah/100 << ","
        << (int) msg.profile << ","
        << timeUTC << ","
        << (float) msg.orient_raw / 4096 << ","  
        << (float) msg.theta / 4096 << ","       
        << (int) msg.static_control << ","
        << msg.throttle_L << ","
        << msg.throttle_R << ","
        << (float) msg.x / 100.0f << ","     
        << (float) msg.y / 100.0f << ","     
        << (int) msg.utm_zone << ","
        << (float) msg.u_raw / 100.0f << "," 
        << (float) msg.v_raw / 100.0f << ","
        << (float) msg.du_raw / 1000.0f << "," 
        << (float) msg.dv_raw / 1000.0f        
        << "\n";

    file.flush(); // asegura que se escriba inmediatamente
}


// Para leer el mensaje de log
bool parseLogMessage(const uint8_t* data, size_t len, LogMessage& out) {
    if (len < sizeof(LogMessage)) {
        return false; // mensaje incompleto
    }

    std::memcpy(&out, data+4, sizeof(LogMessage));
    return true;
}


void sendOKMessage(int fd, uint16_t depth, int ft, int home) {
    
    // printf("Profundidad a enviar: %d \n", depth);
    uint8_t message[13];
    uint16_t timestamp = 0; // Ignoramos el tiempo
    message[0] = 'R'; // Byte de sincronía
    message[1] = 'O'; // Tipo de mensaje
    message[2] = (timestamp >> 8) & 0xFF;
    message[3] = timestamp & 0xFF;
    message[4] = ft; // Error 1, para realimentacion
    message[5] = home;  // posicion home de malacate
    message[6] = (depth >> 8) & 0xFF;
    message[7] = depth & 0xFF;
    message[8] = 0;
    message[9] = 0;
    message[12] = '\n';

    // printf("\n Mensaje enviado: ");
    for (int i = 0; i < 13; i++) {
        // printf("%02X ", message[i]);
        serialPutchar(fd, message[i]);
    }
    // printf("\n");
}


// prof es la profundidad actual (la que mide el sensor)
// pr es la maxima (la que se recibe por el puerto serie)
void leerSerial(int &st, int &sp, int &tem, float& pr, int& tm, int &ft, int &home) {
    int serial_port = serialOpen("/dev/serial0", 115200);  // Abre el puerto serial en el dispositivo con velocidad 115200 baudios
    if (serial_port == -1) {
        std::cerr << "Error al abrir el puerto serial." << std::endl;
        return;
    }

    char dat;
    int msg_len = 0;
    char buffer[MAX_BUFFER_SIZE]; // Buffer para acumular datos
    int buffer_index = 0; // Índice del buffer

    std::ofstream logFile; // Fichero con los datos de navegacion (se crea mas adelante)

    while (true) {
        // Leer un byte
        dat = serialGetchar(serial_port);
        // printf("%02X ", (unsigned char)dat);
        
        // Si es inicio de mensaje
        if (dat == 0x50){   // P
            // std:: cout <<  "Reinicio de Mensaje. Buffer Index: " << buffer_index << std:: endl;
            buffer_index = 0;
            msg_len = 0; // Por si acaso
        }

        // Almacena bytes
        if (buffer_index < MAX_BUFFER_SIZE - 1) {
            buffer[buffer_index] = dat;
            // std::cout << "\t Buffer Index = " << buffer_index << std::endl;
        } 
        else{   // Por si satura el buffer (no deberia)
            buffer_index = 0;
        }

        // Analiza el tipo de mensaje
        if (buffer_index == 1){
            if(buffer[buffer_index] == 'L'){    // Log message
                msg_len = 65;   // REVISAR
            }
            else{   // Sonda Message
                msg_len = 12;
                // printf("\n Tipo de mensaje normal %02X\n", buffer[buffer_index]);
            }
        }


        if((msg_len > 0) && (buffer_index == msg_len)){

            // printf("%02X ", (unsigned char)buffer[buffer_index]);
            

            // Procesar mensaje completo -------------------------------
            // Si es el mensaje de log, se parsea y guarda directamente
            if (strncmp(buffer, "PL", 2) == 0) {
                // uint16_t time_sec = (uint8_t)buffer[2] << 8 | (uint8_t)buffer[3];
                LogMessage msg;
                if (parseLogMessage((uint8_t*)buffer, sizeof(LogMessage), msg)) {
                    if (!sessionReady.load()){
                        printf("Creating CSV log file ... \n");
                        sessionTimestamp = gpsTimeToString(msg.time_week, msg.time_tow);
                        sessionReady.store(true);
                        logFile = createCSV_log(sessionTimestamp);
                    }

                    // Actualizamos perfil actual global (para la sonda)
                    currentProfile.store(msg.profile);

                    // Crea el CSV (uno por sesion)
                    appendToCSV(logFile, msg);
                }
            } 
            
            // Si es de control, se analiza cual y se cambia lo correspondiente
            else if (strncmp(buffer, "PU", 2) == 0) {
                //std:: cout "Estoy aqui  " << std::endl;
                uint16_t profundidad =  (buffer[7] | (buffer[6] << 8)) * (buffer[5] ? -1 : 1);
                pr = (float) profundidad/1000;   // Establece la profundidad maxima de bajada
                st = 0;
                sp = 1;
                tem = 0;
                
                
            } else if (strncmp(buffer, "PD", 2) == 0) {
                //std:: cout "Estoy aqui  bajando" << std::endl;
                uint16_t profundidad =  (buffer[7] | (buffer[6] << 8)) * (buffer[5] ? -1 : 1);
                pr = (float) profundidad/1000;   // Establece la profundidad maxima de bajada
                st = 1;
                sp = 0;
                tem = 0;


            } else if (strncmp(buffer, "PC", 2) == 0) {
                st = 0;
                sp = 0;
                tem = 0;
                uint16_t profundidad =  (buffer[7] | (buffer[6] << 8)) * (buffer[5] ? -1 : 1);
                pr = (float) profundidad/1000;   // Establece la profundidad maxima de bajada
                // uint16_t tiempo = buffer[9] | (buffer[8] << 8); 

                // printf("Profuncidad Recibida = %f m\n", ((float)(profundidad))/1000);
                // printf("Profuncidad Recibida = %f m\n", pr);
                //std:: cout <<  "Parado. Profundidad  " <<  prof << std:: endl;
                

                // web point programar
            } else if (strncmp(buffer, "PA", 2) == 0) {
                uint16_t profundidad =  (buffer[7] | (buffer[6] << 8)) * (buffer[5] ? -1 : 1);
                pr = (float) profundidad/1000;   // Establece la profundidad maxima de bajada
                st = 0;
                sp = 0;
                tem = 1;
            } else if (strncmp(buffer, "PM", 2) == 0) {
                st = 1;
                sp = 0;
                tem = 1; 
                uint16_t profundidad =  (buffer[7] | (buffer[6] << 8)) * (buffer[5] ? -1 : 1);  
                pr = (float) profundidad/1000;   // Establece la profundidad maxima de bajada

            } else if (strncmp(buffer, "PB", 2) == 0) {
                // Otro comando
            } else if (strncmp(buffer, "PE", 2) == 0) {
                uint16_t profundidad =  (buffer[7] | (buffer[6] << 8)) * (buffer[5] ? -1 : 1);
                pr = (float) profundidad/1000;   // Establece la profundidad maxima de bajada
                // uint16_t tiempo = buffer[9] | (buffer[8] << 8);
                st = 1;
                sp = 1;
                tem = 0;
            }

            buffer_index = 0;
            msg_len = 0;
            // printf("\n");
        }
        else{
            buffer_index++;
        }

        // ENVIO DE MENSAJES A PAPARAZZI
        // Hay que mandar prof*1000 (en mm) 
        // int prof1 = ((rand()%20)*1000); // TEST, BORRAR
        uint16_t prof_send = (uint16_t)(prof*1000);
        // printf("Profundidad Enviada: %d \n", prof_send);
        sendOKMessage(serial_port, prof_send, ft, home);

    }

    serialClose(serial_port); // Cerrar el puerto
}

// Esta función ejecuta `leerSerial()` en un hilo separado
void iniciarLecturaEnHilo(int &st, int &sp, int &tem, float &pr, int &tm, int &ft, int &home) {
    std::thread hiloLectura(leerSerial, std::ref(st), std::ref(sp), std::ref(tem), std::ref(pr), std::ref(tm), std::ref(ft), std::ref(home));  // Crear un hilo que ejecute leerSerial
    hiloLectura.detach();  // Desvincula el hilo para que se ejecute independientemente
}


