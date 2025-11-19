#include "operacionesDoctores.hpp"
#include "../persistencia/GestorArchivos.hpp"
#include <iostream>

using namespace std;

void menuDoctores(Hospital& hospital) {
    int opcion;

    do {
        cout << "\n=== MENU DOCTORES ===\n";
        cout << "1. Registrar doctor\n";
        cout << "2. Editar doctor\n";
        cout << "3. Buscar doctor por ID\n";
        cout << "4. Listar doctores\n";
        cout << "5. Eliminar doctor\n";
        cout << "0. Volver\n";
        cout << "Opcion: ";
        cin >> opcion;

        switch (opcion) {
        case 1: registrarDoctor(hospital); break;
        case 2: editarDoctor(hospital); break;
        case 3: buscarDoctorPorID(); break;
        case 4: listarDoctores(); break;
        case 5: eliminarDoctor(); break;
        }

    } while (opcion != 0);
}

void registrarDoctor(Hospital& hospital) {
    Doctor d;
    char buffer[100];

    cin.ignore();

    cout << "Nombre: ";
    cin.getline(buffer, 50);
    d.setNombre(buffer);

    cout << "Apellido: ";
    cin.getline(buffer, 50);
    d.setApellido(buffer);

    cout << "Cedula: ";
    cin.getline(buffer, 20);
    d.setCedula(buffer);

    cout << "Especialidad: ";
    cin.getline(buffer, 50);
    d.setEspecialidad(buffer);

    cout << "Costo consulta: ";
    float costo;
    cin >> costo;
    d.setCostoConsulta(costo);

    if (!d.validarDatos()) {
        cout << "Datos invalidos.\n";
        return;
    }

    d.setId(hospital.generarIDDoctor());
    hospital.incrementarDoctores();

    if (GestorArchivos::guardarDoctor(d))
        cout << "Doctor registrado.\n";
    else
        cout << "Error al registrar.\n";
}

void editarDoctor(Hospital&) {
    cout << "(Implementacion completada cuando conectemos todo)\n";
}

void buscarDoctorPorID() {
    int id;
    cout << "ID: ";
    cin >> id;

    Doctor d;
    if (GestorArchivos::leerDoctorPorID(id, d))
        d.mostrarInformacionCompleta();
    else
        cout << "No encontrado.\n";
}

void listarDoctores() {
    GestorArchivos::listarDoctores();
}

void eliminarDoctor() {
    int id;
    cout << "ID: ";
    cin >> id;

    if (GestorArchivos::eliminarDoctorLogico(id))
        cout << "Eliminado.\n";
    else
        cout << "No encontrado.\n";
}
