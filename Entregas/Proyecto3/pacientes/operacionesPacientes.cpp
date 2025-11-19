#include "operacionesPacientes.hpp"
#include "../persistencia/GestorArchivos.hpp"
#include <iostream>
#include <cstring>

using namespace std;

void limpiarBufferPacientes() {
    cin.ignore(256, '\n');
}
// En Paciente.cpp, agregar:

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
void menuPacientes(Hospital& hospital) {
    int opcion;
    do {
        cout << "\n=== GESTION DE PACIENTES ===" << endl;
        cout << "1. Registrar nuevo paciente" << endl;
        cout << "2. Buscar por ID" << endl;
        cout << "3. Buscar por cedula" << endl;
        cout << "4. Modificar datos" << endl;
        cout << "5. Eliminar" << endl;
        cout << "6. Listar todos" << endl;
        cout << "7. Volver" << endl;
        cout << "\nOpcion: ";
        cin >> opcion;
        limpiarBufferPacientes();
        
        switch(opcion) {
            case 1:
                registrarPaciente(hospital);
                break;
            case 2:
                buscarPacientePorID();
                break;
            case 3:
                buscarPacientePorCedula();
                break;
            case 4:
                modificarPaciente();
                break;
            case 5:
                eliminarPaciente();
                break;
            case 6:
                listarTodosPacientes();
                break;
            case 7:
                cout << "Volviendo al menu principal..." << endl;
                break;
            default:
                cout << "Opcion invalida" << endl;
        }
    } while(opcion != 7);
}

void registrarPaciente(Hospital& hospital) {
    cout << "\n=== REGISTRAR NUEVO PACIENTE ===" << endl;
    
    char nombre[50], apellido[50], cedula[20];
    int edad;
    char sexo;
    char tipoSangre[5], telefono[20], email[50], direccion[80], alergias[100], observaciones[200];
    
    cout << "Nombre: ";
    cin.getline(nombre, 50);
    cout << "Apellido: ";
    cin.getline(apellido, 50);
    cout << "Cedula (V-12345678): ";
    cin.getline(cedula, 20);
    cout << "Edad: ";
    cin >> edad;
    limpiarBufferPacientes();
    cout << "Sexo (M/F/O): ";
    cin >> sexo;
    limpiarBufferPacientes();
    cout << "Tipo de sangre (A+, A-, etc): ";
    cin.getline(tipoSangre, 5);
    cout << "Telefono: ";
    cin.getline(telefono, 20);
    cout << "Email: ";
    cin.getline(email, 50);
    cout << "Direccion: ";
    cin.getline(direccion, 80);
    cout << "Alergias: ";
    cin.getline(alergias, 100);
    cout << "Observaciones: ";
    cin.getline(observaciones, 200);
    
    Paciente paciente(nombre, apellido, cedula);
    paciente.setEdad(edad);
    paciente.setSexo(sexo);
    paciente.setTipoSangre(tipoSangre);
    paciente.setTelefono(telefono);
    paciente.setEmail(email);
    paciente.setDireccion(direccion);
    paciente.setAlergias(alergias);
    paciente.setObservaciones(observaciones);
    
    if (paciente.validarDatos()) {
        if (GestorArchivos::guardarPaciente(paciente)) {
            hospital.incrementarPacientesRegistrados();
            GestorArchivos::guardarHospital(hospital);
            cout << "Paciente registrado exitosamente. ID: " << paciente.getId() << endl;
        } else {
            cout << "Error al guardar el paciente." << endl;
        }
    } else {
        cout << "Error: Datos del paciente no validos." << endl;
    }
}

void buscarPacientePorID() {
    cout << "\n=== BUSCAR PACIENTE POR ID ===" << endl;
    cout << "ID del paciente: ";
    int id;
    cin >> id;
    limpiarBufferPacientes();
    
    Paciente paciente = GestorArchivos::buscarPacientePorID(id);
    if (paciente.getId() != -1) {
        paciente.mostrarInformacionCompleta();
    } else {
        cout << "Paciente no encontrado." << endl;
    }
}

void buscarPacientePorCedula() {
    cout << "\n=== BUSCAR PACIENTE POR CEDULA ===" << endl;
    char cedula[20];
    cout << "Cedula: ";
    cin.getline(cedula, 20);
    
    Paciente paciente = GestorArchivos::buscarPacientePorCedula(cedula);
    if (paciente.getId() != -1) {
        paciente.mostrarInformacionCompleta();
    } else {
        cout << "Paciente no encontrado." << endl;
    }
}

void modificarPaciente() {
    cout << "\n=== MODIFICAR PACIENTE ===" << endl;
    cout << "ID del paciente a modificar: ";
    int id;
    cin >> id;
    limpiarBufferPacientes();
    
    Paciente paciente = GestorArchivos::buscarPacientePorID(id);
    if (paciente.getId() == -1) {
        cout << "Paciente no encontrado." << endl;
        return;
    }
    
    cout << "Paciente actual:" << endl;
    paciente.mostrarInformacionBasica();
    
    char buffer[200];
    cout << "\nDeje en blanco para no cambiar." << endl;
    
    cout << "Nuevo nombre (" << paciente.getNombre() << "): ";
    cin.getline(buffer, 50);
    if (strlen(buffer) > 0) paciente.setNombre(buffer);
    
    cout << "Nuevo apellido (" << paciente.getApellido() << "): ";
    cin.getline(buffer, 50);
    if (strlen(buffer) > 0) paciente.setApellido(buffer);
    
    cout << "Nuevo telefono (" << paciente.getTelefono() << "): ";
    cin.getline(buffer, 20);
    if (strlen(buffer) > 0) paciente.setTelefono(buffer);
    
    cout << "Nuevo email (" << paciente.getEmail() << "): ";
    cin.getline(buffer, 50);
    if (strlen(buffer) > 0) paciente.setEmail(buffer);
    
    cout << "Nuevas alergias (" << paciente.getAlergias() << "): ";
    cin.getline(buffer, 100);
    if (strlen(buffer) > 0) paciente.setAlergias(buffer);
    
    cout << "Nuevas observaciones (" << paciente.getObservaciones() << "): ";
    cin.getline(buffer, 200);
    if (strlen(buffer) > 0) paciente.setObservaciones(buffer);
    
    if (GestorArchivos::guardarPaciente(paciente)) {
        cout << "Paciente modificado exitosamente." << endl;
    } else {
        cout << "Error al modificar el paciente." << endl;
    }
}

void eliminarPaciente() {
    cout << "\n=== ELIMINAR PACIENTE ===" << endl;
    cout << "ID del paciente a eliminar: ";
    int id;
    cin >> id;
    limpiarBufferPacientes();
    
    if (GestorArchivos::eliminarPacienteLogico(id)) {
        cout << "Paciente eliminado exitosamente." << endl;
    } else {
        cout << "Error al eliminar el paciente o paciente no encontrado." << endl;
    }
}

void listarTodosPacientes() {
    cout << "\n=== LISTA DE PACIENTES ===" << endl;
    vector<Paciente> pacientes = GestorArchivos::listarPacientesActivos();
    
    if (pacientes.empty()) {
        cout << "No hay pacientes registrados." << endl;
        return;
    }
    
    for (const auto& paciente : pacientes) {
        paciente.mostrarInformacionBasica();
    }
    cout << "Total: " << pacientes.size() << " pacientes" << endl;
}