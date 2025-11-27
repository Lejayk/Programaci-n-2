#include "HistorialMedico.hpp"
#include "../utilidades/Validaciones.hpp"
#include <iostream>
#include <cstring>
#include <cctype>
#include <iomanip>
#include <ctime>

HistorialMedico::HistorialMedico() {
    memset(fecha, 0, sizeof(fecha));
    memset(hora, 0, sizeof(hora));
    memset(diagnostico, 0, sizeof(diagnostico));
    memset(tratamiento, 0, sizeof(tratamiento));
    memset(medicamentos, 0, sizeof(medicamentos));
    
    id = 0;
    pacienteID = -1;
    doctorID = -1;
    costo = 0.0f;
    siguienteConsultaID = -1;
    eliminado = false;
    fechaRegistro = time(nullptr);
    
    // Establecer fecha y hora actual
    time_t ahora = time(nullptr);
    tm* tiempoLocal = localtime(&ahora);
    snprintf(fecha, sizeof(fecha), "%04d-%02d-%02d", 
             tiempoLocal->tm_year + 1900, 
             tiempoLocal->tm_mon + 1, 
             tiempoLocal->tm_mday);
    snprintf(hora, sizeof(hora), "%02d:%02d", 
             tiempoLocal->tm_hour, 
             tiempoLocal->tm_min);
}

HistorialMedico::HistorialMedico(int pacienteID, int doctorID, const char* diagnostico, 
                               const char* tratamiento, const char* medicamentos, float costo) : HistorialMedico() {
    setPacienteID(pacienteID);
    setDoctorID(doctorID);
    setDiagnostico(diagnostico);
    setTratamiento(tratamiento);
    setMedicamentos(medicamentos);
    setCosto(costo);
}

HistorialMedico::HistorialMedico(const HistorialMedico& other) {
    memcpy(this, &other, sizeof(HistorialMedico));
}

// Getters
int HistorialMedico::getId() const { return id; }
int HistorialMedico::getPacienteID() const { return pacienteID; }
const char* HistorialMedico::getFecha() const { return fecha; }
const char* HistorialMedico::getHora() const { return hora; }
const char* HistorialMedico::getDiagnostico() const { return diagnostico; }
const char* HistorialMedico::getTratamiento() const { return tratamiento; }
const char* HistorialMedico::getMedicamentos() const { return medicamentos; }
int HistorialMedico::getDoctorID() const { return doctorID; }
float HistorialMedico::getCosto() const { return costo; }
int HistorialMedico::getSiguienteConsultaID() const { return siguienteConsultaID; }
bool HistorialMedico::getEliminado() const { return eliminado; }
time_t HistorialMedico::getFechaRegistro() const { return fechaRegistro; }

// Setters con validaciones
void HistorialMedico::setId(int id) {
    this->id = id;
}

void HistorialMedico::setPacienteID(int pacienteID) {
    if (pacienteID <= 0) {
        std::cout << "Error: ID de paciente invalido\n";
        return;
    }
    this->pacienteID = pacienteID;
}

void HistorialMedico::setDoctorID(int doctorID) {
    if (doctorID <= 0) {
        std::cout << "Error: ID de doctor invalido\n";
        return;
    }
    this->doctorID = doctorID;
}

void HistorialMedico::setFecha(const char* fecha) {
    if (!validarFecha(fecha)) {
        std::cout << "Error: Fecha invalida. Formato: YYYY-MM-DD\n";
        return;
    }
    strncpy(this->fecha, fecha, sizeof(this->fecha) - 1);
    this->fecha[sizeof(this->fecha) - 1] = '\0';
}

void HistorialMedico::setHora(const char* hora) {
    if (!validarHora(hora)) {
        std::cout << "Error: Hora invalida. Formato: HH:MM (00-23:00-59)\n";
        return;
    }
    strncpy(this->hora, hora, sizeof(this->hora) - 1);
    this->hora[sizeof(this->hora) - 1] = '\0';
}

void HistorialMedico::setDiagnostico(const char* diagnostico) {
    if (!diagnostico || strlen(diagnostico) == 0) {
        std::cout << "Error: El diagnostico no puede estar vacio\n";
        return;
    }
    if (strlen(diagnostico) >= sizeof(this->diagnostico)) {
        std::cout << "Error: El diagnostico es demasiado largo\n";
        return;
    }
    strncpy(this->diagnostico, diagnostico, sizeof(this->diagnostico) - 1);
    this->diagnostico[sizeof(this->diagnostico) - 1] = '\0';
}

void HistorialMedico::setTratamiento(const char* tratamiento) {
    if (!tratamiento || strlen(tratamiento) == 0) {
        std::cout << "Error: El tratamiento no puede estar vacio\n";
        return;
    }
    if (strlen(tratamiento) >= sizeof(this->tratamiento)) {
        std::cout << "Error: El tratamiento es demasiado largo\n";
        return;
    }
    strncpy(this->tratamiento, tratamiento, sizeof(this->tratamiento) - 1);
    this->tratamiento[sizeof(this->tratamiento) - 1] = '\0';
}

void HistorialMedico::setMedicamentos(const char* medicamentos) {
    if (!medicamentos) {
        strncpy(this->medicamentos, "", sizeof(this->medicamentos) - 1);
        return;
    }
    strncpy(this->medicamentos, medicamentos, sizeof(this->medicamentos) - 1);
    this->medicamentos[sizeof(this->medicamentos) - 1] = '\0';
}

void HistorialMedico::setCosto(float costo) {
    if (costo < 0.0f) {
        std::cout << "Error: El costo no puede ser negativo\n";
        return;
    }
    if (costo > 100000.0f) {
        std::cout << "Error: Costo demasiado alto\n";
        return;
    }
    this->costo = costo;
}

void HistorialMedico::setSiguienteConsultaID(int siguienteConsultaID) {
    this->siguienteConsultaID = siguienteConsultaID;
}

void HistorialMedico::setEliminado(bool eliminado) {
    this->eliminado = eliminado;
}

// Métodos de validación
bool HistorialMedico::validarFecha(const char* fecha) const {
    return Validaciones::validarFecha(fecha);
}

bool HistorialMedico::validarHora(const char* hora) const {
    return Validaciones::validarHora(hora);
}

bool HistorialMedico::validarDatos() const {
    if (pacienteID <= 0) return false;
    if (doctorID <= 0) return false;
    if (!validarFecha(fecha)) return false;
    if (!validarHora(hora)) return false;
    if (strlen(diagnostico) == 0) return false;
    if (strlen(tratamiento) == 0) return false;
    if (costo < 0.0f) return false;
    return true;
}

// Métodos de presentación
void HistorialMedico::mostrarInformacionBasica() const {
    std::cout << "ID: " << id << " - Paciente: " << pacienteID << " - Doctor: " << doctorID
              << " - " << fecha << " " << hora << " - Costo: $" << std::fixed << std::setprecision(2) << costo << "\n";
}

void HistorialMedico::mostrarInformacionCompleta() const {
    std::cout << "\n=== INFORMACION COMPLETA DE CONSULTA ===" << std::endl;
    std::cout << "ID: " << id << "\n";
    std::cout << "Paciente ID: " << pacienteID << "\n";
    std::cout << "Doctor ID: " << doctorID << "\n";
    std::cout << "Fecha: " << fecha << "\n";
    std::cout << "Hora: " << hora << "\n";
    std::cout << "Diagnostico: " << diagnostico << "\n";
    std::cout << "Tratamiento: " << tratamiento << "\n";
    std::cout << "Medicamentos: " << medicamentos << "\n";
    std::cout << "Costo: $" << std::fixed << std::setprecision(2) << costo << "\n";
    std::cout << "Siguiente Consulta ID: " << (siguienteConsultaID == -1 ? "Ninguna" : std::to_string(siguienteConsultaID)) << "\n";
    std::cout << "Eliminada: " << (eliminado ? "Si" : "No") << "\n";
    
    char buffer[80];
    struct tm* timeinfo = localtime(&fechaRegistro);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    std::cout << "Fecha Registro: " << buffer << "\n";
}

size_t HistorialMedico::obtenerTamano() {
    return sizeof(HistorialMedico);
}