#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <cctype>
#include <string>
#include <fstream>
#include <windows.h>

using namespace std;

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
// SISTEMA DE ARCHIVOS ESPECÍFICOS
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
    
    cout << "Sistema de archivos cargado correctamente.\n";
    return hospital;
    Sleep(3);
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

bool guardarPaciente(Paciente paciente) {
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
// FUNCIONES PARA DOCTORES (ARCHIVOS BINARIOS)
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

bool guardarDoctor(Doctor doctor) {
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
// FUNCIONES DE VALIDACIÓN Y UTILIDAD
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
// FUNCIONES DE GESTIÓN (NUEVO SISTEMA)
// =====================================================

Paciente* crearPaciente(Hospital* h, const char* nombre, const char* apellido, const char* cedula,
                       int edad, char sexo, const char* tipoSangre, const char* telefono,
                       const char* direccion, const char* email, const char* alergias, const char* observaciones) {
    
    if (!validarCedula(cedula)) {
        cout << "Error: Cedula invalida. Formato: V-12345678\n";
        return nullptr;
    }
    if (edad < 0 || edad > 120) {
        cout << "Error: Edad debe estar entre 0 y 120 anios.\n";
        return nullptr;
    }
    if (!(sexo=='M' || sexo == 'm' || sexo=='F' || sexo=='f')) {
        cout << "Error: Sexo debe ser M o F.\n";
        return nullptr;
    }

    // Verificar cédula única
    Paciente existente = buscarPacientePorCedula(cedula);
    if (existente.id != -1) {
        cout << "Error: La cedula ya esta registrada.\n";
        return nullptr;
    }

    if (email && email[0] != '\0') {
        if (!validarEmail(email)) {
            cout << "Error: Email invalido. Formato esperado: usuario@dominio.ext\n";
            return nullptr;
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
        h->siguienteIDPaciente++;
        h->totalPacientesRegistrados++;
        guardarDatosHospital(h);
        
        cout << "Paciente creado exitosamente. ID: " << nuevoPaciente.id << "\n";
        
        // Retornar copia en memoria
        Paciente* copiaMemoria = new Paciente;
        *copiaMemoria = nuevoPaciente;
        return copiaMemoria;
    } else {
        cout << "Error: No se pudo guardar el paciente.\n";
        return nullptr;
    }
}

Doctor* crearDoctor(Hospital* h, const char* nombre, const char* apellido, const char* cedula,
                   const char* especialidad, int anios, float costo) {
    
    if (!validarCedula(cedula)) {
        cout << "Error: Cedula profesional invalida.\n";
        return nullptr;
    }
    if (anios < 0) {
        cout << "Error: Anios de experiencia no pueden ser negativos.\n";
        return nullptr;
    }
    if (costo <= 0.0f) {
        cout << "Error: Costo de consulta debe ser mayor a 0.\n";
        return nullptr;
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
        h->siguienteIDDoctor++;
        h->totalDoctoresRegistrados++;
        guardarDatosHospital(h);
        
        cout << "Doctor creado exitosamente. ID: " << nuevoDoctor.id << "\n";
        
        Doctor* copiaMemoria = new Doctor;
        *copiaMemoria = nuevoDoctor;
        return copiaMemoria;
    } else {
        cout << "Error: No se pudo guardar el doctor.\n";
        return nullptr;
    }
}

// =====================================================
// FUNCIONES DE VISUALIZACIÓN
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
        if (!p.eliminado && p.activo) {
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
        if (!d.eliminado) {
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
// MENÚS DEL SISTEMA
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
        cout << "4. Listar todos los pacientes\n";
        cout << "5. Eliminar paciente\n";
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
                
                Paciente* nuevo = crearPaciente(h, nombre, apellido, cedula, edad, sexo, 
                                              tipoSangre, telefono, direccion, email, 
                                              alergias, observaciones);
                if (nuevo) {
                    delete nuevo;
                }
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
                listarPacientes(h);
                break;
            }
            case 5: {
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
        cout << "3. Listar todos los doctores\n";
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
                cout << "Anios experiencia: "; 
                anios = leerEntero();
                if (anios == -1) {
                    cout << "Anios de experiencia invalidos.\n";
                    break;
                }
                cout << "Costo consulta: "; 
                cin >> costo;
                
                limpiarBufferEntrada();
                cout << "Telefono (opcional): ";
                char telefonoDoc[15]; leerLinea(telefonoDoc, 15);
                cout << "Email (opcional): ";
                char emailDoc[50]; leerLinea(emailDoc, 50);
                cout << "Horario atencion (opcional): ";
                char horarioDoc[50]; leerLinea(horarioDoc, 50);
                
                Doctor* nuevo = crearDoctor(h, nombre, apellido, cedula, especialidad, anios, costo);
                if (nuevo) {
                    // Actualizar campos opcionales
                    if (telefonoDoc[0] != '\0') {
                        strncpy(nuevo->telefono, telefonoDoc, sizeof(nuevo->telefono)-1);
                    }
                    if (emailDoc[0] != '\0' && validarEmail(emailDoc)) {
                        strncpy(nuevo->email, emailDoc, sizeof(nuevo->email)-1);
                    }
                    if (horarioDoc[0] != '\0') {
                        strncpy(nuevo->horarioAtencion, horarioDoc, sizeof(nuevo->horarioAtencion)-1);
                    }
                    
                    // Guardar cambios
                    guardarDoctor(*nuevo);
                    delete nuevo;
                }
                break;
            }
            case 2: {
                int id; 
                cout << "ID: "; 
                id = leerEntero();
                Doctor d = buscarDoctorPorID(id);
                if (d.id != -1) {
                    cout << "ID: " << d.id << "\n";
                    cout << "Nombre: " << d.nombre << " " << d.apellido << "\n";
                    cout << "Cedula: " << d.cedulaProfesional << "\n";
                    cout << "Especialidad: " << d.especialidad << "\n";
                    cout << "Anios experiencia: " << d.aniosExperiencia << "\n";
                    cout << "Costo consulta: " << fixed << setprecision(2) << d.costoConsulta << "\n";
                    cout << "Telefono: " << d.telefono << "\n";
                    cout << "Email: " << d.email << "\n";
                    cout << "Horario: " << d.horarioAtencion << "\n";
                    cout << "Pacientes asignados: " << d.cantidadPacientes << "\n";
                    cout << "Citas agendadas: " << d.cantidadCitas << "\n";
                    cout << "Disponible: " << (d.disponible ? "Si" : "No") << "\n";
                } else {
                    cout << "No encontrado.\n";
                }
                break;
            }
            case 3: {
                listarDoctores(h);
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
    
    for (const string& archivo : archivos) {
        string ruta = "DATA/" + archivo;
        ifstream file(ruta, ios::binary);
        if (file.is_open()) {
            cout << "✅ " << archivo << " - EXISTE\n";
            file.close();
        } else {
            cout << "❌ " << archivo << " - NO EXISTE\n";
            todosExisten = false;
        }
    }
    
    // Mostrar estadísticas
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
        cout << "\n⚠️  Algunos archivos no existen. ¿Desea inicializar el sistema? (s/n): ";
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
// FUNCIÓN MAIN
// =====================================================

int main() {
    setlocale(LC_ALL, "spanish");

    cout << "=== SISTEMA DE GESTION HOSPITALARIO v2.0 ===\n";
    cout << "===    (Sistema de Archivos Binarios)    ===\n\n";

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
                cout << "Gestion de Citas - En desarrollo\n";
                system("pause");
                break;
            case 4:
                mostrarDatosHospital(h);
                break;
            case 5:
                verificarSistemaArchivos();
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