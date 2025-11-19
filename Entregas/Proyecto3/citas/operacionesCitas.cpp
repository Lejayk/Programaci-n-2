#include "operacionesCitas.hpp"
#include "../persistencia/GestorArchivos.hpp"
#include "../pacientes/Paciente.hpp"
#include "../doctores/Doctor.hpp"
#include <iostream>

using namespace std;

void menuCitas(Hospital& hospital) {
    int op;

    do {
        cout << "\n=== MENU CITAS ===\n";
        cout << "1. Crear cita\n";
        cout << "2. Buscar cita por ID\n";
        cout << "3. Listar citas\n";
        cout << "4. Cancelar cita\n";
        cout << "5. Marcar cita como atendida\n";
        cout << "0. Volver\n";
        cout << "Opcion: ";
        cin >> op;

        switch (op) {
            case 1: crearCita(hospital); break;
            case 2: buscarCitaPorID(); break;
            case 3: listarCitas(); break;
            case 4: cancelarCita(); break;
            case 5: marcarCitaAtendida(); break;
        }

    } while (op != 0);
}

void crearCita(Hospital& hospital) {
    int idPaciente, idDoctor;

    cout << "ID Paciente: ";
    cin >> idPaciente;

    Paciente p;
    if (!GestorArchivos::leerPacientePorID(idPaciente, p)) {
        cout << "Paciente no encontrado.\n"; return;
    }

    cout << "ID Doctor: ";
    cin >> idDoctor;

    Doctor d;
    if (!GestorArchivos::leerDoctorPorID(idDoctor, d)) {
        cout << "Doctor no encontrado.\n"; return;
    }

    cin.ignore();
    char motivo[100];
    cout << "Motivo: ";
    cin.getline(motivo, 100);

    Cita c(idPaciente, idDoctor, motivo);

    int dia, mes, anio, hora, min;

    cout << "Dia: "; cin >> dia;
    cout << "Mes: "; cin >> mes;
    cout << "Anio: "; cin >> anio;
    cout << "Hora: "; cin >> hora;
    cout << "Minuto: "; cin >> min;

    c.setFecha(dia, mes, anio, hora, min);

    if (!c.validarDatos()) {
        cout << "Datos invalidos.\n";
        return;
    }

    int nuevoID = hospital.generarIDCita();
    c.setId(nuevoID);
    hospital.incrementarCitas();

    if (GestorArchivos::guardarCita(c))
        cout << "Cita creada.\n";
    else
        cout << "Error al guardar.\n";

    p.agregarCitaID(nuevoID);
    d.agregarCitaID(nuevoID);

    GestorArchivos::actualizarPaciente(p);
    GestorArchivos::actualizarDoctor(d);
}

void listarCitas() {
    GestorArchivos::listarCitas();
}

void buscarCitaPorID() {
    int id;
    cout << "ID: ";
    cin >> id;

    Cita c;
    if (GestorArchivos::leerCitaPorID(id, c))
        c.mostrarInformacionCompleta();
    else
        cout << "No encontrada.\n";
}

void cancelarCita() {
    int id;
    cout << "ID: ";
    cin >> id;

    if (GestorArchivos::eliminarCitaLogico(id))
        cout << "Cita cancelada.\n";
    else
        cout << "No encontrada.\n";
}

void marcarCitaAtendida() {
    int id;
    cout << "ID: ";
    cin >> id;

    Cita c;
    if (!GestorArchivos::leerCitaPorID(id, c)) {
        cout << "Cita no encontrada.\n";
        return;
    }

    c.setAtendida(true);
    GestorArchivos::actualizarCita(c);

    cout << "Marcada como atendida.\n";
}
