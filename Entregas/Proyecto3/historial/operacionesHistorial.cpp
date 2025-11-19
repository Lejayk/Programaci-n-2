#include "operacionesHistorial.hpp"
#include "../persistencia/GestorArchivos.hpp"
#include "../pacientes/Paciente.hpp"
#include "../doctores/Doctor.hpp"
#include "../citas/Cita.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

void limpiarBufferHistorial() {
    cin.ignore(256, '\n');
}

HistorialMedico buscarUltimaConsultaPaciente(int pacienteID) {
    Paciente paciente = GestorArchivos::buscarPacientePorID(pacienteID);
    if (paciente.getId() == -1 || paciente.getPrimerConsultaID() == -1) {
        HistorialMedico vacio;
        vacio.setId(-1);
        return vacio;
    }
    
    HistorialMedico actual = GestorArchivos::buscarHistorialPorID(paciente.getPrimerConsultaID());
    if (actual.getId() == -1) {
        return actual;
    }
    
    // Seguir la cadena hasta la última consulta
    while (actual.getSiguienteConsultaID() != -1) {
        actual = GestorArchivos::buscarHistorialPorID(actual.getSiguienteConsultaID());
        if (actual.getId() == -1) {
            break;
        }
    }
    
    return actual;
}

void menuHistorial(Hospital& hospital) {
    int opcion;
    do {
        cout << "\n=== HISTORIAL MEDICO ===" << endl;
        cout << "1. Registrar nueva consulta" << endl;
        cout << "2. Ver historial de paciente" << endl;
        cout << "3. Buscar consulta por ID" << endl;
        cout << "4. Listar todas las consultas" << endl;
        cout << "5. Eliminar consulta" << endl;
        cout << "6. Reparar lista de historial" << endl;
        cout << "7. Volver" << endl;
        cout << "\nOpcion: ";
        cin >> opcion;
        limpiarBufferHistorial();
        
        switch(opcion) {
            case 1:
                registrarConsulta(hospital);
                break;
            case 2:
                verHistorialPaciente();
                break;
            case 3:
                buscarConsultaPorID();
                break;
            case 4:
                listarTodasConsultas();
                break;
            case 5:
                eliminarConsulta();
                break;
            case 6:
                repararListaHistorial();
                break;
            case 7:
                cout << "Volviendo al menu principal..." << endl;
                break;
            default:
                cout << "Opcion invalida" << endl;
        }
    } while(opcion != 7);
}

void registrarConsulta(Hospital& hospital) {
    cout << "\n=== REGISTRAR NUEVA CONSULTA ===" << endl;
    
    int pacienteID, doctorID;
    float costo;
    char diagnostico[200], tratamiento[200], medicamentos[150];
    char fecha[11], hora[6];
    
    cout << "ID del paciente: ";
    cin >> pacienteID;
    cout << "ID del doctor: ";
    cin >> doctorID;
    cout << "Costo de la consulta: ";
    cin >> costo;
    limpiarBufferHistorial();
    
    cout << "Diagnostico: ";
    cin.getline(diagnostico, 200);
    cout << "Tratamiento: ";
    cin.getline(tratamiento, 200);
    cout << "Medicamentos: ";
    cin.getline(medicamentos, 150);
    
    // Usar fecha y hora actual por defecto
    time_t ahora = time(nullptr);
    tm* tiempoLocal = localtime(&ahora);
    snprintf(fecha, sizeof(fecha), "%04d-%02d-%02d", 
             tiempoLocal->tm_year + 1900, 
             tiempoLocal->tm_mon + 1, 
             tiempoLocal->tm_mday);
    snprintf(hora, sizeof(hora), "%02d:%02d", 
             tiempoLocal->tm_hour, 
             tiempoLocal->tm_min);
    
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
    
    HistorialMedico consulta(pacienteID, doctorID, diagnostico, tratamiento, medicamentos, costo);
    consulta.setFecha(fecha);
    consulta.setHora(hora);
    
    if (consulta.validarDatos()) {
        if (GestorArchivos::guardarHistorial(consulta)) {
            // Actualizar la lista enlazada del paciente
            if (paciente.getPrimerConsultaID() == -1) {
                // Primera consulta del paciente
                paciente.setPrimerConsultaID(consulta.getId());
            } else {
                // Buscar la última consulta y actualizar su enlace
                HistorialMedico ultimaConsulta = buscarUltimaConsultaPaciente(pacienteID);
                if (ultimaConsulta.getId() != -1) {
                    ultimaConsulta.setSiguienteConsultaID(consulta.getId());
                    GestorArchivos::guardarHistorial(ultimaConsulta);
                }
            }
            
            // Actualizar contador del paciente
            paciente.setCantidadConsultas(paciente.getCantidadConsultas() + 1);
            GestorArchivos::guardarPaciente(paciente);
            
            hospital.incrementarConsultasRealizadas();
            GestorArchivos::guardarHospital(hospital);
            
            cout << "Consulta registrada exitosamente. ID: " << consulta.getId() << endl;
        } else {
            cout << "Error al guardar la consulta." << endl;
        }
    } else {
        cout << "Error: Datos de la consulta no validos." << endl;
    }
}

void registrarConsultaDesdeCita(Hospital& hospital, int citaID, const char* diagnostico, 
                               const char* tratamiento, const char* medicamentos) {
    Cita cita = GestorArchivos::buscarCitaPorID(citaID);
    if (cita.getId() == -1) {
        cout << "Error: Cita no encontrada." << endl;
        return;
    }
    
    Doctor doctor = GestorArchivos::buscarDoctorPorID(cita.getDoctorID());
    float costo = (doctor.getId() != -1) ? doctor.getCostoConsulta() : 0.0f;
    
    HistorialMedico consulta(cita.getPacienteID(), cita.getDoctorID(), diagnostico, tratamiento, medicamentos, costo);
    
    if (consulta.validarDatos()) {
        if (GestorArchivos::guardarHistorial(consulta)) {
            Paciente paciente = GestorArchivos::buscarPacientePorID(cita.getPacienteID());
            
            // Actualizar la lista enlazada del paciente
            if (paciente.getPrimerConsultaID() == -1) {
                paciente.setPrimerConsultaID(consulta.getId());
            } else {
                HistorialMedico ultimaConsulta = buscarUltimaConsultaPaciente(cita.getPacienteID());
                if (ultimaConsulta.getId() != -1) {
                    ultimaConsulta.setSiguienteConsultaID(consulta.getId());
                    GestorArchivos::guardarHistorial(ultimaConsulta);
                }
            }
            
            // Actualizar contador del paciente
            paciente.setCantidadConsultas(paciente.getCantidadConsultas() + 1);
            GestorArchivos::guardarPaciente(paciente);
            
            // Actualizar cita con el ID de la consulta
            cita.setConsultaID(consulta.getId());
            cita.setEstado("Atendida");
            cita.setAtendida(true);
            GestorArchivos::guardarCita(cita);
            
            hospital.incrementarConsultasRealizadas();
            GestorArchivos::guardarHospital(hospital);
            
            cout << "Consulta registrada desde cita exitosamente. ID: " << consulta.getId() << endl;
        } else {
            cout << "Error al guardar la consulta." << endl;
        }
    } else {
        cout << "Error: Datos de la consulta no validos." << endl;
    }
}

void verHistorialPaciente() {
    cout << "\n=== VER HISTORIAL DE PACIENTE ===" << endl;
    cout << "ID del paciente: ";
    int id;
    cin >> id;
    limpiarBufferHistorial();
    
    Paciente paciente = GestorArchivos::buscarPacientePorID(id);
    if (paciente.getId() == -1) {
        cout << "Paciente no encontrado." << endl;
        return;
    }
    
    if (paciente.getPrimerConsultaID() == -1) {
        cout << "El paciente no tiene historial medico." << endl;
        return;
    }
    
    cout << "\n=== HISTORIAL MEDICO ===" << endl;
    cout << "Paciente: " << paciente.getNombre() << " " << paciente.getApellido() << endl;
    cout << "Total de consultas: " << paciente.getCantidadConsultas() << endl;
    cout << "=========================================" << endl;
    
    int consultaID = paciente.getPrimerConsultaID();
    int contador = 0;
    const int MAX_CONSULTAS = 1000; // Límite de seguridad
    
    while (consultaID != -1 && contador < MAX_CONSULTAS) {
        HistorialMedico consulta = GestorArchivos::buscarHistorialPorID(consultaID);
        if (consulta.getId() == -1) {
            cout << "Error: Consulta #" << consultaID << " no encontrada." << endl;
            break;
        }
        
        Doctor doctor = GestorArchivos::buscarDoctorPorID(consulta.getDoctorID());
        string nombreDoctor = "N/A";
        if (doctor.getId() != -1) {
            nombreDoctor = string("Dr. ") + doctor.getNombre() + " " + doctor.getApellido();
        }
        
        cout << "\n--- Consulta #" << (contador + 1) << " ---" << endl;
        cout << "ID: " << consulta.getId() << endl;
        cout << "Fecha: " << consulta.getFecha() << " " << consulta.getHora() << endl;
        cout << "Doctor: " << nombreDoctor << endl;
        cout << "Diagnostico: " << consulta.getDiagnostico() << endl;
        cout << "Tratamiento: " << consulta.getTratamiento() << endl;
        cout << "Medicamentos: " << consulta.getMedicamentos() << endl;
        cout << "Costo: $" << std::fixed << std::setprecision(2) << consulta.getCosto() << std::endl;
        cout << "-----------------------" << endl;
        
        consultaID = consulta.getSiguienteConsultaID();
        contador++;
    }
    
    if (contador == 0) {
        cout << "No se pudieron cargar las consultas del historial." << endl;
    } else {
        cout << "Total mostradas: " << contador << " consultas" << endl;
    }
}

void buscarConsultaPorID() {
    cout << "\n=== BUSCAR CONSULTA POR ID ===" << endl;
    cout << "ID de la consulta: ";
    int id;
    cin >> id;
    limpiarBufferHistorial();
    
    HistorialMedico consulta = GestorArchivos::buscarHistorialPorID(id);
    if (consulta.getId() != -1) {
        consulta.mostrarInformacionCompleta();
        
        // Mostrar información adicional
        Paciente paciente = GestorArchivos::buscarPacientePorID(consulta.getPacienteID());
        Doctor doctor = GestorArchivos::buscarDoctorPorID(consulta.getDoctorID());
        
        if (paciente.getId() != -1) {
            cout << "\n--- INFORMACION DEL PACIENTE ---" << endl;
            paciente.mostrarInformacionBasica();
        }
        
        if (doctor.getId() != -1) {
            cout << "\n--- INFORMACION DEL DOCTOR ---" << endl;
            doctor.mostrarInformacionBasica();
        }
    } else {
        cout << "Consulta no encontrada." << endl;
    }
}

void listarTodasConsultas() {
    cout << "\n=== LISTA DE TODAS LAS CONSULTAS ===" << endl;
    vector<HistorialMedico> consultas = GestorArchivos::listarHistorialesActivos();
    
    if (consultas.empty()) {
        cout << "No hay consultas registradas." << endl;
        return;
    }
    
    for (const auto& consulta : consultas) {
        consulta.mostrarInformacionBasica();
    }
    cout << "Total: " << consultas.size() << " consultas" << endl;
}

void eliminarConsulta() {
    cout << "\n=== ELIMINAR CONSULTA ===" << endl;
    cout << "ID de la consulta a eliminar: ";
    int id;
    cin >> id;
    limpiarBufferHistorial();
    
    // Nota: La eliminación de consultas en una lista enlazada es compleja
    // Por simplicidad, solo marcamos como eliminada
    if (GestorArchivos::eliminarHistorialLogico(id)) {
        cout << "Consulta eliminada exitosamente." << endl;
        cout << "NOTA: Para reparar la lista enlazada, use la opcion 'Reparar lista de historial'." << endl;
    } else {
        cout << "Error al eliminar la consulta o consulta no encontrada." << endl;
    }
}

void repararListaHistorial() {
    cout << "\n=== REPARAR LISTA DE HISTORIAL ===" << endl;
    cout << "ID del paciente a reparar: ";
    int pacienteID;
    cin >> pacienteID;
    limpiarBufferHistorial();
    
    Paciente paciente = GestorArchivos::buscarPacientePorID(pacienteID);
    if (paciente.getId() == -1) {
        cout << "Paciente no encontrado." << endl;
        return;
    }
    
    cout << "Reparando lista enlazada del paciente " << paciente.getNombre() << " " << paciente.getApellido() << "..." << endl;
    
    // Colectar todas las consultas del paciente
    vector<HistorialMedico> todasConsultas = GestorArchivos::listarHistorialesActivos();
    vector<HistorialMedico> consultasDelPaciente;
    
    for (const auto& consulta : todasConsultas) {
        if (consulta.getPacienteID() == pacienteID) {
            consultasDelPaciente.push_back(consulta);
        }
    }
    
    if (consultasDelPaciente.empty()) {
        cout << "No se encontraron consultas para el paciente." << endl;
        paciente.setPrimerConsultaID(-1);
        paciente.setCantidadConsultas(0);
        GestorArchivos::guardarPaciente(paciente);
        return;
    }
    
    // Ordenar por fecha y hora (más reciente primero)
    sort(consultasDelPaciente.begin(), consultasDelPaciente.end(), 
         [](const HistorialMedico& a, const HistorialMedico& b) {
             // Comparar por fecha y hora
             int cmpFecha = strcmp(a.getFecha(), b.getFecha());
             if (cmpFecha != 0) return cmpFecha > 0; // Fecha más reciente primero
             return strcmp(a.getHora(), b.getHora()) > 0; // Hora más reciente primero
         });
    
    // Reconstruir lista enlazada
    paciente.setPrimerConsultaID(consultasDelPaciente[0].getId());
    paciente.setCantidadConsultas(consultasDelPaciente.size());
    
    for (size_t i = 0; i < consultasDelPaciente.size(); i++) {
        HistorialMedico& consulta = consultasDelPaciente[i];
        if (i == consultasDelPaciente.size() - 1) {
            consulta.setSiguienteConsultaID(-1); // Última consulta
        } else {
            consulta.setSiguienteConsultaID(consultasDelPaciente[i + 1].getId());
        }
        GestorArchivos::guardarHistorial(consulta);
    }
    
    if (GestorArchivos::guardarPaciente(paciente)) {
        cout << "Lista enlazada reparada correctamente. Total consultas: " 
             << consultasDelPaciente.size() << endl;
    } else {
        cout << "Error al guardar los cambios del paciente." << endl;
    }
}