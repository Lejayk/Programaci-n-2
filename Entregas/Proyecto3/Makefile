# Compilador
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I.

# Ejecutable final
TARGET = SistetmaGestionHospitalario3

# Archivos fuente del proyecto
SRC = main.cpp \
	  hospital/Hospital.cpp hospital/menuPrincipal.cpp \
	  pacientes/Paciente.cpp pacientes/operacionesPacientes.cpp \
	  doctores/Doctor.cpp doctores/operacionesDoctores.cpp \
	  citas/Cita.cpp citas/operacionesCitas.cpp \
	  historial/HistorialMedico.cpp historial/operacionesHistorial.cpp \
	  persistencia/GestorArchivos.cpp

# Objetos (.o) generados a partir de .cpp
OBJ = $(SRC:.cpp=.o)

# Regla principal
all: $(TARGET)

# Cómo generar el ejecutable
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

# Regla general para compilar .cpp → .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	rm -f $(OBJ) $(TARGET)

