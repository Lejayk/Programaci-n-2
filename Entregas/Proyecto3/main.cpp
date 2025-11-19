#include <iostream>
#include <cstring>
#include "hospital/Hospital.hpp"
#include "persistencia/GestorArchivos.hpp"
#include "pacientes/operacionesPacientes.hpp"
#include "doctores/operacionesDoctores.hpp"
#include "citas/operacionesCitas.hpp"
#include "historial/operacionesHistorial.hpp"

using namespace std;

void limpiarBuffer() {
    cin.ignore(256, '\n');
}

void leerLinea(char* buffer, int size) {
    cin.getline(buffer, size);
    if (cin.fail()) {
        cin.clear();
        limpiarBuffer();
    }
    buffer[size-1] = '\0';
}

int leerEntero() {
    int x;
    if (!(cin >> x)) {
        cin.clear();
        limpiarBuffer();
        return -1;
    }
    return x;
}

void mostrarMenuPrincipal() {
    cout << "\n========================================" << endl;
    cout << "=   SISTEMA DE GESTION HOSPITALARIA v3   =" << endl;
    cout << "=      (POO y Modularizacion)           =" << endl;
    cout << "========================================" << endl;
    cout << "\n1. Gestion de Pacientes" << endl;
    cout << "2. Gestion de Doctores" << endl;
    cout << "3. Gestion de Citas" << endl;
    cout << "4. Historial Medico" << endl;
    cout << "5. Informacion del Hospital" << endl;
    cout << "6. Guardar y Salir" << endl;
    cout << "\nOpcion: ";
}

int main() {
    // Inicializar sistema de archivos
    if (!GestorArchivos::inicializarSistemaArchivos()) {
        cerr << "Error al inicializar archivos" << endl;
        return 1;
    }
    
    // Cargar o crear Hospital
    Hospital hospital;
    if (!GestorArchivos::cargarHospital(hospital)) {
        hospital = Hospital("Hospital General", "Calle Principal #123", "555-1234");
        GestorArchivos::guardarHospital(hospital);
    }
    
    cout << "=== SISTEMA DE GESTION HOSPITALARIO v3 ===" << endl;
    cout << "===    (POO y Modularizacion)          ===" << endl;
    
    int opcion;
    do {
        mostrarMenuPrincipal();
        opcion = leerEntero();
        limpiarBuffer();
        
        switch(opcion) {
            case 1:
                menuPacientes(hospital);
                break;
            case 2:
                menuDoctores(hospital);
                break;
            case 3:
                menuCitas(hospital);
                break;
            case 4:
                menuHistorial(hospital);
                break;
            case 5:
                hospital.mostrarInformacion();
                break;
            case 6:
                GestorArchivos::guardarHospital(hospital);
                cout << "\n✓ Datos guardados. Hasta pronto!" << endl;
                break;
            default:
                cout << "Opcion invalida" << endl;
        }
    } while(opcion != 6);
    
    return 0;
}