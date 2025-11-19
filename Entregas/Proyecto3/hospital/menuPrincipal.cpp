#include "menuPrincipal.hpp"

#include "../pacientes/operacionesPacientes.hpp"
#include "../doctores/operacionesDoctores.hpp"
#include "../citas/operacionesCitas.hpp"
#include "../historial/operacionesHistorial.hpp"

#include <iostream>

using namespace std;

void menuPrincipal(Hospital& hospital) {
    int op;

    do {
        cout << "\n=========== SISTEMA DE GESTION HOSPITALARIO ==========\n";
        cout << "Pacientes registrados: " << hospital.getTotalPacientes() << "\n";
        cout << "Doctores registrados:  " << hospital.getTotalDoctores()  << "\n";
        cout << "Citas registradas:     " << hospital.getTotalCitas()    << "\n";
        cout << "Historiales:           " << hospital.getTotalHistorial()<< "\n";
        cout << "-------------------------------------------------------\n";

        cout << "1. Pacientes\n";
        cout << "2. Doctores\n";
        cout << "3. Citas\n";
        cout << "4. Historial medico\n";
        cout << "0. Salir\n";
        cout << "Opcion: ";
        cin >> op;

        switch (op) {
        case 1: menuPacientes(hospital); break;
        case 2: menuDoctores(hospital); break;
        case 3: menuCitas(hospital); break;
        case 4: menuHistorial(hospital); break;
        }

    } while (op != 0);

    cout << "Saliendo del sistema...\n";
}
