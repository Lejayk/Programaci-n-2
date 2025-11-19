#include "GestorArchivos.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <cerrno>
#include <cstdio>
#ifdef _WIN32
    #include <direct.h>
    #define CREAR_DIRECTORIO _mkdir
#else
    #include <sys/stat.h>
    #define CREAR_DIRECTORIO mkdir
#endif

using namespace std;

// Métodos auxiliares privados
bool GestorArchivos::crearDirectorioDatos() {
    #ifdef _WIN32
        int res = CREAR_DIRECTORIO("datos");
        if (res == 0) return true;
        return errno == EEXIST;
    #else
        if (CREAR_DIRECTORIO("datos", 0755) == 0) return true;
        return errno == EEXIST;
    #endif
}

long GestorArchivos::calcularPosicion(int indice, size_t tamRegistro) {
    return sizeof(ArchivoHeader) + (indice * tamRegistro);
}

bool GestorArchivos::inicializarArchivo(const char* nombreArchivo) {
    // Si el archivo ya existe, no lo truncan ni reinicializan
    std::ifstream existe(nombreArchivo, std::ios::binary);
    if (existe.is_open()) {
        existe.close();
        return true;
    }

    fstream archivo(nombreArchivo, ios::binary | ios::out);
    if (!archivo.is_open()) return false;

    ArchivoHeader header;
    header.cantidadRegistros = 0;
    header.proximoID = 1;
    header.registrosActivos = 0;
    header.version = VERSION_ARCHIVO;

    archivo.write((char*)&header, sizeof(ArchivoHeader));
    archivo.close();
    return true;
}

// Inicialización del sistema
bool GestorArchivos::inicializarSistemaArchivos() {
    if (!crearDirectorioDatos()) {
        cout << "Error: No se pudo crear directorio datos\n";
        return false;
    }
    
    bool success = true;
    success &= inicializarArchivo(PACIENTES_BIN);
    success &= inicializarArchivo(DOCTORES_BIN);
    success &= inicializarArchivo(CITAS_BIN);
    success &= inicializarArchivo(HISTORIALES_BIN);
    
    // Inicializar hospital si no existe
    fstream archivoHospital(HOSPITAL_BIN, ios::binary | ios::in);
    if (!archivoHospital.is_open()) {
        archivoHospital.close();
        Hospital hospital;
        guardarHospital(hospital);
    } else {
        archivoHospital.close();
    }
    
    if (success) {
        cout << "Sistema de archivos inicializado correctamente.\n";
    } else {
        cout << "Error: No se pudieron crear todos los archivos.\n";
    }
    
    return success;
}

// Operaciones de header
ArchivoHeader GestorArchivos::leerHeader(const char* nombreArchivo) {
    ArchivoHeader header;
    memset(&header, 0, sizeof(ArchivoHeader));
    
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo.is_open()) return header;
    
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    archivo.close();
    return header;
}

bool GestorArchivos::actualizarHeader(const char* nombreArchivo, const ArchivoHeader& nuevoHeader) {
    fstream archivo(nombreArchivo, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) return false;
    
    archivo.seekp(0);
    archivo.write((char*)&nuevoHeader, sizeof(ArchivoHeader));
    archivo.close();
    return true;
}

// Operaciones para Hospital
bool GestorArchivos::guardarHospital(const Hospital& hospital) {
    ofstream archivo(HOSPITAL_BIN, ios::binary | ios::out);
    if (!archivo.is_open()) return false;
    
    archivo.write((char*)&hospital, sizeof(Hospital));
    archivo.close();
    return true;
}

bool GestorArchivos::cargarHospital(Hospital& hospital) {
    ifstream archivo(HOSPITAL_BIN, ios::binary);
    if (!archivo.is_open()) {
        // Si no existe, inicializar sistema
        if (!inicializarSistemaArchivos()) {
            return false;
        }
        archivo.open(HOSPITAL_BIN, ios::binary);
        if (!archivo.is_open()) {
            return false;
        }
    }
    
    archivo.read((char*)&hospital, sizeof(Hospital));
    archivo.close();
    return true;
}

// Operaciones para Pacientes
bool GestorArchivos::guardarPaciente(Paciente& paciente) {
    fstream archivo(PACIENTES_BIN, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir archivo de pacientes\n";
        return false;
    }
    
    ArchivoHeader header = leerHeader(PACIENTES_BIN);
    bool esNuevo = true;
    int indiceEncontrado = -1;
    
    // Buscar si el paciente ya existe
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Paciente temp = leerPacientePorIndice(i);
        if (temp.getId() == paciente.getId() && paciente.getId() != 0) {
            esNuevo = false;
            indiceEncontrado = i;
            break;
        }
    }
    
    if (esNuevo) {
        // NUEVO PACIENTE: Asignar ID desde el header
        paciente.setId(header.proximoID);
        paciente.setEliminado(false);
        
        // Escribir al final del archivo
        archivo.seekp(0, ios::end);
        archivo.write((char*)&paciente, sizeof(Paciente));
        
        // Actualizar header
        header.proximoID++;
        header.cantidadRegistros++;
        header.registrosActivos++;
        
        archivo.seekp(0);
        archivo.write((char*)&header, sizeof(ArchivoHeader));
    } else {
        // ACTUALIZAR PACIENTE EXISTENTE
        long posicion = calcularPosicion(indiceEncontrado, sizeof(Paciente));
        archivo.seekp(posicion);
        archivo.write((char*)&paciente, sizeof(Paciente));
    }
    
    archivo.close();
    return true;
}

Paciente GestorArchivos::leerPacientePorIndice(int indice) {
    Paciente paciente;
    
    ifstream archivo(PACIENTES_BIN, ios::binary);
    if (!archivo.is_open()) return paciente;
    
    ArchivoHeader header = leerHeader(PACIENTES_BIN);
    if (indice < 0 || indice >= header.cantidadRegistros) {
        archivo.close();
        paciente.setId(-1);
        return paciente;
    }
    
    long posicion = calcularPosicion(indice, sizeof(Paciente));
    archivo.seekg(posicion);
    archivo.read((char*)&paciente, sizeof(Paciente));
    archivo.close();
    
    return paciente;
}

Paciente GestorArchivos::buscarPacientePorID(int id) {
    ArchivoHeader header = leerHeader(PACIENTES_BIN);
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Paciente paciente = leerPacientePorIndice(i);
        if (paciente.getId() == id && !paciente.getEliminado()) {
            return paciente;
        }
    }
    
    Paciente noEncontrado;
    noEncontrado.setId(-1);
    return noEncontrado;
}

Paciente GestorArchivos::buscarPacientePorCedula(const char* cedula) {
    ArchivoHeader header = leerHeader(PACIENTES_BIN);
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Paciente paciente = leerPacientePorIndice(i);
        if (!paciente.getEliminado() && strcmp(paciente.getCedula(), cedula) == 0) {
            return paciente;
        }
    }
    
    Paciente noEncontrado;
    noEncontrado.setId(-1);
    return noEncontrado;
}

bool GestorArchivos::eliminarPacienteLogico(int id) {
    Paciente paciente = buscarPacientePorID(id);
    if (paciente.getId() == -1) return false;
    
    paciente.setEliminado(true);
    return guardarPaciente(paciente);
}

vector<Paciente> GestorArchivos::listarPacientesActivos() {
    vector<Paciente> pacientes;
    ArchivoHeader header = leerHeader(PACIENTES_BIN);
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Paciente paciente = leerPacientePorIndice(i);
        if (!paciente.getEliminado()) {
            pacientes.push_back(paciente);
        }
    }
    
    return pacientes;
}

// Operaciones para Doctores
bool GestorArchivos::guardarDoctor(Doctor& doctor) {
    fstream archivo(DOCTORES_BIN, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir archivo de doctores\n";
        return false;
    }
    
    ArchivoHeader header = leerHeader(DOCTORES_BIN);
    bool esNuevo = true;
    int indiceEncontrado = -1;
    
    // Buscar si el doctor ya existe
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Doctor temp = leerDoctorPorIndice(i);
        if (temp.getId() == doctor.getId() && doctor.getId() != 0) {
            esNuevo = false;
            indiceEncontrado = i;
            break;
        }
    }
    
    if (esNuevo) {
        // NUEVO DOCTOR: Asignar ID desde el header
        doctor.setId(header.proximoID);
        doctor.setEliminado(false);
        
        // Escribir al final del archivo
        archivo.seekp(0, ios::end);
        archivo.write((char*)&doctor, sizeof(Doctor));
        
        // Actualizar header
        header.proximoID++;
        header.cantidadRegistros++;
        header.registrosActivos++;
        
        archivo.seekp(0);
        archivo.write((char*)&header, sizeof(ArchivoHeader));
    } else {
        // ACTUALIZAR DOCTOR EXISTENTE
        long posicion = calcularPosicion(indiceEncontrado, sizeof(Doctor));
        archivo.seekp(posicion);
        archivo.write((char*)&doctor, sizeof(Doctor));
    }
    
    archivo.close();
    return true;
}

Doctor GestorArchivos::leerDoctorPorIndice(int indice) {
    Doctor doctor;
    
    ifstream archivo(DOCTORES_BIN, ios::binary);
    if (!archivo.is_open()) return doctor;
    
    ArchivoHeader header = leerHeader(DOCTORES_BIN);
    if (indice < 0 || indice >= header.cantidadRegistros) {
        archivo.close();
        doctor.setId(-1);
        return doctor;
    }
    
    long posicion = calcularPosicion(indice, sizeof(Doctor));
    archivo.seekg(posicion);
    archivo.read((char*)&doctor, sizeof(Doctor));
    archivo.close();
    
    return doctor;
}

Doctor GestorArchivos::buscarDoctorPorID(int id) {
    ArchivoHeader header = leerHeader(DOCTORES_BIN);
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Doctor doctor = leerDoctorPorIndice(i);
        if (doctor.getId() == id && !doctor.getEliminado()) {
            return doctor;
        }
    }
    
    Doctor noEncontrado;
    noEncontrado.setId(-1);
    return noEncontrado;
}

bool GestorArchivos::eliminarDoctorLogico(int id) {
    Doctor doctor = buscarDoctorPorID(id);
    if (doctor.getId() == -1) return false;
    
    doctor.setEliminado(true);
    doctor.setDisponible(false);
    return guardarDoctor(doctor);
}

vector<Doctor> GestorArchivos::listarDoctoresActivos() {
    vector<Doctor> doctores;
    ArchivoHeader header = leerHeader(DOCTORES_BIN);
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Doctor doctor = leerDoctorPorIndice(i);
        if (!doctor.getEliminado()) {
            doctores.push_back(doctor);
        }
    }
    
    return doctores;
}

// Operaciones para Citas
bool GestorArchivos::guardarCita(Cita& cita) {
    fstream archivo(CITAS_BIN, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir archivo de citas\n";
        return false;
    }
    
    ArchivoHeader header = leerHeader(CITAS_BIN);
    bool esNueva = true;
    int indiceEncontrado = -1;
    
    // Buscar si la cita ya existe
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Cita temp = leerCitaPorIndice(i);
        if (temp.getId() == cita.getId() && cita.getId() != 0) {
            esNueva = false;
            indiceEncontrado = i;
            break;
        }
    }
    
    if (esNueva) {
        // NUEVA CITA: Asignar ID desde el header
        cita.setId(header.proximoID);
        cita.setEliminado(false);
        
        // Escribir al final del archivo
        archivo.seekp(0, ios::end);
        archivo.write((char*)&cita, sizeof(Cita));
        
        // Actualizar header
        header.proximoID++;
        header.cantidadRegistros++;
        header.registrosActivos++;
        
        archivo.seekp(0);
        archivo.write((char*)&header, sizeof(ArchivoHeader));
    } else {
        // ACTUALIZAR CITA EXISTENTE
        long posicion = calcularPosicion(indiceEncontrado, sizeof(Cita));
        archivo.seekp(posicion);
        archivo.write((char*)&cita, sizeof(Cita));
    }
    
    archivo.close();
    return true;
}

Cita GestorArchivos::leerCitaPorIndice(int indice) {
    Cita cita;
    
    ifstream archivo(CITAS_BIN, ios::binary);
    if (!archivo.is_open()) return cita;
    
    ArchivoHeader header = leerHeader(CITAS_BIN);
    if (indice < 0 || indice >= header.cantidadRegistros) {
        archivo.close();
        cita.setId(-1);
        return cita;
    }
    
    long posicion = calcularPosicion(indice, sizeof(Cita));
    archivo.seekg(posicion);
    archivo.read((char*)&cita, sizeof(Cita));
    archivo.close();
    
    return cita;
}

Cita GestorArchivos::buscarCitaPorID(int id) {
    ArchivoHeader header = leerHeader(CITAS_BIN);
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Cita cita = leerCitaPorIndice(i);
        if (cita.getId() == id && !cita.getEliminado()) {
            return cita;
        }
    }
    
    Cita noEncontrado;
    noEncontrado.setId(-1);
    return noEncontrado;
}

bool GestorArchivos::eliminarCitaLogico(int id) {
    Cita cita = buscarCitaPorID(id);
    if (cita.getId() == -1) return false;
    
    cita.setEliminado(true);
    cita.setEstado("Cancelada");
    return guardarCita(cita);
}

vector<Cita> GestorArchivos::listarCitasActivas() {
    vector<Cita> citas;
    ArchivoHeader header = leerHeader(CITAS_BIN);
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Cita cita = leerCitaPorIndice(i);
        if (!cita.getEliminado()) {
            citas.push_back(cita);
        }
    }
    
    return citas;
}

// Operaciones para Historial Médico
bool GestorArchivos::guardarHistorial(HistorialMedico& historial) {
    fstream archivo(HISTORIALES_BIN, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir archivo de historiales\n";
        return false;
    }
    
    ArchivoHeader header = leerHeader(HISTORIALES_BIN);
    bool esNuevo = true;
    int indiceEncontrado = -1;
    
    // Buscar si el historial ya existe
    for (int i = 0; i < header.cantidadRegistros; i++) {
        HistorialMedico temp = leerHistorialPorIndice(i);
        if (temp.getId() == historial.getId() && historial.getId() != 0) {
            esNuevo = false;
            indiceEncontrado = i;
            break;
        }
    }
    
    if (esNuevo) {
        // NUEVO HISTORIAL: Asignar ID desde el header
        historial.setId(header.proximoID);
        historial.setEliminado(false);
        
        // Escribir al final del archivo
        archivo.seekp(0, ios::end);
        archivo.write((char*)&historial, sizeof(HistorialMedico));
        
        // Actualizar header
        header.proximoID++;
        header.cantidadRegistros++;
        header.registrosActivos++;
        
        archivo.seekp(0);
        archivo.write((char*)&header, sizeof(ArchivoHeader));
    } else {
        // ACTUALIZAR HISTORIAL EXISTENTE
        long posicion = calcularPosicion(indiceEncontrado, sizeof(HistorialMedico));
        archivo.seekp(posicion);
        archivo.write((char*)&historial, sizeof(HistorialMedico));
    }
    
    archivo.close();
    return true;
}

HistorialMedico GestorArchivos::leerHistorialPorIndice(int indice) {
    HistorialMedico historial;
    
    ifstream archivo(HISTORIALES_BIN, ios::binary);
    if (!archivo.is_open()) return historial;
    
    ArchivoHeader header = leerHeader(HISTORIALES_BIN);
    if (indice < 0 || indice >= header.cantidadRegistros) {
        archivo.close();
        historial.setId(-1);
        return historial;
    }
    
    long posicion = calcularPosicion(indice, sizeof(HistorialMedico));
    archivo.seekg(posicion);
    archivo.read((char*)&historial, sizeof(HistorialMedico));
    archivo.close();
    
    return historial;
}

HistorialMedico GestorArchivos::buscarHistorialPorID(int id) {
    ArchivoHeader header = leerHeader(HISTORIALES_BIN);
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        HistorialMedico historial = leerHistorialPorIndice(i);
        if (historial.getId() == id && !historial.getEliminado()) {
            return historial;
        }
    }
    
    HistorialMedico noEncontrado;
    noEncontrado.setId(-1);
    return noEncontrado;
}

bool GestorArchivos::eliminarHistorialLogico(int id) {
    HistorialMedico historial = buscarHistorialPorID(id);
    if (historial.getId() == -1) return false;
    
    historial.setEliminado(true);
    return guardarHistorial(historial);
}

vector<HistorialMedico> GestorArchivos::listarHistorialesActivos() {
    vector<HistorialMedico> historiales;
    ArchivoHeader header = leerHeader(HISTORIALES_BIN);
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        HistorialMedico historial = leerHistorialPorIndice(i);
        if (!historial.getEliminado()) {
            historiales.push_back(historial);
        }
    }
    
    return historiales;
}

// Mantenimiento
bool GestorArchivos::compactarArchivo(const char* nombreArchivo, size_t tamRegistro) {
    string archivoTemp = string(nombreArchivo) + ".tmp";
    ofstream archivoTempStream(archivoTemp, ios::binary);
    if (!archivoTempStream.is_open()) {
        cout << "Error: No se pudo crear archivo temporal para compactacion\n";
        return false;
    }

    // Escribir header provisional
    ArchivoHeader headerNuevo;
    headerNuevo.cantidadRegistros = 0;
    headerNuevo.proximoID = 0;
    headerNuevo.registrosActivos = 0;
    headerNuevo.version = VERSION_ARCHIVO;
    archivoTempStream.write((char*)&headerNuevo, sizeof(ArchivoHeader));

    ArchivoHeader headerOrig = leerHeader(nombreArchivo);
    int nuevosRegistros = 0;

    if (strcmp(nombreArchivo, PACIENTES_BIN) == 0) {
        for (int i = 0; i < headerOrig.cantidadRegistros; i++) {
            Paciente p = leerPacientePorIndice(i);
            if (p.getId() != -1 && !p.getEliminado()) {
                archivoTempStream.write((char*)&p, sizeof(Paciente));
                nuevosRegistros++;
            }
        }
    } else if (strcmp(nombreArchivo, DOCTORES_BIN) == 0) {
        for (int i = 0; i < headerOrig.cantidadRegistros; i++) {
            Doctor d = leerDoctorPorIndice(i);
            if (d.getId() != -1 && !d.getEliminado()) {
                archivoTempStream.write((char*)&d, sizeof(Doctor));
                nuevosRegistros++;
            }
        }
    } else if (strcmp(nombreArchivo, CITAS_BIN) == 0) {
        for (int i = 0; i < headerOrig.cantidadRegistros; i++) {
            Cita c = leerCitaPorIndice(i);
            if (c.getId() != -1 && !c.getEliminado()) {
                archivoTempStream.write((char*)&c, sizeof(Cita));
                nuevosRegistros++;
            }
        }
    } else if (strcmp(nombreArchivo, HISTORIALES_BIN) == 0) {
        for (int i = 0; i < headerOrig.cantidadRegistros; i++) {
            HistorialMedico h = leerHistorialPorIndice(i);
            if (h.getId() != -1 && !h.getEliminado()) {
                archivoTempStream.write((char*)&h, sizeof(HistorialMedico));
                nuevosRegistros++;
            }
        }
    } else {
        archivoTempStream.close();
        remove(archivoTemp.c_str());
        cout << "Error: nombre de archivo no reconocido para compactacion\n";
        return false;
    }

    // Actualizar header del archivo temporal
    headerNuevo.cantidadRegistros = nuevosRegistros;
    headerNuevo.registrosActivos = nuevosRegistros;
    headerNuevo.proximoID = headerOrig.proximoID;
    headerNuevo.version = headerOrig.version;
    archivoTempStream.seekp(0);
    archivoTempStream.write((char*)&headerNuevo, sizeof(ArchivoHeader));
    archivoTempStream.close();

    // Reemplazar archivo original
    if (remove(nombreArchivo) != 0) {
        // si no se pudo eliminar, intentar renombrar de todas formas
    }
    if (rename(archivoTemp.c_str(), nombreArchivo) != 0) {
        cout << "Error: No se pudo renombrar el archivo temporal\n";
        remove(archivoTemp.c_str());
        return false;
    }

    cout << "Archivo compactado: " << nuevosRegistros << " registros activos.\n";
    return true;
}