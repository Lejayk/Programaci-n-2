#include "operacionesMantenimiento.hpp"
#include <iostream>
#include <string>
#include <vector>
// Detect filesystem support
#if __has_include(<filesystem>)
#define FS_AVAILABLE 1
#include <filesystem>
namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#define FS_AVAILABLE 1
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#define FS_AVAILABLE 0
#endif
#include <chrono>
#include <iomanip>
#include <cstdio>
#include <sys/stat.h>

#include "../persistencia/GestorArchivos.hpp"
#include "../pacientes/Paciente.hpp"
#include "../doctores/Doctor.hpp"
#include "../citas/Cita.hpp"
#include "../historial/HistorialMedico.hpp"

using namespace std;

static string timestamp() {
    auto now = chrono::system_clock::now();
    time_t tt = chrono::system_clock::to_time_t(now);
    tm local_tm;
#if defined(_MSC_VER)
    localtime_s(&local_tm, &tt);
#else
    tm *tmp = localtime(&tt);
    if (tmp) local_tm = *tmp; else memset(&local_tm, 0, sizeof(local_tm));
#endif
    char buf[40];
    strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &local_tm);
    return string(buf);
}

void pauseAndClear() {
    system("pause");
    system("cls");
}

void menuMantenimiento(Hospital& hospital) {
    int opcion;
    do {
        system("cls");
        cout << "\n=== MANTENIMIENTO DEL SISTEMA ===" << endl;
        cout << "1. Compactar todos los archivos" << endl;
        cout << "2. Crear respaldo" << endl;
        cout << "3. Restaurar respaldo" << endl;
        cout << "4. Verificar integridad referencial" << endl;
        cout << "5. Reparar integridad referencial" << endl;
        cout << "6. Mostrar estadisticas detalladas" << endl;
        cout << "7. Verificar sistema de archivos" << endl;
        cout << "0. Volver" << endl;
        cout << "Opcion: ";
        cin >> opcion;
        cin.ignore(256, '\n');

        switch(opcion) {
            case 1: compactarTodosArchivos(); pauseAndClear(); break;
            case 2: crearRespaldo(); pauseAndClear(); break;
            case 3: restaurarRespaldo(); pauseAndClear(); break;
            case 4: verificarIntegridadReferencial(); pauseAndClear(); break;
            case 5: repararIntegridadReferencial(); pauseAndClear(); break;
            case 6: mostrarEstadisticasDetalladas(); pauseAndClear(); break;
            case 7: verificarSistemaArchivos(); pauseAndClear(); break;
            case 0: break;
            default: cout << "Opcion invalida" << endl; pauseAndClear();
        }
    } while(opcion != 0);
}

void compactarTodosArchivos() {
    cout << "Compactando archivos..." << endl;
    // Llamar a GestorArchivos para compactar cada archivo conocido
    GestorArchivos::compactarArchivo("datos/pacientes.bin", Paciente::obtenerTamano());
    GestorArchivos::compactarArchivo("datos/doctores.bin", Doctor::obtenerTamano());
    GestorArchivos::compactarArchivo("datos/citas.bin", Cita::obtenerTamano());
    GestorArchivos::compactarArchivo("datos/historiales.bin", HistorialMedico::obtenerTamano());
    cout << "Compactacion finalizada." << endl;
}

void crearRespaldo() {
#if FS_AVAILABLE
    cout << "Creando respaldo de la carpeta 'datos'..." << endl;
    try {
        if (!fs::exists("datos")) {
            cout << "No existe la carpeta 'datos' para respaldar." << endl;
            return;
        }
        string dest = string("backup_") + timestamp();
        fs::create_directory(dest);
        for (auto &p : fs::directory_iterator("datos")) {
            fs::copy(p.path(), fs::path(dest) / p.path().filename(), fs::copy_options::overwrite_existing);
        }
        cout << "Respaldo creado en: " << dest << endl;
    } catch (const fs::filesystem_error& e) {
        cout << "Error al crear respaldo: " << e.what() << endl;
    }
#else
    cout << "Crear respaldo: <filesystem> no disponible en este compilador. Operacion no soportada." << endl;
#endif
}

void restaurarRespaldo() {
#if FS_AVAILABLE
    cout << "Restaurar respaldo: ingrese el nombre del directorio de respaldo: ";
    string dir;
    getline(cin, dir);
    if (dir.empty()) {
        cout << "Operacion cancelada." << endl;
        return;
    }
    try {
        if (!fs::exists(dir) || !fs::is_directory(dir)) {
            cout << "Directorio de respaldo no encontrado." << endl;
            return;
        }
        for (auto &p : fs::directory_iterator(dir)) {
            fs::copy(p.path(), fs::path("datos") / p.path().filename(), fs::copy_options::overwrite_existing);
        }
        cout << "Restauracion completada desde: " << dir << endl;
    } catch (const fs::filesystem_error& e) {
        cout << "Error al restaurar respaldo: " << e.what() << endl;
    }
#else
    cout << "Restaurar respaldo: <filesystem> no disponible en este compilador. Operacion no soportada." << endl;
#endif
}

void verificarIntegridadReferencial() {
    cout << "Verificando integridad referencial..." << endl;
    // Revisar citas
    auto citas = GestorArchivos::listarCitasActivas();
    for (const auto &c : citas) {
        if (GestorArchivos::buscarPacientePorID(c.getPacienteID()).getId() == -1) {
            cout << "Cita " << c.getId() << " referencia paciente inexistente: " << c.getPacienteID() << endl;
        }
        if (GestorArchivos::buscarDoctorPorID(c.getDoctorID()).getId() == -1) {
            cout << "Cita " << c.getId() << " referencia doctor inexistente: " << c.getDoctorID() << endl;
        }
    }

    // Revisar historiales
    auto historiales = GestorArchivos::listarHistorialesActivos();
    for (const auto &h : historiales) {
        if (GestorArchivos::buscarPacientePorID(h.getPacienteID()).getId() == -1) {
            cout << "Historial " << h.getId() << " referencia paciente inexistente: " << h.getPacienteID() << endl;
        }
        if (GestorArchivos::buscarDoctorPorID(h.getDoctorID()).getId() == -1) {
            cout << "Historial " << h.getId() << " referencia doctor inexistente: " << h.getDoctorID() << endl;
        }
    }

    cout << "Verificacion completada." << endl;
}

void repararIntegridadReferencial() {
    cout << "Reparando integridad referencial (se marcaran registros huérfanos como eliminados)..." << endl;
    auto citas = GestorArchivos::listarCitasActivas();
    for (auto c : citas) {
        bool mod = false;
        if (GestorArchivos::buscarPacientePorID(c.getPacienteID()).getId() == -1) {
            c.setEliminado(true);
            mod = true;
            cout << "Marcando cita " << c.getId() << " como eliminada (paciente faltante)." << endl;
        }
        if (GestorArchivos::buscarDoctorPorID(c.getDoctorID()).getId() == -1) {
            c.setEliminado(true);
            mod = true;
            cout << "Marcando cita " << c.getId() << " como eliminada (doctor faltante)." << endl;
        }
        if (mod) GestorArchivos::guardarCita(c);
    }

    auto historiales = GestorArchivos::listarHistorialesActivos();
    for (auto h : historiales) {
        bool mod = false;
        if (GestorArchivos::buscarPacientePorID(h.getPacienteID()).getId() == -1) {
            h.setEliminado(true);
            mod = true;
            cout << "Marcando historial " << h.getId() << " como eliminado (paciente faltante)." << endl;
        }
        if (GestorArchivos::buscarDoctorPorID(h.getDoctorID()).getId() == -1) {
            h.setEliminado(true);
            mod = true;
            cout << "Marcando historial " << h.getId() << " como eliminado (doctor faltante)." << endl;
        }
        if (mod) GestorArchivos::guardarHistorial(h);
    }

    cout << "Reparacion completada." << endl;
}

void mostrarEstadisticasDetalladas() {
    cout << "Mostrando estadisticas (headers de archivos)..." << endl;
    ArchivoHeader ph = GestorArchivos::leerHeader("datos/pacientes.bin");
    ArchivoHeader dh = GestorArchivos::leerHeader("datos/doctores.bin");
    ArchivoHeader ch = GestorArchivos::leerHeader("datos/citas.bin");
    ArchivoHeader hh = GestorArchivos::leerHeader("datos/historiales.bin");
    auto fileSizeFallback = [](const string& path) -> long long {
        struct stat st;
        if (stat(path.c_str(), &st) == 0) return (long long)st.st_size;
        return -1LL;
    };

    auto printStats = [&](const char* path, const ArchivoHeader& h, const char* label){
        long long size = -1;
#if FS_AVAILABLE
        try { size = (long long)fs::file_size(path); } catch (...) { size = fileSizeFallback(path); }
#else
        size = fileSizeFallback(path);
#endif
        cout << label << " - registros: " << h.cantidadRegistros << " activos: " << h.registrosActivos << " proximoID: " << h.proximoID;
        if (size >= 0) cout << " | tamano(bytes): " << size;
        cout << endl;
    };

    printStats("datos/pacientes.bin", ph, "Pacientes");
    printStats("datos/doctores.bin", dh, "Doctores ");
    printStats("datos/citas.bin", ch, "Citas    ");
    printStats("datos/historiales.bin", hh, "Historiales");
}

void verificarSistemaArchivos() {
#if FS_AVAILABLE
    cout << "Verificando sistema de archivos (carpeta 'datos' y archivos esperados)..." << endl;
    if (!fs::exists("datos")) {
        cout << "Carpeta 'datos' no existe." << endl;
        return;
    }
    vector<string> esperados = {"datos/pacientes.bin","datos/doctores.bin","datos/citas.bin","datos/historiales.bin","datos/hospital.bin"};
    for (auto &f : esperados) {
        if (!fs::exists(f)) cout << "Falta: " << f << endl;
        else cout << "OK: " << f << endl;
    }
#else
    cout << "Verificar sistema de archivos: <filesystem> no disponible en este compilador. Operacion no soportada." << endl;
#endif
}
