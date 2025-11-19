#include "Paciente.hpp"
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
    // Validacion basada en SistemaDeGestionHospitalario2.cpp::validarCedula
    if (!c) {
        std::cout << "Error: Cedula invalida\n";
        return;
    }
    int len = (int)strlen(c);
    if (len == 0 || len > (int)sizeof(cedula)-1) {
        std::cout << "Error: Cedula invalida\n";
        return;
    }
    if (len < 3) {
        std::cout << "Error: Cedula invalida\n";
        return;
    }
    if (c[1] != '-') {
        std::cout << "Error: Formato de cedula invalido. Use V-12345678\n";
        return;
    }
    for (int i = 2; i < len; i++) {
        if (!isdigit((unsigned char)c[i])) {
            std::cout << "Error: Formato de cedula invalido. Los caracteres despues del guion deben ser numeros\n";
            return;
        }
    }
    char primera = c[0];
    if (primera != 'V' && primera != 'v' && primera != 'E' && primera != 'e') {
        std::cout << "Error: Cedula debe comenzar con V o E\n";
        return;
    }
    strncpy(cedula, c, sizeof(cedula)-1);
    cedula[sizeof(cedula)-1] = '\0';
    fechaModificacion = time(nullptr);
}

void Paciente::setEdad(int e) {
    if (e < 0 || e > 120) {
        std::cout << "Error: Edad fuera de rango\n";
        return;
    }
    edad = e;
    fechaModificacion = time(nullptr);
}

void Paciente::setSexo(char s) {
    s = toupper(s);
    if (s != 'M' && s != 'F' && s != 'O') {
        std::cout << "Error: Sexo invalido (M/F/O)\n";
        return;
    }
    sexo = s;
    fechaModificacion = time(nullptr);
}

void Paciente::setTipoSangre(const char* t) {
    const char* validos[] = {"A+","A-","B+","B-","AB+","AB-","O+","O-"};
    bool ok = false;
    for (const char* v : validos) if (strcmp(t, v) == 0) ok = true;

    if (!ok) {
        std::cout << "Error: Tipo de sangre invalido\n";
        return;
    }

    strncpy(tipoSangre, t, sizeof(tipoSangre)-1);
    fechaModificacion = time(nullptr);
}

void Paciente::setTelefono(const char* tel) {
    if (!tel) {
        std::cout << "Error: Telefono invalido\n";
        return;
    }
    if (strlen(tel) >= sizeof(telefono)) {
        std::cout << "Error: El telefono es demasiado largo\n";
        return;
    }
    // Requerir al menos 7 digitos numericos (ignorar separadores)
    int digitCount = 0;
    for (int i = 0; tel[i] != '\0'; i++) if (isdigit((unsigned char)tel[i])) digitCount++;
    if (digitCount < 7) {
        std::cout << "Error: El telefono debe contener al menos 7 digitos\n";
        return;
    }
    strncpy(telefono, tel, sizeof(telefono)-1);
    telefono[sizeof(telefono)-1] = '\0';
    fechaModificacion = time(nullptr);
}

void Paciente::setEmail(const char* mail) {
    // Validacion basada en SistemaDeGestionHospitalario2.cpp::validarEmail
    if (!mail || strlen(mail) == 0) {
        std::cout << "Error: Email invalido\n";
        return;
    }
    if (strlen(mail) > (sizeof(email)-1)) {
        std::cout << "Error: Email invalido (longitud)\n";
        return;
    }
    const char* at = strchr(mail, '@');
    if (!at) {
        std::cout << "Error: Email invalido\n";
        return;
    }
    if (strchr(at + 1, '@')) {
        std::cout << "Error: Email invalido\n";
        return;
    }
    if (at == mail) {
        std::cout << "Error: Email invalido\n";
        return;
    }
    if (*(at + 1) == '\0') {
        std::cout << "Error: Email invalido\n";
        return;
    }
    const char* dot = strchr(at, '.');
    if (!dot) {
        std::cout << "Error: Email invalido\n";
        return;
    }
    if (dot == at + 1) {
        std::cout << "Error: Email invalido\n";
        return;
    }
    const char* lastDot = strrchr(mail, '.');
    if (!lastDot || strlen(lastDot) <= 1) {
        std::cout << "Error: Email invalido\n";
        return;
    }
    if (strlen(lastDot) < 3) {
        std::cout << "Error: Email invalido\n";
        return;
    }
    for (const char* p = mail; p < at; p++) {
        if (!isalnum((unsigned char)*p) && *p != '.' && *p != '_' && *p != '-' && *p != '+') {
            std::cout << "Error: Email invalido\n";
            return;
        }
    }
    if (mail[0] == '.' || *(at - 1) == '.') {
        std::cout << "Error: Email invalido\n";
        return;
    }
    for (const char* p = at + 1; *p; p++) {
        if (!isalnum((unsigned char)*p) && *p != '.' && *p != '-') {
            std::cout << "Error: Email invalido\n";
            return;
        }
    }

    strncpy(email, mail, sizeof(email)-1);
    email[sizeof(email)-1] = '\0';
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
