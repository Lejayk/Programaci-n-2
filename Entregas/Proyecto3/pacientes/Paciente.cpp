#include "Paciente.hpp"
#include "../utilidades/Validaciones.hpp"
#include <iostream>
#include <cstring>
#include <cctype>

Paciente::Paciente() {
    id = 0;
    edad = 0;
    sexo = 'O';
    eliminado = false;

    memset(nombre, 0, sizeof(nombre));
    memset(apellido, 0, sizeof(apellido));
    memset(cedula, 0, sizeof(cedula));
    memset(tipoSangre, 0, sizeof(tipoSangre));
    memset(telefono, 0, sizeof(telefono));
    memset(email, 0, sizeof(email));
    memset(direccion, 0, sizeof(direccion));
    memset(alergias, 0, sizeof(alergias));
    memset(observaciones, 0, sizeof(observaciones));

    cantidadCitas = 0;
    for (int i = 0; i < 20; i++) citasID[i] = -1;

    cantidadConsultas = 0;
    primerConsultaID = -1;

    fechaCreacion = time(nullptr);
    fechaModificacion = fechaCreacion;
}

Paciente::Paciente(const char* nom, const char* ape, const char* ced) : Paciente() {
    setNombre(nom);
    setApellido(ape);
    setCedula(ced);
}

Paciente::Paciente(const Paciente& o) {
    memcpy(this, &o, sizeof(Paciente));
}

// Getters
int Paciente::getId() const { return id; }
const char* Paciente::getNombre() const { return nombre; }
const char* Paciente::getApellido() const { return apellido; }
const char* Paciente::getCedula() const { return cedula; }
int Paciente::getEdad() const { return edad; }
char Paciente::getSexo() const { return sexo; }
const char* Paciente::getTipoSangre() const { return tipoSangre; }
const char* Paciente::getTelefono() const { return telefono; }
const char* Paciente::getEmail() const { return email; }
const char* Paciente::getDireccion() const { return direccion; }
const char* Paciente::getAlergias() const { return alergias; }
const char* Paciente::getObservaciones() const { return observaciones; }

int Paciente::getCantidadCitas() const { return cantidadCitas; }
const int* Paciente::getCitasID() const { return citasID; }

int Paciente::getCantidadConsultas() const { return cantidadConsultas; }
int Paciente::getPrimerConsultaID() const { return primerConsultaID; }

void Paciente::setCantidadConsultas(int cantidad) {
    this->cantidadConsultas = cantidad;
    fechaModificacion = time(nullptr);
}

void Paciente::setPrimerConsultaID(int primerConsultaID) {
    this->primerConsultaID = primerConsultaID;
    fechaModificacion = time(nullptr);
}

bool Paciente::getEliminado() const { return eliminado; }
time_t Paciente::getFechaCreacion() const { return fechaCreacion; }
time_t Paciente::getFechaModificacion() const { return fechaModificacion; }

// Setters
void Paciente::setId(int id) {
    this->id = id;
    fechaModificacion = time(nullptr);
}

void Paciente::setNombre(const char* nom) {
    if (!nom || strlen(nom) == 0) {
        std::cout << "Error: Nombre invalido\n"; 
        return;
    }
    strncpy(nombre, nom, sizeof(nombre)-1);
    fechaModificacion = time(nullptr);
}

void Paciente::setApellido(const char* ape) {
    if (!ape || strlen(ape) == 0) {
        std::cout << "Error: Apellido invalido\n";
        return;
    }
    strncpy(apellido, ape, sizeof(apellido)-1);
    fechaModificacion = time(nullptr);
}

void Paciente::setCedula(const char* c) {
    if (!Validaciones::validarCedula(c)) {
        std::cout << "Error: Cedula invalida\n";
        return;
    }
    strncpy(cedula, c, sizeof(cedula)-1);
    fechaModificacion = time(nullptr);
}

void Paciente::setEdad(int e) {
    if (!Validaciones::validarRango(e, 0, 120)) {
        std::cout << "Error: Edad fuera de rango\n";
        return;
    }
    edad = e;
    fechaModificacion = time(nullptr);
}

void Paciente::setSexo(char s) {
    if (!Validaciones::validarSexo(s)) {
        std::cout << "Error: Sexo invalido (M/F/O)\n";
        return;
    }
    sexo = toupper(s);
    fechaModificacion = time(nullptr);
}

void Paciente::setTipoSangre(const char* t) {
    if (!Validaciones::validarTipoSangre(t)) {
        std::cout << "Error: Tipo de sangre invalido\n";
        return;
    }

    strncpy(tipoSangre, t, sizeof(tipoSangre)-1);
    fechaModificacion = time(nullptr);
}

void Paciente::setTelefono(const char* tel) {
    if (!Validaciones::validarTelefono(tel)) {
        std::cout << "Error: Telefono invalido\n";
        return;
    }
    strncpy(telefono, tel, sizeof(telefono)-1);
    fechaModificacion = time(nullptr);
}

void Paciente::setEmail(const char* mail) {
    if (!Validaciones::validarEmail(mail)) {
        std::cout << "Error: Email invalido\n";
        return;
    }
    strncpy(email, mail, sizeof(email)-1);
    fechaModificacion = time(nullptr);
}

void Paciente::setDireccion(const char* d) {
    if (!d) return;
    strncpy(direccion, d, sizeof(direccion)-1);
    fechaModificacion = time(nullptr);
}

void Paciente::setAlergias(const char* a) {
    if (!a) return;
    strncpy(alergias, a, sizeof(alergias)-1);
    fechaModificacion = time(nullptr);
}

void Paciente::setObservaciones(const char* o) {
    if (!o) return;
    strncpy(observaciones, o, sizeof(observaciones)-1);
    fechaModificacion = time(nullptr);
}

// Gestión de citas
bool Paciente::agregarCitaID(int idCita) {
    if (idCita <= 0) return false;
    if (cantidadCitas >= 20) return false;

    for (int i = 0; i < cantidadCitas; i++)
        if (citasID[i] == idCita) return false;

    citasID[cantidadCitas++] = idCita;
    fechaModificacion = time(nullptr);
    return true;
}

bool Paciente::eliminarCitaID(int idCita) {
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

bool Paciente::tieneCitaID(int idCita) const {
    for (int i = 0; i < cantidadCitas; i++)
        if (citasID[i] == idCita) return true;
    return false;
}

// Marcado
void Paciente::setEliminado(bool e) {
    eliminado = e;
    fechaModificacion = time(nullptr);
}

// Presentación
void Paciente::mostrarInformacionBasica() const {
    std::cout << "ID: " << id << " - " << nombre << " " << apellido
              << " | Citas: " << cantidadCitas << "\n";
}

void Paciente::mostrarInformacionCompleta() const {
    std::cout << "\n=== PACIENTE ID " << id << " ===\n";
    std::cout << "Nombre: " << nombre << " " << apellido << "\n";
    std::cout << "Cedula: " << cedula << "\n";
    std::cout << "Edad: " << edad << " | Sexo: " << sexo << "\n";
    std::cout << "Tipo de sangre: " << tipoSangre << "\n";
    std::cout << "Telefono: " << telefono << " | Email: " << email << "\n";
    std::cout << "Direccion: " << direccion << "\n";
    std::cout << "Alergias: " << alergias << "\n";
    std::cout << "Observaciones: " << observaciones << "\n";
    std::cout << "Citas registradas: " << cantidadCitas << "\n";
}

// Validacion global
bool Paciente::validarDatos() const {
    if (strlen(nombre) == 0) return false;
    if (strlen(apellido) == 0) return false;
    if (strlen(cedula) < 5) return false;
    if (edad < 0 || edad > 120) return false;
    if (sexo != 'M' && sexo != 'F' && sexo != 'O') return false;
    return true;
}

size_t Paciente::obtenerTamano() {
    return sizeof(Paciente);
}
