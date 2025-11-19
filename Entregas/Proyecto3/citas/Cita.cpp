#include "Cita.hpp"
#include <iostream>
#include <cstring>

Cita::Cita() {
    id = 0;
    idDoctor = 0;
    idPaciente = 0;
    dia = mes = anio = hora = minuto = 0;
    atendida = false;
    eliminada = false;
    memset(motivo, 0, sizeof(motivo));

    fechaCreacion = time(nullptr);
    fechaModificacion = fechaCreacion;
}

Cita::Cita(int p, int d, const char* m) : Cita() {
    idPaciente = p;
    idDoctor = d;
    strncpy(motivo, m, sizeof(motivo)-1);
}

int Cita::getId() const { return id; }
int Cita::getIdPaciente() const { return idPaciente; }
int Cita::getIdDoctor() const { return idDoctor; }
int Cita::getDia() const { return dia; }
int Cita::getMes() const { return mes; }
int Cita::getAnio() const { return anio; }
int Cita::getHora() const { return hora; }
int Cita::getMinuto() const { return minuto; }
const char* Cita::getMotivo() const { return motivo; }
bool Cita::getAtendida() const { return atendida; }
bool Cita::getEliminada() const { return eliminada; }

void Cita::setId(int id) {
    this->id = id;
    fechaModificacion = time(nullptr);
}

void Cita::setFecha(int d, int m, int a, int h, int min) {
    dia=d; mes=m; anio=a; hora=h; minuto=min;
    fechaModificacion = time(nullptr);
}

void Cita::setMotivo(const char* m) {
    strncpy(motivo, m, sizeof(motivo)-1);
    fechaModificacion = time(nullptr);
}

void Cita::setAtendida(bool a) {
    atendida = a;
    fechaModificacion = time(nullptr);
}

void Cita::setEliminada(bool e) {
    eliminada = e;
    fechaModificacion = time(nullptr);
}

void Cita::mostrarInformacionBasica() const {
    std::cout << "Cita ID: " << id
              << " Paciente: " << idPaciente
              << " Doctor: " << idDoctor
              << " Fecha: " << dia << "/" << mes << "/" << anio
              << "\n";
}

void Cita::mostrarInformacionCompleta() const {
    std::cout << "\n=== CITA ID " << id << " ===\n";
    std::cout << "Paciente: " << idPaciente << "\n";
    std::cout << "Doctor: " << idDoctor << "\n";
    std::cout << "Motivo: " << motivo << "\n";
    std::cout << "Fecha: " << dia << "/" << mes << "/" << anio
              << " " << hora << ":" << minuto << "\n";
    std::cout << "Atendida: " << (atendida ? "SI" : "NO") << "\n";
}

bool Cita::validarDatos() const {
    if (idPaciente <= 0 || idDoctor <= 0) return false;
    if (strlen(motivo) == 0) return false;
    if (dia < 1 || dia > 31) return false;
    if (mes < 1 || mes > 12) return false;
    if (anio < 2020 || anio > 2050) return false;
    return true;
}

size_t Cita::obtenerTamano() { return sizeof(Cita); }

bool Cita::getEliminado() const { return eliminada; }
void Cita::setEliminado(bool e) { setEliminada(e); }
