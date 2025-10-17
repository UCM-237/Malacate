
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

// Change this to choose between the two probes
// #define SONDA_OLD
#define SONDA_NEW

std::atomic<float> prof(0);

#ifdef SONDA_OLD
const int direcciones[] = {
    10, 19, 21, 23, 25, 27, 29
};
#elif defined(SONDA_NEW)
#define REG_TIME_BASE   10      // Base address for time (3 regs)
#define REG_MODE        20120
#define REG_PERIOD      20124
#define REG_CONFIRM     20199
const int direcciones[] = {
    30101, 31001, 31101, 31201, 31301, 31401, 31501, 31601
};
#else
#error "Debe definir SONDA_OLD o SONDA_NEW"
#endif

volatile bool keepRunning = true;


float read_float_inverse(modbus_t* ctx, int addr) {
    uint16_t regs[2];
    if (modbus_read_registers(ctx, addr, 2, regs) != 2) {
        //std::cerr << "Error leyendo en dirección " << addr << ": "
                  //<< modbus_strerror(errno) << std::endl;
        return NAN;
    }
    uint16_t swapped[2] = { regs[1], regs[0] };
    float result;
    std::memcpy(&result, swapped, sizeof(float));
    return result;
}


#ifdef SONDA_NEW

// Estas funciones solo aplican para la sonda nueva

// Cambia la hora del sistema remoto (3 registros: yymm, ddhh, mmss)
bool change_time(modbus_t* ctx,
                 uint16_t year, uint16_t month, uint16_t day,
                 uint16_t hour, uint16_t minute, uint16_t second){
    uint16_t regs_time[3];

    uint16_t yy = year % 100;
    regs_time[0] = (yy << 8) | month;        // yymm
    regs_time[1] = (day << 8) | hour;        // ddhh
    regs_time[2] = (minute << 8) | second;   // mmss

    int n = modbus_write_registers(ctx, REG_TIME_BASE, 3, regs_time);
    if (n == -1) {
        std::cerr << "Error writing time: " << modbus_strerror(errno) << std::endl;
        return false;
    }

    std::cout << "[OK] Time set: "
              << std::setfill('0') << std::setw(2) << yy << "/"
              << std::setw(2) << month << " "
              << std::setw(2) << day << " "
              << std::setw(2) << hour << ":"
              << std::setw(2) << minute << ":"
              << std::setw(2) << second << std::endl;
    return true;
}


// Cambia el modo de adquisición (puntual / continuo) y el periodo
bool change_mode(modbus_t* ctx, uint8_t mode, uint16_t period_ms){
    // mode: 0 = puntual, 1 = continuo
    uint16_t reg_type[1]   = { mode };
    uint16_t reg_period[1] = { period_ms };
    uint16_t reg_confirm[1]= { 0x01 };

    int n;

    n = modbus_write_registers(ctx, REG_MODE, 1, reg_type);
    if (n == -1) {
        std::cerr << "Error writing mode: " << modbus_strerror(errno) << std::endl;
        return false;
    }

    n = modbus_write_registers(ctx, REG_PERIOD, 1, reg_period);
    if (n == -1) {
        std::cerr << "Error writing period: " << modbus_strerror(errno) << std::endl;
        return false;
    }

    n = modbus_write_registers(ctx, REG_CONFIRM, 1, reg_confirm);
    if (n == -1) {
        std::cerr << "Error writing confirm: " << modbus_strerror(errno) << std::endl;
        return false;
    }

    std::cout << "[OK] Mode set to "
              << (mode == 0 ? "Punctual" : "Continuous")
              << " | Period = " << period_ms << " ms" << std::endl;
    return true;
}

#endif // SONDA_NEW

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
    #ifdef SONDA_OLD
        file << "Tiempo,Perfil,Profundidad,Temperatura,pH,DO_SAT,DO,Blue,Chl\n";
    #elif defined(SONDA_NEW)
        file << "Tiempo,Perfil,Profundidad,Temperatura,pH,DO_SAT,DO,Blue,Chl,C\n";
    #endif

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
        else if(prof < 0){
            // prof = abs(prof);
            prof = 0.0;
        }

        
        /*
        std::cout << "Profundidad:     " << datos[0] << "\n";
        std::cout << "Temperatura:     " << datos[1] << "\n";
        std::cout << "pH:              " << datos[2] << "\n";
        std::cout << "DO Saturación:   " << datos[3] << "\n";
        std::cout << "DO:              " << datos[4] << "\n";
        std::cout << "Blue:            " << datos[5] << "\n";
        std::cout << "Chl:             " << datos[6] << "\n";
        std::cout << "C:               " << datos[7] << "\n \n";
        */
        

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
