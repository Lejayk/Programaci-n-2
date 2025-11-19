#include "operacionesCitas.hpp"
#include "../persistencia/GestorArchivos.hpp"
#include "../pacientes/Paciente.hpp"
#include "../doctores/Doctor.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>

using namespace std;

void limpiarBufferCitas() {
    cin.ignore(256, '\n');
}

bool verificarDisponibilidadDoctor(int idDoctor, const char* fecha, const char* hora, int citaID = -1) {
    vector<Cita> citas = GestorArchivos::listarCitasActivas();
    
    for (const auto& cita : citas) {
        // Si es la misma cita, ignorar (para modificaciones)
        if (cita.getId() == citaID) continue;
        
        if (cita.getDoctorID() == idDoctor && 
            strcmp(cita.getFecha(), fecha) == 0 && 
            strcmp(cita.getHora(), hora) == 0 &&
            cita.estaAgendada()) {
            return false; // No disponible
        }
    }
    return true; // Disponible
}

void menuCitas(Hospital& hospital) {
    int opcion;
    do {
        system("pause");
        system("cls");
        cout << "\n=== GESTION DE CITAS ===" << endl;
        cout << "1. Agendar cita" << endl;
        cout << "2. Cancelar cita" << endl;
        cout << "3. Atender cita" << endl;
        cout << "4. Buscar por ID" << endl;
        cout << "5. Listar por paciente" << endl;
        cout << "6. Listar por doctor" << endl;
        cout << "7. Listar por fecha" << endl;
        cout << "8. Listar citas pendientes" << endl;
        cout << "9. Modificar cita" << endl;
        cout << "10. Volver" << endl;
        cout << "\nOpcion: ";
        cin >> opcion;
        limpiarBufferCitas();
        
        switch(opcion) {
            case 1:
                agendarCita(hospital);
                break;
            case 2:
                cancelarCita();
                break;
            case 3:
                atenderCita(hospital);
                break;
            case 4:
                buscarCitaPorID();
                break;
            case 5:
                listarCitasPorPaciente();
                break;
            case 6:
                listarCitasPorDoctor();
                break;
            case 7:
                listarCitasPorFecha();
                break;
            case 8:
                listarCitasPendientes();
                break;
            case 9:
                modificarCita();
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

void agendarCita(Hospital& hospital) {
    cout << "\n=== AGENDAR CITA ===" << endl;
    
    int pacienteID, doctorID;
    char fecha[11], hora[6], motivo[150];
    
    cout << "ID del paciente: ";
    cin >> pacienteID;
    cout << "ID del doctor: ";
    cin >> doctorID;
    limpiarBufferCitas();
    cout << "Fecha (YYYY-MM-DD): ";
    cin.getline(fecha, 11);
    cout << "Hora (HH:MM): ";
    cin.getline(hora, 6);
    cout << "Motivo de la cita: ";
    cin.getline(motivo, 150);
    
    // Verificar que paciente existe
    Paciente paciente = GestorArchivos::buscarPacientePorID(pacienteID);
    if (paciente.getId() == -1) {
        cout << "Error: Paciente no encontrado." << endl;
        return;
    }
    
    // Verificar que doctor existe
    Doctor doctor = GestorArchivos::buscarDoctorPorID(doctorID);
    if (doctor.getId() == -1) {
        cout << "Error: Doctor no encontrado." << endl;
        return;
    }
    
    if (!doctor.getDisponible()) {
        cout << "Error: El doctor no esta disponible." << endl;
        return;
    }
    
    // Verificar disponibilidad del doctor
    if (!verificarDisponibilidadDoctor(doctorID, fecha, hora)) {
        cout << "Error: El doctor no esta disponible en esa fecha y hora." << endl;
        return;
    }
    
    Cita cita(pacienteID, doctorID, fecha, hora, motivo);
    
    if (cita.validarDatos()) {
        if (GestorArchivos::guardarCita(cita)) {
            // Actualizar paciente
            if (paciente.agregarCitaID(cita.getId())) {
                GestorArchivos::guardarPaciente(paciente);
            }
            
            // Actualizar doctor
            if (doctor.agregarCitaID(cita.getId())) {
                GestorArchivos::guardarDoctor(doctor);
            }
            
            hospital.incrementarCitasAgendadas();
            GestorArchivos::guardarHospital(hospital);
            
            cout << "Cita agendada exitosamente. ID: " << cita.getId() << endl;
        } else {
            cout << "Error al guardar la cita." << endl;
        }
    } else {
        cout << "Error: Datos de la cita no validos." << endl;
    }
}

void cancelarCita() {
    cout << "\n=== CANCELAR CITA ===" << endl;
    cout << "ID de la cita a cancelar: ";
    int id;
    cin >> id;
    limpiarBufferCitas();
    
    Cita cita = GestorArchivos::buscarCitaPorID(id);
    if (cita.getId() == -1) {
        cout << "Cita no encontrada." << endl;
        return;
    }
    
    if (cita.estaCancelada()) {
        cout << "La cita ya esta cancelada." << endl;
        return;
    }
    
    if (cita.estaAtendida()) {
        cout << "No se puede cancelar una cita ya atendida." << endl;
        return;
    }
    
    cita.setEstado("Cancelada");
    
    if (GestorArchivos::guardarCita(cita)) {
        cout << "Cita cancelada exitosamente." << endl;
    } else {
        cout << "Error al cancelar la cita." << endl;
    }
}

void atenderCita(Hospital& hospital) {
    cout << "\n=== ATENDER CITA ===" << endl;
    cout << "ID de la cita a atender: ";
    int id;
    cin >> id;
    limpiarBufferCitas();
    
    Cita cita = GestorArchivos::buscarCitaPorID(id);
    if (cita.getId() == -1) {
        cout << "Cita no encontrada." << endl;
        return;
    }
    
    if (!cita.estaAgendada()) {
        cout << "Error: La cita no esta en estado 'Agendada'." << endl;
        return;
    }
    
    char observaciones[200];
    cout << "Observaciones de la consulta: ";
    cin.getline(observaciones, 200);
    
    cita.setEstado("Atendida");
    cita.setAtendida(true);
    cita.setObservaciones(observaciones);
    // Nota: consultaID se asignará cuando se cree el historial médico
    
    if (GestorArchivos::guardarCita(cita)) {
        hospital.incrementarConsultasRealizadas();
        GestorArchivos::guardarHospital(hospital);
        cout << "Cita marcada como atendida exitosamente." << endl;
        cout << "NOTA: Para completar el proceso, debe registrar la consulta en el historial médico." << endl;
    } else {
        cout << "Error al atender la cita." << endl;
    }
}

void buscarCitaPorID() {
    cout << "\n=== BUSCAR CITA POR ID ===" << endl;
    cout << "ID de la cita: ";
    int id;
    cin >> id;
    limpiarBufferCitas();
    
    Cita cita = GestorArchivos::buscarCitaPorID(id);
    if (cita.getId() != -1) {
        cita.mostrarInformacionCompleta();
        
        // Mostrar información adicional del paciente y doctor
        Paciente paciente = GestorArchivos::buscarPacientePorID(cita.getPacienteID());
        Doctor doctor = GestorArchivos::buscarDoctorPorID(cita.getDoctorID());
        
        if (paciente.getId() != -1) {
            cout << "\n--- INFORMACION DEL PACIENTE ---" << endl;
            paciente.mostrarInformacionBasica();
        }
        
        if (doctor.getId() != -1) {
            cout << "\n--- INFORMACION DEL DOCTOR ---" << endl;
            doctor.mostrarInformacionBasica();
        }
    } else {
        cout << "Cita no encontrada." << endl;
    }
}

void listarCitasPorPaciente() {
    cout << "\n=== LISTAR CITAS POR PACIENTE ===" << endl;
    cout << "ID del paciente: ";
    int id;
    cin >> id;
    limpiarBufferCitas();
    
    Paciente paciente = GestorArchivos::buscarPacientePorID(id);
    if (paciente.getId() == -1) {
        cout << "Paciente no encontrado." << endl;
        return;
    }
    
    vector<Cita> citas = GestorArchivos::listarCitasActivas();
    int contador = 0;
    
    cout << "\nCitas del paciente " << paciente.getNombre() << " " << paciente.getApellido() << ":" << endl;
    for (const auto& cita : citas) {
        if (cita.getPacienteID() == id) {
            cita.mostrarInformacionBasica();
            contador++;
        }
    }
    
    if (contador == 0) {
        cout << "No se encontraron citas para este paciente." << endl;
    } else {
        cout << "Total: " << contador << " citas" << endl;
    }
}

void listarCitasPorDoctor() {
    cout << "\n=== LISTAR CITAS POR DOCTOR ===" << endl;
    cout << "ID del doctor: ";
    int id;
    cin >> id;
    limpiarBufferCitas();
    
    Doctor doctor = GestorArchivos::buscarDoctorPorID(id);
    if (doctor.getId() == -1) {
        cout << "Doctor no encontrado." << endl;
        return;
    }
    
    vector<Cita> citas = GestorArchivos::listarCitasActivas();
    int contador = 0;
    
    cout << "\nCitas del Dr. " << doctor.getNombre() << " " << doctor.getApellido() << ":" << endl;
    for (const auto& cita : citas) {
        if (cita.getDoctorID() == id) {
            cita.mostrarInformacionBasica();
            contador++;
        }
    }
    
    if (contador == 0) {
        cout << "No se encontraron citas para este doctor." << endl;
    } else {
        cout << "Total: " << contador << " citas" << endl;
    }
}

void listarCitasPorFecha() {
    cout << "\n=== LISTAR CITAS POR FECHA ===" << endl;
    char fecha[11];
    cout << "Fecha (YYYY-MM-DD): ";
    cin.getline(fecha, 11);
    
    Cita citaTemp;
    if (!citaTemp.validarFecha(fecha)) {
        cout << "Error: Fecha invalida." << endl;
        return;
    }
    
    vector<Cita> citas = GestorArchivos::listarCitasActivas();
    int contador = 0;
    
    cout << "\nCitas del " << fecha << ":" << endl;
    for (const auto& cita : citas) {
        if (strcmp(cita.getFecha(), fecha) == 0) {
            cita.mostrarInformacionBasica();
            contador++;
        }
    }
    
    if (contador == 0) {
        cout << "No se encontraron citas para esta fecha." << endl;
    } else {
        cout << "Total: " << contador << " citas" << endl;
    }
}

void listarCitasPendientes() {
    cout << "\n=== CITAS PENDIENTES ===" << endl;
    
    vector<Cita> citas = GestorArchivos::listarCitasActivas();
    int contador = 0;
    
    for (const auto& cita : citas) {
        if (cita.estaAgendada()) {
            cita.mostrarInformacionBasica();
            contador++;
        }
    }
    
    if (contador == 0) {
        cout << "No hay citas pendientes." << endl;
    } else {
        cout << "Total: " << contador << " citas pendientes" << endl;
    }
}

void modificarCita() {
    cout << "\n=== MODIFICAR CITA ===" << endl;
    cout << "ID de la cita a modificar: ";
    int id;
    cin >> id;
    limpiarBufferCitas();
    
    Cita cita = GestorArchivos::buscarCitaPorID(id);
    if (cita.getId() == -1) {
        cout << "Cita no encontrada." << endl;
        return;
    }
    
    if (cita.estaAtendida()) {
        cout << "Error: No se puede modificar una cita ya atendida." << endl;
        return;
    }
    
    cout << "Cita actual:" << endl;
    cita.mostrarInformacionBasica();
    
    char buffer[200];
    int opcion;
    
    cout << "\n¿Que desea modificar?" << endl;
    cout << "1. Fecha" << endl;
    cout << "2. Hora" << endl;
    cout << "3. Observaciones" << endl;
    cout << "4. Estado" << endl;
    cout << "5. Cancelar" << endl;
    cout << "Opcion: ";
    cin >> opcion;
    limpiarBufferCitas();
    
    switch(opcion) {
        case 1: {
            cout << "Nueva fecha (YYYY-MM-DD): ";
            cin.getline(buffer, 11);
            if (strlen(buffer) > 0) {
                // Verificar disponibilidad si cambia fecha/hora
                if (!verificarDisponibilidadDoctor(cita.getDoctorID(), buffer, cita.getHora(), cita.getId())) {
                    cout << "Error: El doctor no esta disponible en esa fecha." << endl;
                    return;
                }
                cita.setFecha(buffer);
            }
            break;
        }
        case 2: {
            cout << "Nueva hora (HH:MM): ";
            cin.getline(buffer, 6);
            if (strlen(buffer) > 0) {
                // Verificar disponibilidad
                if (!verificarDisponibilidadDoctor(cita.getDoctorID(), cita.getFecha(), buffer, cita.getId())) {
                    cout << "Error: El doctor no esta disponible a esa hora." << endl;
                    return;
                }
                cita.setHora(buffer);
            }
            break;
        }
        case 3: {
            cout << "Nuevas observaciones: ";
            cin.getline(buffer, 200);
            if (strlen(buffer) > 0) {
                cita.setObservaciones(buffer);
            }
            break;
        }
        case 4: {
            cout << "Nuevo estado (Agendada/Cancelada/Reprogramada): ";
            cin.getline(buffer, 20);
            if (strlen(buffer) > 0) {
                cita.setEstado(buffer);
            }
            break;
        }
        case 5:
            cout << "Modificacion cancelada." << endl;
            return;
        default:
            cout << "Opcion invalida." << endl;
            return;
    }
    
    if (GestorArchivos::guardarCita(cita)) {
        cout << "Cita modificada exitosamente." << endl;
    } else {
        cout << "Error al modificar la cita." << endl;
    }
}