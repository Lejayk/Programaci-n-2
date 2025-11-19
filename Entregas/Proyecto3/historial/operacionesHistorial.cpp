#include "operacionesHistorial.hpp"
#include "../persistencia/GestorArchivos.hpp"
#include "../pacientes/Paciente.hpp"
#include "../doctores/Doctor.hpp"
#include <iostream>

using namespace std;

void menuHistorial(Hospital& hospital) {
    int op;

    do {
        cout << "\n=== MENU HISTORIAL MEDICO ===\n";
        cout << "1. Crear registro de historial\n";
        cout << "2. Buscar historial por ID\n";
        cout << "3. Listar historial\n";
        cout << "4. Eliminar historial\n";
        cout << "0. Volver\n";
        cout << "Opcion: ";
        cin >> op;

        switch (op) {
        case 1: crearRegistroHistorial(hospital); break;
        case 2: buscarHistorialPorID(); break;
        case 3: listarHistorial(); break;
        case 4: eliminarHistorial(); break;
        }

    } while (op != 0);
}

void crearRegistroHistorial(Hospital& hospital) {
    int idPaciente, idDoctor;

    cout << "ID Paciente: ";
    cin >> idPaciente;
    Paciente p;
    if (!GestorArchivos::leerPacientePorID(idPaciente, p)) {
        cout << "Paciente no encontrado.\n";
        return;
    }

    cout << "ID Doctor: ";
    cin >> idDoctor;
    Doctor d;
    if (!GestorArchivos::leerDoctorPorID(idDoctor, d)) {
        cout << "Doctor no encontrado.\n";
        return;
    }

    cin.ignore();

    HistorialMedico h(idPaciente, idDoctor);

    char buffer[300];

    cout << "Descripcion del caso: ";
    cin.getline(buffer, 300);
    h.setDescripcion(buffer);

    cout << "Diagnostico: ";
    cin.getline(buffer, 200);
    h.setDiagnostico(buffer);

    cout << "Tratamiento: ";
    cin.getline(buffer, 200);
    h.setTratamiento(buffer);

    int dia, mes, anio;
    cout << "Fecha (dia mes año): ";
    cin >> dia >> mes >> anio;
    h.setFecha(dia, mes, anio);

    if (!h.validarDatos()) {
        cout << "Datos invalidos.\n";
        return;
    }

    int nuevoID = hospital.generarIDHistorial();
    h.setId(nuevoID);
    hospital.incrementarHistorial();

    if (GestorArchivos::guardarHistorial(h))
        cout << "Historial registrado.\n";
    else
        cout << "Error al guardar.\n";
}

void buscarHistorialPorID() {
    int id;
    cout << "ID: ";
    cin >> id;

    HistorialMedico h;
    if (GestorArchivos::leerHistorialPorID(id, h))
        h.mostrarInformacionCompleta();
    else
        cout << "No encontrado.\n";
}

void listarHistorial() {
    GestorArchivos::listarHistorial();
}

void eliminarHistorial() {
    int id;
    cout << "ID: ";
    cin >> id;

    if (GestorArchivos::eliminarHistorialLogico(id))
        cout << "Eliminado.\n";
    else
        cout << "No encontrado.\n";
}
