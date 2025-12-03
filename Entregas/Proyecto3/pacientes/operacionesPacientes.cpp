#include "operacionesPacientes.hpp"
#include "../persistencia/GestorArchivos.hpp"
#include "../utilidades/Formatos.hpp"
#include "../utilidades/Validaciones.hpp"
#include <iostream>
#include <cstring>
#include <cctype>
#include <iomanip>

using namespace std;

namespace {
    constexpr int MAX_RESULTADOS_BUSQUEDA = 100;
}

// (Funciones de Paciente implementadas en Paciente.cpp)
void menuPacientes(Hospital& hospital) {
    int opcion;
    do {
        system("pause");
        system("cls");
        cout << "\n=== GESTION DE PACIENTES ===" << endl;
        cout << "1. Registrar nuevo paciente" << endl;
        cout << "2. Buscar por ID" << endl;
        cout << "3. Buscar por cedula" << endl;
        cout << "4. Buscar por nombre" << endl;
        cout << "5. Modificar datos" << endl;
        cout << "6. Eliminar" << endl;
        cout << "7. Listar todos" << endl;
        cout << "8. Ver historial medico" << endl;
        cout << "0. Volver" << endl;
        cout << "\nOpcion: ";
        opcion = Formatos::leerEntero();
        
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
                buscarPacientesPorNombre();
                break;
            case 5:
                modificarPaciente();
                break;
            case 6:
                eliminarPaciente();
                break;
            case 7:
                listarTodosPacientes();
                break;
            case 8:
                verHistorialPaciente();
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

void buscarPacientesPorNombre() {
    cout << "\nResultados de busqueda:\n";
    cout << string(100, '_') << endl;
    cout << "Nombre o parte del nombre: ";
    char buffer[100];
    Formatos::leerLinea(buffer, 100);
    string busc = buffer;
    if (busc.empty()) {
        cout << "Operacion cancelada." << endl;
        return;
    }
    // tolower search
    for (auto &c : busc) c = tolower((unsigned char)c);

    vector<Paciente> pacientes = GestorArchivos::listarPacientesActivos();
    int contador = 0;
    for (const auto &p : pacientes) {
        if (p.getEliminado()) continue;
        if (contador >= MAX_RESULTADOS_BUSQUEDA) break;
        string nombre = string(p.getNombre());
        string apellido = string(p.getApellido());
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
            if (contador >= MAX_RESULTADOS_BUSQUEDA) break;
            string nombreCompleto = p.getNombre();
            nombreCompleto += " ";
            nombreCompleto += p.getApellido();

            cout << left << setw(4) << p.getId()
                 << setw(26) << nombreCompleto
                 << setw(16) << p.getCedula()
                 << setw(6) << p.getEdad()
                 << setw(6) << p.getTipoSangre()
                 << setw(16) << p.getTelefono()
                 << setw(30) << p.getEmail() << "\n";
            cout << string(100, '_') << endl;
            contador++;
        }
    }

    if (contador == 0) {
        cout << "No se encontraron pacientes con ese nombre." << endl;
    } else {
        cout << "Total encontrados: " << contador << endl;
    }
    system("pause");
    system("cls");
}

// verHistorialPaciente() is defined in the historial module (operacionesHistorial.cpp)

void registrarPaciente(Hospital& hospital) {
    cout << "\n=== REGISTRAR NUEVO PACIENTE ===" << endl;
    
    char nombre[50], apellido[50], cedula[20];
    int edad;
    char sexo;
    char tipoSangre[5], telefono[20], email[50], direccion[80], alergias[100], observaciones[200];
    char bufferSexo[5];
    
    cout << "Nombre: ";
    Formatos::leerLinea(nombre, 50);
    cout << "Apellido: ";
    Formatos::leerLinea(apellido, 50);
    cout << "Cedula (V-12345678): ";
    Formatos::leerLinea(cedula, 20);
    cout << "Edad: ";
    edad = Formatos::leerEntero();
    cout << "Sexo (M/F/O): ";
    Formatos::leerLinea(bufferSexo, 5);
    sexo = bufferSexo[0];
    cout << "Tipo de sangre (A+, A-, etc): ";
    Formatos::leerLinea(tipoSangre, 5);
    cout << "Telefono: ";
    Formatos::leerLinea(telefono, 20);
    cout << "Email: ";
    Formatos::leerLinea(email, 50);
    cout << "Direccion: ";
    Formatos::leerLinea(direccion, 80);
    cout << "Alergias: ";
    Formatos::leerLinea(alergias, 100);
    cout << "Observaciones: ";
    Formatos::leerLinea(observaciones, 200);
    
    if (!Validaciones::validarCedula(cedula)) {
        cout << "Error: Cedula invalida. Formato esperado V-12345678" << endl;
        return;
    }
    if (!Validaciones::validarRango(edad, 0, 120)) {
        cout << "Error: Edad debe estar entre 0 y 120 anios." << endl;
        return;
    }
    sexo = toupper(sexo);
    if (!(sexo == 'M' || sexo == 'F')) {
        cout << "Error: Sexo debe ser M o F." << endl;
        return;
    }

    Paciente existente = GestorArchivos::buscarPacientePorCedula(cedula);
    if (existente.getId() != -1) {
        cout << "Error: La cedula ya esta registrada." << endl;
        return;
    }

    if (email[0] != '\0' && !Validaciones::validarEmail(email)) {
        cout << "Error: Email invalido. Formato usuario@dominio.ext" << endl;
        return;
    }

    Paciente paciente(nombre, apellido, cedula);
    paciente.setEdad(edad);
    paciente.setSexo(sexo);
    if (tipoSangre[0] != '\0') paciente.setTipoSangre(tipoSangre);
    if (telefono[0] != '\0') paciente.setTelefono(telefono);
    if (email[0] != '\0') paciente.setEmail(email);
    if (direccion[0] != '\0') paciente.setDireccion(direccion);
    if (alergias[0] != '\0') paciente.setAlergias(alergias);
    if (observaciones[0] != '\0') paciente.setObservaciones(observaciones);
    
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
    int id = Formatos::leerEntero();
    
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
    Formatos::leerLinea(cedula, 20);
    
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
    int id = Formatos::leerEntero();
    
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
    Formatos::leerLinea(buffer, 50);
    if (strlen(buffer) > 0) paciente.setNombre(buffer);
    
    cout << "Nuevo apellido (" << paciente.getApellido() << "): ";
    Formatos::leerLinea(buffer, 50);
    if (strlen(buffer) > 0) paciente.setApellido(buffer);
    
    char bufferTS[5];
    cout << "Tipo de sangre registrado: " << paciente.getTipoSangre() << "\nTipo de sangre a registrar: ";
    Formatos::leerLinea(bufferTS, 5);
    if (strlen(bufferTS) > 0) paciente.setTipoSangre(bufferTS);

    char bufferSexo[5];
    cout << "Sexo registrado: " << paciente.getSexo() << "\nSexo a registrar (M/F): ";
    Formatos::leerLinea(bufferSexo, 5);
    if (strlen(bufferSexo) > 0) {
        char nuevoSexo = toupper(bufferSexo[0]);
        if (nuevoSexo == 'M' || nuevoSexo == 'F') {
            paciente.setSexo(nuevoSexo);
        } else {
            cout << "Sexo invalido. Se mantiene el anterior." << endl;
        }
    }

    cout << "Telefono actual: " << paciente.getTelefono() << "\nNuevo telefono: ";
    Formatos::leerLinea(buffer, 20);
    if (strlen(buffer) > 0) paciente.setTelefono(buffer);

    cout << "Direccion actual: " << paciente.getDireccion() << "\nNueva direccion: ";
    Formatos::leerLinea(buffer, 80);
    if (strlen(buffer) > 0) paciente.setDireccion(buffer);

    cout << "Email actual: " << paciente.getEmail() << "\nNuevo email: ";
    Formatos::leerLinea(buffer, 50);
    if (strlen(buffer) > 0) {
        if (Validaciones::validarEmail(buffer)) {
            paciente.setEmail(buffer);
            cout << "Email actualizado correctamente." << endl;
        } else {
            cout << "Error: Email invalido. El valor no fue actualizado." << endl;
        }
    }

    cout << "Alergias actuales: " << paciente.getAlergias() << "\nNuevas alergias: ";
    Formatos::leerLinea(buffer, 100);
    if (strlen(buffer) > 0) paciente.setAlergias(buffer);

    cout << "Observaciones actuales: " << paciente.getObservaciones() << "\nNuevas observaciones: ";
    Formatos::leerLinea(buffer, 200);
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
    int id = Formatos::leerEntero();
    
    if (GestorArchivos::eliminarPacienteLogico(id)) {
        cout << "Paciente eliminado exitosamente." << endl;
    } else {
        cout << "Error al eliminar el paciente o paciente no encontrado." << endl;
    }
}

void listarTodosPacientes() {
    vector<Paciente> pacientes = GestorArchivos::listarPacientesActivos();
    if (pacientes.empty()) {
        cout << "No hay pacientes registrados." << endl;
        return;
    }

    cout << "\n" << left << setw(4) << "ID" << setw(26) << "NOMBRE" << setw(16) << "CEDULA"
         << setw(6) << "EDAD" << setw(6) << "TS" << setw(16) << "TELEFONO" << setw(30) << "EMAIL" << "\n";
    cout << string(100, '_') << endl;

    int contador = 0;
    for (const auto& p : pacientes) {
        if (p.getEliminado()) continue;
        if (contador >= MAX_RESULTADOS_BUSQUEDA) break;

        string nombreCompleto = p.getNombre();
        nombreCompleto += " ";
        nombreCompleto += p.getApellido();

        cout << left << setw(4) << p.getId()
             << setw(26) << nombreCompleto
             << setw(16) << p.getCedula()
             << setw(6) << p.getEdad()
             << setw(6) << p.getTipoSangre()
             << setw(16) << p.getTelefono()
             << setw(30) << p.getEmail() << "\n";
        contador++;
    }

    cout << "\nTotal de pacientes activos: " << contador << endl;
}