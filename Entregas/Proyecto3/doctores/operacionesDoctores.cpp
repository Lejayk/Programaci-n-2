#include "operacionesDoctores.hpp"
#include "../persistencia/GestorArchivos.hpp"
#include "../pacientes/Paciente.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <cctype>

// Comparacion case-insensitive portable
static int strcase_cmp(const char* a, const char* b) {
    if (!a) return (b ? -1 : 0);
    if (!b) return 1;
    while (*a && *b) {
        unsigned char ca = (unsigned char)std::tolower((unsigned char)*a);
        unsigned char cb = (unsigned char)std::tolower((unsigned char)*b);
        if (ca != cb) return ca - cb;
        ++a; ++b;
    }
    return (unsigned char)std::tolower((unsigned char)*a) - (unsigned char)std::tolower((unsigned char)*b);
}

using namespace std;

void limpiarBufferDoctores() {
    cin.ignore(256, '\n');
}

void menuDoctores(Hospital& hospital) {
    int opcion;
    do {
        cout << "\n=== GESTION DE DOCTORES ===" << endl;
        cout << "1. Registrar nuevo doctor" << endl;
        cout << "2. Buscar por ID" << endl;
        cout << "3. Buscar por cedula profesional" << endl;
        cout << "4. Modificar datos" << endl;
        cout << "5. Eliminar" << endl;
        cout << "6. Listar todos" << endl;
        cout << "7. Listar por especialidad" << endl;
        cout << "8. Asignar paciente a doctor" << endl;
        cout << "9. Remover paciente de doctor" << endl;
        cout << "10. Listar pacientes de doctor" << endl;
        cout << "11. Volver" << endl;
        cout << "\nOpcion: ";
        cin >> opcion;
        limpiarBufferDoctores();
        
        switch(opcion) {
            case 1:
                registrarDoctor(hospital);
                break;
            case 2:
                buscarDoctorPorID();
                break;
            case 3:
                buscarDoctorPorCedula();
                break;
            case 4:
                modificarDoctor();
                break;
            case 5:
                eliminarDoctor();
                break;
            case 6:
                listarTodosDoctores();
                break;
            case 7:
                listarDoctoresPorEspecialidad();
                break;
            case 8:
                asignarPacienteADoctor();
                break;
            case 9:
                removerPacienteDeDoctor();
                break;
            case 10:
                listarPacientesDeDoctor();
                break;
            case 11:
                cout << "Volviendo al menu principal..." << endl;
                break;
            default:
                cout << "Opcion invalida" << endl;
        }
    } while(opcion != 11);
}

void registrarDoctor(Hospital& hospital) {
    cout << "\n=== REGISTRAR NUEVO DOCTOR ===" << endl;
    
    char nombre[50], apellido[50], cedulaProfesional[20], especialidad[50];
    int aniosExperiencia;
    float costoConsulta;
    char horarioAtencion[50], telefono[20], email[50];
    
    cout << "Nombre: ";
    cin.getline(nombre, 50);
    cout << "Apellido: ";
    cin.getline(apellido, 50);
    cout << "Cedula Profesional: ";
    cin.getline(cedulaProfesional, 20);
    cout << "Especialidad: ";
    cin.getline(especialidad, 50);
    cout << "Años de Experiencia: ";
    cin >> aniosExperiencia;
    limpiarBufferDoctores();
    cout << "Costo de Consulta: ";
    cin >> costoConsulta;
    limpiarBufferDoctores();
    cout << "Horario de Atencion: ";
    cin.getline(horarioAtencion, 50);
    cout << "Telefono: ";
    cin.getline(telefono, 20);
    cout << "Email: ";
    cin.getline(email, 50);
    
    Doctor doctor(nombre, apellido, cedulaProfesional, especialidad);
    doctor.setAniosExperiencia(aniosExperiencia);
    doctor.setCostoConsulta(costoConsulta);
    doctor.setHorarioAtencion(horarioAtencion);
    doctor.setTelefono(telefono);
    doctor.setEmail(email);
    doctor.setDisponible(true);
    
    if (doctor.validarDatos()) {
        if (GestorArchivos::guardarDoctor(doctor)) {
            hospital.incrementarDoctoresRegistrados();
            GestorArchivos::guardarHospital(hospital);
            cout << "Doctor registrado exitosamente. ID: " << doctor.getId() << endl;
        } else {
            cout << "Error al guardar el doctor." << endl;
        }
    } else {
        cout << "Error: Datos del doctor no validos." << endl;
    }
}

void buscarDoctorPorID() {
    cout << "\n=== BUSCAR DOCTOR POR ID ===" << endl;
    cout << "ID del doctor: ";
    int id;
    cin >> id;
    limpiarBufferDoctores();
    
    Doctor doctor = GestorArchivos::buscarDoctorPorID(id);
    if (doctor.getId() != -1) {
        doctor.mostrarInformacionCompleta();
    } else {
        cout << "Doctor no encontrado." << endl;
    }
}

void buscarDoctorPorCedula() {
    cout << "\n=== BUSCAR DOCTOR POR CEDULA PROFESIONAL ===" << endl;
    char cedula[20];
    cout << "Cedula Profesional: ";
    cin.getline(cedula, 20);
    
    // Buscar en todos los doctores
    vector<Doctor> doctores = GestorArchivos::listarDoctoresActivos();
    bool encontrado = false;
    for (const auto& doctor : doctores) {
        if (strcmp(doctor.getCedulaProfesional(), cedula) == 0) {
            doctor.mostrarInformacionCompleta();
            encontrado = true;
            break;
        }
    }
    
    if (!encontrado) {
        cout << "Doctor no encontrado." << endl;
    }
}

void modificarDoctor() {
    cout << "\n=== MODIFICAR DOCTOR ===" << endl;
    cout << "ID del doctor a modificar: ";
    int id;
    cin >> id;
    limpiarBufferDoctores();
    
    Doctor doctor = GestorArchivos::buscarDoctorPorID(id);
    if (doctor.getId() == -1) {
        cout << "Doctor no encontrado." << endl;
        return;
    }
    
    cout << "Doctor actual:" << endl;
    doctor.mostrarInformacionBasica();
    
    char buffer[200];
    cout << "\nDeje en blanco para no cambiar." << endl;
    
    cout << "Nuevo nombre (" << doctor.getNombre() << "): ";
    cin.getline(buffer, 50);
    if (strlen(buffer) > 0) doctor.setNombre(buffer);
    
    cout << "Nuevo apellido (" << doctor.getApellido() << "): ";
    cin.getline(buffer, 50);
    if (strlen(buffer) > 0) doctor.setApellido(buffer);
    
    cout << "Nuevo telefono (" << doctor.getTelefono() << "): ";
    cin.getline(buffer, 20);
    if (strlen(buffer) > 0) doctor.setTelefono(buffer);
    
    cout << "Nuevo email (" << doctor.getEmail() << "): ";
    cin.getline(buffer, 50);
    if (strlen(buffer) > 0) doctor.setEmail(buffer);
    
    cout << "Nuevo horario (" << doctor.getHorarioAtencion() << "): ";
    cin.getline(buffer, 50);
    if (strlen(buffer) > 0) doctor.setHorarioAtencion(buffer);
    
    cout << "Nuevo costo de consulta (" << doctor.getCostoConsulta() << "): ";
    float nuevoCosto;
    if (cin >> nuevoCosto) {
        doctor.setCostoConsulta(nuevoCosto);
    }
    limpiarBufferDoctores();
    
    cout << "Disponible (1=Si, 0=No) (" << (doctor.getDisponible() ? "Si" : "No") << "): ";
    char disp[2];
    cin.getline(disp, 2);
    if (strlen(disp) > 0) {
        doctor.setDisponible(disp[0] == '1');
    }
    
    if (GestorArchivos::guardarDoctor(doctor)) {
        cout << "Doctor modificado exitosamente." << endl;
    } else {
        cout << "Error al modificar el doctor." << endl;
    }
}

void eliminarDoctor() {
    cout << "\n=== ELIMINAR DOCTOR ===" << endl;
    cout << "ID del doctor a eliminar: ";
    int id;
    cin >> id;
    limpiarBufferDoctores();
    
    if (GestorArchivos::eliminarDoctorLogico(id)) {
        cout << "Doctor eliminado exitosamente." << endl;
    } else {
        cout << "Error al eliminar el doctor o doctor no encontrado." << endl;
    }
}

void listarTodosDoctores() {
    cout << "\n=== LISTA DE DOCTORES ===" << endl;
    vector<Doctor> doctores = GestorArchivos::listarDoctoresActivos();
    
    if (doctores.empty()) {
        cout << "No hay doctores registrados." << endl;
        return;
    }
    
    for (const auto& doctor : doctores) {
        doctor.mostrarInformacionBasica();
    }
    cout << "Total: " << doctores.size() << " doctores" << endl;
}

void listarDoctoresPorEspecialidad() {
    cout << "\n=== LISTAR DOCTORES POR ESPECIALIDAD ===" << endl;
    char especialidad[50];
    cout << "Especialidad a buscar: ";
    cin.getline(especialidad, 50);
    
    vector<Doctor> doctores = GestorArchivos::listarDoctoresActivos();
    int contador = 0;
    
    cout << "\nDoctores de " << especialidad << ":" << endl;
    for (const auto& doctor : doctores) {
        if (strcase_cmp(doctor.getEspecialidad(), especialidad) == 0) {
            doctor.mostrarInformacionBasica();
            contador++;
        }
    }
    
    if (contador == 0) {
        cout << "No se encontraron doctores con esa especialidad." << endl;
    } else {
        cout << "Total: " << contador << " doctores" << endl;
    }
}

void asignarPacienteADoctor() {
    cout << "\n=== ASIGNAR PACIENTE A DOCTOR ===" << endl;
    int idDoctor, idPaciente;
    
    cout << "ID del doctor: ";
    cin >> idDoctor;
    cout << "ID del paciente: ";
    cin >> idPaciente;
    limpiarBufferDoctores();
    
    Doctor doctor = GestorArchivos::buscarDoctorPorID(idDoctor);
    if (doctor.getId() == -1) {
        cout << "Doctor no encontrado." << endl;
        return;
    }
    
    Paciente paciente = GestorArchivos::buscarPacientePorID(idPaciente);
    if (paciente.getId() == -1) {
        cout << "Paciente no encontrado." << endl;
        return;
    }
    
    if (doctor.agregarPacienteID(idPaciente)) {
        if (GestorArchivos::guardarDoctor(doctor)) {
            cout << "Paciente asignado exitosamente al doctor." << endl;
        } else {
            cout << "Error al guardar los cambios del doctor." << endl;
        }
    } else {
        cout << "Error al asignar el paciente al doctor." << endl;
    }
}

void removerPacienteDeDoctor() {
    cout << "\n=== REMOVER PACIENTE DE DOCTOR ===" << endl;
    int idDoctor, idPaciente;
    
    cout << "ID del doctor: ";
    cin >> idDoctor;
    cout << "ID del paciente: ";
    cin >> idPaciente;
    limpiarBufferDoctores();
    
    Doctor doctor = GestorArchivos::buscarDoctorPorID(idDoctor);
    if (doctor.getId() == -1) {
        cout << "Doctor no encontrado." << endl;
        return;
    }
    
    if (doctor.eliminarPacienteID(idPaciente)) {
        if (GestorArchivos::guardarDoctor(doctor)) {
            cout << "Paciente removido exitosamente del doctor." << endl;
        } else {
            cout << "Error al guardar los cambios del doctor." << endl;
        }
    } else {
        cout << "Error: El paciente no estaba asignado a este doctor." << endl;
    }
}

void listarPacientesDeDoctor() {
    cout << "\n=== LISTAR PACIENTES DE DOCTOR ===" << endl;
    int idDoctor;
    
    cout << "ID del doctor: ";
    cin >> idDoctor;
    limpiarBufferDoctores();
    
    Doctor doctor = GestorArchivos::buscarDoctorPorID(idDoctor);
    if (doctor.getId() == -1) {
        cout << "Doctor no encontrado." << endl;
        return;
    }
    
    const int* pacientesIDs = doctor.getPacientesIDs();
    int cantidad = doctor.getCantidadPacientes();
    
    if (cantidad == 0) {
        cout << "El doctor no tiene pacientes asignados." << endl;
        return;
    }
    
    cout << "\nPacientes del Dr. " << doctor.getNombre() << " " << doctor.getApellido() << ":" << endl;
    for (int i = 0; i < cantidad; i++) {
        Paciente paciente = GestorArchivos::buscarPacientePorID(pacientesIDs[i]);
        if (paciente.getId() != -1) {
            paciente.mostrarInformacionBasica();
        }
    }
    cout << "Total: " << cantidad << " pacientes" << endl;
}