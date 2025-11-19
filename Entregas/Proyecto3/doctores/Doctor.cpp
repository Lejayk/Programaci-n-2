#include "Doctor.hpp"
#include <iostream>

Doctor::Doctor() {
    id = 0;
    eliminado = false;
    costoConsulta = 0.0f;

    memset(nombre, 0, sizeof(nombre));
    memset(apellido, 0, sizeof(apellido));
    memset(cedula, 0, sizeof(cedula));
    memset(especialidad, 0, sizeof(especialidad));

    cantidadCitas = 0;
    for (int i = 0; i < 20; i++) citasID[i] = -1;

    fechaCreacion = time(nullptr);
    fechaModificacion = fechaCreacion;
}

Doctor::Doctor(const char* n, const char* a, const char* c, const char* e, float costo) : Doctor() {
    setNombre(n);
    setApellido(a);
    setCedula(c);
    setEspecialidad(e);
    setCostoConsulta(costo);
}

Doctor::Doctor(const Doctor& o) {
    memcpy(this, &o, sizeof(Doctor));
}

int Doctor::getId() const { return id; }
const char* Doctor::getNombre() const { return nombre; }
const char* Doctor::getApellido() const { return apellido; }
const char* Doctor::getCedula() const { return cedula; }
const char* Doctor::getEspecialidad() const { return especialidad; }
float Doctor::getCostoConsulta() const { return costoConsulta; }
bool Doctor::getEliminado() const { return eliminado; }
int Doctor::getCantidadCitas() const { return cantidadCitas; }
const int* Doctor::getCitasID() const { return citasID; }

time_t Doctor::getFechaCreacion() const { return fechaCreacion; }
time_t Doctor::getFechaModificacion() const { return fechaModificacion; }

void Doctor::setId(int id) {
    this->id = id;
    fechaModificacion = time(nullptr);
}

void Doctor::setNombre(const char* n) {
    strncpy(nombre, n, sizeof(nombre)-1);
    fechaModificacion = time(nullptr);
}

void Doctor::setApellido(const char* a) {
    strncpy(apellido, a, sizeof(apellido)-1);
    fechaModificacion = time(nullptr);
}

void Doctor::setCedula(const char* c) {
    strncpy(cedula, c, sizeof(cedula)-1);
    fechaModificacion = time(nullptr);
}

void Doctor::setEspecialidad(const char* e) {
    strncpy(especialidad, e, sizeof(especialidad)-1);
    fechaModificacion = time(nullptr);
}

void Doctor::setCostoConsulta(float c) {
    if (c < 0) return;
    costoConsulta = c;
    fechaModificacion = time(nullptr);
}

void Doctor::setEliminado(bool e) {
    eliminado = e;
    fechaModificacion = time(nullptr);
}

bool Doctor::agregarCitaID(int idCita) {
    if (cantidadCitas >= 20) return false;
    citasID[cantidadCitas++] = idCita;
    fechaModificacion = time(nullptr);
    return true;
}

bool Doctor::eliminarCitaID(int idCita) {
    for (int i = 0; i < cantidadCitas; i++) {
        if (citasID[i] == idCita) {
            for (int j = i; j < cantidadCitas-1; j++)
                citasID[j] = citasID[j+1];
            cantidadCitas--;
            citasID[cantidadCitas] = -1;
            fechaModificacion = time(nullptr);
            return true;
        }
    }
    return false;
}

bool Doctor::tieneCitaID(int idCita) const {
    for (int i = 0; i < cantidadCitas; i++)
        if (citasID[i] == idCita) return true;
    return false;
}

void Doctor::mostrarInformacionBasica() const {
    std::cout << "ID: " << id << " - "
              << nombre << " " << apellido
              << " | Especialidad: " << especialidad << "\n";
}

void Doctor::mostrarInformacionCompleta() const {
    std::cout << "\n=== DOCTOR ID " << id << " ===\n";
    std::cout << "Nombre: " << nombre << " " << apellido << "\n";
    std::cout << "Cedula: " << cedula << "\n";
    std::cout << "Especialidad: " << especialidad << "\n";
    std::cout << "Costo consulta: " << costoConsulta << "\n";
    std::cout << "Citas: " << cantidadCitas << "\n";
}

bool Doctor::validarDatos() const {
    if (strlen(nombre)==0 || strlen(apellido)==0) return false;
    if (strlen(cedula)<5) return false;
    if (strlen(especialidad)==0) return false;
    if (costoConsulta < 0) return false;
    return true;
}

size_t Doctor::obtenerTamano() { return sizeof(Doctor); }
