#include "HistorialMedico.hpp"
#include <cstring>
#include <iostream>

HistorialMedico::HistorialMedico() {
    id = 0;
    idPaciente = 0;
    idDoctor = 0;
    dia = mes = anio = 0;
    eliminado = false;

    memset(descripcion, 0, sizeof(descripcion));
    memset(diagnostico, 0, sizeof(diagnostico));
    memset(tratamiento, 0, sizeof(tratamiento));

    fechaCreacion = time(nullptr);
    fechaModificacion = fechaCreacion;
}

HistorialMedico::HistorialMedico(int p, int d) : HistorialMedico() {
    idPaciente = p;
    idDoctor = d;
}

int HistorialMedico::getId() const { return id; }
int HistorialMedico::getIdPaciente() const { return idPaciente; }
int HistorialMedico::getIdDoctor() const { return idDoctor; }
const char* HistorialMedico::getDescripcion() const { return descripcion; }
const char* HistorialMedico::getDiagnostico() const { return diagnostico; }
const char* HistorialMedico::getTratamiento() const { return tratamiento; }
int HistorialMedico::getDia() const { return dia; }
int HistorialMedico::getMes() const { return mes; }
int HistorialMedico::getAnio() const { return anio; }
bool HistorialMedico::getEliminado() const { return eliminado; }

void HistorialMedico::setId(int id) {
    this->id = id;
    fechaModificacion = time(nullptr);
}

void HistorialMedico::setDescripcion(const char* d) {
    strncpy(descripcion, d, sizeof(descripcion)-1);
    fechaModificacion = time(nullptr);
}

void HistorialMedico::setDiagnostico(const char* diag) {
    strncpy(diagnostico, diag, sizeof(diagnostico)-1);
    fechaModificacion = time(nullptr);
}

void HistorialMedico::setTratamiento(const char* t) {
    strncpy(tratamiento, t, sizeof(tratamiento)-1);
    fechaModificacion = time(nullptr);
}

void HistorialMedico::setFecha(int d, int m, int a) {
    dia = d; mes = m; anio = a;
    fechaModificacion = time(nullptr);
}

void HistorialMedico::setEliminado(bool e) {
    eliminado = e;
    fechaModificacion = time(nullptr);
}

bool HistorialMedico::validarDatos() const {
    if (idPaciente <= 0 || idDoctor <= 0) return false;
    if (strlen(descripcion) == 0) return false;
    if (dia < 1 || dia > 31) return false;
    if (mes < 1 || mes > 12) return false;
    if (anio < 1990 || anio > 2050) return false;
    return true;
}

void HistorialMedico::mostrarInformacionBasica() const {
    std::cout << "Historial " << id 
              << " | Paciente: " << idPaciente
              << " | Doctor: " << idDoctor
              << " | Fecha: " << dia << "/" << mes << "/" << anio << "\n";
}

void HistorialMedico::mostrarInformacionCompleta() const {
    std::cout << "\n=== HISTORIAL " << id << " ===\n";
    std::cout << "Paciente: " << idPaciente << "\n";
    std::cout << "Doctor: " << idDoctor << "\n";
    std::cout << "Fecha: " << dia << "/" << mes << "/" << anio << "\n";
    std::cout << "Descripcion:\n" << descripcion << "\n";
    std::cout << "Diagnostico:\n" << diagnostico << "\n";
    std::cout << "Tratamiento:\n" << tratamiento << "\n";
}

size_t HistorialMedico::obtenerTamano() {
    return sizeof(HistorialMedico);
}
