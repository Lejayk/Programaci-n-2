#include "../persistencia/GestorArchivos.hpp"
#include "../pacientes/Paciente.hpp"
#include <iostream>
#include <cstring>

int main(int argc, char** argv) {
    if (!GestorArchivos::inicializarSistemaArchivos()) {
        std::cerr << "No se pudo inicializar sistema de archivos\n";
        return 1;
    }

    if (argc < 2) {
        std::cout << "Usage: persistence_test write|read\n";
        return 0;
    }

    std::string mode = argv[1];
    if (mode == "write") {
        Paciente p("Juan","Perez","V-12345678");
        p.setEdad(30);
        p.setSexo('M');
        p.setTelefono("0212-5555555");
        p.setEmail("juan.perez@example.com");
        if (GestorArchivos::guardarPaciente(p)) {
            std::cout << "Paciente guardado. ID asignado: " << p.getId() << "\n";
        } else {
            std::cout << "Error al guardar paciente\n";
            return 1;
        }
        ArchivoHeader h = GestorArchivos::leerHeader("datos/pacientes.bin");
        std::cout << "Header: cantidadRegistros=" << h.cantidadRegistros << " proximoID=" << h.proximoID << " registrosActivos=" << h.registrosActivos << "\n";
    } else if (mode == "read") {
        auto list = GestorArchivos::listarPacientesActivos();
        std::cout << "Pacientes activos: " << list.size() << "\n";
        for (auto &p : list) {
            p.mostrarInformacionCompleta();
        }
    } else {
        std::cout << "Modo desconocido: use write o read\n";
    }

    return 0;
}
