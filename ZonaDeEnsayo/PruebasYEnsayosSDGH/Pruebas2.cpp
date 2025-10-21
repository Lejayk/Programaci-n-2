// hospital_fixed.cpp
// Versión corregida y parcheada del sistema de gestión hospitalaria
// Correcciones aplicadas:
//  - Entrada robusta (leerLinea, leerEntero, limpiarBufferEntrada)
//  - Búsquedas case-insensitive (nombres, cédulas, especialidad)
//  - Inicialización correcta en redimensionamiento de arrays (citas)
//  - Reemplazo de system("cls")/system("pause") por funciones portables
//  - Otras mejoras y limpiezas menores
//
// Nota: No se aplicó el parche de año bisiesto (según petición).

#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>

using namespace std;

// ====================== ESTRUCTURAS ======================

struct HistorialMedico {
    int idConsulta;
    char fecha[11];       // "YYYY-MM-DD"
    char hora[6];         // "HH:MM"
    char diagnostico[200];
    char tratamiento[200];
    char medicamentos[150];
    int idDoctor;
    float costo;
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

    HistorialMedico* historial;
    int cantidadConsultas;
    int capacidadHistorial;

    int* citasAgendadas;
    int cantidadCitas;
    int capacidadCitas;

    char alergias[500];
    char observaciones[500];

    bool activo;
};

struct Doctor {
    int id;
    char nombre[50];
    char apellido[50];
    char cedula[20];
    char especialidad[50];
    int aniosExperiencia;
    float costoConsulta;
    char horarioAtencion[50];
    char telefono[15];
    char email[50];

    int* pacientesAsignados;
    int cantidadPacientes;
    int capacidadPacientes;

    int* citasAgendadas;
    int cantidadCitas;
    int capacidadCitas;

    bool disponible;
};

struct Cita {
    int id;
    int idPaciente;
    int idDoctor;
    char fecha[11];
    char hora[6];
    char motivo[150];
    char estado[20]; // Agendada, Atendida, Cancelada
    char observaciones[200];
    bool atendida;
};

struct Hospital {
    char nombre[100];
    char direccion[150];
    char telefono[15];

    Paciente* pacientes;
    int cantidadPacientes;
    int capacidadPacientes;

    Doctor* doctores;
    int cantidadDoctores;
    int capacidadDoctores;

    Cita* citas;
    int cantidadCitas;
    int capacidadCitas;

    int siguienteIdPaciente;
    int siguienteIdDoctor;
    int siguienteIdCita;
    int siguienteIdConsulta;
};

// ====================== CONSTANTES INICIALES ======================

const int CAP_PACIENTES_INICIAL = 10;
const int CAP_DOCTORES_INICIAL = 10;
const int CAP_CITAS_INICIAL = 20;
const int CAP_HISTORIAL_INICIAL = 5;
const int CAP_CITAS_PACIENTE_INICIAL = 5;
const int CAP_PACIENTOS_DOCTOR_INICIAL = 5;
const int CAP_CITAS_DOCTOR_INICIAL = 10;

// ====================== UTILIDADES ======================

// Limpiar buffer: ignorar hasta que ocurra '\n' o se consuman muchos caracteres.
// Usamos un número grande (10000) en lugar de numeric_limits para evitar incluir <limits>.
inline void limpiarBufferEntrada() {
    cin.ignore(10000, '\n');
}

// Leer una línea segura en char buffer (size incluye espacio para '\0').
// Evita doble consumo de '\n' usando peek.
inline void leerLinea(char* buffer, int size) {
    if (cin.peek() == '\n') cin.get();
    cin.getline(buffer, size);
    if (cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
    }
    buffer[size - 1] = '\0';
}

// Leer entero robusto: limpia resto de línea tras leer; devuelve -1 en error.
int leerEntero() {
    int x;
    if (!(cin >> x)) {
        cin.clear();
        cin.ignore(10000, '\n');
        return -1;
    }
    // quitar resto de la línea (incluye el '\n')
    cin.ignore(10000, '\n');
    return x;
}

// Convertir src a minúsculas en dst (dstSize incluye terminador).
void toLowerCopy(const char* src, char* dst, int dstSize) {
    int i = 0;
    for (; src[i] != '\0' && i < dstSize - 1; ++i) {
        char c = src[i];
        if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
        dst[i] = c;
    }
    dst[i] = '\0';
}

// Comparación substring case-insensitive: devuelve true si needle está en haystack.
bool containsCaseInsensitive(const char* haystack, const char* needle) {
    // buffers temporales suficientemente grandes para nombres/combinaciones
    char h[512], n[256];
    toLowerCopy(haystack, h, sizeof(h));
    toLowerCopy(needle, n, sizeof(n));
    return (strstr(h, n) != nullptr);
}

// Comparar dos cadenas por igualdad case-insensitive.
bool equalsCaseInsensitive(const char* a, const char* b) {
    char A[256], B[256];
    toLowerCopy(a, A, sizeof(A));
    toLowerCopy(b, B, sizeof(B));
    return (strcmp(A, B) == 0);
}

// Validar cédula: mínima validación de longitud (no nula, no vacía, longitud <= 19)
bool validarCedula(const char* cedula) {
    if (!cedula) return false;
    int len = (int)strlen(cedula);
    if (len == 0 || len > 19) return false;
    return true;
}

// Validación básica de email (contiene @ y . después de @)
bool validarEmail(const char* email) {
    if (!email) return false;
    const char* at = strchr(email, '@');
    if (!at) return false;
    const char* dot = strchr(at, '.');
    if (!dot) return false;
    if (at == email) return false;
    if (*(dot + 1) == '\0') return false;
    return true;
}

// Validar formato fecha "YYYY-MM-DD" (no se aplica bisiesto según petición)
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
    int y = (fecha[0] - '0') * 1000 + (fecha[1] - '0') * 100 + (fecha[2] - '0') * 10 + (fecha[3] - '0');
    int m = (fecha[5] - '0') * 10 + (fecha[6] - '0');
    int d = (fecha[8] - '0') * 10 + (fecha[9] - '0');
    if (y < 1900 || y > 2100) return false;
    if (m < 1 || m > 12) return false;
    int diasMes = 31;
    if (m == 4 || m == 6 || m == 9 || m == 11) diasMes = 30;
    else if (m == 2) diasMes = 28; // no considerar bisiesto según petición
    if (d < 1 || d > diasMes) return false;
    return true;
}

// Validar hora "HH:MM"
bool validarHora(const char* hora) {
    if (!hora) return false;
    if (strlen(hora) != 5) return false;
    if (hora[2] != ':') return false;
    for (int i = 0; i < 5; i++) {
        if (i == 2) continue;
        if (hora[i] < '0' || hora[i] > '9') return false;
    }
    int hh = (hora[0] - '0') * 10 + (hora[1] - '0');
    int mm = (hora[3] - '0') * 10 + (hora[4] - '0');
    if (hh < 0 || hh > 23) return false;
    if (mm < 0 || mm > 59) return false;
    return true;
}

// Comparar fechas lexicográficamente (formato YYYY-MM-DD)
int compararFechas(const char* f1, const char* f2) {
    int cmp = strcmp(f1, f2);
    if (cmp < 0) return -1;
    if (cmp > 0) return 1;
    return 0;
}

// ====================== PANTALLA / PAUSA PORTABLE ======================

void pausePrompt() {
    char tmp[2];
    cout << "\nPresione ENTER para continuar...";
    leerLinea(tmp, 2);
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\x1B[2J\x1B[H";
#endif
}

// ====================== INICIALIZACION Y REDIMENSIONAMIENTO ======================

Hospital* crearHospital(const char* nombreHospital, const char* direccion, const char* telefono) {
    Hospital* h = new Hospital;
    strncpy(h->nombre, nombreHospital, sizeof(h->nombre) - 1); h->nombre[sizeof(h->nombre) - 1] = '\0';
    strncpy(h->direccion, direccion, sizeof(h->direccion) - 1); h->direccion[sizeof(h->direccion) - 1] = '\0';
    strncpy(h->telefono, telefono, sizeof(h->telefono) - 1); h->telefono[sizeof(h->telefono) - 1] = '\0';

    h->capacidadPacientes = CAP_PACIENTES_INICIAL;
    h->capacidadDoctores = CAP_DOCTORES_INICIAL;
    h->capacidadCitas = CAP_CITAS_INICIAL;

    h->cantidadPacientes = 0;
    h->cantidadDoctores = 0;
    h->cantidadCitas = 0;

    h->siguienteIdPaciente = 1;
    h->siguienteIdDoctor = 1;
    h->siguienteIdCita = 1;
    h->siguienteIdConsulta = 1;

    h->pacientes = new Paciente[h->capacidadPacientes];
    h->doctores = new Doctor[h->capacidadDoctores];
    h->citas = new Cita[h->capacidadCitas];

    // Inicializar punteros internos en slots disponibles
    for (int i = 0; i < h->capacidadPacientes; i++) {
        h->pacientes[i].historial = nullptr;
        h->pacientes[i].citasAgendadas = nullptr;
        h->pacientes[i].activo = false;
    }
    for (int i = 0; i < h->capacidadDoctores; i++) {
        h->doctores[i].pacientesAsignados = nullptr;
        h->doctores[i].citasAgendadas = nullptr;
        h->doctores[i].disponible = false;
    }
    for (int i = 0; i < h->capacidadCitas; i++) {
        h->citas[i].id = 0;
        h->citas[i].idPaciente = 0;
        h->citas[i].idDoctor = 0;
        h->citas[i].fecha[0] = '\0';
        h->citas[i].hora[0] = '\0';
        h->citas[i].motivo[0] = '\0';
        h->citas[i].estado[0] = '\0';
        h->citas[i].observaciones[0] = '\0';
        h->citas[i].atendida = false;
    }

    return h;
}

void redimensionarArrayPacientes(Hospital* h) {
    int nueva = h->capacidadPacientes * 2;
    Paciente* arr = new Paciente[nueva];
    for (int i = 0; i < h->cantidadPacientes; i++) arr[i] = h->pacientes[i];
    for (int i = h->cantidadPacientes; i < nueva; i++) {
        arr[i].historial = nullptr;
        arr[i].citasAgendadas = nullptr;
        arr[i].activo = false;
    }
    delete[] h->pacientes;
    h->pacientes = arr;
    h->capacidadPacientes = nueva;
}

void redimensionarArrayDoctores(Hospital* h) {
    int nueva = h->capacidadDoctores * 2;
    Doctor* arr = new Doctor[nueva];
    for (int i = 0; i < h->cantidadDoctores; i++) arr[i] = h->doctores[i];
    for (int i = h->cantidadDoctores; i < nueva; i++) {
        arr[i].pacientesAsignados = nullptr;
        arr[i].citasAgendadas = nullptr;
        arr[i].disponible = false;
    }
    delete[] h->doctores;
    h->doctores = arr;
    h->capacidadDoctores = nueva;
}

void redimensionarArrayCitas(Hospital* h) {
    int nueva = h->capacidadCitas * 2;
    Cita* arr = new Cita[nueva];
    for (int i = 0; i < h->cantidadCitas; i++) arr[i] = h->citas[i];
    for (int i = h->cantidadCitas; i < nueva; i++) {
        arr[i].id = 0;
        arr[i].idPaciente = 0;
        arr[i].idDoctor = 0;
        arr[i].fecha[0] = '\0';
        arr[i].hora[0] = '\0';
        arr[i].motivo[0] = '\0';
        arr[i].estado[0] = '\0';
        arr[i].observaciones[0] = '\0';
        arr[i].atendida = false;
    }
    delete[] h->citas;
    h->citas = arr;
    h->capacidadCitas = nueva;
}

// ====================== PACIENTES ======================

void inicializarPacienteEstructura(Paciente& p) {
    p.historial = new HistorialMedico[CAP_HISTORIAL_INICIAL];
    p.cantidadConsultas = 0;
    p.capacidadHistorial = CAP_HISTORIAL_INICIAL;
    p.citasAgendadas = new int[CAP_CITAS_PACIENTE_INICIAL];
    p.cantidadCitas = 0;
    p.capacidadCitas = CAP_CITAS_PACIENTE_INICIAL;
    p.alergias[0] = '\0';
    p.observaciones[0] = '\0';
    p.activo = true;
    // inicializar historial con valores seguros
    for (int i = 0; i < p.capacidadHistorial; ++i) {
        p.historial[i].idConsulta = 0;
        p.historial[i].fecha[0] = '\0';
        p.historial[i].hora[0] = '\0';
        p.historial[i].diagnostico[0] = '\0';
        p.historial[i].tratamiento[0] = '\0';
        p.historial[i].medicamentos[0] = '\0';
        p.historial[i].idDoctor = 0;
        p.historial[i].costo = 0.0f;
    }
}

Paciente* crearPaciente(Hospital* h, const char* nombre, const char* apellido, const char* cedula, int edad, char sexo) {
    if (!validarCedula(cedula)) return nullptr;
    if (edad < 0 || edad > 120) return nullptr;
    if (!(sexo == 'M' || sexo == 'F')) return nullptr;
    // verificar cedula única (case-insensitive)
    for (int i = 0; i < h->cantidadPacientes; i++) {
        if (equalsCaseInsensitive(h->pacientes[i].cedula, cedula)) return nullptr;
    }
    if (h->cantidadPacientes >= h->capacidadPacientes) redimensionarArrayPacientes(h);
    int idx = h->cantidadPacientes;
    Paciente& p = h->pacientes[idx];
    p.id = h->siguienteIdPaciente++;
    strncpy(p.nombre, nombre, sizeof(p.nombre) - 1); p.nombre[sizeof(p.nombre) - 1] = '\0';
    strncpy(p.apellido, apellido, sizeof(p.apellido) - 1); p.apellido[sizeof(p.apellido) - 1] = '\0';
    strncpy(p.cedula, cedula, sizeof(p.cedula) - 1); p.cedula[sizeof(p.cedula) - 1] = '\0';
    p.edad = edad;
    p.sexo = sexo;
    p.tipoSangre[0] = '\0';
    p.telefono[0] = '\0';
    p.direccion[0] = '\0';
    p.email[0] = '\0';
    inicializarPacienteEstructura(p);
    h->cantidadPacientes++;
    return &h->pacientes[idx];
}

Paciente* buscarPacientePorId(Hospital* h, int id) {
    for (int i = 0; i < h->cantidadPacientes; i++) if (h->pacientes[i].id == id) return &h->pacientes[i];
    return nullptr;
}

Paciente* buscarPacientePorCedula(Hospital* h, const char* cedula) {
    for (int i = 0; i < h->cantidadPacientes; i++) {
        if (equalsCaseInsensitive(h->pacientes[i].cedula, cedula)) return &h->pacientes[i];
    }
    return nullptr;
}

Paciente** buscarPacientesPorNombre(Hospital* h, const char* nombre, int* cantidad) {
    *cantidad = 0;
    for (int i = 0; i < h->cantidadPacientes; i++) {
        char full[120]; full[0] = '\0';
        strncat(full, h->pacientes[i].nombre, sizeof(full) - 1);
        strncat(full, " ", sizeof(full) - 1);
        strncat(full, h->pacientes[i].apellido, sizeof(full) - 1);
        if (containsCaseInsensitive(full, nombre)) (*cantidad)++;
    }
    if (*cantidad == 0) return nullptr;
    Paciente** res = new Paciente*[*cantidad];
    int pos = 0;
    for (int i = 0; i < h->cantidadPacientes; i++) {
        char full[120]; full[0] = '\0';
        strncat(full, h->pacientes[i].nombre, sizeof(full) - 1);
        strncat(full, " ", sizeof(full) - 1);
        strncat(full, h->pacientes[i].apellido, sizeof(full) - 1);
        if (containsCaseInsensitive(full, nombre)) res[pos++] = &h->pacientes[i];
    }
    return res;
}

bool actualizarPaciente(Hospital* h, int id) {
    Paciente* p = buscarPacientePorId(h, id);
    if (!p) return false;
    char buffer[512];
    cout << "Dejar vacío para no cambiar.\n";
    cout << "Telefono actual: " << p->telefono << "\nNuevo telefono: ";
    leerLinea(buffer, 15);
    if (strlen(buffer) > 0) { strncpy(p->telefono, buffer, 14); p->telefono[14] = '\0'; }
    cout << "Direccion actual: " << p->direccion << "\nNueva direccion: ";
    leerLinea(buffer, 100);
    if (strlen(buffer) > 0) { strncpy(p->direccion, buffer, 99); p->direccion[99] = '\0'; }
    cout << "Email actual: " << p->email << "\nNuevo email: ";
    leerLinea(buffer, 50);
    if (strlen(buffer) > 0) {
        if (validarEmail(buffer)) { strncpy(p->email, buffer, 49); p->email[49] = '\0'; }
        else cout << "Email invalido. No se actualizo.\n";
    }
    cout << "Alergias actuales: " << p->alergias << "\nNuevas alergias: ";
    leerLinea(buffer, 500);
    if (strlen(buffer) > 0) { strncpy(p->alergias, buffer, 499); p->alergias[499] = '\0'; }
    cout << "Observaciones actuales: " << p->observaciones << "\nNuevas observaciones: ";
    leerLinea(buffer, 500);
    if (strlen(buffer) > 0) { strncpy(p->observaciones, buffer, 499); p->observaciones[499] = '\0'; }
    return true;
}

bool eliminarPaciente(Hospital* h, int id) {
    int idx = -1;
    for (int i = 0; i < h->cantidadPacientes; i++) if (h->pacientes[i].id == id) { idx = i; break; }
    if (idx == -1) return false;
    Paciente& p = h->pacientes[idx];

    // Cancelar citas asociadas y remover referencias de doctores
    for (int i = 0; i < h->cantidadCitas; i++) {
        if (h->citas[i].idPaciente == id && strcmp(h->citas[i].estado, "Agendada") == 0) {
            strcpy(h->citas[i].estado, "Cancelada");
            h->citas[i].atendida = false;
            // remover de doctor
            Doctor* d = nullptr;
            for (int j = 0; j < h->cantidadDoctores; j++) if (h->doctores[j].id == h->citas[i].idDoctor) { d = &h->doctores[j]; break; }
            if (d) {
                for (int k = 0; k < d->cantidadCitas; k++) {
                    if (d->citasAgendadas[k] == h->citas[i].id) {
                        for (int m = k; m < d->cantidadCitas - 1; m++) d->citasAgendadas[m] = d->citasAgendadas[m + 1];
                        d->cantidadCitas--;
                        break;
                    }
                }
            }
        }
    }
    // remover de doctores asignados
    for (int i = 0; i < h->cantidadDoctores; i++) {
        Doctor& d = h->doctores[i];
        for (int j = 0; j < d.cantidadPacientes; j++) {
            if (d.pacientesAsignados[j] == id) {
                for (int k = j; k < d.cantidadPacientes - 1; k++) d.pacientesAsignados[k] = d.pacientesAsignados[k + 1];
                d.cantidadPacientes--;
                break;
            }
        }
    }

    // liberar memoria interna del paciente
    if (p.historial) delete[] p.historial;
    if (p.citasAgendadas) delete[] p.citasAgendadas;

    // compactar array
    for (int i = idx; i < h->cantidadPacientes - 1; i++) h->pacientes[i] = h->pacientes[i + 1];
    h->cantidadPacientes--;
    return true;
}

void listarPacientes(Hospital* h) {
    cout << "\nID  Nombre                Cedula         Edad  Consultas\n";
    cout << "---------------------------------------------------------\n";
    for (int i = 0; i < h->cantidadPacientes; i++) {
        Paciente& p = h->pacientes[i];
        char full[120]; full[0] = '\0';
        strncat(full, p.nombre, sizeof(full) - 1);
        strncat(full, " ", sizeof(full) - 1);
        strncat(full, p.apellido, sizeof(full) - 1);
        cout << setw(3) << p.id << "  " << setw(20) << full << "  " << setw(12) << p.cedula << "  " << setw(4) << p.edad << "  " << setw(8) << p.cantidadConsultas << "\n";
    }
}

// ====================== HISTORIAL MEDICO ======================

void agregarConsultaAlHistorial(Paciente* paciente, HistorialMedico consulta) {
    if (paciente->cantidadConsultas >= paciente->capacidadHistorial) {
        int nueva = paciente->capacidadHistorial * 2;
        HistorialMedico* arr = new HistorialMedico[nueva];
        for (int i = 0; i < paciente->cantidadConsultas; i++) arr[i] = paciente->historial[i];
        // inicializar nuevas entradas
        for (int i = paciente->cantidadConsultas; i < nueva; ++i) {
            arr[i].idConsulta = 0;
            arr[i].fecha[0] = '\0';
            arr[i].hora[0] = '\0';
            arr[i].diagnostico[0] = '\0';
            arr[i].tratamiento[0] = '\0';
            arr[i].medicamentos[0] = '\0';
            arr[i].idDoctor = 0;
            arr[i].costo = 0.0f;
        }
        delete[] paciente->historial;
        paciente->historial = arr;
        paciente->capacidadHistorial = nueva;
    }
    paciente->historial[paciente->cantidadConsultas++] = consulta;
}

HistorialMedico* obtenerHistorialCompleto(Paciente* paciente, int* cantidad) {
    *cantidad = paciente->cantidadConsultas;
    return paciente->historial;
}

void mostrarHistorialMedico(Paciente* paciente) {
    cout << "\nHistorial de " << paciente->nombre << " " << paciente->apellido << " (ID " << paciente->id << ")\n";
    cout << "ID   Fecha       Hora  Doctor  Costo   Diagnostico\n";
    cout << "---------------------------------------------------------------\n";
    for (int i = 0; i < paciente->cantidadConsultas; i++) {
        HistorialMedico& h = paciente->historial[i];
        cout << setw(4) << h.idConsulta << "  " << setw(10) << h.fecha << "  " << setw(5) << h.hora << "  " << setw(6) << h.idDoctor << "  " << setw(6) << fixed << setprecision(2) << h.costo << "  " << h.diagnostico << "\n";
    }
}

HistorialMedico* obtenerUltimaConsulta(Paciente* paciente) {
    if (paciente->cantidadConsultas == 0) return nullptr;
    return &paciente->historial[paciente->cantidadConsultas - 1];
}

// ====================== DOCTORES ======================

void inicializarDoctorEstructura(Doctor& d) {
    d.pacientesAsignados = new int[CAP_PACIENTOS_DOCTOR_INICIAL];
    d.cantidadPacientes = 0;
    d.capacidadPacientes = CAP_PACIENTOS_DOCTOR_INICIAL;
    d.citasAgendadas = new int[CAP_CITAS_DOCTOR_INICIAL];
    d.cantidadCitas = 0;
    d.capacidadCitas = CAP_CITAS_DOCTOR_INICIAL;
    d.disponible = true;
}

Doctor* crearDoctor(Hospital* h, const char* nombre, const char* apellido, const char* cedula, const char* especialidad, int anios, float costo) {
    if (!validarCedula(cedula)) return nullptr;
    if (anios < 0) return nullptr;
    if (costo <= 0.0f) return nullptr;
    // verificar cedula única (case-insensitive)
    for (int i = 0; i < h->cantidadDoctores; i++) if (equalsCaseInsensitive(h->doctores[i].cedula, cedula)) return nullptr;
    if (h->cantidadDoctores >= h->capacidadDoctores) redimensionarArrayDoctores(h);
    int idx = h->cantidadDoctores;
    Doctor& d = h->doctores[idx];
    d.id = h->siguienteIdDoctor++;
    strncpy(d.nombre, nombre, sizeof(d.nombre) - 1); d.nombre[sizeof(d.nombre) - 1] = '\0';
    strncpy(d.apellido, apellido, sizeof(d.apellido) - 1); d.apellido[sizeof(d.apellido) - 1] = '\0';
    strncpy(d.cedula, cedula, sizeof(d.cedula) - 1); d.cedula[sizeof(d.cedula) - 1] = '\0';
    strncpy(d.especialidad, especialidad, sizeof(d.especialidad) - 1); d.especialidad[sizeof(d.especialidad) - 1] = '\0';
    d.aniosExperiencia = anios;
    d.costoConsulta = costo;
    d.horarioAtencion[0] = '\0';
    d.telefono[0] = '\0';
    d.email[0] = '\0';
    inicializarDoctorEstructura(d);
    h->cantidadDoctores++;
    return &h->doctores[idx];
}

Doctor* buscarDoctorPorId(Hospital* h, int id) {
    for (int i = 0; i < h->cantidadDoctores; i++) if (h->doctores[i].id == id) return &h->doctores[i];
    return nullptr;
}

Doctor** buscarDoctoresPorEspecialidad(Hospital* h, const char* especialidad, int* cantidad) {
    *cantidad = 0;
    for (int i = 0; i < h->cantidadDoctores; i++) if (equalsCaseInsensitive(h->doctores[i].especialidad, especialidad)) (*cantidad)++;
    if (*cantidad == 0) return nullptr;
    Doctor** res = new Doctor*[*cantidad];
    int pos = 0;
    for (int i = 0; i < h->cantidadDoctores; i++) if (equalsCaseInsensitive(h->doctores[i].especialidad, especialidad)) res[pos++] = &h->doctores[i];
    return res;
}

bool asignarPacienteADoctor(Doctor* d, int idPaciente) {
    for (int i = 0; i < d->cantidadPacientes; i++) if (d->pacientesAsignados[i] == idPaciente) return false;
    if (d->cantidadPacientes >= d->capacidadPacientes) {
        int nueva = d->capacidadPacientes * 2;
        int* arr = new int[nueva];
        for (int i = 0; i < d->cantidadPacientes; i++) arr[i] = d->pacientesAsignados[i];
        delete[] d->pacientesAsignados;
        d->pacientesAsignados = arr;
        d->capacidadPacientes = nueva;
    }
    d->pacientesAsignados[d->cantidadPacientes++] = idPaciente;
    return true;
}

bool removerPacienteDeDoctor(Doctor* d, int idPaciente) {
    for (int i = 0; i < d->cantidadPacientes; i++) {
        if (d->pacientesAsignados[i] == idPaciente) {
            for (int k = i; k < d->cantidadPacientes - 1; k++) d->pacientesAsignados[k] = d->pacientesAsignados[k + 1];
            d->cantidadPacientes--;
            return true;
        }
    }
    return false;
}

void listarDoctores(Hospital* h) {
    cout << "\nID  Nombre                Cedula         Especialidad       Costo\n";
    cout << "-----------------------------------------------------------------\n";
    for (int i = 0; i < h->cantidadDoctores; i++) {
        Doctor& d = h->doctores[i];
        char full[120]; full[0] = '\0';
        strncat(full, d.nombre, sizeof(full) - 1);
        strncat(full, " ", sizeof(full) - 1);
        strncat(full, d.apellido, sizeof(full) - 1);
        cout << setw(3) << d.id << "  " << setw(20) << full << "  " << setw(12) << d.cedula << "  " << setw(15) << d.especialidad << "  " << fixed << setprecision(2) << d.costoConsulta << "\n";
    }
}

bool eliminarDoctor(Hospital* h, int id) {
    int idx = -1;
    for (int i = 0; i < h->cantidadDoctores; i++) if (h->doctores[i].id == id) { idx = i; break; }
    if (idx == -1) return false;
    Doctor& d = h->doctores[idx];
    // cancelar citas asociadas
    for (int i = 0; i < h->cantidadCitas; i++) {
        if (h->citas[i].idDoctor == id && strcmp(h->citas[i].estado, "Agendada") == 0) {
            strcpy(h->citas[i].estado, "Cancelada");
            h->citas[i].atendida = false;
            // remover de paciente
            Paciente* p = nullptr;
            for (int j = 0; j < h->cantidadPacientes; j++) if (h->pacientes[j].id == h->citas[i].idPaciente) { p = &h->pacientes[j]; break; }
            if (p) {
                for (int k = 0; k < p->cantidadCitas; k++) {
                    if (p->citasAgendadas[k] == h->citas[i].id) {
                        for (int m = k; m < p->cantidadCitas - 1; m++) p->citasAgendadas[m] = p->citasAgendadas[m + 1];
                        p->cantidadCitas--;
                        break;
                    }
                }
            }
        }
    }
    // liberar arrays
    if (d.pacientesAsignados) delete[] d.pacientesAsignados;
    if (d.citasAgendadas) delete[] d.citasAgendadas;
    // compactar
    for (int i = idx; i < h->cantidadDoctores - 1; i++) h->doctores[i] = h->doctores[i + 1];
    h->cantidadDoctores--;
    return true;
}

// ====================== CITAS ======================

bool verificarDisponibilidad(Hospital* h, int idDoctor, const char* fecha, const char* hora) {
    for (int i = 0; i < h->cantidadCitas; i++) {
        if (h->citas[i].idDoctor == idDoctor && strcmp(h->citas[i].estado, "Agendada") == 0) {
            if (strcmp(h->citas[i].fecha, fecha) == 0 && strcmp(h->citas[i].hora, hora) == 0) return false;
        }
    }
    return true;
}

Cita* agendarCita(Hospital* h, int idPaciente, int idDoctor, const char* fecha, const char* hora, const char* motivo) {
    Paciente* p = buscarPacientePorId(h, idPaciente);
    if (!p) return nullptr;
    Doctor* d = buscarDoctorPorId(h, idDoctor);
    if (!d) return nullptr;
    if (!validarFecha(fecha) || !validarHora(hora)) return nullptr;
    if (!verificarDisponibilidad(h, idDoctor, fecha, hora)) return nullptr;
    if (h->cantidadCitas >= h->capacidadCitas) redimensionarArrayCitas(h);
    int idx = h->cantidadCitas;
    Cita& c = h->citas[idx];
    c.id = h->siguienteIdCita++;
    c.idPaciente = idPaciente;
    c.idDoctor = idDoctor;
    strncpy(c.fecha, fecha, 10); c.fecha[10] = '\0';
    strncpy(c.hora, hora, 5); c.hora[5] = '\0';
    strncpy(c.motivo, motivo, sizeof(c.motivo) - 1); c.motivo[sizeof(c.motivo) - 1] = '\0';
    strcpy(c.estado, "Agendada");
    c.observaciones[0] = '\0';
    c.atendida = false;
    // agregar al paciente
    if (p->cantidadCitas >= p->capacidadCitas) {
        int nueva = p->capacidadCitas * 2;
        int* arr = new int[nueva];
        for (int i = 0; i < p->cantidadCitas; i++) arr[i] = p->citasAgendadas[i];
        delete[] p->citasAgendadas;
        p->citasAgendadas = arr;
        p->capacidadCitas = nueva;
    }
    p->citasAgendadas[p->cantidadCitas++] = c.id;
    // agregar al doctor
    if (d->cantidadCitas >= d->capacidadCitas) {
        int nueva = d->capacidadCitas * 2;
        int* arr = new int[nueva];
        for (int i = 0; i < d->cantidadCitas; i++) arr[i] = d->citasAgendadas[i];
        delete[] d->citasAgendadas;
        d->citasAgendadas = arr;
        d->capacidadCitas = nueva;
    }
    d->citasAgendadas[d->cantidadCitas++] = c.id;
    h->cantidadCitas++;
    return &h->citas[idx];
}

bool cancelarCita(Hospital* h, int idCita) {
    int idx = -1;
    for (int i = 0; i < h->cantidadCitas; i++) if (h->citas[i].id == idCita) { idx = i; break; }
    if (idx == -1) return false;
    Cita& c = h->citas[idx];
    if (strcmp(c.estado, "Cancelada") == 0) return false;
    strcpy(c.estado, "Cancelada");
    c.atendida = false;
    // remover de paciente
    Paciente* p = buscarPacientePorId(h, c.idPaciente);
    if (p) {
        for (int i = 0; i < p->cantidadCitas; i++) {
            if (p->citasAgendadas[i] == idCita) {
                for (int k = i; k < p->cantidadCitas - 1; k++) p->citasAgendadas[k] = p->citasAgendadas[k + 1];
                p->cantidadCitas--;
                break;
            }
        }
    }
    // remover de doctor
    Doctor* d = buscarDoctorPorId(h, c.idDoctor);
    if (d) {
        for (int i = 0; i < d->cantidadCitas; i++) {
            if (d->citasAgendadas[i] == idCita) {
                for (int k = i; k < d->cantidadCitas - 1; k++) d->citasAgendadas[k] = d->citasAgendadas[k + 1];
                d->cantidadCitas--;
                break;
            }
        }
    }
    return true;
}

bool atenderCita(Hospital* h, int idCita, const char* diagnostico, const char* tratamiento, const char* medicamentos) {
    Cita* c = nullptr;
    for (int i = 0; i < h->cantidadCitas; i++) if (h->citas[i].id == idCita) { c = &h->citas[i]; break; }
    if (!c) return false;
    if (strcmp(c->estado, "Agendada") != 0) return false;
    strcpy(c->estado, "Atendida");
    c->atendida = true;
    HistorialMedico hm;
    hm.idConsulta = h->siguienteIdConsulta++;
    strncpy(hm.fecha, c->fecha, 10); hm.fecha[10] = '\0';
    strncpy(hm.hora, c->hora, 5); hm.hora[5] = '\0';
    strncpy(hm.diagnostico, diagnostico, sizeof(hm.diagnostico) - 1); hm.diagnostico[sizeof(hm.diagnostico) - 1] = '\0';
    strncpy(hm.tratamiento, tratamiento, sizeof(hm.tratamiento) - 1); hm.tratamiento[sizeof(hm.tratamiento) - 1] = '\0';
    strncpy(hm.medicamentos, medicamentos, sizeof(hm.medicamentos) - 1); hm.medicamentos[sizeof(hm.medicamentos) - 1] = '\0';
    hm.idDoctor = c->idDoctor;
    Doctor* d = buscarDoctorPorId(h, c->idDoctor);
    hm.costo = (d ? d->costoConsulta : 0.0f);
    Paciente* p = buscarPacientePorId(h, c->idPaciente);
    if (!p) return false;
    agregarConsultaAlHistorial(p, hm);
    return true;
}

Cita** obtenerCitasDePaciente(Hospital* h, int idPaciente, int* cantidad) {
    *cantidad = 0;
    for (int i = 0; i < h->cantidadCitas; i++) if (h->citas[i].idPaciente == idPaciente) (*cantidad)++;
    if (*cantidad == 0) return nullptr;
    Cita** res = new Cita*[*cantidad];
    int pos = 0;
    for (int i = 0; i < h->cantidadCitas; i++) if (h->citas[i].idPaciente == idPaciente) res[pos++] = &h->citas[i];
    return res;
}

Cita** obtenerCitasDeDoctor(Hospital* h, int idDoctor, int* cantidad) {
    *cantidad = 0;
    for (int i = 0; i < h->cantidadCitas; i++) if (h->citas[i].idDoctor == idDoctor) (*cantidad)++;
    if (*cantidad == 0) return nullptr;
    Cita** res = new Cita*[*cantidad];
    int pos = 0;
    for (int i = 0; i < h->cantidadCitas; i++) if (h->citas[i].idDoctor == idDoctor) res[pos++] = &h->citas[i];
    return res;
}

Cita** obtenerCitasPorFecha(Hospital* h, const char* fecha, int* cantidad) {
    *cantidad = 0;
    for (int i = 0; i < h->cantidadCitas; i++) if (strcmp(h->citas[i].fecha, fecha) == 0) (*cantidad)++;
    if (*cantidad == 0) return nullptr;
    Cita** res = new Cita*[*cantidad];
    int pos = 0;
    for (int i = 0; i < h->cantidadCitas; i++) if (strcmp(h->citas[i].fecha, fecha) == 0) res[pos++] = &h->citas[i];
    return res;
}

void listarCitasPendientes(Hospital* h) {
    cout << "\nCitas pendientes:\nID  Paciente  Doctor  Fecha       Hora  Motivo\n";
    cout << "------------------------------------------------\n";
    for (int i = 0; i < h->cantidadCitas; i++) {
        Cita& c = h->citas[i];
        if (strcmp(c.estado, "Agendada") == 0) {
            cout << setw(3) << c.id << "  " << setw(8) << c.idPaciente << "  " << setw(6) << c.idDoctor << "  " << setw(10) << c.fecha << "  " << setw(5) << c.hora << "  " << c.motivo << "\n";
        }
    }
}

// ====================== DESTRUCCION ======================

void destruirHospital(Hospital* h) {
    if (!h) return;
    for (int i = 0; i < h->cantidadPacientes; i++) {
        if (h->pacientes[i].historial) delete[] h->pacientes[i].historial;
        if (h->pacientes[i].citasAgendadas) delete[] h->pacientes[i].citasAgendadas;
    }
    for (int i = 0; i < h->cantidadDoctores; i++) {
        if (h->doctores[i].pacientesAsignados) delete[] h->doctores[i].pacientesAsignados;
        if (h->doctores[i].citasAgendadas) delete[] h->doctores[i].citasAgendadas;
    }
    delete[] h->pacientes;
    delete[] h->doctores;
    delete[] h->citas;
    delete h;
}

// ====================== MENUS ======================

void mostrarMenuPrincipal() {
    cout << "\n╔════════════════════════════════════════╗\n";
    cout << "║   SISTEMA DE GESTIÓN HOSPITALARIA     ║\n";
    cout << "╚════════════════════════════════════════╝\n";
    cout << "1. Gestión de Pacientes\n2. Gestión de Doctores\n3. Gestión de Citas\n4. Listar Pacientes\n5. Listar Doctores\n6. Listar Citas Pendientes\n0. Salir\nElija una opción: ";
}

void menuPacientes(Hospital* h) {
    clearScreen();
    int op = -1;
    do {
        cout << "\n--- Menú Pacientes ---\n1. Registrar paciente\n2. Buscar paciente por cédula\n3. Buscar paciente por nombre\n4. Ver historial\n5. Actualizar paciente\n6. Listar todos\n7. Eliminar paciente\n0. Volver\nElija: ";
        op = leerEntero();
        if (op == -1) { cout << "Entrada invalida.\n"; continue; }
        if (op == 1) {
            char nombre[50], apellido[50], cedula[20]; int edad; char sexo;
            cout << "Nombre: "; leerLinea(nombre, 50);
            cout << "Apellido: "; leerLinea(apellido, 50);
            cout << "Cedula: "; leerLinea(cedula, 20);
            cout << "Edad: "; edad = leerEntero();
            cout << "Sexo (M/F): "; cin >> sexo; limpiarBufferEntrada();
            Paciente* p = crearPaciente(h, nombre, apellido, cedula, edad, sexo);
            if (p) cout << "Paciente creado, ID: " << p->id << "\n";
            else cout << "Error al crear paciente (cedula duplicada o datos invalidos).\n";
            pausePrompt();
            clearScreen();
        } else if (op == 2) {
            char ced[20]; cout << "Cedula: "; leerLinea(ced, 20);
            Paciente* p = buscarPacientePorCedula(h, ced);
            if (p) cout << "Encontrado: " << p->nombre << " " << p->apellido << "\n";
            else cout << "No encontrado.\n";
            pausePrompt();
            clearScreen();
        } else if (op == 3) {
            char nombre[50]; cout << "Nombre parcial: "; leerLinea(nombre, 50);
            int cnt; Paciente** res = buscarPacientesPorNombre(h, nombre, &cnt);
            if (!res) cout << "No se encontraron pacientes.\n";
            else {
                cout << "Resultados:\n";
                for (int i = 0; i < cnt; i++) cout << res[i]->id << " - " << res[i]->nombre << " " << res[i]->apellido << "\n";
                delete[] res;
            }
            pausePrompt();
            clearScreen();
        } else if (op == 4) {
            int id; cout << "ID paciente: "; id = leerEntero();
            Paciente* p = buscarPacientePorId(h, id);
            if (!p) cout << "No encontrado.\n";
            else mostrarHistorialMedico(p);
            pausePrompt();
            clearScreen();
        } else if (op == 5) {
            int id; cout << "ID paciente: "; id = leerEntero();
            if (!actualizarPaciente(h, id)) cout << "No se pudo actualizar.\n";
            pausePrompt();
            clearScreen();
        } else if (op == 6) {
            listarPacientes(h);
            pausePrompt();
            clearScreen();
        } else if (op == 7) {
            int id; cout << "ID a eliminar: "; id = leerEntero();
            if (eliminarPaciente(h, id)) cout << "Paciente eliminado.\n"; else cout << "No se pudo eliminar.\n";
            pausePrompt();
            clearScreen();
        } else if (op == 0) break;
        else cout << "Opcion invalida.\n";
    } while (op != 0);
    clearScreen();
}

void menuDoctores(Hospital* h) {
    clearScreen();
    int op = -1;
    do {
        cout << "\n--- Menú Doctores ---\n1. Registrar doctor\n2. Buscar doctor por ID\n3. Buscar doctores por especialidad\n4. Asignar paciente a doctor\n5. Mostrar pacientes asignados\n6. Listar doctores\n7. Eliminar doctor\n0. Volver\nElija: ";
        op = leerEntero();
        if (op == -1) { cout << "Entrada invalida.\n"; continue; }
        if (op == 1) {
            char nombre[50], apellido[50], cedula[20], especialidad[50]; int anios; float costo;
            cout << "Nombre: "; leerLinea(nombre, 50);
            cout << "Apellido: "; leerLinea(apellido, 50);
            cout << "Cedula profesional: "; leerLinea(cedula, 20);
            cout << "Especialidad: "; leerLinea(especialidad, 50);
            cout << "Anios experiencia: "; anios = leerEntero();
            cout << "Costo consulta: "; cin >> costo; limpiarBufferEntrada();
            Doctor* d = crearDoctor(h, nombre, apellido, cedula, especialidad, anios, costo);
            if (d) cout << "Doctor creado ID: " << d->id << "\n";
            else cout << "Error al crear doctor.\n";
            pausePrompt();
            clearScreen();
        } else if (op == 2) {
            int id; cout << "ID: "; id = leerEntero();
            Doctor* d = buscarDoctorPorId(h, id);
            if (d) cout << "Encontrado: " << d->nombre << " " << d->apellido << "\n"; else cout << "No encontrado.\n";
            pausePrompt();
            clearScreen();
        } else if (op == 3) {
            char esp[50]; cout << "Especialidad: "; leerLinea(esp, 50);
            int cnt; Doctor** res = buscarDoctoresPorEspecialidad(h, esp, &cnt);
            if (!res) cout << "No se encontraron.\n"; else {
                for (int i = 0; i < cnt; i++) cout << res[i]->id << " - " << res[i]->nombre << " " << res[i]->apellido << "\n";
                delete[] res;
            }
            pausePrompt();
            clearScreen();
        } else if (op == 4) {
            int idDoc, idPac; cout << "ID doctor: "; idDoc = leerEntero();
            cout << "ID paciente: "; idPac = leerEntero();
            Doctor* d = buscarDoctorPorId(h, idDoc);
            Paciente* p = buscarPacientePorId(h, idPac);
            if (!d || !p) cout << "Doctor o paciente no encontrados.\n";
            else if (asignarPacienteADoctor(d, idPac)) cout << "Asignado correctamente.\n";
            else cout << "Paciente ya asignado.\n";
            pausePrompt();
            clearScreen();
        } else if (op == 5) {
            int id; cout << "ID doctor: "; id = leerEntero();
            Doctor* d = buscarDoctorPorId(h, id);
            if (!d) cout << "Doctor no encontrado.\n";
            else {
                cout << "Pacientes asignados:\n";
                for (int i = 0; i < d->cantidadPacientes; i++) {
                    Paciente* p = buscarPacientePorId(h, d->pacientesAsignados[i]);
                    if (p) cout << p->id << " - " << p->nombre << " " << p->apellido << "\n";
                }
            }
            pausePrompt();
            clearScreen();
        } else if (op == 6) {
            listarDoctores(h);
            pausePrompt();
            clearScreen();
        } else if (op == 7) {
            int id; cout << "ID a eliminar: "; id = leerEntero();
            if (eliminarDoctor(h, id)) cout << "Doctor eliminado.\n"; else cout << "No se pudo eliminar.\n";
            pausePrompt();
            clearScreen();
        } else if (op == 0) break;
        else cout << "Opcion invalida.\n";
    } while (op != 0);
    clearScreen();
}

void menuCitas(Hospital* h) {
    clearScreen();
    int op = -1;
    do {
        cout << "\n--- Menú Citas ---\n1. Agendar cita\n2. Cancelar cita\n3. Atender cita\n4. Ver citas de paciente\n5. Ver citas de doctor\n6. Ver citas por fecha\n7. Ver citas pendientes\n0. Volver\nElija: ";
        op = leerEntero();
        if (op == -1) { cout << "Entrada invalida.\n"; continue; }
        if (op == 1) {
            int idPac, idDoc; char fecha[11], hora[6], motivo[150];
            cout << "ID paciente: "; idPac = leerEntero();
            cout << "ID doctor: "; idDoc = leerEntero();
            cout << "Fecha (YYYY-MM-DD): "; leerLinea(fecha, 11);
            cout << "Hora (HH:MM): "; leerLinea(hora, 6);
            cout << "Motivo: "; leerLinea(motivo, 150);
            Cita* c = agendarCita(h, idPac, idDoc, fecha, hora, motivo);
            if (c) cout << "Cita agendada ID: " << c->id << "\n"; else cout << "No se pudo agendar.\n";
            pausePrompt();
            clearScreen();
        } else if (op == 2) {
            int id; cout << "ID cita: "; id = leerEntero();
            if (cancelarCita(h, id)) cout << "Cita cancelada.\n"; else cout << "No se pudo cancelar.\n";
            pausePrompt();
            clearScreen();
        } else if (op == 3) {
            int id; cout << "ID cita: "; id = leerEntero();
            char diag[200], trat[200], meds[150];
            cout << "Diagnostico: "; leerLinea(diag, 200);
            cout << "Tratamiento: "; leerLinea(trat, 200);
            cout << "Medicamentos: "; leerLinea(meds, 150);
            if (atenderCita(h, id, diag, trat, meds)) cout << "Cita atendida.\n"; else cout << "No se pudo atender.\n";
            pausePrompt();
            clearScreen();
        } else if (op == 4) {
            int id; cout << "ID paciente: "; id = leerEntero();
            int cnt; Cita** res = obtenerCitasDePaciente(h, id, &cnt);
            if (!res) cout << "No hay citas.\n"; else {
                for (int i = 0; i < cnt; i++) cout << res[i]->id << " - " << res[i]->fecha << " " << res[i]->hora << " - " << res[i]->estado << "\n";
                delete[] res;
            }
            pausePrompt();
            clearScreen();
        } else if (op == 5) {
            int id; cout << "ID doctor: "; id = leerEntero();
            int cnt; Cita** res = obtenerCitasDeDoctor(h, id, &cnt);
            if (!res) cout << "No hay citas.\n"; else {
                for (int i = 0; i < cnt; i++) cout << res[i]->id << " - Paciente " << res[i]->idPaciente << " - " << res[i]->fecha << " " << res[i]->hora << " - " << res[i]->estado << "\n";
                delete[] res;
            }
            pausePrompt();
            clearScreen();
        } else if (op == 6) {
            char fecha[11]; cout << "Fecha (YYYY-MM-DD): "; leerLinea(fecha, 11);
            int cnt; Cita** res = obtenerCitasPorFecha(h, fecha, &cnt);
            if (!res) cout << "No hay citas.\n"; else {
                for (int i = 0; i < cnt; i++) cout << res[i]->id << " - Paciente " << res[i]->idPaciente << " - Doctor " << res[i]->idDoctor << " - " << res[i]->hora << " - " << res[i]->estado << "\n";
                delete[] res;
            }
            pausePrompt();
            clearScreen();
        } else if (op == 7) {
            listarCitasPendientes(h);
            pausePrompt();
            clearScreen();
        } else if (op == 0) break;
        else cout << "Opcion invalida.\n";
    } while (op != 0);
    clearScreen();
}

// ====================== MAIN ======================

int main() {
    setlocale(LC_ALL, "spanish");
    Hospital* h = crearHospital("Hospital Central", "Av. Principal", "0212-0000000");

    // Pre-carga mínima para pruebas
    crearDoctor(h, "Carlos", "Perez", "D-0001", "Cardiologia", 10, 50.0f);
    create:
    crearDoctor(h, "Ana", "Lopez", "D-0002", "Pediatria", 5, 35.0f);
    crearDoctor(h, "Luis", "Martinez", "D-0003", "Traumatologia", 8, 45.0f);

    crearPaciente(h, "Juan", "Perez", "V-100", 35, 'M');
    crearPaciente(h, "Maria", "Gonzalez", "V-101", 28, 'F');
    crearPaciente(h, "Juana", "Garcia", "V-102", 22, 'F');

    int opcion = -1;
    do {
        mostrarMenuPrincipal();
        opcion = leerEntero();
        if (opcion == -1) { cout << "Entrada invalida.\n"; continue; }
        switch (opcion) {
            case 1: menuPacientes(h); break;
            case 2: menuDoctores(h); break;
            case 3: menuCitas(h); break;
            case 4: listarPacientes(h); pausePrompt(); clearScreen(); break;
            case 5: listarDoctores(h); pausePrompt(); clearScreen(); break;
            case 6: listarCitasPendientes(h); pausePrompt(); clearScreen(); break;
            case 0: cout << "Saliendo...\n"; break;
            default: cout << "Opcion invalida.\n"; break;
        }
    } while (opcion != 0);

    destruirHospital(h);
    return 0;
}
