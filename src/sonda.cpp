
// sonda.cpp
#include "sonda.h"
#include "serial_reader.h"
#include <modbus/modbus.h>
#include <iostream>
#include <fstream>
#include <csignal>
#include <unistd.h>
#include <cstring>
#include <cstdint>
#include <chrono>
#include <ctime>
#include <cmath>
#include <vector>

#include <iomanip>
#include <sstream>
#include <filesystem>

#define DEVICE     "/dev/ttyUSB0"
#define BAUDRATE   9600
#define MODBUS_ID  80

std::atomic<float> prof(0);

const int direcciones[] = {
    40011, 40029, 40031, 40033, 40035, 40037, 40039
};

volatile bool keepRunning = true;

/*
void signalHandler(int) {
    std::cout << "\n[!] Interrupción recibida. Terminando...\n";
    keepRunning = false;
}
*/

float read_float_inverse(modbus_t* ctx, int addr) {
    uint16_t regs[2];
    if (modbus_read_registers(ctx, addr - 40001, 2, regs) != 2) {
        //std::cerr << "Error leyendo en dirección " << addr << ": "
                  //<< modbus_strerror(errno) << std::endl;
        return NAN;
    }
    uint16_t swapped[2] = { regs[1], regs[0] };
    float result;
    std::memcpy(&result, swapped, sizeof(float));
    return result;
}

// void guardar_datos_csv(const std::vector<float>& datos) {
//     std::ofstream archivo("datos_sensor.csv", std::ios::app);

//     if (!archivo.is_open()) {
//         std::cerr << "No se pudo abrir el archivo CSV para escritura.\n";
//         return;
//     }

//     static bool encabezado_escrito = false;
//     if (!encabezado_escrito) {
//         archivo << "Tiempo,Profundidad,Temperatura,pH,DO_SAT,DO,Blue,Chl\n";
//         encabezado_escrito = true;
//     }

//     auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
//     archivo << std::ctime(&now);
//     for (const auto& valor : datos) {
//         archivo << "," << valor;
//     }
//     archivo << "\n";

//     archivo.close();
// }


std::ofstream createCSV_probe(std::string time_string) {
    
    // No estoy comprobando si existe la carpeta

    // Fecha/hora actual
    // auto t = std::time(nullptr);
    // std::tm tm = *std::localtime(&t);

    std::ostringstream filename;
    // printf("Profile %d \n", profile);
    filename << "logs/sonda/sonda_"
             << time_string
             << ".csv";

    std::ofstream file(filename.str(), std::ios::out | std::ios::app);
    if (!file.is_open()) {
        throw std::runtime_error("No se pudo crear el archivo CSV");
    }
    else{
        printf("Created CSV for the Probe ... \n");
    }

    // Cabecera
    file << "Tiempo,Perfil,Profundidad,Temperatura,pH,DO_SAT,DO,Blue,Chl\n";

    return file;
}



void appendToCSV_probe(std::ofstream& file, const std::vector<float>& datos, uint8_t profile) {

    // Tiempo actual como string YYYY-MM-DD HH:MM:SS
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);

    file << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "," << static_cast<int>(profile);

    for (const auto& valor : datos) {
        file << "," << valor;
    }
    file << "\n";
    file.flush();
}



void sonda() {
    modbus_t* ctx = modbus_new_rtu(DEVICE, BAUDRATE, 'N', 8, 1);
    if (!ctx) {
        std::cerr << "Fallo al crear contexto Modbus\n";
        return;
    }

    modbus_set_slave(ctx, MODBUS_ID);

    if (modbus_connect(ctx) == -1) {
        std::cerr << "Fallo al conectar Modbus: " << modbus_strerror(errno) << std::endl;
        modbus_free(ctx);
        return;
    }

    std::ofstream sondaFile;  // Crea el CSV (uno por perfil), se crean mas adelante
    bool measure_flag = false;
    // uint8_t profile = 0; // Now global (delete)

    //std::cout << "[INFO] Iniciando lectura en hilo. Presiona Ctrl+C para salir...\n";

    while (true) {
        std::vector<float> datos;

        //std::cout << "\n--- Nueva lectura ---\n";
        for (int addr : direcciones) {
            float valor = read_float_inverse(ctx, addr);
            datos.push_back(valor);
        }

        prof= datos[0];
        //std::cout << "Profundidad:     " << datos[0] << "\n";

        if (isnan(prof)){
            prof=0.0;
        }
	    /*
        else if(prof < 0){
            prof = abs(prof);
            //prof = 0.0;
        }*/

        
        
        std::cout << "Profundidad:     " << datos[0] << "\n";
        std::cout << "Temperatura:     " << datos[1] << "\n";
        std::cout << "pH:              " << datos[2] << "\n";
        std::cout << "DO Saturación:   " << datos[3] << "\n";
        std::cout << "DO:              " << datos[4] << "\n";
        std::cout << "Blue:            " << datos[5] << "\n";
        std::cout << "Chl:             " << datos[6] << "\n";
        
        

        // Almacenamiento en el csv
        // prof = 5;  // For testing in the lab
        if(sessionReady.load()){
            uint8_t perfil_actual = currentProfile.load();
            if (prof <= 0){
                perfil_actual = 0;
            }
            if (!measure_flag){
                sondaFile = createCSV_probe(sessionTimestamp);
                measure_flag = true;
            }
            appendToCSV_probe(sondaFile, datos, perfil_actual);
        }
        
        sleep(5);

    }

    modbus_close(ctx);
    modbus_free(ctx);
}
