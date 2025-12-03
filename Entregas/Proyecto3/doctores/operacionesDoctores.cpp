#include "operacionesDoctores.hpp"
#include "../persistencia/GestorArchivos.hpp"
#include "../pacientes/Paciente.hpp"
#include "../utilidades/Formatos.hpp"
#include "../utilidades/Validaciones.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>

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

namespace {
    constexpr int MAX_RESULTADOS_BUSQUEDA = 100;
}

void menuDoctores(Hospital& hospital) {
    int opcion;
    do {
        system("pause");
        system("cls");
        cout << "\n=== GESTION DE DOCTORES ===" << endl;
        cout << "1. Registrar nuevo doctor" << endl;
        cout << "2. Buscar por ID" << endl;
        cout << "3. Buscar por cedula profesional" << endl;
        cout << "4. Buscar por nombre" << endl;
        cout << "5. Modificar datos" << endl;
        cout << "6. Eliminar" << endl;
        cout << "7. Listar todos" << endl;
        cout << "8. Listar por especialidad" << endl;
        cout << "9. Asignar paciente a doctor" << endl;
        cout << "10. Remover paciente de doctor" << endl;
        cout << "11. Listar pacientes de doctor" << endl;
        cout << "0. Volver" << endl;
        cout << "\nOpcion: ";
        opcion = Formatos::leerEntero();
        
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
                buscarDoctoresPorNombre();
                break;
            case 5:
                modificarDoctor();
                break;
            case 6:
                eliminarDoctor();
                break;
            case 7:
                listarTodosDoctores();
                break;
            case 8:
                listarDoctoresPorEspecialidad();
                break;
            case 9:
                asignarPacienteADoctor();
                break;
            case 10:
                removerPacienteDeDoctor();
                break;
            case 11:
                listarPacientesDeDoctor();
                break;
            case 0:
                cout << "Volviendo al menu principal..." << endl;
                system("pause");
                 system("cls");
                break;
            default:
                cout << "Opcion invalida" << endl;
        }
    } while(opcion != 0);
}

void registrarDoctor(Hospital& hospital) {
    cout << "\n=== REGISTRAR NUEVO DOCTOR ===" << endl;
    
    char nombre[50], apellido[50], cedulaProfesional[20], especialidad[50];
    int aniosExperiencia;
    float costoConsulta;
    char horarioAtencion[50], telefono[20], email[50];
    
    cout << "Nombre: ";
    Formatos::leerLinea(nombre, 50);
    cout << "Apellido: ";
    Formatos::leerLinea(apellido, 50);
    cout << "Cedula Profesional: ";
    Formatos::leerLinea(cedulaProfesional, 20);
    cout << "Especialidad: ";
    Formatos::leerLinea(especialidad, 50);
    cout << "Anios de Experiencia: ";
    aniosExperiencia = Formatos::leerEntero();
    cout << "Costo de Consulta: ";
    costoConsulta = Formatos::leerFloat();
    cout << "Horario de Atencion: ";
    Formatos::leerLinea(horarioAtencion, 50);
    cout << "Telefono: ";
    Formatos::leerLinea(telefono, 20);
    cout << "Email: ";
    Formatos::leerLinea(email, 50);
    
    if (!Validaciones::validarCedulaProfesional(cedulaProfesional)) {
        cout << "Error: Cedula profesional invalida (V-12345678 o E-12345678)." << endl;
        return;
    }
    if (aniosExperiencia < 0) {
        cout << "Error: Anios de experiencia no pueden ser negativos." << endl;
        return;
    }
    if (costoConsulta <= 0.0f) {
        cout << "Error: Costo de consulta debe ser mayor a 0." << endl;
        return;
    }
    if (email[0] != '\0' && !Validaciones::validarEmail(email)) {
        cout << "Error: Email invalido. Formato usuario@dominio.ext" << endl;
        return;
    }

    Doctor doctor(nombre, apellido, cedulaProfesional, especialidad);
    doctor.setAniosExperiencia(aniosExperiencia);
    doctor.setCostoConsulta(costoConsulta);
    if (horarioAtencion[0] != '\0') doctor.setHorarioAtencion(horarioAtencion);
    if (telefono[0] != '\0') doctor.setTelefono(telefono);
    if (email[0] != '\0') doctor.setEmail(email);
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
    int id = Formatos::leerEntero();
    
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
    Formatos::leerLinea(cedula, 20);
    
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
    int id = Formatos::leerEntero();
    
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
    Formatos::leerLinea(buffer, 50);
    if (strlen(buffer) > 0) doctor.setNombre(buffer);
    
    cout << "Nuevo apellido (" << doctor.getApellido() << "): ";
    Formatos::leerLinea(buffer, 50);
    if (strlen(buffer) > 0) doctor.setApellido(buffer);
    
    cout << "Nuevo telefono (" << doctor.getTelefono() << "): ";
    Formatos::leerLinea(buffer, 20);
    if (strlen(buffer) > 0) doctor.setTelefono(buffer);

    cout << "Nuevo email (" << doctor.getEmail() << "): ";
    Formatos::leerLinea(buffer, 50);
    if (strlen(buffer) > 0) {
        if (Validaciones::validarEmail(buffer)) {
            doctor.setEmail(buffer);
            cout << "Email actualizado correctamente." << endl;
        } else {
            cout << "Error: Email invalido. El valor no se actualizo." << endl;
        }
    }
    
    cout << "Nuevo horario (" << doctor.getHorarioAtencion() << "): ";
    Formatos::leerLinea(buffer, 50);
    if (strlen(buffer) > 0) doctor.setHorarioAtencion(buffer);
    
    cout << "Nuevo costo de consulta (" << doctor.getCostoConsulta() << "): ";
    Formatos::leerLinea(buffer, 50);
    if (strlen(buffer) > 0) {
        float nuevoCosto = static_cast<float>(atof(buffer));
        if (nuevoCosto > 0.0f) {
            doctor.setCostoConsulta(nuevoCosto);
        } else {
            cout << "Error: El costo debe ser mayor a 0. Se mantiene el actual." << endl;
        }
    }
    
    cout << "Disponible (1=Si, 0=No) (" << (doctor.getDisponible() ? "Si" : "No") << "): ";
    char disp[2];
    Formatos::leerLinea(disp, 2);
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
    int id = Formatos::leerEntero();
    
    if (GestorArchivos::eliminarDoctorLogico(id)) {
        cout << "Doctor eliminado exitosamente." << endl;
    } else {
        cout << "Error al eliminar el doctor o doctor no encontrado." << endl;
    }
}

void listarTodosDoctores() {
    vector<Doctor> doctores = GestorArchivos::listarDoctoresActivos();
    if (doctores.empty()) {
        cout << "No hay doctores registrados." << endl;
        return;
    }

    cout << "\n" << left << setw(4) << "ID" << setw(26) << "NOMBRE" << setw(16) << "CEDULA"
         << setw(20) << "ESPECIALIDAD" << setw(6) << "ANIOS" << setw(8) << "COSTO"
         << setw(8) << "DISP" << "\n";
    cout << string(100, '-') << endl;

    int contador = 0;
    for (const auto& d : doctores) {
        if (d.getEliminado()) continue;
        if (contador >= MAX_RESULTADOS_BUSQUEDA) break;

        string nombreCompleto = d.getNombre();
        nombreCompleto += " ";
        nombreCompleto += d.getApellido();

        ostringstream costo;
        costo << fixed << setprecision(2) << d.getCostoConsulta();

        cout << left << setw(4) << d.getId()
             << setw(26) << nombreCompleto
             << setw(16) << d.getCedulaProfesional()
             << setw(20) << d.getEspecialidad()
             << setw(6) << d.getAniosExperiencia()
             << setw(8) << costo.str()
             << setw(8) << (d.getDisponible() ? "Si" : "No") << "\n";
        contador++;
    }

    cout << "\nTotal de doctores activos: " << contador << endl;
}

void listarDoctoresPorEspecialidad() {
    cout << "\n=== LISTAR DOCTORES POR ESPECIALIDAD ===" << endl;
    char especialidad[50];
    cout << "Especialidad a buscar: ";
    Formatos::leerLinea(especialidad, 50);
    
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
    idDoctor = Formatos::leerEntero();
    cout << "ID del paciente: ";
    idPaciente = Formatos::leerEntero();
    
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
    idDoctor = Formatos::leerEntero();
    cout << "ID del paciente: ";
    idPaciente = Formatos::leerEntero();
    
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
    idDoctor = Formatos::leerEntero();
    
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

void buscarDoctoresPorNombre() {
    cout << "\nResultados de busqueda:\n";
    cout << string(100, '-') << endl;
    cout << "Nombre o parte del nombre: ";
    char buffer[100];
    cin.getline(buffer, 100);
    string busc = buffer;
    if (busc.empty()) {
        cout << "Operacion cancelada." << endl;
        return;
    }
    for (auto &c : busc) c = tolower((unsigned char)c);

    vector<Doctor> doctores = GestorArchivos::listarDoctoresActivos();
    int contador = 0;
    for (const auto &d : doctores) {
        if (d.getEliminado()) continue;
        if (contador >= MAX_RESULTADOS_BUSQUEDA) break;
        string nombre = string(d.getNombre());
        string apellido = string(d.getApellido());
        string nombre_l = nombre;
        string apellido_l = apellido;
        for (auto &c : nombre_l) c = tolower((unsigned char)c);
        for (auto &c : apellido_l) c = tolower((unsigned char)c);

        bool coincide = (nombre_l.find(busc) != string::npos) || (apellido_l.find(busc) != string::npos);
        if (!coincide) {
            string completo = nombre + " " + apellido;
            for (auto &c : completo) c = tolower((unsigned char)c);
            if (completo.find(busc) != string::npos) coincide = true;
        }

        if (coincide) {
            string nombreCompleto = d.getNombre();
            nombreCompleto += " ";
            nombreCompleto += d.getApellido();

            ostringstream costo;
            costo << fixed << setprecision(2) << d.getCostoConsulta();

            cout << left << setw(4) << d.getId()
                 << setw(26) << nombreCompleto
                 << setw(16) << d.getCedulaProfesional()
                 << setw(20) << d.getEspecialidad()
                 << setw(6) << d.getAniosExperiencia()
                 << setw(8) << costo.str()
                 << setw(8) << (d.getDisponible() ? "Si" : "No") << "\n";
            cout << string(100, '-') << endl;
            contador++;
            if (contador >= MAX_RESULTADOS_BUSQUEDA) break;
        }
    }

    if (contador == 0) {
        cout << "No se encontraron doctores con ese nombre." << endl;
    } else {
        cout << "Total encontrados: " << contador << endl;
    }
    system("pause");
    system("cls");
}