# Compilador
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I.

# Ejecutable final
TARGET = SistemaDeGestionHospitalario3.exe

# Archivos fuente del proyecto
SRC = main.cpp \
	  hospital/Hospital.cpp hospital/menuPrincipal.cpp \
	  pacientes/Paciente.cpp pacientes/operacionesPacientes.cpp \
	  doctores/Doctor.cpp doctores/operacionesDoctores.cpp \
	  citas/Cita.cpp citas/operacionesCitas.cpp \
	  historial/HistorialMedico.cpp historial/operacionesHistorial.cpp \
	  persistencia/GestorArchivos.cpp \
	  mantenimiento/operacionesMantenimiento.cpp \
	  utilidades/Formatos.cpp utilidades/Validaciones.cpp

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
	del /Q $(subst /,\,$(OBJ)) $(TARGET) 2>nul || rm -f $(OBJ) $(TARGET)

