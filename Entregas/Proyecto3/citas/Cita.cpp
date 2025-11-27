#include "Cita.hpp"
#include "../utilidades/Validaciones.hpp"
#include <iostream>
#include <cstring>
#include <cctype>
#include <iomanip>

Cita::Cita() {
    memset(fecha, 0, sizeof(fecha));
    memset(hora, 0, sizeof(hora));
    memset(motivo, 0, sizeof(motivo));
    memset(estado, 0, sizeof(estado));
    memset(observaciones, 0, sizeof(observaciones));
    
    id = 0;
    pacienteID = -1;
    doctorID = -1;
    strcpy(estado, "Agendada");
    atendida = false;
    consultaID = -1;
    eliminado = false;
    fechaCreacion = time(nullptr);
    fechaModificacion = fechaCreacion;
}

Cita::Cita(int pacienteID, int doctorID, const char* fecha, const char* hora, const char* motivo) : Cita() {
    setPacienteID(pacienteID);
    setDoctorID(doctorID);
    setFecha(fecha);
    setHora(hora);
    setMotivo(motivo);
}

Cita::Cita(const Cita& other) {
    memcpy(this, &other, sizeof(Cita));
}

// Getters
int Cita::getId() const { return id; }
int Cita::getPacienteID() const { return pacienteID; }
int Cita::getDoctorID() const { return doctorID; }
const char* Cita::getFecha() const { return fecha; }
const char* Cita::getHora() const { return hora; }
const char* Cita::getMotivo() const { return motivo; }
const char* Cita::getEstado() const { return estado; }
const char* Cita::getObservaciones() const { return observaciones; }
bool Cita::getAtendida() const { return atendida; }
int Cita::getConsultaID() const { return consultaID; }
bool Cita::getEliminado() const { return eliminado; }
time_t Cita::getFechaCreacion() const { return fechaCreacion; }
time_t Cita::getFechaModificacion() const { return fechaModificacion; }

// Setters con validaciones
void Cita::setId(int id) {
    this->id = id;
    fechaModificacion = time(nullptr);
}

void Cita::setPacienteID(int pacienteID) {
    if (pacienteID <= 0) {
        std::cout << "Error: ID de paciente invalido\n";
        return;
    }
    this->pacienteID = pacienteID;
    fechaModificacion = time(nullptr);
}

void Cita::setDoctorID(int doctorID) {
    if (doctorID <= 0) {
        std::cout << "Error: ID de doctor invalido\n";
        return;
    }
    this->doctorID = doctorID;
    fechaModificacion = time(nullptr);
}

void Cita::setFecha(const char* fecha) {
    if (!validarFecha(fecha)) {
        std::cout << "Error: Fecha invalida. Formato: YYYY-MM-DD\n";
        return;
    }
    strncpy(this->fecha, fecha, sizeof(this->fecha) - 1);
    this->fecha[sizeof(this->fecha) - 1] = '\0';
    fechaModificacion = time(nullptr);
}

void Cita::setHora(const char* hora) {
    if (!validarHora(hora)) {
        std::cout << "Error: Hora invalida. Formato: HH:MM (00-23:00-59)\n";
        return;
    }
    strncpy(this->hora, hora, sizeof(this->hora) - 1);
    this->hora[sizeof(this->hora) - 1] = '\0';
    fechaModificacion = time(nullptr);
}

void Cita::setMotivo(const char* motivo) {
    if (!motivo || strlen(motivo) == 0) {
        std::cout << "Error: El motivo no puede estar vacio\n";
        return;
    }
    if (strlen(motivo) >= sizeof(this->motivo)) {
        std::cout << "Error: El motivo es demasiado largo\n";
        return;
    }
    strncpy(this->motivo, motivo, sizeof(this->motivo) - 1);
    this->motivo[sizeof(this->motivo) - 1] = '\0';
    fechaModificacion = time(nullptr);
}

void Cita::setEstado(const char* estado) {
    if (!estado || strlen(estado) == 0) {
        std::cout << "Error: El estado no puede estar vacio\n";
        return;
    }
    
    // Estados válidos
    const char* estadosValidos[] = {"Agendada", "Cancelada", "Atendida", "No asistio", "Reprogramada"};
    bool valido = false;
    for (int i = 0; i < 5; i++) {
        if (strcmp(estado, estadosValidos[i]) == 0) {
            valido = true;
            break;
        }
    }
    
    if (!valido) {
        std::cout << "Error: Estado invalido. Use: Agendada, Cancelada, Atendida, No asistio, Reprogramada\n";
        return;
    }
    
    strncpy(this->estado, estado, sizeof(this->estado) - 1);
    this->estado[sizeof(this->estado) - 1] = '\0';
    
    // Actualizar estado de atendida automáticamente
    if (strcmp(estado, "Atendida") == 0) {
        this->atendida = true;
    } else {
        this->atendida = false;
    }
    
    fechaModificacion = time(nullptr);
}

void Cita::setObservaciones(const char* observaciones) {
    if (!observaciones) {
        strncpy(this->observaciones, "", sizeof(this->observaciones) - 1);
        return;
    }
    strncpy(this->observaciones, observaciones, sizeof(this->observaciones) - 1);
    this->observaciones[sizeof(this->observaciones) - 1] = '\0';
    fechaModificacion = time(nullptr);
}

void Cita::setAtendida(bool atendida) {
    this->atendida = atendida;
    if (atendida) {
        strncpy(this->estado, "Atendida", sizeof(this->estado) - 1);
    }
    fechaModificacion = time(nullptr);
}

void Cita::setConsultaID(int consultaID) {
    this->consultaID = consultaID;
    fechaModificacion = time(nullptr);
}

void Cita::setEliminado(bool eliminado) {
    this->eliminado = eliminado;
    fechaModificacion = time(nullptr);
}

// Métodos de validación
bool Cita::validarFecha(const char* fecha) const {
    return Validaciones::validarFecha(fecha);
}

bool Cita::validarHora(const char* hora) const {
    return Validaciones::validarHora(hora);
}

bool Cita::validarDatos() const {
    if (pacienteID <= 0) return false;
    if (doctorID <= 0) return false;
    if (!validarFecha(fecha)) return false;
    if (!validarHora(hora)) return false;
    if (strlen(motivo) == 0) return false;
    if (strlen(estado) == 0) return false;
    return true;
}

// Métodos de estado
bool Cita::estaAgendada() const {
    return strcmp(estado, "Agendada") == 0;
}

bool Cita::estaCancelada() const {
    return strcmp(estado, "Cancelada") == 0;
}

bool Cita::estaAtendida() const {
    return strcmp(estado, "Atendida") == 0;
}

// Métodos de presentación
void Cita::mostrarInformacionBasica() const {
    std::cout << "ID: " << id << " - Paciente: " << pacienteID << " - Doctor: " << doctorID
              << " - " << fecha << " " << hora << " - " << estado << "\n";
}

void Cita::mostrarInformacionCompleta() const {
    std::cout << "\n=== INFORMACION COMPLETA DE LA CITA ===\n";
    std::cout << "ID: " << id << "\n";
    std::cout << "Paciente ID: " << pacienteID << "\n";
    std::cout << "Doctor ID: " << doctorID << "\n";
    std::cout << "Fecha: " << fecha << "\n";
    std::cout << "Hora: " << hora << "\n";
    std::cout << "Motivo: " << motivo << "\n";
    std::cout << "Estado: " << estado << "\n";
    std::cout << "Observaciones: " << observaciones << "\n";
    std::cout << "Atendida: " << (atendida ? "Si" : "No") << "\n";
    if (atendida) {
        std::cout << "Consulta ID: " << consultaID << "\n";
    }
    std::cout << "Eliminada: " << (eliminado ? "Si" : "No") << "\n";
    
    char buffer[80];
    struct tm* timeinfo = localtime(&fechaCreacion);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    std::cout << "Fecha Creacion: " << buffer << "\n";
    
    timeinfo = localtime(&fechaModificacion);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    std::cout << "Fecha Modificacion: " << buffer << "\n";
}

size_t Cita::obtenerTamano() {
    return sizeof(Cita);
}