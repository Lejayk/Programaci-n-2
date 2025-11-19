#include "GestorArchivos.hpp"
#include <iostream>
#include <cstring>
#include <vector>
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
        return CREAR_DIRECTORIO("datos") == 0 || GetLastError() == ERROR_ALREADY_EXISTS;
    #else
        return CREAR_DIRECTORIO("datos", 0755) == 0 || errno == EEXIST;
    #endif
}

long GestorArchivos::calcularPosicion(int indice, size_t tamRegistro) {
    return sizeof(ArchivoHeader) + (indice * tamRegistro);
}

bool GestorArchivos::inicializarArchivo(const char* nombreArchivo) {
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
    
    ifstream archivoOrig(nombreArchivo, ios::binary);
    ofstream archivoTempStream(archivoTemp, ios::binary);
    
    if (!archivoOrig.is_open() || !archivoTempStream.is_open()) {
        cout << "Error: No se pudieron abrir los archivos para compactacion\n";
        return false;
    }
    
    // Leer header original
    ArchivoHeader headerOrig;
    archivoOrig.read((char*)&headerOrig, sizeof(ArchivoHeader));
    
    // Preparar nuevo header
    ArchivoHeader headerNuevo;
    headerNuevo.cantidadRegistros = 0;
    headerNuevo.proximoID = headerOrig.proximoID;
    headerNuevo.registrosActivos = 0;
    headerNuevo.version = headerOrig.version;
    
    // Escribir header temporal
    archivoTempStream.write((char*)&headerNuevo, sizeof(ArchivoHeader));
    
    // Procesar registros
    int nuevosRegistros = 0;
    for (int i = 0; i < headerOrig.cantidadRegistros; i++) {
        char* buffer = new char[tamRegistro];
        archivoOrig.read(buffer, tamRegistro);
        
        // Verificar si el registro está eliminado (asumiendo que eliminado está en posición fija)
        // Para Paciente: después de id (4 bytes) + arrays de chars + ints + bool eliminado
        bool eliminado = false;
        if (nombreArchivo == PACIENTES_BIN) {
            eliminado = *((bool*)(buffer + sizeof(Paciente) - 2 * sizeof(time_t) - sizeof(bool)));
        } else if (nombreArchivo == DOCTORES_BIN) {
            eliminado = *((bool*)(buffer + sizeof(Doctor) - 2 * sizeof(time_t) - sizeof(bool)));
        } else if (nombreArchivo == CITAS_BIN) {
            eliminado = *((bool*)(buffer + sizeof(Cita) - 2 * sizeof(time_t) - sizeof(bool)));
        } else if (nombreArchivo == HISTORIALES_BIN) {
            eliminado = *((bool*)(buffer + sizeof(HistorialMedico) - sizeof(time_t) - sizeof(bool)));
        }
        
        if (!eliminado) {
            archivoTempStream.write(buffer, tamRegistro);
            nuevosRegistros++;
        }
        delete[] buffer;
    }
    
    // Actualizar header del archivo temporal
    headerNuevo.cantidadRegistros = nuevosRegistros;
    headerNuevo.registrosActivos = nuevosRegistros;
    archivoTempStream.seekp(0);
    archivoTempStream.write((char*)&headerNuevo, sizeof(ArchivoHeader));
    
    archivoOrig.close();
    archivoTempStream.close();
    
    // Reemplazar archivo original
    if (remove(nombreArchivo) != 0) {
        cout << "Error: No se pudo eliminar el archivo original\n";
        return false;
    }
    
    if (rename(archivoTemp.c_str(), nombreArchivo) != 0) {
        cout << "Error: No se pudo renombrar el archivo temporal\n";
        return false;
    }
    
    cout << "Archivo compactado: " << nuevosRegistros << " registros activos.\n";
    return true;
}