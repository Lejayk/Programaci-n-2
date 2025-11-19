#include "Doctor.hpp"
#include <iostream>
#include <cstring>
#include <cctype>
#include <iomanip>

Doctor::Doctor() {
    memset(nombre, 0, sizeof(nombre));
    memset(apellido, 0, sizeof(apellido));
    memset(cedulaProfesional, 0, sizeof(cedulaProfesional));
    memset(especialidad, 0, sizeof(especialidad));
    memset(horarioAtencion, 0, sizeof(horarioAtencion));
    memset(telefono, 0, sizeof(telefono));
    memset(email, 0, sizeof(email));
    
    id = 0;
    aniosExperiencia = 0;
    costoConsulta = 0.0f;
    disponible = true;
    cantidadPacientes = 0;
    for (int i = 0; i < 50; i++) {
        pacientesIDs[i] = -1;
    }
    cantidadCitas = 0;
    for (int i = 0; i < 30; i++) {
        citasIDs[i] = -1;
    }
    eliminado = false;
    fechaCreacion = time(nullptr);
    fechaModificacion = fechaCreacion;
}

Doctor::Doctor(const char* nombre, const char* apellido, const char* cedulaProfesional, const char* especialidad) : Doctor() {
    setNombre(nombre);
    setApellido(apellido);
    setCedulaProfesional(cedulaProfesional);
    setEspecialidad(especialidad);
}

Doctor::Doctor(const Doctor& other) {
    memcpy(this, &other, sizeof(Doctor));
}

// Getters
int Doctor::getId() const { return id; }
const char* Doctor::getNombre() const { return nombre; }
const char* Doctor::getApellido() const { return apellido; }
const char* Doctor::getCedulaProfesional() const { return cedulaProfesional; }
const char* Doctor::getEspecialidad() const { return especialidad; }
int Doctor::getAniosExperiencia() const { return aniosExperiencia; }
float Doctor::getCostoConsulta() const { return costoConsulta; }
const char* Doctor::getHorarioAtencion() const { return horarioAtencion; }
const char* Doctor::getTelefono() const { return telefono; }
const char* Doctor::getEmail() const { return email; }
bool Doctor::getDisponible() const { return disponible; }
int Doctor::getCantidadPacientes() const { return cantidadPacientes; }
const int* Doctor::getPacientesIDs() const { return pacientesIDs; }
int Doctor::getCantidadCitas() const { return cantidadCitas; }
const int* Doctor::getCitasIDs() const { return citasIDs; }
bool Doctor::getEliminado() const { return eliminado; }
time_t Doctor::getFechaCreacion() const { return fechaCreacion; }
time_t Doctor::getFechaModificacion() const { return fechaModificacion; }

// Setters con validaciones
void Doctor::setId(int id) {
    this->id = id;
    fechaModificacion = time(nullptr);
}

void Doctor::setNombre(const char* nombre) {
    if (!nombre || strlen(nombre) == 0) {
        std::cout << "Error: El nombre no puede estar vacio\n";
        return;
    }
    if (strlen(nombre) >= sizeof(this->nombre)) {
        std::cout << "Error: El nombre es demasiado largo\n";
        return;
    }
    strncpy(this->nombre, nombre, sizeof(this->nombre) - 1);
    this->nombre[sizeof(this->nombre) - 1] = '\0';
    fechaModificacion = time(nullptr);
}

void Doctor::setApellido(const char* apellido) {
    if (!apellido || strlen(apellido) == 0) {
        std::cout << "Error: El apellido no puede estar vacio\n";
        return;
    }
    if (strlen(apellido) >= sizeof(this->apellido)) {
        std::cout << "Error: El apellido es demasiado largo\n";
        return;
    }
    strncpy(this->apellido, apellido, sizeof(this->apellido) - 1);
    this->apellido[sizeof(this->apellido) - 1] = '\0';
    fechaModificacion = time(nullptr);
}

void Doctor::setCedulaProfesional(const char* cedulaProfesional) {
    if (!cedulaProfesional || strlen(cedulaProfesional) == 0) {
        std::cout << "Error: La cedula profesional no puede estar vacia\n";
        return;
    }
    if (strlen(cedulaProfesional) >= sizeof(this->cedulaProfesional)) {
        std::cout << "Error: La cedula profesional es demasiado larga\n";
        return;
    }
    
    // Validación básica de formato (similar a cédula de paciente)
    if (cedulaProfesional[1] != '-') {
        std::cout << "Error: Formato de cedula profesional invalido. Use V-12345678\n";
        return;
    }
    
    char primerCaracter = cedulaProfesional[0];
    if (primerCaracter != 'V' && primerCaracter != 'v' && 
        primerCaracter != 'E' && primerCaracter != 'e') {
        std::cout << "Error: Cedula profesional debe comenzar con V o E\n";
        return;
    }
    
    strncpy(this->cedulaProfesional, cedulaProfesional, sizeof(this->cedulaProfesional) - 1);
    this->cedulaProfesional[sizeof(this->cedulaProfesional) - 1] = '\0';
    fechaModificacion = time(nullptr);
}

void Doctor::setEspecialidad(const char* especialidad) {
    if (!especialidad || strlen(especialidad) == 0) {
        std::cout << "Error: La especialidad no puede estar vacia\n";
        return;
    }
    if (strlen(especialidad) >= sizeof(this->especialidad)) {
        std::cout << "Error: La especialidad es demasiado larga\n";
        return;
    }
    strncpy(this->especialidad, especialidad, sizeof(this->especialidad) - 1);
    this->especialidad[sizeof(this->especialidad) - 1] = '\0';
    fechaModificacion = time(nullptr);
}

void Doctor::setAniosExperiencia(int aniosExperiencia) {
    if (aniosExperiencia < 0) {
        std::cout << "Error: Los años de experiencia no pueden ser negativos\n";
        return;
    }
    if (aniosExperiencia > 100) {
        std::cout << "Error: Años de experiencia no validos (maximo 100)\n";
        return;
    }
    this->aniosExperiencia = aniosExperiencia;
    fechaModificacion = time(nullptr);
}

void Doctor::setCostoConsulta(float costoConsulta) {
    if (costoConsulta < 0.0f) {
        std::cout << "Error: El costo de consulta no puede ser negativo\n";
        return;
    }
    if (costoConsulta > 10000.0f) {
        std::cout << "Error: Costo de consulta demasiado alto\n";
        return;
    }
    this->costoConsulta = costoConsulta;
    fechaModificacion = time(nullptr);
}

void Doctor::setHorarioAtencion(const char* horarioAtencion) {
    if (!horarioAtencion) {
        strncpy(this->horarioAtencion, "", sizeof(this->horarioAtencion) - 1);
        return;
    }
    strncpy(this->horarioAtencion, horarioAtencion, sizeof(this->horarioAtencion) - 1);
    this->horarioAtencion[sizeof(this->horarioAtencion) - 1] = '\0';
    fechaModificacion = time(nullptr);
}

void Doctor::setTelefono(const char* telefono) {
    if (!telefono || strlen(telefono) < 7) {
        std::cout << "Error: El telefono debe tener al menos 7 digitos\n";
        return;
    }
    if (strlen(telefono) >= sizeof(this->telefono)) {
        std::cout << "Error: El telefono es demasiado largo\n";
        return;
    }
    
    // Contar dígitos
    int digitCount = 0;
    for (int i = 0; telefono[i] != '\0'; i++) {
        if (isdigit(telefono[i])) digitCount++;
    }
    
    if (digitCount < 7) {
        std::cout << "Error: El telefono debe contener al menos 7 digitos\n";
        return;
    }
    
    strncpy(this->telefono, telefono, sizeof(this->telefono) - 1);
    this->telefono[sizeof(this->telefono) - 1] = '\0';
    fechaModificacion = time(nullptr);
}

void Doctor::setEmail(const char* email) {
    if (!email || strlen(email) == 0) {
        // Permitir vacío
        strncpy(this->email, "", sizeof(this->email) - 1);
        return;
    }
    
    if (strlen(email) >= sizeof(this->email)) {
        std::cout << "Error: El email es demasiado largo\n";
        return;
    }
    
    // Validación básica de email
    const char* at = strchr(email, '@');
    if (!at) {
        std::cout << "Error: El email debe contener @\n";
        return;
    }
    
    strncpy(this->email, email, sizeof(this->email) - 1);
    this->email[sizeof(this->email) - 1] = '\0';
    fechaModificacion = time(nullptr);
}

void Doctor::setDisponible(bool disponible) {
    this->disponible = disponible;
    fechaModificacion = time(nullptr);
}

void Doctor::setEliminado(bool eliminado) {
    this->eliminado = eliminado;
    fechaModificacion = time(nullptr);
}

// Métodos de gestión de pacientes
bool Doctor::agregarPacienteID(int idPaciente) {
    if (idPaciente <= 0) {
        std::cout << "Error: ID de paciente invalido\n";
        return false;
    }
    
    if (cantidadPacientes >= 50) {
        std::cout << "Error: Limite de pacientes alcanzado (max 50)\n";
        return false;
    }
    
    // Verificar duplicado
    for (int i = 0; i < cantidadPacientes; i++) {
        if (pacientesIDs[i] == idPaciente) {
            std::cout << "Error: El paciente ya esta asociado al doctor\n";
            return false;
        }
    }
    
    pacientesIDs[cantidadPacientes] = idPaciente;
    cantidadPacientes++;
    fechaModificacion = time(nullptr);
    return true;
}

bool Doctor::eliminarPacienteID(int idPaciente) {
    if (idPaciente <= 0) return false;
    
    for (int i = 0; i < cantidadPacientes; i++) {
        if (pacientesIDs[i] == idPaciente) {
            // Mover elementos restantes
            for (int j = i; j < cantidadPacientes - 1; j++) {
                pacientesIDs[j] = pacientesIDs[j + 1];
            }
            cantidadPacientes--;
            pacientesIDs[cantidadPacientes] = -1;
            fechaModificacion = time(nullptr);
            return true;
        }
    }
    return false;
}

bool Doctor::tienePacienteID(int idPaciente) const {
    for (int i = 0; i < cantidadPacientes; i++) {
        if (pacientesIDs[i] == idPaciente) {
            return true;
        }
    }
    return false;
}

// Métodos de gestión de citas
bool Doctor::agregarCitaID(int idCita) {
    if (idCita <= 0) {
        std::cout << "Error: ID de cita invalido\n";
        return false;
    }
    
    if (cantidadCitas >= 30) {
        std::cout << "Error: Limite de citas alcanzado (max 30)\n";
        return false;
    }
    
    // Verificar duplicado
    for (int i = 0; i < cantidadCitas; i++) {
        if (citasIDs[i] == idCita) {
            std::cout << "Error: La cita ya esta asociada al doctor\n";
            return false;
        }
    }
    
    citasIDs[cantidadCitas] = idCita;
    cantidadCitas++;
    fechaModificacion = time(nullptr);
    return true;
}

bool Doctor::eliminarCitaID(int idCita) {
    if (idCita <= 0) return false;
    
    for (int i = 0; i < cantidadCitas; i++) {
        if (citasIDs[i] == idCita) {
            // Mover elementos restantes
            for (int j = i; j < cantidadCitas - 1; j++) {
                citasIDs[j] = citasIDs[j + 1];
            }
            cantidadCitas--;
            citasIDs[cantidadCitas] = -1;
            fechaModificacion = time(nullptr);
            return true;
        }
    }
    return false;
}

bool Doctor::tieneCitaID(int idCita) const {
    for (int i = 0; i < cantidadCitas; i++) {
        if (citasIDs[i] == idCita) {
            return true;
        }
    }
    return false;
}

// Métodos de presentación
void Doctor::mostrarInformacionBasica() const {
    std::cout << "ID: " << id << " - Dr. " << nombre << " " << apellido 
              << " (" << especialidad << ") - Costo: $" << costoConsulta 
              << " - Disponible: " << (disponible ? "Si" : "No") << "\n";
}

void Doctor::mostrarInformacionCompleta() const {
    std::cout << "\n=== INFORMACION COMPLETA DEL DOCTOR ===\n";
    std::cout << "ID: " << id << "\n";
    std::cout << "Nombre: Dr. " << nombre << " " << apellido << "\n";
    std::cout << "Cedula Profesional: " << cedulaProfesional << "\n";
    std::cout << "Especialidad: " << especialidad << "\n";
    std::cout << "Años de Experiencia: " << aniosExperiencia << "\n";
    std::cout << "Costo de Consulta: $" << std::fixed << std::setprecision(2) << costoConsulta << "\n";
    std::cout << "Horario de Atencion: " << horarioAtencion << "\n";
    std::cout << "Telefono: " << telefono << "\n";
    std::cout << "Email: " << email << "\n";
    std::cout << "Disponible: " << (disponible ? "Si" : "No") << "\n";
    std::cout << "Pacientes Asignados: " << cantidadPacientes << "\n";
    std::cout << "Citas Activas: " << cantidadCitas << "\n";
    std::cout << "Eliminado: " << (eliminado ? "Si" : "No") << "\n";
    
    char buffer[80];
    struct tm* timeinfo = localtime(&fechaCreacion);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    std::cout << "Fecha Creacion: " << buffer << "\n";
    
    timeinfo = localtime(&fechaModificacion);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    std::cout << "Fecha Modificacion: " << buffer << "\n";
}

// Métodos de validación
bool Doctor::validarDatos() const {
    if (strlen(nombre) == 0) return false;
    if (strlen(apellido) == 0) return false;
    if (strlen(cedulaProfesional) == 0) return false;
    if (strlen(especialidad) == 0) return false;
    if (aniosExperiencia < 0) return false;
    if (costoConsulta < 0.0f) return false;
    if (strlen(telefono) > 0 && strlen(telefono) < 7) return false;
    if (strlen(email) > 0 && strchr(email, '@') == nullptr) return false;
    return true;
}

bool Doctor::estaDisponible() const {
    return disponible && !eliminado;
}

size_t Doctor::obtenerTamano() {
    return sizeof(Doctor);
}