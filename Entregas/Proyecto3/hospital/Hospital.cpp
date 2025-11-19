#include "Hospital.hpp"
#include <iostream>
#include <cstring>

Hospital::Hospital() {
    memset(nombre, 0, sizeof(nombre));
    memset(direccion, 0, sizeof(direccion));
    memset(telefono, 0, sizeof(telefono));
    
    strcpy(nombre, "Hospital SALTO ANGEL");
    strcpy(direccion, "Av. Secundaria #777");
    strcpy(telefono, "0412-0000000");
    
    siguienteIDPaciente = 1;
    siguienteIDDoctor = 1;
    siguienteIDCita = 1;
    siguienteIDConsulta = 1;
    
    totalPacientesRegistrados = 0;
    totalDoctoresRegistrados = 0;
    totalCitasAgendadas = 0;
    totalConsultasRealizadas = 0;
}

Hospital::Hospital(const char* nombre, const char* direccion, const char* telefono) {
    memset(this->nombre, 0, sizeof(this->nombre));
    memset(this->direccion, 0, sizeof(this->direccion));
    memset(this->telefono, 0, sizeof(this->telefono));
    
    strncpy(this->nombre, nombre, sizeof(this->nombre) - 1);
    strncpy(this->direccion, direccion, sizeof(this->direccion) - 1);
    strncpy(this->telefono, telefono, sizeof(this->telefono) - 1);
    
    siguienteIDPaciente = 1;
    siguienteIDDoctor = 1;
    siguienteIDCita = 1;
    siguienteIDConsulta = 1;
    
    totalPacientesRegistrados = 0;
    totalDoctoresRegistrados = 0;
    totalCitasAgendadas = 0;
    totalConsultasRealizadas = 0;
}

// Getters
const char* Hospital::getNombre() const { return nombre; }
const char* Hospital::getDireccion() const { return direccion; }
const char* Hospital::getTelefono() const { return telefono; }
int Hospital::getSiguienteIDPaciente() const { return siguienteIDPaciente; }
int Hospital::getSiguienteIDDoctor() const { return siguienteIDDoctor; }
int Hospital::getSiguienteIDCita() const { return siguienteIDCita; }
int Hospital::getSiguienteIDConsulta() const { return siguienteIDConsulta; }
int Hospital::getTotalPacientesRegistrados() const { return totalPacientesRegistrados; }
int Hospital::getTotalDoctoresRegistrados() const { return totalDoctoresRegistrados; }
int Hospital::getTotalCitasAgendadas() const { return totalCitasAgendadas; }
int Hospital::getTotalConsultasRealizadas() const { return totalConsultasRealizadas; }

// Setters
void Hospital::setNombre(const char* nombre) {
    strncpy(this->nombre, nombre, sizeof(this->nombre) - 1);
    this->nombre[sizeof(this->nombre) - 1] = '\0';
}

void Hospital::setDireccion(const char* direccion) {
    strncpy(this->direccion, direccion, sizeof(this->direccion) - 1);
    this->direccion[sizeof(this->direccion) - 1] = '\0';
}

void Hospital::setTelefono(const char* telefono) {
    strncpy(this->telefono, telefono, sizeof(this->telefono) - 1);
    this->telefono[sizeof(this->telefono) - 1] = '\0';
}

void Hospital::setSiguienteIDPaciente(int id) { siguienteIDPaciente = id; }
void Hospital::setSiguienteIDDoctor(int id) { siguienteIDDoctor = id; }
void Hospital::setSiguienteIDCita(int id) { siguienteIDCita = id; }
void Hospital::setSiguienteIDConsulta(int id) { siguienteIDConsulta = id; }
void Hospital::setTotalPacientesRegistrados(int total) { totalPacientesRegistrados = total; }
void Hospital::setTotalDoctoresRegistrados(int total) { totalDoctoresRegistrados = total; }
void Hospital::setTotalCitasAgendadas(int total) { totalCitasAgendadas = total; }
void Hospital::setTotalConsultasRealizadas(int total) { totalConsultasRealizadas = total; }

// Métodos de gestión de IDs
int Hospital::generarIDPaciente() { return siguienteIDPaciente++; }
int Hospital::generarIDDoctor() { return siguienteIDDoctor++; }
int Hospital::generarIDCita() { return siguienteIDCita++; }
int Hospital::generarIDConsulta() { return siguienteIDConsulta++; }

// Métodos de estadísticas
void Hospital::incrementarPacientesRegistrados() { totalPacientesRegistrados++; }
void Hospital::incrementarDoctoresRegistrados() { totalDoctoresRegistrados++; }
void Hospital::incrementarCitasAgendadas() { totalCitasAgendadas++; }
void Hospital::incrementarConsultasRealizadas() { totalConsultasRealizadas++; }

void Hospital::mostrarInformacion() const {
    std::cout << "\n=== INFORMACION DEL HOSPITAL ===\n";
    std::cout << "Nombre: " << nombre << "\n";
    std::cout << "Direccion: " << direccion << "\n";
    std::cout << "Telefono: " << telefono << "\n";
    std::cout << "Total Pacientes: " << totalPacientesRegistrados << "\n";
    std::cout << "Total Doctores: " << totalDoctoresRegistrados << "\n";
    std::cout << "Total Citas: " << totalCitasAgendadas << "\n";
    std::cout << "Total Consultas: " << totalConsultasRealizadas << "\n";
}

size_t Hospital::obtenerTamano() {
    return sizeof(Hospital);
}

// Compatibilidad: alias de getters (nombres antiguos)
int Hospital::getTotalPacientes() const { return totalPacientesRegistrados; }
int Hospital::getTotalDoctores() const { return totalDoctoresRegistrados; }
int Hospital::getTotalCitas() const { return totalCitasAgendadas; }
int Hospital::getTotalHistorial() const { return totalConsultasRealizadas; }