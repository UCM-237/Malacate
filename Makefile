# Nombre del compilador
CXX = g++

# Flags de compilación
CXXFLAGS = -Wall -std=c++11 -pthread -I lib

# Librerías necesarias
LIBS = -lwiringPi -lmodbus

# Nombre del ejecutable
TARGET = onda_cuadrada

# Directorios
SRC_DIR = src
LIB_DIR = lib

# Archivos fuente y objeto
#SRC_FILES = $(SRC_DIR)/abajo.cpp $(SRC_DIR)/casa.cpp $(SRC_DIR)/serial_reader.cpp $(SRC_DIR)/generarOndaCuadrada.cpp $(SRC_DIR)/power.cpp $(SRC_DIR)/sensor.cpp $(SRC_DIR)/leerUSB.cpp $(SRC_DIR)/mision.cpp main.cpp
#OBJ_FILES = $(SRC_FILES:.cpp=.o)

SRC_DIR = src


SRC_FILES =  $(SRC_DIR)/ambulanceSiren.cpp $(SRC_DIR)/arriba.cpp $(SRC_DIR)/abajo.cpp $(SRC_DIR)/casa.cpp $(SRC_DIR)/serial_reader.cpp $(SRC_DIR)/generarOndaCuadrada.cpp $(SRC_DIR)/sonda.cpp $(SRC_DIR)/sensor.cpp main.cpp
OBJ_FILES = $(SRC_FILES:.cpp=.o)


# Regla para compilar
all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ_FILES) $(LIBS)

# Regla para compilar archivos .cpp a .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para limpiar archivos generados
clean:
	rm -f $(OBJ_FILES) $(TARGET)
