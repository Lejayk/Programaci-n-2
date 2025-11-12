#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <cctype>
#include <string>
#include <fstream>
#include <windows.h>

using namespace std;

// Prototipos usados antes de su definicion
bool validarFecha(const char* fecha);
bool validarHora(const char* hora);
void limpiarBufferEntrada();

// =====================================================
// ESTRUCTURAS PARA PERSISTENCIA
// =====================================================

struct ArchivoHeader {
    int cantidadRegistros;
    int proximoID;
    int registrosActivos;
    int version;
};

struct Hospital {
    char nombre[100];
    char direccion[150];
    char telefono[15];
    
    int siguienteIDPaciente;
    int siguienteIDDoctor;
    int siguienteIDCita;
    int siguienteIDConsulta;
    
    int totalPacientesRegistrados;
    int totalDoctoresRegistrados;
    int totalCitasAgendadas;
    int totalConsultasRealizadas;
};

struct Paciente {
    int id;
    char nombre[50];
    char apellido[50];
    char cedula[20];
    int edad;
    char sexo;
    char tipoSangre[5];
    char telefono[15];
    char direccion[100];
    char email[50];
    char alergias[500];
    char observaciones[500];
    bool activo;
    
    int cantidadConsultas;
    int primerConsultaID;
    
    int cantidadCitas;
    int citasIDs[20];
    
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaModificacion;
};

struct HistorialMedico {
    int id;
    int pacienteID;
    char fecha[11];
    char hora[6];
    char diagnostico[200];
    char tratamiento[200];
    char medicamentos[150];
    int doctorID;
    float costo;
    
    int siguienteConsultaID;
    
    bool eliminado;
    time_t fechaRegistro;
};

struct Doctor {
    int id;
    char nombre[50];
    char apellido[50];
    char cedulaProfesional[20];
    char especialidad[50];
    int aniosExperiencia;
    float costoConsulta;
    char horarioAtencion[50];
    char telefono[15];
    char email[50];
    bool disponible;
    
    int cantidadPacientes;
    int pacientesIDs[50];
    
    int cantidadCitas;
    int citasIDs[30];
    
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaModificacion;
};

struct Cita {
    int id;
    int pacienteID;
    int doctorID;
    char fecha[11];
    char hora[6];
    char motivo[150];
    char estado[20];
    char observaciones[200];
    bool atendida;
    
    int consultaID;
    
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaModificacion;
};

// =====================================================
// CONSTANTES
// =====================================================
const int VERSION_ARCHIVO = 1;
const int MAX_RESULTADOS_BUSQUEDA = 100;
const int MAX_CONSULTAS_PACIENTE = 1000;

// =====================================================
// FUNCIONES DE MANEJO DE ARCHIVOS
// =====================================================

bool crearDirectorioData() {
    #ifdef _WIN32
        return system("mkdir DATA 2>nul") == 0;
    #else
        return system("mkdir -p DATA 2>/dev/null") == 0;
    #endif
}

bool inicializarArchivo(const char* nombreArchivo) {
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

ArchivoHeader leerHeader(const char* nombreArchivo) {
    ArchivoHeader header;
    memset(&header, 0, sizeof(ArchivoHeader));
    
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo.is_open()) return header;
    
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    archivo.close();
    return header;
}

bool actualizarHeader(const char* nombreArchivo, ArchivoHeader nuevoHeader) {
    fstream archivo(nombreArchivo, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) return false;
    
    archivo.seekp(0);
    archivo.write((char*)&nuevoHeader, sizeof(ArchivoHeader));
    archivo.close();
    return true;
}

// =====================================================
// SISTEMA DE ARCHIVOS ESPECIFICOS
// =====================================================

bool inicializarSistemaArchivos() {
    if (!crearDirectorioData()) {
        cout << "Error: No se pudo crear directorio DATA\n";
        return false;
    }
    
    // Inicializar hospital.bin
    fstream archivoHospital("DATA/hospital.bin", ios::binary | ios::out);
    if (!archivoHospital.is_open()) return false;
    
    Hospital hospital;
    memset(&hospital, 0, sizeof(Hospital));
    strcpy(hospital.nombre, "Hospital SALTO ANGEL");
    strcpy(hospital.direccion, "Av. Secundaria #777");
    strcpy(hospital.telefono, "0412-0000000");
    hospital.siguienteIDPaciente = 1;
    hospital.siguienteIDDoctor = 1;
    hospital.siguienteIDCita = 1;
    hospital.siguienteIDConsulta = 1;
    
    archivoHospital.write((char*)&hospital, sizeof(Hospital));
    archivoHospital.close();
    
    // Inicializar archivos de datos
    bool success = true;
    success &= inicializarArchivo("DATA/pacientes.bin");
    success &= inicializarArchivo("DATA/doctores.bin");
    success &= inicializarArchivo("DATA/citas.bin");
    success &= inicializarArchivo("DATA/historiales.bin");
    
    if (success) {
        cout << "Sistema de archivos inicializado correctamente.\n";
    } else {
        cout << "Error: No se pudieron crear todos los archivos.\n";
    }
    
    return success;
}

Hospital* cargarDatosHospital() {
    Hospital* hospital = new Hospital;
    
    ifstream archivo("DATA/hospital.bin", ios::binary);
    if (!archivo.is_open()) {
        // Si no existe, inicializar sistema
        if (!inicializarSistemaArchivos()) {
            delete hospital;
            return nullptr;
        }
        archivo.open("DATA/hospital.bin", ios::binary);
        if (!archivo.is_open()) {
            delete hospital;
            return nullptr;
        }
    }
    
    archivo.read((char*)hospital, sizeof(Hospital));
    archivo.close();
    cout << "=== SISTEMA DE GESTION HOSPITALARIO v2.0 ===\n";
    cout << "===    (Sistema de Archivos Binarios)    ===\n\n";
    cout << "Sistema de archivos cargado correctamente.\n";
    system("pause");
    system("cls");
    
    return hospital;
}

bool guardarDatosHospital(Hospital* hospital) {
    ofstream archivo("DATA/hospital.bin", ios::binary | ios::out);
    if (!archivo.is_open()) return false;
    
    archivo.write((char*)hospital, sizeof(Hospital));
    archivo.close();
    return true;
}

// =====================================================
// FUNCIONES PARA PACIENTES (ARCHIVOS BINARIOS)
// =====================================================

long calcularPosicionPaciente(int indice) {
    return sizeof(ArchivoHeader) + (indice * sizeof(Paciente));
}

Paciente leerPacientePorIndice(int indice) {
    Paciente paciente;
    paciente.id = -1;
    
    ifstream archivo("DATA/pacientes.bin", ios::binary);
    if (!archivo.is_open()) return paciente;
    
    ArchivoHeader header = leerHeader("DATA/pacientes.bin");
    if (indice < 0 || indice >= header.cantidadRegistros) {
        archivo.close();
        return paciente;
    }
    
    long posicion = calcularPosicionPaciente(indice);
    archivo.seekg(posicion);
    archivo.read((char*)&paciente, sizeof(Paciente));
    archivo.close();
    
    return paciente;
}

Paciente buscarPacientePorID(int id) {
    ArchivoHeader header = leerHeader("DATA/pacientes.bin");
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Paciente paciente = leerPacientePorIndice(i);
        if (paciente.id == id && !paciente.eliminado) {
            return paciente;
        }
    }
    
    Paciente noEncontrado;
    noEncontrado.id = -1;
    return noEncontrado;
}

Paciente buscarPacientePorCedula(const char* cedula) {
    ArchivoHeader header = leerHeader("DATA/pacientes.bin");
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Paciente paciente = leerPacientePorIndice(i);
        if (!paciente.eliminado && strcmp(paciente.cedula, cedula) == 0) {
            return paciente;
        }
    }
    
    Paciente noEncontrado;
    noEncontrado.id = -1;
    return noEncontrado;
}

bool guardarPaciente(Paciente &paciente) {
    fstream archivo("DATA/pacientes.bin", ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) return false;
    
    ArchivoHeader header = leerHeader("DATA/pacientes.bin");
    bool esNuevo = true;
    int indiceEncontrado = -1;
    
    // Buscar si el paciente ya existe
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Paciente temp = leerPacientePorIndice(i);
        if (temp.id == paciente.id) {
            esNuevo = false;
            indiceEncontrado = i;
            break;
        }
    }
    
    if (esNuevo) {
        // Nuevo paciente
        paciente.id = header.proximoID++;
        paciente.fechaCreacion = time(0);
        paciente.eliminado = false;
        header.cantidadRegistros++;
        header.registrosActivos++;
        
        // Escribir al final
        archivo.seekp(0, ios::end);
        archivo.write((char*)&paciente, sizeof(Paciente));
        
        // Actualizar header
        actualizarHeader("DATA/pacientes.bin", header);
    } else {
        // Actualizar paciente existente
        paciente.fechaModificacion = time(0);
        long posicion = calcularPosicionPaciente(indiceEncontrado);
        archivo.seekp(posicion);
        archivo.write((char*)&paciente, sizeof(Paciente));
    }
    
    archivo.close();
    return true;
}

bool eliminarPacienteLogico(int id) {
    Paciente paciente = buscarPacientePorID(id);
    if (paciente.id == -1) return false;
    
    paciente.eliminado = true;
    paciente.activo = false;
    paciente.fechaModificacion = time(0);
    
    return guardarPaciente(paciente);
}

// =====================================================
// FUNCIONES COMPLETAS PARA DOCTORES
// =====================================================

long calcularPosicionDoctor(int indice) {
    return sizeof(ArchivoHeader) + (indice * sizeof(Doctor));
}

Doctor leerDoctorPorIndice(int indice) {
    Doctor doctor;
    doctor.id = -1;
    
    ifstream archivo("DATA/doctores.bin", ios::binary);
    if (!archivo.is_open()) return doctor;
    
    ArchivoHeader header = leerHeader("DATA/doctores.bin");
    if (indice < 0 || indice >= header.cantidadRegistros) {
        archivo.close();
        return doctor;
    }
    
    long posicion = calcularPosicionDoctor(indice);
    archivo.seekg(posicion);
    archivo.read((char*)&doctor, sizeof(Doctor));
    archivo.close();
    
    return doctor;
}

Doctor buscarDoctorPorID(int id) {
    ArchivoHeader header = leerHeader("DATA/doctores.bin");
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Doctor doctor = leerDoctorPorIndice(i);
        if (doctor.id == id && !doctor.eliminado) {
            return doctor;
        }
    }
    
    Doctor noEncontrado;
    noEncontrado.id = -1;
    return noEncontrado;
}

bool guardarDoctor(Doctor &doctor) {
    fstream archivo("DATA/doctores.bin", ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) return false;
    
    ArchivoHeader header = leerHeader("DATA/doctores.bin");
    bool esNuevo = true;
    int indiceEncontrado = -1;
    
    // Buscar si el doctor ya existe
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Doctor temp = leerDoctorPorIndice(i);
        if (temp.id == doctor.id) {
            esNuevo = false;
            indiceEncontrado = i;
            break;
        }
    }
    
    if (esNuevo) {
        // Nuevo doctor
        doctor.id = header.proximoID++;
        doctor.fechaCreacion = time(0);
        doctor.eliminado = false;
        header.cantidadRegistros++;
        header.registrosActivos++;
        
        // Escribir al final
        archivo.seekp(0, ios::end);
        archivo.write((char*)&doctor, sizeof(Doctor));
        
        // Actualizar header
        actualizarHeader("DATA/doctores.bin", header);
    } else {
        // Actualizar doctor existente
        doctor.fechaModificacion = time(0);
        long posicion = calcularPosicionDoctor(indiceEncontrado);
        archivo.seekp(posicion);
        archivo.write((char*)&doctor, sizeof(Doctor));
    }
    
    archivo.close();
    return true;
}

// =====================================================
// FUNCIONES BASICAS PARA CITAS (ARCHIVOS BINARIOS)
// =====================================================

long calcularPosicionCita(int indice) {
    return sizeof(ArchivoHeader) + (indice * sizeof(Cita));
}

Cita leerCitaPorIndice(int indice) {
    Cita cita;
    cita.id = -1;
    
    ifstream archivo("DATA/citas.bin", ios::binary);
    if (!archivo.is_open()) return cita;
    
    ArchivoHeader header = leerHeader("DATA/citas.bin");
    if (indice < 0 || indice >= header.cantidadRegistros) {
        archivo.close();
        return cita;
    }
    
    long posicion = calcularPosicionCita(indice);
    archivo.seekg(posicion);
    archivo.read((char*)&cita, sizeof(Cita));
    archivo.close();
    
    return cita;
}

Cita buscarCitaPorID(int id) {
    ArchivoHeader header = leerHeader("DATA/citas.bin");
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Cita cita = leerCitaPorIndice(i);
        if (cita.id == id && !cita.eliminado) {
            return cita;
        }
    }
    
    Cita noEncontrado;
    noEncontrado.id = -1;
    return noEncontrado;
}

bool guardarCita(Cita &cita) {
    fstream archivo("DATA/citas.bin", ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir archivo de citas\n";
        return false;
    }
    
    ArchivoHeader header = leerHeader("DATA/citas.bin");
    bool esNueva = true;
    int indiceEncontrado = -1;
    
    // Buscar si la cita ya existe
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Cita temp = leerCitaPorIndice(i);
        if (temp.id == cita.id && cita.id != 0) {
            esNueva = false;
            indiceEncontrado = i;
            break;
        }
    }
    
    if (esNueva) {
        // NUEVA CITA: Asignar ID desde el header
        cita.id = header.proximoID;
        cita.fechaCreacion = time(0);
        cita.fechaModificacion = time(0);
        cita.eliminado = false;
        cita.atendida = false;
        cita.consultaID = -1;
        strcpy(cita.estado, "Agendada");
        
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
        cita.fechaModificacion = time(0);
        long posicion = calcularPosicionCita(indiceEncontrado);
        archivo.seekp(posicion);
        archivo.write((char*)&cita, sizeof(Cita));
    }
    
    archivo.close();
    return true;
}

// =====================================================
// FUNCIONES BASICAS PARA HISTORIAL (ARCHIVOS BINARIOS)
// =====================================================

long calcularPosicionHistorial(int indice) {
    return sizeof(ArchivoHeader) + (indice * sizeof(HistorialMedico));
}

HistorialMedico leerHistorialPorIndice(int indice) {
    HistorialMedico historial;
    historial.id = -1;
    
    ifstream archivo("DATA/historiales.bin", ios::binary);
    if (!archivo.is_open()) return historial;
    
    ArchivoHeader header = leerHeader("DATA/historiales.bin");
    if (indice < 0 || indice >= header.cantidadRegistros) {
        archivo.close();
        return historial;
    }
    
    long posicion = calcularPosicionHistorial(indice);
    archivo.seekg(posicion);
    archivo.read((char*)&historial, sizeof(HistorialMedico));
    archivo.close();
    
    return historial;
}

HistorialMedico buscarHistorialPorID(int id) {
    ArchivoHeader header = leerHeader("DATA/historiales.bin");
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        HistorialMedico historial = leerHistorialPorIndice(i);
        if (historial.id == id && !historial.eliminado) {
            return historial;
        }
    }
    
    HistorialMedico noEncontrado;
    noEncontrado.id = -1;
    return noEncontrado;
}

bool guardarHistorial(HistorialMedico &historial) {
    fstream archivo("DATA/historiales.bin", ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir archivo de historiales\n";
        return false;
    }
    
    ArchivoHeader header = leerHeader("DATA/historiales.bin");
    bool esNuevo = true;
    int indiceEncontrado = -1;
    
    // Buscar si el historial ya existe
    for (int i = 0; i < header.cantidadRegistros; i++) {
        HistorialMedico temp = leerHistorialPorIndice(i);
        if (temp.id == historial.id && historial.id != 0) {
            esNuevo = false;
            indiceEncontrado = i;
            break;
        }
    }
    
    if (esNuevo) {
        // NUEVO HISTORIAL: Asignar ID desde el header
        historial.id = header.proximoID;
        historial.fechaRegistro = time(0);
        historial.eliminado = false;
        
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
        // Actualizar historial existente
        long posicion = calcularPosicionHistorial(indiceEncontrado);
        archivo.seekp(posicion);
        archivo.write((char*)&historial, sizeof(HistorialMedico));
    }
    
    archivo.close();
    return true;
}

// =====================================================
// SISTEMA COMPLETO DE HISTORIAL MEDICO ENLAZADO
// =====================================================

HistorialMedico buscarUltimaConsultaPaciente(int pacienteID) {
    Paciente paciente = buscarPacientePorID(pacienteID);
    if (paciente.id == -1 || paciente.primerConsultaID == -1) {
        HistorialMedico vacio;
        vacio.id = -1;
        return vacio;
    }
    
    HistorialMedico actual = buscarHistorialPorID(paciente.primerConsultaID);
    if (actual.id == -1) {
        return actual;
    }
    
    // Seguir la cadena hasta la ultima consulta
    while (actual.siguienteConsultaID != -1) {
        actual = buscarHistorialPorID(actual.siguienteConsultaID);
        if (actual.id == -1) {
            break;
        }
    }
    
    return actual;
}

bool agregarConsultaAlHistorial(int pacienteID, const char* diagnostico, 
                               const char* tratamiento, const char* medicamentos, 
                               int doctorID, float costo) {
    
    Paciente paciente = buscarPacientePorID(pacienteID);
    if (paciente.id == -1) {
        cout << "Error: Paciente no encontrado.\n";
        return false;
    }
    
    // Crear nueva consulta
    HistorialMedico nuevaConsulta;
    memset(&nuevaConsulta, 0, sizeof(HistorialMedico));
    
    ArchivoHeader header = leerHeader("DATA/historiales.bin");
    nuevaConsulta.id = header.proximoID;
    nuevaConsulta.pacienteID = pacienteID;
    nuevaConsulta.doctorID = doctorID;
    
    // Obtener fecha y hora actual
    time_t ahora = time(0);
    tm* tiempoLocal = localtime(&ahora);
    snprintf(nuevaConsulta.fecha, sizeof(nuevaConsulta.fecha), 
             "%04d-%02d-%02d", 
             tiempoLocal->tm_year + 1900, 
             tiempoLocal->tm_mon + 1, 
             tiempoLocal->tm_mday);
    snprintf(nuevaConsulta.hora, sizeof(nuevaConsulta.hora), 
             "%02d:%02d", 
             tiempoLocal->tm_hour, 
             tiempoLocal->tm_min);
    
    strncpy(nuevaConsulta.diagnostico, diagnostico, sizeof(nuevaConsulta.diagnostico)-1);
    strncpy(nuevaConsulta.tratamiento, tratamiento, sizeof(nuevaConsulta.tratamiento)-1);
    strncpy(nuevaConsulta.medicamentos, medicamentos, sizeof(nuevaConsulta.medicamentos)-1);
    nuevaConsulta.costo = costo;
    nuevaConsulta.fechaRegistro = ahora;
    nuevaConsulta.eliminado = false;
    nuevaConsulta.siguienteConsultaID = -1; // Siempre sera la ultima
    
    if (paciente.primerConsultaID == -1) {
        // Primera consulta del paciente
        paciente.primerConsultaID = nuevaConsulta.id;
    } else {
    // Buscar la ultima consulta y actualizar su enlace
        HistorialMedico ultimaConsulta = buscarUltimaConsultaPaciente(pacienteID);
        if (ultimaConsulta.id != -1) {
            ultimaConsulta.siguienteConsultaID = nuevaConsulta.id;
            if (!guardarHistorial(ultimaConsulta)) {
                cout << "Error: No se pudo actualizar el enlace de la ultima consulta.\n";
                return false;
            }
        } else {
            cout << "Advertencia: No se pudo encontrar la ultima consulta para enlazar.\n";
        }
    }
    
    // Guardar la nueva consulta
    if (!guardarHistorial(nuevaConsulta)) {
        cout << "Error: No se pudo guardar la nueva consulta.\n";
        return false;
    }
    
    // Actualizar contador del paciente
    paciente.cantidadConsultas++;
    if (!guardarPaciente(paciente)) {
        cout << "Error: No se pudo actualizar el contador de consultas del paciente.\n";
        return false;
    }
    
    cout << "Consulta agregada al historial correctamente. ID: " << nuevaConsulta.id << "\n";
    return true;
}

bool atenderCitaMejorada(Hospital* h, int idCita, const char* diagnostico,
                        const char* tratamiento, const char* medicamentos) {
    
    Cita cita = buscarCitaPorID(idCita);
    if (cita.id == -1) {
        cout << "Cita no encontrada.\n";
        return false;
    }
    
    if (strcmp(cita.estado, "Agendada") != 0) {
        cout << "Error: La cita no esta en estado 'Agendada'.\n";
        return false;
    }
    
    // Obtener costo del doctor
    Doctor doctor = buscarDoctorPorID(cita.doctorID);
    float costoConsulta = (doctor.id != -1) ? doctor.costoConsulta : 0.0f;
    
    // Usar el sistema enlazado para agregar la consulta
    if (!agregarConsultaAlHistorial(cita.pacienteID, diagnostico, tratamiento, 
                                   medicamentos, cita.doctorID, costoConsulta)) {
        return false;
    }
    
    // Obtener el ID de la consulta recien creada
    ArchivoHeader headerHistorial = leerHeader("DATA/historiales.bin");
    int idConsultaCreada = headerHistorial.proximoID - 1;
    
    // Actualizar cita
    cita.consultaID = idConsultaCreada;
    strcpy(cita.estado, "Atendida");
    cita.atendida = true;
    cita.fechaModificacion = time(0);
    
    if (guardarCita(cita)) {
        h->totalConsultasRealizadas++;
        guardarDatosHospital(h);
        cout << "Cita atendida y agregada al historial correctamente.\n";
        return true;
    } else {
        cout << "Error al guardar los cambios de la cita.\n";
        return false;
    }
}

// Funcion auxiliar para ordenamiento burbuja de enteros
void ordenarBurbuja(int arr[], int n) {
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                int temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}

bool repararListaHistorialPaciente(int pacienteID) {
    Paciente paciente = buscarPacientePorID(pacienteID);
    if (paciente.id == -1) {
        cout << "Paciente no encontrado.\n";
        return false;
    }
    
    if (paciente.primerConsultaID == -1) {
        cout << "El paciente no tiene consultas en el historial.\n";
        return true;
    }
    
    cout << "Reparando lista enlazada del paciente " << paciente.nombre << "...\n";
    
    // Recorrer todas las consultas del paciente y reconstruir la lista
    ArchivoHeader header = leerHeader("DATA/historiales.bin");
    int consultasDelPaciente[MAX_CONSULTAS_PACIENTE];
    int numConsultas = 0;
    
    // Colectar todas las consultas del paciente
    for (int i = 0; i < header.cantidadRegistros && numConsultas < MAX_CONSULTAS_PACIENTE; i++) {
        HistorialMedico consulta = leerHistorialPorIndice(i);
        if (!consulta.eliminado && consulta.pacienteID == pacienteID) {
            consultasDelPaciente[numConsultas++] = consulta.id;
        }
    }
    
    if (numConsultas == 0) {
        cout << "No se encontraron consultas para el paciente.\n";
        paciente.primerConsultaID = -1;
        paciente.cantidadConsultas = 0;
        return guardarPaciente(paciente);
    }
    
    // Ordenar por ID (asumiendo que IDs mas altos son mas recientes)
    ordenarBurbuja(consultasDelPaciente, numConsultas);
    
    // Reconstruir enlaces
    paciente.primerConsultaID = consultasDelPaciente[0];
    paciente.cantidadConsultas = numConsultas;
    
    for (int i = 0; i < numConsultas; i++) {
        HistorialMedico consulta = buscarHistorialPorID(consultasDelPaciente[i]);
        if (consulta.id != -1) {
            if (i == numConsultas - 1) {
                consulta.siguienteConsultaID = -1; // Ultima consulta
            } else {
                consulta.siguienteConsultaID = consultasDelPaciente[i + 1];
            }
            guardarHistorial(consulta);
        }
    }
    
    if (guardarPaciente(paciente)) {
        cout << "Lista enlazada reparada correctamente. Total consultas: " 
             << numConsultas << "\n";
        return true;
    }
    
    return false;
}

// =====================================================
// SISTEMA COMPLETO DE MANTENIMIENTO
// =====================================================

bool compactarArchivo(const char* nombreArchivo, size_t tamanoRegistro) {
    string archivoTemp = string(nombreArchivo) + ".tmp";
    
    // Abrir archivo original y temporal
    ifstream archivoOrig(nombreArchivo, ios::binary);
    ofstream archivoTempStream(archivoTemp, ios::binary);
    
    if (!archivoOrig.is_open() || !archivoTempStream.is_open()) {
        cout << "Error: No se pudieron abrir los archivos para compactacion.\n";
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
        char* buffer = new char[tamanoRegistro];
        archivoOrig.read(buffer, tamanoRegistro);
        
    // Verificar si el registro esta eliminado (primer byte despues del ID)
    bool eliminado = *((bool*)(buffer + sizeof(int)));
        
        if (!eliminado) {
            archivoTempStream.write(buffer, tamanoRegistro);
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
        cout << "Error: No se pudo eliminar el archivo original.\n";
        return false;
    }
    
    if (rename(archivoTemp.c_str(), nombreArchivo) != 0) {
        cout << "Error: No se pudo renombrar el archivo temporal.\n";
        return false;
    }
    
    cout << "Archivo compactado: " << nuevosRegistros << " registros activos.\n";
    return true;
}

void compactarTodosArchivos() {
    cout << "\n=== COMPACTACION DE ARCHIVOS ===\n";

    cout << "Compactando pacientes...\n";
    if (compactarArchivo("DATA/pacientes.bin", sizeof(Paciente))) {
        cout << "Pacientes compactados correctamente.\n";
    } else {
        cout << "Error compactando pacientes.\n";
    }

    cout << "Compactando doctores...\n";
    if (compactarArchivo("DATA/doctores.bin", sizeof(Doctor))) {
        cout << "Doctores compactados correctamente.\n";
    } else {
        cout << "Error compactando doctores.\n";
    }

    cout << "Compactando citas...\n";
    if (compactarArchivo("DATA/citas.bin", sizeof(Cita))) {
        cout << "Citas compactadas correctamente.\n";
    } else {
        cout << "Error compactando citas.\n";
    }

    cout << "Compactando historiales...\n";
    if (compactarArchivo("DATA/historiales.bin", sizeof(HistorialMedico))) {
        cout << "Historiales compactados correctamente.\n";
    } else {
        cout << "Error compactando historiales.\n";
    }

    cout << "Compactacion completada.\n";
}

bool crearRespaldo(const char* directorio) {
    string comando = "mkdir \"" + string(directorio) + "\" 2>nul";
    system(comando.c_str());
    
    const char* archivos[] = {"hospital.bin", "pacientes.bin", "doctores.bin", "citas.bin", "historiales.bin"};
    int numArchivos = 5;
    bool success = true;
    
    cout << "\n=== CREANDO RESPALDO ===\n";
    
    for (int i = 0; i < numArchivos; i++) {
        string origen = "DATA/" + string(archivos[i]);
        string destino = string(directorio) + "/" + archivos[i];
        
        ifstream src(origen, ios::binary);
        ofstream dst(destino, ios::binary);
        
        if (src.is_open() && dst.is_open()) {
            dst << src.rdbuf();
            cout << archivos[i] << " respaldado correctamente.\n";
        } else {
            cout << "Error respaldando " << archivos[i] << "\n";
            success = false;
        }
        
        src.close();
        dst.close();
    }
    
    if (success) {
        cout << "Respaldo completado en: " << directorio << "\n";
    }
    
    return success;
}

bool restaurarRespaldo(const char* directorio) {
    cout << "\nADVERTENCIA: Esta accion sobreescribira todos los datos actuales.\n";
    cout << "Esta seguro? (s/n): ";
    char confirmacion;
    cin >> confirmacion;
    limpiarBufferEntrada();
    
    if (confirmacion != 's' && confirmacion != 'S') {
        cout << "Restauracion cancelada.\n";
        return false;
    }
    
    const char* archivos[] = {"hospital.bin", "pacientes.bin", "doctores.bin", "citas.bin", "historiales.bin"};
    int numArchivos = 5;
    bool success = true;
    
    cout << "\n=== RESTAURANDO DESDE RESPALDO ===\n";
    
    for (int i = 0; i < numArchivos; i++) {
        string origen = string(directorio) + "/" + archivos[i];
    string destino = string("DATA/") + archivos[i];
        
        ifstream src(origen, ios::binary);
        ofstream dst(destino, ios::binary);
        
        if (src.is_open() && dst.is_open()) {
            dst << src.rdbuf();
            cout << archivos[i] << " restaurado correctamente.\n";
        } else {
            cout << "Error restaurando " << archivos[i] << "\n";
            success = false;
        }
        
        src.close();
        dst.close();
    }
    
    if (success) {
        cout << "Restauracion completada desde: " << directorio << "\n";
    }
    
    return success;
}

void verificarIntegridadReferencial() {
    cout << "\n=== VERIFICACION DE INTEGRIDAD REFERENCIAL ===\n";
    
    int errores = 0;
    
    // Verificar citas -> pacientes
    ArchivoHeader headerCitas = leerHeader("DATA/citas.bin");
    for (int i = 0; i < headerCitas.cantidadRegistros; i++) {
        Cita cita = leerCitaPorIndice(i);
        if (!cita.eliminado) {
            Paciente paciente = buscarPacientePorID(cita.pacienteID);
                if (paciente.id == -1 || paciente.eliminado) {
                cout << "Cita #" << cita.id << " referencia paciente inexistente: " << cita.pacienteID << "\n";
                errores++;
            }
            
            Doctor doctor = buscarDoctorPorID(cita.doctorID);
            if (doctor.id == -1 || doctor.eliminado) {
                cout << "Cita #" << cita.id << " referencia doctor inexistente: " << cita.doctorID << "\n";
                errores++;
            }
        }
    }
    
    // Verificar historiales -> pacientes
    ArchivoHeader headerHistorial = leerHeader("DATA/historiales.bin");
    for (int i = 0; i < headerHistorial.cantidadRegistros; i++) {
        HistorialMedico historial = leerHistorialPorIndice(i);
        if (!historial.eliminado) {
            Paciente paciente = buscarPacientePorID(historial.pacienteID);
            if (paciente.id == -1 || paciente.eliminado) {
                cout << "Historial #" << historial.id << " referencia paciente inexistente: " << historial.pacienteID << "\n";
                errores++;
            }
            
            Doctor doctor = buscarDoctorPorID(historial.doctorID);
            if (doctor.id == -1 || doctor.eliminado) {
                cout << "Historial #" << historial.id << " referencia doctor inexistente: " << historial.doctorID << "\n";
                errores++;
            }
        }
    }
    
    // Verificar listas enlazadas de historial
    ArchivoHeader headerPacientes = leerHeader("DATA/pacientes.bin");
    for (int i = 0; i < headerPacientes.cantidadRegistros; i++) {
        Paciente paciente = leerPacientePorIndice(i);
        if (!paciente.eliminado && paciente.primerConsultaID != -1) {
            int consultaID = paciente.primerConsultaID;
            int contador = 0;
            int maxConsultas = paciente.cantidadConsultas + 10; // Limite de seguridad
            
            while (consultaID != -1 && contador < maxConsultas) {
                HistorialMedico consulta = buscarHistorialPorID(consultaID);
                    if (consulta.id == -1 || consulta.eliminado) {
                    cout << "Paciente #" << paciente.id << " tiene referencia rota en historial: " << consultaID << "\n";
                    errores++;
                    break;
                }
                consultaID = consulta.siguienteConsultaID;
                contador++;
            }
            
            if (contador >= maxConsultas) {
                cout << "Paciente #" << paciente.id << " posible ciclo infinito en historial\n";
            }
        }
    }
    
    if (errores == 0) {
        cout << "Integridad referencial verificada: Sin errores encontrados.\n";
    } else {
        cout << "Se encontraron " << errores << " errores de integridad.\n";
    }
}

void repararIntegridadReferencial() {
    cout << "\n=== REPARACION DE INTEGRIDAD REFERENCIAL ===\n";
    
    int reparaciones = 0;
    
    // Reparar citas con referencias rotas
    ArchivoHeader headerCitas = leerHeader("DATA/citas.bin");
    for (int i = 0; i < headerCitas.cantidadRegistros; i++) {
        Cita cita = leerCitaPorIndice(i);
        if (!cita.eliminado) {
            bool necesitaReparacion = false;
            
            Paciente paciente = buscarPacientePorID(cita.pacienteID);
            if (paciente.id == -1 || paciente.eliminado) {
                cout << "Reparando cita #" << cita.id << ": Paciente " << cita.pacienteID << " no existe\n";
                cita.eliminado = true;
                strcpy(cita.estado, "Cancelada");
                necesitaReparacion = true;
            }
            
            Doctor doctor = buscarDoctorPorID(cita.doctorID);
            if (doctor.id == -1 || doctor.eliminado) {
                cout << "Reparando cita #" << cita.id << ": Doctor " << cita.doctorID << " no existe\n";
                cita.eliminado = true;
                strcpy(cita.estado, "Cancelada");
                necesitaReparacion = true;
            }
            
            if (necesitaReparacion) {
                guardarCita(cita);
                reparaciones++;
            }
        }
    }
    
    // Reparar historiales con referencias rotas
    ArchivoHeader headerHistorial = leerHeader("DATA/historiales.bin");
    for (int i = 0; i < headerHistorial.cantidadRegistros; i++) {
        HistorialMedico historial = leerHistorialPorIndice(i);
        if (!historial.eliminado) {
            Paciente paciente = buscarPacientePorID(historial.pacienteID);
            if (paciente.id == -1 || paciente.eliminado) {
                cout << "Marcando historial #" << historial.id << " como eliminado: Paciente no existe\n";
                historial.eliminado = true;
                guardarHistorial(historial);
                reparaciones++;
            }
        }
    }
    
    cout << "Reparacion completada: " << reparaciones << " registros reparados.\n";
}

void mostrarEstadisticasDetalladas() {
    cout << "\n=== ESTADISTICAS DETALLADAS DEL SISTEMA ===\n";
    
    ArchivoHeader hp = leerHeader("DATA/pacientes.bin");
    ArchivoHeader hd = leerHeader("DATA/doctores.bin");
    ArchivoHeader hc = leerHeader("DATA/citas.bin");
    ArchivoHeader hh = leerHeader("DATA/historiales.bin");
    
    cout << "PACIENTES:\n";
    cout << "   Total registros: " << hp.cantidadRegistros << "\n";
    cout << "   Registros activos: " << hp.registrosActivos << "\n";
    cout << "   Registros eliminados: " << (hp.cantidadRegistros - hp.registrosActivos) << "\n";
    cout << "   Proximo ID: " << hp.proximoID << "\n";
    
    cout << "DOCTORES:\n";
    cout << "   Total registros: " << hd.cantidadRegistros << "\n";
    cout << "   Registros activos: " << hd.registrosActivos << "\n";
    cout << "   Registros eliminados: " << (hd.cantidadRegistros - hd.registrosActivos) << "\n";
    cout << "   Proximo ID: " << hd.proximoID << "\n";
    
    cout << "CITAS:\n";
    cout << "   Total registros: " << hc.cantidadRegistros << "\n";
    cout << "   Registros activos: " << hc.registrosActivos << "\n";
    cout << "   Registros eliminados: " << (hc.cantidadRegistros - hc.registrosActivos) << "\n";
    cout << "   Proximo ID: " << hc.proximoID << "\n";
    
    cout << "HISTORIALES:\n";
    cout << "   Total registros: " << hh.cantidadRegistros << "\n";
    cout << "   Registros activos: " << hh.registrosActivos << "\n";
    cout << "   Registros eliminados: " << (hh.cantidadRegistros - hh.registrosActivos) << "\n";
    cout << "   Proximo ID: " << hh.proximoID << "\n";
    
    // Calcular uso de disco
    long tamanoTotal = 
        (hp.cantidadRegistros * sizeof(Paciente)) +
        (hd.cantidadRegistros * sizeof(Doctor)) +
        (hc.cantidadRegistros * sizeof(Cita)) +
        (hh.cantidadRegistros * sizeof(HistorialMedico)) +
        sizeof(ArchivoHeader) * 4;

    cout << "USO DE DISCO:\n";
    cout << "   Tamano aproximado: " << (tamanoTotal / 1024.0) << " KB\n";
    cout << "   Espacio desperdiciado (eliminados): " 
         << (((hp.cantidadRegistros - hp.registrosActivos) * sizeof(Paciente) +
             ((hd.cantidadRegistros - hd.registrosActivos) * sizeof(Doctor)) +
             ((hc.cantidadRegistros - hc.registrosActivos) * sizeof(Cita)) +
             ((hh.cantidadRegistros - hh.registrosActivos) * sizeof(HistorialMedico))) / 1024.0)
         << " KB\n";
}

// =====================================================
// FUNCIONES DE GESTION DE CITAS (SIN MEMORIA DINAMICA)
// =====================================================

bool verificarDisponibilidadDoctor(int idDoctor, const char* fecha, const char* hora) {
    ArchivoHeader header = leerHeader("DATA/citas.bin");
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Cita cita = leerCitaPorIndice(i);
        if (!cita.eliminado && 
            cita.doctorID == idDoctor && 
            strcmp(cita.estado, "Agendada") == 0 &&
            strcmp(cita.fecha, fecha) == 0 && 
            strcmp(cita.hora, hora) == 0) {
            return false; // No disponible
        }
    }
    return true; // Disponible
}

bool agendarCita(Hospital* h, int idPaciente, int idDoctor, const char* fecha,
                  const char* hora, const char* motivo) {
    
    // Verificar que paciente existe
    Paciente paciente = buscarPacientePorID(idPaciente);
    if (paciente.id == -1) {
        cout << "Error: Paciente no encontrado.\n";
        return false;
    }
    
    // Verificar que doctor existe
    Doctor doctor = buscarDoctorPorID(idDoctor);
    if (doctor.id == -1) {
        cout << "Error: Doctor no encontrado.\n";
        return false;
    }
    
    // Validar fecha y hora
    if (!validarFecha(fecha)) {
        cout << "Error: Fecha invalida. Formato: YYYY-MM-DD\n";
        return false;
    }
    
    if (!validarHora(hora)) {
        cout << "Error: Hora invalida. Formato: HH:MM\n";
        return false;
    }
    
    // Verificar disponibilidad del doctor
    if (!verificarDisponibilidadDoctor(idDoctor, fecha, hora)) {
        cout << "Error: El doctor no esta disponible en esa fecha y hora.\n";
        return false;
    }
    
    // Crear nueva cita
    Cita nuevaCita;
    memset(&nuevaCita, 0, sizeof(Cita));
    
    nuevaCita.pacienteID = idPaciente;
    nuevaCita.doctorID = idDoctor;
    strncpy(nuevaCita.fecha, fecha, sizeof(nuevaCita.fecha)-1);
    strncpy(nuevaCita.hora, hora, sizeof(nuevaCita.hora)-1);
    strncpy(nuevaCita.motivo, motivo, sizeof(nuevaCita.motivo)-1);
    strcpy(nuevaCita.estado, "Agendada");
    nuevaCita.atendida = false;
    nuevaCita.consultaID = -1;
    
    if (guardarCita(nuevaCita)) {
        h->totalCitasAgendadas++;
        guardarDatosHospital(h);
        
        cout << "Cita agendada exitosamente. ID: " << nuevaCita.id << "\n";
        
        // Actualizar paciente (agregar cita a su lista)
        if (paciente.cantidadCitas < 20) {
            paciente.citasIDs[paciente.cantidadCitas] = nuevaCita.id;
            paciente.cantidadCitas++;
            guardarPaciente(paciente);
        } else {
            cout << "Advertencia: El paciente ha alcanzado el maximo de citas.\n";
        }
        
        // Actualizar doctor (agregar cita a su lista)
        if (doctor.cantidadCitas < 30) {
            doctor.citasIDs[doctor.cantidadCitas] = nuevaCita.id;
            doctor.cantidadCitas++;
            guardarDoctor(doctor);
        } else {
            cout << "Advertencia: El doctor ha alcanzado el maximo de citas.\n";
        }
        
        return true;
    } else {
        cout << "Error: No se pudo guardar la cita.\n";
        return false;
    }
}

bool cancelarCita(int idCita) {
    Cita cita = buscarCitaPorID(idCita);
    if (cita.id == -1) {
        cout << "Cita no encontrada.\n";
        return false;
    }
    
    if (strcmp(cita.estado, "Cancelada") == 0) {
        cout << "La cita ya esta cancelada.\n";
        return false;
    }
    
    if (strcmp(cita.estado, "Atendida") == 0) {
        cout << "No se puede cancelar una cita ya atendida.\n";
        return false;
    }
    
    strcpy(cita.estado, "Cancelada");
    cita.atendida = false;
    cita.fechaModificacion = time(0);
    
    if (guardarCita(cita)) {
        cout << "Cita cancelada correctamente.\n";
        return true;
    } else {
        cout << "Error al cancelar la cita.\n";
        return false;
    }
}

// Reemplazar la función atenderCita existente con la versión mejorada
bool atenderCita(Hospital* h, int idCita, const char* diagnostico,
                 const char* tratamiento, const char* medicamentos) {
    return atenderCitaMejorada(h, idCita, diagnostico, tratamiento, medicamentos);
}

// =====================================================
// FUNCIONES DE BUSQUEDA (SIN MEMORIA DINAMICA)
// =====================================================

bool contieneCadena(const char* texto, const char* busqueda) {
    if (!texto || !busqueda || busqueda[0] == '\0') return false;
    
    char textoLower[256];
    char busquedaLower[256];
    
    // Convertir a minusculas
    int i;
    for (i = 0; texto[i] && i < 255; i++) {
        textoLower[i] = tolower(texto[i]);
    }
    textoLower[i] = '\0';
    
    for (i = 0; busqueda[i] && i < 255; i++) {
        busquedaLower[i] = tolower(busqueda[i]);
    }
    busquedaLower[i] = '\0';
    
    return strstr(textoLower, busquedaLower) != nullptr;
}

void buscarPacientesPorNombre(const char* nombreBuscado) {
    ArchivoHeader header = leerHeader("DATA/pacientes.bin");
    int contador = 0;
    
    cout << "\nResultados de busqueda:\n";
    cout << string(60, '-') << endl;
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Paciente p = leerPacientePorIndice(i);
        if (!p.eliminado && p.activo) {
            bool coincide = contieneCadena(p.nombre, nombreBuscado) ||
                           contieneCadena(p.apellido, nombreBuscado);
            
            if (!coincide) {
                char nombreCompleto[120];
                snprintf(nombreCompleto, sizeof(nombreCompleto), "%s %s", p.nombre, p.apellido);
                coincide = contieneCadena(nombreCompleto, nombreBuscado);
            }
            
            if (coincide && contador < MAX_RESULTADOS_BUSQUEDA) {
                cout << "ID: " << p.id << " - " << p.nombre << " " << p.apellido << "\n";
                cout << "  Cedula: " << p.cedula << "  Edad: " << p.edad << "  Sexo: " << p.sexo << "\n";
                cout << "  Telefono: " << p.telefono << "  Email: " << p.email << "\n";
                cout << string(60, '-') << endl;
                contador++;
            }
        }
    }
    
    if (contador == 0) {
        cout << "No se encontraron pacientes con ese nombre.\n";
    } else {
        cout << "Total encontrados: " << contador << "\n";
    }
}

bool contieneCadenaDoctor(const char* texto, const char* busqueda) {
    if (!texto || !busqueda || busqueda[0] == '\0') return false;
    
    char textoLower[256];
    char busquedaLower[256];
    
    // Convertir a minusculas
    int i;
    for (i = 0; texto[i] && i < 255; i++) {
        textoLower[i] = tolower(texto[i]);
    }
    textoLower[i] = '\0';
    
    for (i = 0; busqueda[i] && i < 255; i++) {
        busquedaLower[i] = tolower(busqueda[i]);
    }
    busquedaLower[i] = '\0';
    
    return strstr(textoLower, busquedaLower) != nullptr;
}

void buscarDoctoresPorNombre(const char* nombreBuscado) {
    ArchivoHeader header = leerHeader("DATA/doctores.bin");
    int contador = 0;
    
    cout << "\nResultados de busqueda:\n";
    cout << string(60, '-') << endl;
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Doctor d = leerDoctorPorIndice(i);
        if (!d.eliminado) {
            bool coincide = contieneCadenaDoctor(d.nombre, nombreBuscado) ||
                           contieneCadenaDoctor(d.apellido, nombreBuscado);
            
            if (!coincide) {
                char nombreCompleto[120];
                snprintf(nombreCompleto, sizeof(nombreCompleto), "%s %s", d.nombre, d.apellido);
                coincide = contieneCadenaDoctor(nombreCompleto, nombreBuscado);
            }
            
            if (coincide && contador < MAX_RESULTADOS_BUSQUEDA) {
                cout << "ID: " << d.id << " - " << d.nombre << " " << d.apellido << "\n";
                cout << "  Cedula: " << d.cedulaProfesional << "  Especialidad: " << d.especialidad << "\n";
                cout << "  Telefono: " << d.telefono << "  Email: " << d.email << "\n";
                cout << string(60, '-') << endl;
                contador++;
            }
        }
    }
    
    if (contador == 0) {
        cout << "No se encontraron doctores con ese nombre.\n";
    } else {
        cout << "Total encontrados: " << contador << "\n";
    }
}

void buscarDoctoresPorEspecialidad(const char* especialidad) {
    ArchivoHeader header = leerHeader("DATA/doctores.bin");
    int contador = 0;
    
    cout << "\nDoctores de " << especialidad << ":\n";
    cout << string(60, '-') << endl;
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Doctor d = leerDoctorPorIndice(i);
        if (!d.eliminado && strcasecmp(d.especialidad, especialidad) == 0 && contador < MAX_RESULTADOS_BUSQUEDA) {
            cout << "ID: " << d.id << " - " << d.nombre << " " << d.apellido << "\n";
            cout << "  Anos experiencia: " << d.aniosExperiencia << "  Costo: " << d.costoConsulta << "\n";
            cout << "  Telefono: " << d.telefono << "  Horario: " << d.horarioAtencion << "\n";
            cout << string(60, '-') << endl;
            contador++;
        }
    }
    
    if (contador == 0) {
        cout << "No se encontraron doctores con esa especialidad.\n";
    } else {
        cout << "Total encontrados: " << contador << "\n";
    }
}

void obtenerCitasDePaciente(int idPaciente) {
    ArchivoHeader header = leerHeader("DATA/citas.bin");
    int contador = 0;
    
    cout << "\nCitas del paciente:\n";
    cout << string(80, '-') << endl;
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Cita c = leerCitaPorIndice(i);
        if (!c.eliminado && c.pacienteID == idPaciente && contador < MAX_RESULTADOS_BUSQUEDA) {
            cout << "ID: " << c.id << " - Doctor: " << c.doctorID 
                 << " - " << c.fecha << " " << c.hora 
                 << " - " << c.estado << "\n";
            cout << "  Motivo: " << c.motivo << "\n";
            if (c.atendida) {
                cout << "  Consulta ID: " << c.consultaID << "\n";
            }
            cout << string(80, '-') << endl;
            contador++;
        }
    }
    
    if (contador == 0) {
        cout << "No hay citas para este paciente.\n";
    }
}

void obtenerCitasDeDoctor(int idDoctor) {
    ArchivoHeader header = leerHeader("DATA/citas.bin");
    int contador = 0;
    
    cout << "\nCitas del doctor:\n";
    cout << string(80, '-') << endl;
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Cita c = leerCitaPorIndice(i);
        if (!c.eliminado && c.doctorID == idDoctor && contador < MAX_RESULTADOS_BUSQUEDA) {
            Paciente p = buscarPacientePorID(c.pacienteID);
            char nombrePaciente[50] = "N/A";
            if (p.id != -1) {
                snprintf(nombrePaciente, sizeof(nombrePaciente), "%s %s", p.nombre, p.apellido);
            }
            
            cout << "ID: " << c.id << " - Paciente: " << nombrePaciente
                 << " - " << c.fecha << " " << c.hora 
                 << " - " << c.estado << "\n";
            cout << "  Motivo: " << c.motivo << "\n";
            cout << string(80, '-') << endl;
            contador++;
        }
    }
    
    if (contador == 0) {
        cout << "No hay citas para este doctor.\n";
    }
}

void obtenerCitasPorFecha(const char* fecha) {
    ArchivoHeader header = leerHeader("DATA/citas.bin");
    int contador = 0;
    
    cout << "\nCitas del " << fecha << ":\n";
    cout << string(80, '-') << endl;
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Cita c = leerCitaPorIndice(i);
        if (!c.eliminado && strcmp(c.fecha, fecha) == 0 && contador < MAX_RESULTADOS_BUSQUEDA) {
            Paciente p = buscarPacientePorID(c.pacienteID);
            Doctor d = buscarDoctorPorID(c.doctorID);
            
            char nombrePaciente[50] = "N/A";
            char nombreDoctor[50] = "N/A";
            
            if (p.id != -1) {
                snprintf(nombrePaciente, sizeof(nombrePaciente), "%s %s", p.nombre, p.apellido);
            }
            if (d.id != -1) {
                snprintf(nombreDoctor, sizeof(nombreDoctor), "%s %s", d.nombre, d.apellido);
            }
            
            cout << "ID: " << c.id << " - Paciente: " << nombrePaciente
                 << " - Doctor: " << nombreDoctor
                 << " - " << c.hora << " - " << c.estado << "\n";
            cout << "  Motivo: " << c.motivo << "\n";
            cout << string(80, '-') << endl;
            contador++;
        }
    }
    
    if (contador == 0) {
        cout << "No hay citas para esta fecha.\n";
    }
}

void listarCitasPendientes() {
    ArchivoHeader header = leerHeader("DATA/citas.bin");
    int contador = 0;
    
    cout << "\nCitas pendientes:\n";
    cout << left << setw(4) << "ID" << setw(10) << "PACIENTE" << setw(8) << "DOCTOR" 
         << setw(12) << "FECHA" << setw(6) << "HORA" << setw(20) << "MOTIVO" << setw(12) << "ESTADO" << "\n";
    cout << string(80, '-') << endl;
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Cita cita = leerCitaPorIndice(i);
        if (!cita.eliminado && strcmp(cita.estado, "Agendada") == 0 && contador < MAX_RESULTADOS_BUSQUEDA) {
            Paciente p = buscarPacientePorID(cita.pacienteID);
            char nombrePaciente[30] = "N/A";
            if (p.id != -1) {
                snprintf(nombrePaciente, sizeof(nombrePaciente), "%s %s", p.nombre, p.apellido);
            }
            
            cout << left << setw(4) << cita.id
                 << setw(10) << nombrePaciente
                 << setw(8) << cita.doctorID
                 << setw(12) << cita.fecha
                 << setw(6) << cita.hora
                 << setw(20) << cita.motivo
                 << setw(12) << cita.estado << "\n";
            contador++;
        }
    }
    
    if (contador == 0) {
        cout << "No hay citas pendientes.\n";
    } else {
        cout << "\nTotal de citas pendientes: " << contador << "\n";
    }
}

// =====================================================
// FUNCIONES DE GESTION DE HISTORIAL MEDICO
// =====================================================

void mostrarHistorialPaciente(int pacienteID) {
    Paciente paciente = buscarPacientePorID(pacienteID);
    if (paciente.id == -1) {
        cout << "Paciente no encontrado.\n";
        return;
    }

    if (paciente.primerConsultaID == -1) {
    cout << "El paciente no tiene historial medico.\n";
        return;
    }

    cout << "\n--- Historial Medico del Paciente ---\n";
    cout << "Paciente: " << paciente.nombre << " " << paciente.apellido << "\n";
    cout << "Total de consultas: " << paciente.cantidadConsultas << "\n";
    cout << string(80, '-') << endl;

    int consultaID = paciente.primerConsultaID;
    int contador = 0;
    
    while (consultaID != -1 && contador < MAX_RESULTADOS_BUSQUEDA) {
        HistorialMedico consulta = buscarHistorialPorID(consultaID);
        if (consulta.id == -1) {
            break;
        }

        Doctor doctor = buscarDoctorPorID(consulta.doctorID);
        char nombreDoctor[50] = "N/A";
        if (doctor.id != -1) {
            snprintf(nombreDoctor, sizeof(nombreDoctor), "Dr. %s %s", doctor.nombre, doctor.apellido);
        }

        cout << "Consulta #" << (contador + 1) << " - ID: " << consulta.id << "\n";
        cout << "Fecha: " << consulta.fecha << " " << consulta.hora << "\n";
        cout << "Doctor: " << nombreDoctor << "\n";
        cout << "Diagnostico: " << consulta.diagnostico << "\n";
        cout << "Tratamiento: " << consulta.tratamiento << "\n";
        cout << "Medicamentos: " << consulta.medicamentos << "\n";
        cout << "Costo: $" << fixed << setprecision(2) << consulta.costo << "\n";
        cout << string(80, '-') << endl;

        consultaID = consulta.siguienteConsultaID;
        contador++;
    }

    if (contador == 0) {
        cout << "No se pudieron cargar las consultas del historial.\n";
    }
}

// =====================================================
// FUNCIONES DE VALIDACION Y UTILIDAD
// =====================================================

void limpiarBufferEntrada() {
    cin.ignore(256, '\n');
}

void leerLinea(char* buffer, int size) {
    cin.getline(buffer, size);
    if (cin.fail()) {
        cin.clear();
        limpiarBufferEntrada();
    }
    buffer[size-1] = '\0';
}

bool validarCedula(const char* cedula) {
    if (!cedula) return false;
    int len = (int)strlen(cedula);
    if (len == 0 || len > 19) return false;
    if (cedula[1] != '-') return false;
    for (int i = 2; i < len; i++) {
        if (!isdigit(cedula[i])) return false;
    }
    if (cedula[0] != 'V' && cedula[0] != 'v' && cedula[0] != 'E' && cedula[0] != 'e') return false;
    return true;
}

bool validarEmail(const char* email) {
    if (!email || strlen(email) == 0) return false;
    if (strlen(email) > 50) return false;
    const char* at = strchr(email, '@');
    if (!at) return false;
    if (strchr(at + 1, '@')) return false;
    if (at == email) return false;
    if (*(at + 1) == '\0') return false;
    const char* dot = strchr(at, '.');
    if (!dot) return false;
    if (dot == at + 1) return false;
    const char* lastDot = strrchr(email, '.');
    if (!lastDot || strlen(lastDot) <= 1) return false;
    if (strlen(lastDot) < 3) return false;
    for (const char* p = email; p < at; p++) {
        if (!isalnum(*p) && *p != '.' && *p != '_' && *p != '-' && *p != '+') {
            return false;
        }
    }
    if (email[0] == '.' || *(at - 1) == '.') return false;
    for (const char* p = at + 1; *p; p++) {
        if (!isalnum(*p) && *p != '.' && *p != '-') {
            return false;
        }
    }
    for (const char* p = email; *p && *(p + 1); p++) {
        if (*p == '.' && *(p + 1) == '.') return false;
    }
    return true;
}

bool validarFecha(const char* fecha) {
    if (!fecha) return false;
    if (strlen(fecha) != 10) return false;
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            if (fecha[i] != '-') return false;
        } else {
            if (fecha[i] < '0' || fecha[i] > '9') return false;
        }
    }
    int m = (fecha[5]-'0')*10 + (fecha[6]-'0');
    int d = (fecha[8]-'0')*10 + (fecha[9]-'0');
    if (m < 1 || m > 12) return false;
    int diasMes = 31;
    if (m==4 || m==6 || m==9 || m==11) diasMes = 30;
    else if (m == 2) diasMes = 28;
    if (d < 1 || d > diasMes) return false;
    return true;
}

bool validarHora(const char* hora) {
    if (!hora) return false;
    if (strlen(hora) != 5) return false;
    if (hora[2] != ':') return false;
    for (int i = 0; i < 5; i++) {
        if (i == 2) continue;
        if (hora[i] < '0' || hora[i] > '9') return false;
    }
    int hh = (hora[0]-'0')*10 + (hora[1]-'0');
    int mm = (hora[3]-'0')*10 + (hora[4]-'0');
    if (hh < 0 || hh > 23) return false;
    if (mm < 0 || mm > 59) return false;
    return true;
}

int leerEntero() {
    int x;
    if (!(cin >> x)) {
        cin.clear();
        limpiarBufferEntrada();
        return -1;
    }
    return x;
}

// =====================================================
// FUNCIONES DE GESTION (SIN MEMORIA DINAMICA)
// =====================================================

bool crearPaciente(Hospital* h, const char* nombre, const char* apellido, const char* cedula,
                   int edad, char sexo, const char* tipoSangre, const char* telefono,
                   const char* direccion, const char* email, const char* alergias, const char* observaciones) {
    
    if (!validarCedula(cedula)) {
        cout << "Error: Cedula invalida. Formato: V-12345678\n";
        return false;
    }
    if (edad < 0 || edad > 120) {
        cout << "Error: Edad debe estar entre 0 y 120 anios.\n";
        return false;
    }
    if (!(sexo=='M' || sexo == 'm' || sexo=='F' || sexo=='f')) {
        cout << "Error: Sexo debe ser M o F.\n";
        return false;
    }

    // Verificar cedula unica
    Paciente existente = buscarPacientePorCedula(cedula);
    if (existente.id != -1) {
        cout << "Error: La cedula ya esta registrada.\n";
        return false;
    }

    if (email && email[0] != '\0') {
        if (!validarEmail(email)) {
            cout << "Error: Email invalido. Formato esperado: usuario@dominio.ext\n";
            return false;
        }
    }

    // Crear nuevo paciente
    Paciente nuevoPaciente;
    memset(&nuevoPaciente, 0, sizeof(Paciente));
    
    strncpy(nuevoPaciente.nombre, nombre, sizeof(nuevoPaciente.nombre)-1);
    strncpy(nuevoPaciente.apellido, apellido, sizeof(nuevoPaciente.apellido)-1);
    strncpy(nuevoPaciente.cedula, cedula, sizeof(nuevoPaciente.cedula)-1);
    nuevoPaciente.edad = edad;
    nuevoPaciente.sexo = toupper(sexo);
    nuevoPaciente.activo = true;
    
    if (tipoSangre && tipoSangre[0] != '\0') {
        strncpy(nuevoPaciente.tipoSangre, tipoSangre, sizeof(nuevoPaciente.tipoSangre)-1);
    }
    if (telefono && telefono[0] != '\0') {
        strncpy(nuevoPaciente.telefono, telefono, sizeof(nuevoPaciente.telefono)-1);
    }
    if (direccion && direccion[0] != '\0') {
        strncpy(nuevoPaciente.direccion, direccion, sizeof(nuevoPaciente.direccion)-1);
    }
    if (email && email[0] != '\0') {
        strncpy(nuevoPaciente.email, email, sizeof(nuevoPaciente.email)-1);
    }
    if (alergias && alergias[0] != '\0') {
        strncpy(nuevoPaciente.alergias, alergias, sizeof(nuevoPaciente.alergias)-1);
    }
    if (observaciones && observaciones[0] != '\0') {
        strncpy(nuevoPaciente.observaciones, observaciones, sizeof(nuevoPaciente.observaciones)-1);
    }
    
    nuevoPaciente.cantidadConsultas = 0;
    nuevoPaciente.primerConsultaID = -1;
    nuevoPaciente.cantidadCitas = 0;
    
    if (guardarPaciente(nuevoPaciente)) {
        h->totalPacientesRegistrados++;
        guardarDatosHospital(h);
        
        cout << "Paciente creado exitosamente. ID: " << nuevoPaciente.id << "\n";
        return true;
    } else {
        cout << "Error: No se pudo guardar el paciente.\n";
        return false;
    }
}

bool crearDoctor(Hospital* h, const char* nombre, const char* apellido, const char* cedula,
                 const char* especialidad, int anios, float costo) {
    
    if (!validarCedula(cedula)) {
        cout << "Error: Cedula profesional invalida.\n";
        return false;
    }
    if (anios < 0) {
        cout << "Error: Anios de experiencia no pueden ser negativos.\n";
        return false;
    }
    if (costo <= 0.0f) {
        cout << "Error: Costo de consulta debe ser mayor a 0.\n";
        return false;
    }

    // Crear nuevo doctor
    Doctor nuevoDoctor;
    memset(&nuevoDoctor, 0, sizeof(Doctor));
    
    strncpy(nuevoDoctor.nombre, nombre, sizeof(nuevoDoctor.nombre)-1);
    strncpy(nuevoDoctor.apellido, apellido, sizeof(nuevoDoctor.apellido)-1);
    strncpy(nuevoDoctor.cedulaProfesional, cedula, sizeof(nuevoDoctor.cedulaProfesional)-1);
    strncpy(nuevoDoctor.especialidad, especialidad, sizeof(nuevoDoctor.especialidad)-1);
    nuevoDoctor.aniosExperiencia = anios;
    nuevoDoctor.costoConsulta = costo;
    nuevoDoctor.disponible = true;
    nuevoDoctor.cantidadPacientes = 0;
    nuevoDoctor.cantidadCitas = 0;
    
    if (guardarDoctor(nuevoDoctor)) {
        h->totalDoctoresRegistrados++;
        guardarDatosHospital(h);
        
        cout << "Doctor creado exitosamente. ID: " << nuevoDoctor.id << "\n";
        return true;
    } else {
        cout << "Error: No se pudo guardar el doctor.\n";
        return false;
    }
}

// =====================================================
// FUNCIONES DE VISUALIZACION
// =====================================================

void mostrarDatosHospital(Hospital* h) {
    system("pause");
    system("cls");
    cout << "\nDatos del Hospital:\n";
    cout << "Nombre: " << h->nombre << "\n";
    cout << "Direccion: " << h->direccion << "\n";
    cout << "Telefono: " << h->telefono << "\n";
    cout << "Total Pacientes Registrados: " << h->totalPacientesRegistrados << "\n";
    cout << "Total Doctores Registrados: " << h->totalDoctoresRegistrados << "\n";
    cout << "Total Citas Agendadas: " << h->totalCitasAgendadas << "\n";
    cout << "Total Consultas Realizadas: " << h->totalConsultasRealizadas << "\n";
    system("pause");
    system("cls");
}

void listarPacientes(Hospital* h) {
    ArchivoHeader header = leerHeader("DATA/pacientes.bin");
    
    if (header.registrosActivos == 0) {
        cout << "No hay pacientes registrados.\n";
        return;
    }
    
    cout << "\n" << left << setw(4) << "ID" << setw(26) << "NOMBRE" << setw(16) << "CEDULA" << setw(6) << "EDAD"
         << setw(6) << "TS" << setw(16) << "TELEFONO" << setw(30) << "EMAIL" << "\n";
    cout << string(100, '_') << endl;
    
    int contador = 0;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Paciente p = leerPacientePorIndice(i);
        if (!p.eliminado && p.activo && contador < MAX_RESULTADOS_BUSQUEDA) {
            char full[120];
            full[0] = '\0';
            strncat(full, p.nombre, sizeof(full)-1);
            strncat(full, " ", sizeof(full)-1);
            strncat(full, p.apellido, sizeof(full)-1);
            
            cout << left << setw(4) << p.id
                 << setw(26) << full
                 << setw(16) << p.cedula
                 << setw(6) << p.edad
                 << setw(6) << p.tipoSangre
                 << setw(16) << p.telefono
                 << setw(30) << p.email << "\n";
            contador++;
        }
    }
    
    cout << "\nTotal de pacientes activos: " << contador << "\n";
}

void listarDoctores(Hospital* h) {
    ArchivoHeader header = leerHeader("DATA/doctores.bin");
    
    if (header.registrosActivos == 0) {
        cout << "No hay doctores registrados.\n";
        return;
    }
    
    cout << "\n" << left << setw(4) << "ID" << setw(26) << "NOMBRE" << setw(16) << "CEDULA"
         << setw(20) << "ESPECIALIDAD" << setw(6) << "ANIOS" << setw(8) << "COSTO"
         << setw(8) << "DISP" << "\n";
    cout << string(100, '-') << endl;
    
    int contador = 0;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Doctor d = leerDoctorPorIndice(i);
        if (!d.eliminado && contador < MAX_RESULTADOS_BUSQUEDA) {
            char full[200];
            full[0] = '\0';
            strncat(full, d.nombre, sizeof(full)-1);
            strncat(full, " ", sizeof(full)-1);
            strncat(full, d.apellido, sizeof(full)-1);
            
            cout << left << setw(4) << d.id
                 << setw(26) << full
                 << setw(16) << d.cedulaProfesional
                 << setw(20) << d.especialidad
                 << setw(6) << d.aniosExperiencia
                 << setw(8) << fixed << setprecision(2) << d.costoConsulta
                 << setw(8) << (d.disponible ? "Si" : "No") << "\n";
            contador++;
        }
    }
    
    cout << "\nTotal de doctores activos: " << contador << "\n";
}

// =====================================================
// FUNCION ACTUALIZAR PACIENTE (SISTEMA ARCHIVOS)
// =====================================================

bool actualizarPaciente(Hospital* h, int id) {
    Paciente p = buscarPacientePorID(id);
    if (p.id == -1) {
        cout << "Paciente no encontrado.\n";
        return false;
    }

    char buffer[256];
    cout << "Dejar vacio para no cambiar.\n";
    cout << "Tipo de sangre registrado: " << p.tipoSangre << "\nTipo de sangre a registrar: ";
    limpiarBufferEntrada();
    leerLinea(buffer, 5);
    if (strlen(buffer) > 0) {
        strncpy(p.tipoSangre, buffer, sizeof(p.tipoSangre)-1);
        p.tipoSangre[sizeof(p.tipoSangre)-1] = '\0';
    }
    cout << "Sexo registrado: " << p.sexo << "\nSexo a registrar (M/F): ";
   leerLinea(buffer, 2);
        if (strlen(buffer) > 0) {
        p.sexo = toupper(buffer[0]); // Asegurar que sea mayuscula
    }

    cout << "Telefono actual: " << p.telefono << "\nNuevo telefono: ";
    leerLinea(buffer, 15);
    if (strlen(buffer) > 0) {
        strncpy(p.telefono, buffer, sizeof(p.telefono)-1);
        p.telefono[sizeof(p.telefono)-1] = '\0';
    }

    cout << "Direccion actual: " << p.direccion << "\nNueva direccion: ";
    leerLinea(buffer, 100);
    if (strlen(buffer) > 0) {
        strncpy(p.direccion, buffer, sizeof(p.direccion)-1);
        p.direccion[sizeof(p.direccion)-1] = '\0';
    }

    cout << "Email actual: " << p.email << "\nNuevo email: ";
    leerLinea(buffer, 50);
    if (strlen(buffer) > 0) {
        if (validarEmail(buffer)) {
            strncpy(p.email, buffer, sizeof(p.email)-1);
            p.email[sizeof(p.email)-1] = '\0';
            cout << "Email actualizado correctamente.\n";
        } else {
            cout << "Error: Email invalido. Formato esperado: usuario@dominio.ext\n";
            cout << "El email no se actualizo.\n";
        }
    }

    cout << "Alergias actuales: " << p.alergias << "\nNuevas alergias: ";
    leerLinea(buffer, 500);
    if (strlen(buffer) > 0) {
        strncpy(p.alergias, buffer, sizeof(p.alergias)-1);
        p.alergias[sizeof(p.alergias)-1] = '\0';
    }

    cout << "Observaciones actuales: " << p.observaciones << "\nNuevas observaciones: ";
    leerLinea(buffer, 500);
    if (strlen(buffer) > 0) {
        strncpy(p.observaciones, buffer, sizeof(p.observaciones)-1);
        p.observaciones[sizeof(p.observaciones)-1] = '\0';
    }

    // Actualizar fecha de modificacion
    p.fechaModificacion = time(0);

    if (guardarPaciente(p)) {
        cout << "Paciente actualizado correctamente.\n";
        return true;
    } else {
        cout << "Error al guardar los cambios.\n";
        return false;
    }
}

// =====================================================
// FUNCIONES COMPLETAS PARA DOCTORES (CONTINUACION)
// =====================================================

bool actualizarDoctor(Hospital* h, int id) {
    Doctor d = buscarDoctorPorID(id);
    if (d.id == -1) {
        cout << "Doctor no encontrado.\n";
        return false;
    }

    char buffer[256];
    cout << "Dejar vacio para no cambiar.\n";

    cout << "Telefono actual: " << d.telefono << "\nNuevo telefono: ";
    limpiarBufferEntrada();
    leerLinea(buffer, 15);
    if (strlen(buffer) > 0) {
        strncpy(d.telefono, buffer, sizeof(d.telefono)-1);
        d.telefono[sizeof(d.telefono)-1] = '\0';
    }

    cout << "Email actual: " << d.email << "\nNuevo email: ";
    leerLinea(buffer, 50);
    if (strlen(buffer) > 0) {
        if (validarEmail(buffer)) {
            strncpy(d.email, buffer, sizeof(d.email)-1);
            d.email[sizeof(d.email)-1] = '\0';
            cout << "Email actualizado correctamente.\n";
        } else {
            cout << "Error: Email invalido. Formato esperado: usuario@dominio.ext\n";
            cout << "El email no se actualizo.\n";
        }
    }

    cout << "Horario actual: " << d.horarioAtencion << "\nNuevo horario: ";
    leerLinea(buffer, 50);
    if (strlen(buffer) > 0) {
        strncpy(d.horarioAtencion, buffer, sizeof(d.horarioAtencion)-1);
        d.horarioAtencion[sizeof(d.horarioAtencion)-1] = '\0';
    }

    cout << "Costo consulta actual: " << d.costoConsulta << "\nNuevo costo: ";
    float nuevoCosto;
    if (cin >> nuevoCosto) {
        if (nuevoCosto > 0) {
            d.costoConsulta = nuevoCosto;
        } else {
            cout << "Error: El costo debe ser mayor a 0.\n";
        }
    }
    limpiarBufferEntrada();

    cout << "Disponible actual: " << (d.disponible ? "Si" : "No") << "\nNuevo estado (1=Si, 0=No): ";
    char estado[2];
    leerLinea(estado, 2);
    if (strlen(estado) > 0) {
        d.disponible = (estado[0] == '1');
    }

    // Actualizar fecha de modificacion
    d.fechaModificacion = time(0);

    if (guardarDoctor(d)) {
        cout << "Doctor actualizado correctamente.\n";
        return true;
    } else {
        cout << "Error al guardar los cambios.\n";
        return false;
    }
}

bool asignarPacienteADoctor(int idDoctor, int idPaciente) {
    Doctor d = buscarDoctorPorID(idDoctor);
    if (d.id == -1) {
        cout << "Doctor no encontrado.\n";
        return false;
    }
    Paciente p = buscarPacientePorID(idPaciente);
    if (p.id == -1) {
        cout << "Paciente no encontrado.\n";
        return false;
    }

    // Verificar si ya esta asignado
    for (int i = 0; i < d.cantidadPacientes; i++) {
        if (d.pacientesIDs[i] == idPaciente) {
            cout << "El paciente ya esta asignado a este doctor.\n";
            return false;
        }
    }

    // Verificar capacidad
    if (d.cantidadPacientes >= 50) {
        cout << "Error: El doctor ya tiene el maximo de pacientes asignados (50).\n";
        return false;
    }

    // Asignar paciente
    d.pacientesIDs[d.cantidadPacientes] = idPaciente;
    d.cantidadPacientes++;
    d.fechaModificacion = time(0);

    if (guardarDoctor(d)) {
        cout << "Paciente asignado correctamente al doctor.\n";
        return true;
    } else {
        cout << "Error al guardar la asignacion.\n";
        return false;
    }
}

bool removerPacienteDeDoctor(int idDoctor, int idPaciente) {
    Doctor d = buscarDoctorPorID(idDoctor);
    if (d.id == -1) {
        cout << "Doctor no encontrado.\n";
        return false;
    }

    bool encontrado = false;
    for (int i = 0; i < d.cantidadPacientes; i++) {
        if (d.pacientesIDs[i] == idPaciente) {
            // Mover los elementos restantes
            for (int j = i; j < d.cantidadPacientes - 1; j++) {
                d.pacientesIDs[j] = d.pacientesIDs[j + 1];
            }
            d.cantidadPacientes--;
            encontrado = true;
            break;
        }
    }

    if (!encontrado) {
        cout << "Paciente no encontrado en la lista del doctor.\n";
        return false;
    }

    d.fechaModificacion = time(0);

    if (guardarDoctor(d)) {
        cout << "Paciente removido correctamente del doctor.\n";
        return true;
    } else {
        cout << "Error al guardar los cambios.\n";
        return false;
    }
}

bool eliminarDoctorLogico(int id) {
    Doctor d = buscarDoctorPorID(id);
    if (d.id == -1) return false;
    
    d.eliminado = true;
    d.disponible = false;
    d.fechaModificacion = time(0);
    
    return guardarDoctor(d);
}

void listarPacientesDeDoctor(int idDoctor) {
    Doctor d = buscarDoctorPorID(idDoctor);
    if (d.id == -1) {
        cout << "Doctor no encontrado.\n";
        return;
    }

    if (d.cantidadPacientes == 0) {
        cout << "El doctor no tiene pacientes asignados.\n";
        return;
    }

    cout << "\nPacientes asignados al Dr. " << d.nombre << " " << d.apellido << ":\n";
    cout << string(60, '-') << endl;

    for (int i = 0; i < d.cantidadPacientes && i < MAX_RESULTADOS_BUSQUEDA; i++) {
        Paciente p = buscarPacientePorID(d.pacientesIDs[i]);
        if (p.id != -1 && !p.eliminado) {
            cout << "ID: " << p.id << " - " << p.nombre << " " << p.apellido << "\n";
            cout << "  Cedula: " << p.cedula << "  Edad: " << p.edad << "  Sexo: " << p.sexo << "\n";
            cout << "  Telefono: " << p.telefono << "  Email: " << p.email << "\n";
            cout << string(60, '-') << endl;
        }
    }
}

// =====================================================
// MENUS DEL SISTEMA
// =====================================================

void mostrarMenuPrincipal() {
    cout << "\n----------------------------------------\n";
    cout << "|   SISTEMA DE GESTION HOSPITALARIO   |\n";
    cout << "|         (Sistema de Archivos)       |\n";
    cout << "----------------------------------------\n";
    cout << "1. Gestion de Pacientes\n";
    cout << "2. Gestion de Doctores\n";
    cout << "3. Gestion de Citas\n";
    cout << "4. Mostrar Datos del Hospital\n";
    cout << "5. Verificar Sistema de Archivos\n";
    cout << "6. Mantenimiento del Sistema\n";
    cout << "0. Salir\n";
    cout << "Elija una opcion: ";
}

void menuPacientes(Hospital* h) {
    int op = -1;
    do {
        system("pause");
        system("cls");
        cout << "\n--- Menu Pacientes (Sistema Archivos) ---\n";
        cout << "1. Registrar paciente\n";
        cout << "2. Buscar paciente por cedula\n";
        cout << "3. Buscar paciente por ID\n";
        cout << "4. Buscar pacientes por nombre\n";
        cout << "5. Listar todos los pacientes\n";
        cout << "6. Actualizar paciente\n";
        cout << "7. Eliminar paciente\n";
        cout << "8. Ver historial medico\n";
        cout << "0. Volver\n";
        cout << "Elija: ";
        op = leerEntero();
        
        if (op == -1) { 
            cout << "Entrada invalida.\n"; 
            continue; 
        }
        
        switch(op) {
            case 1: {
                char nombre[50], apellido[50], cedula[20]; 
                int edad; 
                char sexo;
                cout << "Nombre: "; 
                limpiarBufferEntrada(); 
                leerLinea(nombre, 50);
                cout << "Apellido: "; 
                leerLinea(apellido, 50);
                cout << "Cedula (Formato V-00000000): "; 
                leerLinea(cedula, 20);
                cout << "Edad: "; 
                edad = leerEntero();
                if (edad == -1) {
                    cout << "Edad invalida.\n";
                    break;
                }
                cout << "Sexo (M/F): "; 
                cin >> sexo;
                cout << "Tipo sangre: "; 
                limpiarBufferEntrada(); 
                char tipoSangre[5]; 
                leerLinea(tipoSangre, 5);
                cout << "Telefono: "; 
                char telefono[15]; 
                leerLinea(telefono, 15);
                cout << "Direccion: "; 
                char direccion[100]; 
                leerLinea(direccion, 100);
                cout << "Email: "; 
                char email[50]; 
                leerLinea(email, 50);
                cout << "Alergias: ";
                char alergias[500];
                leerLinea(alergias, 500);
                cout << "Observaciones: ";
                char observaciones[500];
                leerLinea(observaciones, 500);
                
                crearPaciente(h, nombre, apellido, cedula, edad, sexo, 
                              tipoSangre, telefono, direccion, email, 
                              alergias, observaciones);
                break;
            }
            case 2: {
                char ced[20]; 
                cout << "Cedula: "; 
                limpiarBufferEntrada(); 
                leerLinea(ced, 20);
                Paciente p = buscarPacientePorCedula(ced);
                if (p.id != -1) {
                    cout << "Encontrado: " << p.nombre << " " << p.apellido << " (ID " << p.id << ")\n";
                    cout << "Cedula: " << p.cedula << "\n";
                    cout << "Edad: " << p.edad << "  Sexo: " << p.sexo << "\n";
                    cout << "Telefono: " << p.telefono << "\n";
                    cout << "Direccion: " << p.direccion << "\n";
                    cout << "Email: " << p.email << "\n";
                    cout << "Alergias: " << p.alergias << "\n";
                    cout << "Observaciones: " << p.observaciones << "\n";
                } else {
                    cout << "No encontrado.\n";
                }
                break;
            }
            case 3: {
                int id; 
                cout << "ID paciente: "; 
                id = leerEntero();
                Paciente p = buscarPacientePorID(id);
                if (p.id != -1) {
                    cout << "ID: " << p.id << "\n";
                    cout << "Nombre: " << p.nombre << " " << p.apellido << "\n";
                    cout << "Cedula: " << p.cedula << "\n";
                    cout << "Edad: " << p.edad << "  Sexo: " << p.sexo << "\n";
                    cout << "Tipo sangre: " << p.tipoSangre << "\n";
                    cout << "Telefono: " << p.telefono << "\n";
                    cout << "Direccion: " << p.direccion << "\n";
                    cout << "Email: " << p.email << "\n";
                    cout << "Consultas: " << p.cantidadConsultas << "\n";
                    cout << "Alergias: " << p.alergias << "\n";
                    cout << "Observaciones: " << p.observaciones << "\n";
                    cout << "Activo: " << (p.activo ? "Si" : "No") << "\n";
                } else {
                    cout << "Paciente no encontrado.\n";
                }
                break;
            }
            case 4: {
                char nombre[50]; 
                cout << "Nombre o apellido a buscar: "; 
                limpiarBufferEntrada(); 
                leerLinea(nombre, 50);
                buscarPacientesPorNombre(nombre);
                break;
            }
            case 5: {
                listarPacientes(h);
                break;
            }
            case 6: {
                int id; 
                cout << "ID del paciente a actualizar: "; 
                id = leerEntero();
                if (id <= 0) {
                    cout << "ID invalido.\n";
                    break;
                }
                if (!actualizarPaciente(h, id)) {
                    cout << "No se pudo actualizar el paciente.\n";
                }
                break;
            }
            case 7: {
                int id; 
                cout << "ID a eliminar: "; 
                id = leerEntero();
                if (eliminarPacienteLogico(id)) {
                    cout << "Paciente eliminado correctamente.\n";
                    h->totalPacientesRegistrados--;
                    guardarDatosHospital(h);
                } else {
                    cout << "No se pudo eliminar el paciente.\n";
                }
                break;
            }
            case 8: {
                int id;
                cout << "ID del paciente: ";
                id = leerEntero();
                mostrarHistorialPaciente(id);
                break;
            }
            case 0: {
                system("cls");
                break;
            }
            default: {
                cout << "Opcion invalida.\n";
                break;
            }
        }
    } while (op != 0);
}

void menuDoctores(Hospital* h) {
    int op = -1;
    do {
        system("pause");
        system("cls");
        cout << "\n--- Menu Doctores (Sistema Archivos) ---\n";
        cout << "1. Registrar doctor\n";
        cout << "2. Buscar doctor por ID\n";
        cout << "3. Buscar doctor por cedula\n";
        cout << "4. Buscar doctores por nombre\n";
        cout << "5. Buscar doctores por especialidad\n";
        cout << "6. Actualizar doctor\n";
        cout << "7. Asignar paciente a doctor\n";
        cout << "8. Remover paciente de doctor\n";
        cout << "9. Listar pacientes de doctor\n";
        cout << "10. Listar todos los doctores\n";
        cout << "11. Eliminar doctor\n";
        cout << "0. Volver\n";
        cout << "Elija: ";
        op = leerEntero();
        
        if (op == -1) { 
            cout << "Entrada invalida.\n"; 
            continue; 
        }
        
        switch(op) {
            case 1: {
                char nombre[50], apellido[50], cedula[20], especialidad[50];
                int anios;
                float costo;
                
                cout << "Nombre: ";
                limpiarBufferEntrada();
                leerLinea(nombre, 50);
                cout << "Apellido: ";
                leerLinea(apellido, 50);
                cout << "Cedula profesional: ";
                leerLinea(cedula, 20);
                cout << "Especialidad: ";
                leerLinea(especialidad, 50);
                cout << "Anos de experiencia: ";
                anios = leerEntero();
                cout << "Costo de consulta: ";
                cin >> costo;
                limpiarBufferEntrada();
                
                crearDoctor(h, nombre, apellido, cedula, especialidad, anios, costo);
                break;
            }
            case 2: {
                int id;
                cout << "ID del doctor: ";
                id = leerEntero();
                
                Doctor d = buscarDoctorPorID(id);
                if (d.id != -1) {
                    cout << "ID: " << d.id << " - " << d.nombre << " " << d.apellido << "\n";
                    cout << "  Cedula: " << d.cedulaProfesional << "  Especialidad: " << d.especialidad << "\n";
                    cout << "  Anos experiencia: " << d.aniosExperiencia << "  Costo: " << d.costoConsulta << "\n";
                    cout << "  Telefono: " << d.telefono << "  Email: " << d.email << "\n";
                    cout << "  Horario: " << d.horarioAtencion << "  Disponible: " << (d.disponible ? "Si" : "No") << "\n";
                } else {
                    cout << "Doctor no encontrado.\n";
                }
                break;
            }
            case 3: {
                char cedula[20];
                cout << "Cedula profesional: ";
                limpiarBufferEntrada();
                leerLinea(cedula, 20);
                
                ArchivoHeader header = leerHeader("DATA/doctores.bin");
                bool encontrado = false;
                for (int i = 0; i < header.cantidadRegistros && !encontrado; i++) {
                    Doctor d = leerDoctorPorIndice(i);
                    if (!d.eliminado && strcmp(d.cedulaProfesional, cedula) == 0) {
                        cout << "ID: " << d.id << " - " << d.nombre << " " << d.apellido << "\n";
                        cout << "  Especialidad: " << d.especialidad << "  Anos: " << d.aniosExperiencia << "\n";
                        cout << "  Telefono: " << d.telefono << "  Email: " << d.email << "\n";
                        encontrado = true;
                    }
                }
                if (!encontrado) {
                    cout << "Doctor no encontrado.\n";
                }
                break;
            }
            case 4: {
                char nombre[50];
                cout << "Nombre o apellido a buscar: ";
                limpiarBufferEntrada();
                leerLinea(nombre, 50);
                buscarDoctoresPorNombre(nombre);
                break;
            }
            case 5: {
                char especialidad[50];
                cout << "Especialidad a buscar: ";
                limpiarBufferEntrada();
                leerLinea(especialidad, 50);
                buscarDoctoresPorEspecialidad(especialidad);
                break;
            }
            case 6: {
                int id;
                cout << "ID del doctor a actualizar: ";
                id = leerEntero();
                if (id <= 0) {
                    cout << "ID invalido.\n";
                    break;
                }
                if (!actualizarDoctor(h, id)) {
                    cout << "No se pudo actualizar el doctor.\n";
                }
                break;
            }
            case 7: {
                int idDoctor, idPaciente;
                cout << "ID del doctor: ";
                idDoctor = leerEntero();
                cout << "ID del paciente: ";
                idPaciente = leerEntero();
                
                if (asignarPacienteADoctor(idDoctor, idPaciente)) {
                    cout << "Asignacion exitosa.\n";
                } else {
                    cout << "Error en la asignacion.\n";
                }
                break;
            }
            case 8: {
                int idDoctor, idPaciente;
                cout << "ID del doctor: ";
                idDoctor = leerEntero();
                cout << "ID del paciente: ";
                idPaciente = leerEntero();
                
                if (removerPacienteDeDoctor(idDoctor, idPaciente)) {
                    cout << "Paciente removido exitosamente.\n";
                } else {
                    cout << "Error al remover el paciente.\n";
                }
                break;
            }
            case 9: {
                int idDoctor;
                cout << "ID del doctor: ";
                idDoctor = leerEntero();
                listarPacientesDeDoctor(idDoctor);
                break;
            }
            case 10: {
                listarDoctores(h);
                break;
            }
            case 11: {
                int id;
                cout << "ID del doctor a eliminar: ";
                id = leerEntero();
                if (eliminarDoctorLogico(id)) {
                    cout << "Doctor eliminado correctamente.\n";
                    h->totalDoctoresRegistrados--;
                    guardarDatosHospital(h);
                } else {
                    cout << "No se pudo eliminar el doctor.\n";
                }
                break;
            }
            case 0: {
                system("cls");
                break;
            }
            default: {
                cout << "Opcion invalida.\n";
                break;
            }
        }
    } while (op != 0);
}
void mostrarDirectorioActual() {
    char buffer[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, buffer);
    cout << " Directorio actual: " << buffer << "\n";
}
void menuCitas(Hospital* h) {
    int op = -1;
    do {
        system("pause");
        system("cls");
        cout << "\n--- Menu Citas (Sistema Archivos) ---\n";
        cout << "1. Agendar cita\n";
        cout << "2. Cancelar cita\n";
        cout << "3. Atender cita\n";
        cout << "4. Ver citas de paciente\n";
        cout << "5. Ver citas de doctor\n";
        cout << "6. Ver citas por fecha\n";
        cout << "7. Ver citas pendientes\n";
        cout << "8. Buscar cita por ID\n";
        cout << "0. Volver\n";
        cout << "Elija: ";
        op = leerEntero();
        
        if (op == -1) { 
            cout << "Entrada invalida.\n"; 
            continue; 
        }
        
        switch(op) {
            case 1: {
                int idPac, idDoc;
                char fecha[11], hora[6], motivo[150];
                
                cout << "ID del paciente: ";
                idPac = leerEntero();
                cout << "ID del doctor: ";
                idDoc = leerEntero();
                cout << "Fecha (YYYY-MM-DD): ";
                limpiarBufferEntrada();
                leerLinea(fecha, 11);
                cout << "Hora (HH:MM): ";
                leerLinea(hora, 6);
                cout << "Motivo de la cita: ";
                leerLinea(motivo, 150);
                
                agendarCita(h, idPac, idDoc, fecha, hora, motivo);
                break;
            }
            case 2: {
                int id;
                cout << "ID de la cita a cancelar: ";
                id = leerEntero();
                if (!cancelarCita(id)) {
                    cout << "No se pudo cancelar la cita.\n";
                }
                break;
            }
            case 3: {
                int id;
                char diagnostico[200], tratamiento[200], medicamentos[150];
                
                cout << "ID de la cita a atender: ";
                id = leerEntero();
                cout << "Diagnostico: ";
                limpiarBufferEntrada();
                leerLinea(diagnostico, 200);
                cout << "Tratamiento: ";
                leerLinea(tratamiento, 200);
                cout << "Medicamentos: ";
                leerLinea(medicamentos, 150);
                
                if (!atenderCita(h, id, diagnostico, tratamiento, medicamentos)) {
                    cout << "No se pudo atender la cita.\n";
                }
                break;
            }
            case 4: {
                int id;
                cout << "ID del paciente: ";
                id = leerEntero();
                obtenerCitasDePaciente(id);
                break;
            }
            case 5: {
                int id;
                cout << "ID del doctor: ";
                id = leerEntero();
                obtenerCitasDeDoctor(id);
                break;
            }
            case 6: {
                char fecha[11];
                cout << "Fecha (YYYY-MM-DD): ";
                limpiarBufferEntrada();
                leerLinea(fecha, 11);
                
                if (!validarFecha(fecha)) {
                    cout << "Fecha invalida.\n";
                    break;
                }
                
                obtenerCitasPorFecha(fecha);
                break;
            }
            case 7: {
                listarCitasPendientes();
                break;
            }
            case 8: {
                int id;
                cout << "ID de la cita: ";
                id = leerEntero();
                
                Cita cita = buscarCitaPorID(id);
                if (cita.id == -1) {
                    cout << "Cita no encontrada.\n";
                } else {
                    Paciente p = buscarPacientePorID(cita.pacienteID);
                    Doctor d = buscarDoctorPorID(cita.doctorID);
                    
                    cout << "\n--- Informacion de la Cita ---\n";
                    cout << "ID: " << cita.id << "\n";
                    cout << "Paciente: " << p.nombre << " " << p.apellido << " (ID: " << p.id << ")\n";
                    cout << "Doctor: " << d.nombre << " " << d.apellido << " (ID: " << d.id << ")\n";
                    cout << "Fecha: " << cita.fecha << " " << cita.hora << "\n";
                    cout << "Motivo: " << cita.motivo << "\n";
                    cout << "Estado: " << cita.estado << "\n";
                    cout << "Atendida: " << (cita.atendida ? "Si" : "No") << "\n";
                    if (cita.atendida) {
                        cout << "Consulta ID: " << cita.consultaID << "\n";
                    }
                    cout << "Observaciones: " << cita.observaciones << "\n";
                }
                break;
            }
            case 0: {
                system("cls");
                break;
            }
            default: {
                cout << "Opcion invalida.\n";
                break;
            }
        }
    } while (op != 0);
}

void verificarSistemaArchivos() {
    system("pause");
    system("cls");
    cout << "\n--- Verificacion del Sistema de Archivos ---\n";
    
    // Verificar existencia de archivos
    string archivos[] = {"hospital.bin", "pacientes.bin", "doctores.bin", "citas.bin", "historiales.bin"};
    bool todosExisten = true;
    
    for (int i = 0; i < 5; i++) {
    string ruta = string("DATA/") + archivos[i];
        ifstream file(ruta, ios::binary);
        if (file.is_open()) {
            cout << archivos[i] << " - EXISTE\n";
            file.close();
        } else {
            cout << archivos[i] << " - NO EXISTE\n";
            todosExisten = false;
        }
    }
    
    // Mostrar estadisticas
    if (todosExisten) {
        cout << "\n--- Estadisticas ---\n";
        ArchivoHeader hp = leerHeader("DATA/pacientes.bin");
        ArchivoHeader hd = leerHeader("DATA/doctores.bin");
        ArchivoHeader hc = leerHeader("DATA/citas.bin");
        ArchivoHeader hh = leerHeader("DATA/historiales.bin");
        
        cout << "Pacientes registrados: " << hp.registrosActivos << "/" << hp.cantidadRegistros << "\n";
        cout << "Doctores registrados: " << hd.registrosActivos << "/" << hd.cantidadRegistros << "\n";
        cout << "Citas registradas: " << hc.registrosActivos << "/" << hc.cantidadRegistros << "\n";
        cout << "Consultas en historial: " << hh.registrosActivos << "/" << hh.cantidadRegistros << "\n";
    } else {
        cout << "\nAlgunos archivos no existen. Desea inicializar el sistema? (s/n): ";
        char op;
        cin >> op;
        if (op == 's' || op == 'S') {
            if (inicializarSistemaArchivos()) {
                cout << "Sistema inicializado correctamente.\n";
            } else {
                cout << "Error al inicializar el sistema.\n";
            }
        }
    }
    
    system("pause");
    system("cls");
}

// =====================================================
// MENU DE MANTENIMIENTO COMPLETO
// =====================================================

void menuMantenimiento(Hospital* h) {
    int op = -1;
    do {
        
        system("cls");
    cout << "\n=== MENU DE MANTENIMIENTO ===\n";
        cout << "1. Verificar integridad de archivos\n";
        cout << "2. Verificar integridad referencial\n";
        cout << "3. Reparar integridad referencial\n";
        cout << "4. Compactar archivos (eliminar registros borrados)\n";
        cout << "5. Crear respaldo de datos\n";
        cout << "6. Restaurar desde respaldo\n";
        cout << "7. Mostrar estadisticas detalladas\n";
        cout << "8. Reparar lista de historial de paciente\n";
        cout << "0. Volver al menu principal\n";
        cout << "Elija una opcion: ";
        op = leerEntero();
        
        switch(op) {
            case 1: {
                mostrarDirectorioActual();
                cout << "\n=== VERIFICACION DE SISTEMA DE ARCHIVOS ===\n";
                string archivos[] = {"hospital.bin", "pacientes.bin", "doctores.bin", "citas.bin", "historiales.bin"};
                bool todosExisten = true;
                
                for (int i = 0; i < 5; i++) {
                    string ruta = string("DATA/") + archivos[i];
                    ifstream file(ruta, ios::binary);
                    if (file.is_open()) {
                        cout << archivos[i] << " - EXISTE\n";
                        file.close();
                    } else {
                        cout << archivos[i] << " - NO EXISTE\n";
                        todosExisten = false;
                    }
                }
                
                if (todosExisten) {
                    cout << "\nTodos los archivos del sistema estan presentes.\n";
                } else {
                    cout << "\nAlgunos archivos no existen. El sistema puede no funcionar correctamente.\n";
                }
                break;
            }
            case 2: {
                verificarIntegridadReferencial();
                break;
            }
            case 3: {
                repararIntegridadReferencial();
                break;
            }
            case 4: {
                cout << "\nADVERTENCIA: La compactacion eliminara permanentemente\n";
                cout << "todos los registros marcados como eliminados.\n";
                cout << "Desea continuar? (s/n): ";
                char confirmacion;
                cin >> confirmacion;
                limpiarBufferEntrada();
                
                if (confirmacion == 's' || confirmacion == 'S') {
                    compactarTodosArchivos();
                } else {
                    cout << "Compactacion cancelada.\n";
                }
                break;
            }
            case 5: {
                char directorio[100];
                cout << "Directorio para respaldo (ej: BACKUP): ";
                limpiarBufferEntrada();
                leerLinea(directorio, 100);
                crearRespaldo(directorio);
                break;
            }
            case 6: {
                char directorio[100];
                cout << "Directorio del respaldo a restaurar: ";
                limpiarBufferEntrada();
                leerLinea(directorio, 100);
                restaurarRespaldo(directorio);
                break;
            }
            case 7: {
                mostrarEstadisticasDetalladas();
                break;
            }
            case 8: {
                int idPaciente;
                cout << "ID del paciente a reparar: ";
                idPaciente = leerEntero();
                repararListaHistorialPaciente(idPaciente);
                break;
            }
            case 0: {
                system("cls");
                break;
            }
            default: {
                cout << "Opcion invalida.\n";
                break;
            }
        }
        
        if (op != 0) {
            system("cls");
        }
    } while (op != 0);
}

// =====================================================
// FUNCION MAIN
// =====================================================

int main() {
    setlocale(LC_ALL, "spanish");
    Hospital* h = cargarDatosHospital();
    if (!h) {
        cout << "Error: No se pudo cargar el sistema de archivos.\n";
        return 1;
    }

    int opcion = -1;
    do {
        mostrarMenuPrincipal();
        opcion = leerEntero();

        switch(opcion) {
            case 1:
                menuPacientes(h);
                break;
            case 2:
                menuDoctores(h);
                break;
            case 3:
                menuCitas(h);
                break;
            case 4:
                mostrarDatosHospital(h);
                break;
            case 5:
                verificarSistemaArchivos();
                break;
            case 6:
                menuMantenimiento(h);
                break;
            case 0:
                cout << "Saliendo del sistema...\n";
                break;
            default:
                cout << "Opcion invalida.\n";
                system("pause");
                break;
        }
    } while (opcion != 0);

    // Guardar datos finales del hospital
    if (guardarDatosHospital(h)) {
        cout << "Datos del hospital guardados correctamente.\n";
    } else {
        cout << "Error al guardar datos del hospital.\n";
    }
    
    delete h;
    cout << "Sistema cerrado correctamente.\n";
    return 0;
}