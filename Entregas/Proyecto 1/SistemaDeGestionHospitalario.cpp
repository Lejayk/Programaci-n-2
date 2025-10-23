#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <cctype>

using namespace std;

struct HistorialMedico {
    int idConsulta;
    char fecha[11];       
    char hora[6];        
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
    char estado[20]; 
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

// Constantes
const int CAP_PACIENTES_INICIAL = 10;
const int CAP_DOCTORES_INICIAL = 10;
const int CAP_CITAS_INICIAL = 20;
const int CAP_HISTORIAL_INICIAL = 5;
const int CAP_CITAS_PACIENTE_INICIAL = 5;
const int CAP_PACIENTOS_DOCTOR_INICIAL = 5;
const int CAP_CITAS_DOCTOR_INICIAL = 10;

// Declaraciones de funciones

// Implementación de funciones

void mostrarDatosHospital(Hospital* h) {
    system("pause");
    system("cls");
    cout << "\nDatos del Hospital:\n";
    cout << "Nombre: " << h->nombre << "\n";
    cout << "Direccion: " << h->direccion << "\n";
    cout << "Telefono: " << h->telefono << "\n";
    cout << "Cantidad de Pacientes: " << h->cantidadPacientes << "\n";
    cout << "Cantidad de Doctores: " << h->cantidadDoctores << "\n";
    cout << "Cantidad de Citas: " << h->cantidadCitas << "\n";
    system("pause");
    system("cls");
}

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
    return true;
}

// FUNCIÓN VALIDAR EMAIL CORREGIDA
bool validarEmail(const char* email) {
    if (!email || strlen(email) == 0) return false;
    
    // Longitud razonable para email
    if (strlen(email) > 50) return false;
    
    const char* at = strchr(email, '@');
    
    // Debe tener exactamente un '@'
    if (!at) return false;
    if (strchr(at + 1, '@')) return false; // Más de un '@'
    
    // El '@' no puede estar al inicio ni al final
    if (at == email) return false; // "@dominio.com"
    if (*(at + 1) == '\0') return false; // "usuario@"
    
    // Buscar punto después del '@'
    const char* dot = strchr(at, '.');
    if (!dot) return false;
    
    // Verificar que haya algo entre '@' y '.'
    if (dot == at + 1) return false; // "usuario@.com"
    
    // Verificar que haya algo después del último punto
    const char* lastDot = strrchr(email, '.');
    if (!lastDot || strlen(lastDot) <= 1) return false; // "usuario@dominio."
    
    // El dominio debe tener al menos 2 caracteres después del punto
    if (strlen(lastDot) < 3) return false; // "a.b" (mínimo 2 letras en TLD)
    
    // Verificar caracteres válidos en la parte local (antes del @)
    for (const char* p = email; p < at; p++) {
        if (!isalnum(*p) && *p != '.' && *p != '_' && *p != '-' && *p != '+') {
            return false;
        }
    }
    
    // Verificar que no empiece o termine con punto en la parte local
    if (email[0] == '.' || *(at - 1) == '.') return false;
    
    // Verificar caracteres válidos en el dominio (después del @)
    for (const char* p = at + 1; *p; p++) {
        if (!isalnum(*p) && *p != '.' && *p != '-') {
            return false;
        }
    }
    
    // Verificar que no haya dos puntos consecutivos
    for (const char* p = email; *p && *(p + 1); p++) {
        if (*p == '.' && *(p + 1) == '.') return false;
    }
    
    return true;
}

bool validarFecha(const char* fecha) {
    if (!fecha) return false;
    if (strlen(fecha) != 10) return false;
    for (int i=0;i<10;i++) {
        if (i==4 || i==7) {
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
    else if (m == 2) {
        diasMes = 28;
    }
    if (d < 1 || d > diasMes) return false;
    return true;
}

bool validarHora(const char* hora) {
    if (!hora) return false;
    if (strlen(hora) != 5) return false;
    if (hora[2] != ':') return false;
    for (int i=0;i<5;i++) {
        if (i==2) continue;
        if (hora[i] < '0' || hora[i] > '9') return false;
    }
    int hh = (hora[0]-'0')*10 + (hora[1]-'0');
    int mm = (hora[3]-'0')*10 + (hora[4]-'0');
    if (hh < 0 || hh > 23) return false;
    if (mm < 0 || mm > 59) return false;
    return true;
}

int compararFechas(const char* f1, const char* f2) {
    int cmp = strcmp(f1, f2);
    if (cmp < 0) return -1;
    if (cmp > 0) return 1;
    return 0;
}

Hospital* crearHospital(const char* nombreHospital, const char* direccion, const char* telefono) {
    Hospital* h = new Hospital;
    
    // Inicializar TODOS los campos
    strncpy(h->nombre, nombreHospital, sizeof(h->nombre)-1); 
    h->nombre[sizeof(h->nombre)-1] = '\0';
    strncpy(h->direccion, direccion, sizeof(h->direccion)-1); 
    h->direccion[sizeof(h->direccion)-1] = '\0';
    strncpy(h->telefono, telefono, sizeof(h->telefono)-1); 
    h->telefono[sizeof(h->telefono)-1] = '\0';

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

    // Inicializar arrays internos vacíos
    for (int i=0;i<h->capacidadPacientes;i++) {
        h->pacientes[i].historial = nullptr;
        h->pacientes[i].citasAgendadas = nullptr;
        h->pacientes[i].cantidadConsultas = 0;
        h->pacientes[i].capacidadHistorial = 0;
        h->pacientes[i].cantidadCitas = 0;
        h->pacientes[i].capacidadCitas = 0;
    }
    for (int i=0;i<h->capacidadDoctores;i++) {
        h->doctores[i].pacientesAsignados = nullptr;
        h->doctores[i].citasAgendadas = nullptr;
        h->doctores[i].cantidadPacientes = 0;
        h->doctores[i].capacidadPacientes = 0;
        h->doctores[i].cantidadCitas = 0;
        h->doctores[i].capacidadCitas = 0;
    }

    cout << "Hospital creado exitosamente.\n";
    return h;
}

void redimensionarArrayPacientes(Hospital* h) {
    int nueva = h->capacidadPacientes * 2;
    Paciente* arr = new Paciente[nueva];
    for (int i=0;i<h->cantidadPacientes;i++) arr[i] = h->pacientes[i];
    for (int i=h->cantidadPacientes;i<nueva;i++) {
        arr[i].historial = nullptr;
        arr[i].citasAgendadas = nullptr;
        arr[i].cantidadConsultas = 0;
        arr[i].capacidadHistorial = 0;
        arr[i].cantidadCitas = 0;
        arr[i].capacidadCitas = 0;
    }
    delete[] h->pacientes;
    h->pacientes = arr;
    h->capacidadPacientes = nueva;
}

void redimensionarArrayDoctores(Hospital* h) {
    int nueva = h->capacidadDoctores * 2;
    Doctor* arr = new Doctor[nueva];
    for (int i=0;i<h->cantidadDoctores;i++) arr[i] = h->doctores[i];
    for (int i=h->cantidadDoctores;i<nueva;i++) {
        arr[i].pacientesAsignados = nullptr;
        arr[i].citasAgendadas = nullptr;
        arr[i].cantidadPacientes = 0;
        arr[i].capacidadPacientes = 0;
        arr[i].cantidadCitas = 0;
        arr[i].capacidadCitas = 0;
    }
    delete[] h->doctores;
    h->doctores = arr;
    h->capacidadDoctores = nueva;
}

void redimensionarArrayCitas(Hospital* h) {
    int nueva = h->capacidadCitas * 2;
    Cita* arr = new Cita[nueva];
    for (int i=0;i<h->cantidadCitas;i++) arr[i] = h->citas[i];
    delete[] h->citas;
    h->citas = arr;
    h->capacidadCitas = nueva;
}

void inicializarPacienteEstructura(Paciente& p) {
    p.historial = new HistorialMedico[CAP_HISTORIAL_INICIAL];
    p.cantidadConsultas = 0;
    p.capacidadHistorial = CAP_HISTORIAL_INICIAL;
    p.citasAgendadas = new int[CAP_CITAS_PACIENTE_INICIAL];
    p.cantidadCitas = 0;
    p.capacidadCitas = CAP_CITAS_PACIENTE_INICIAL;
    // No sobrescribir alergias/observaciones aquí: podrían haberse proporcionado al crear
    p.activo = true;
}

Paciente* crearPaciente(Hospital* h, const char* nombre, const char* apellido, const char* cedula, 
                       int edad, char sexo, const char* tipoSangre, const char* telefono, 
                       const char* direccion, const char* email, const char* alergias, const char* observaciones) {
    if (!validarCedula(cedula)) {
        cout << "Error: Cédula inválida.\n";
        return nullptr;
    }
    if (edad < 0 || edad > 120) {
        cout << "Error: Edad debe estar entre 0 y 120 años.\n";
        return nullptr;
    }
    if (!(sexo=='M' || sexo == 'm' || sexo=='f' || sexo=='F')) {
        cout << "Error: Sexo debe ser M o F.\n";
        return nullptr;
    }
    
    // Verificar cedula única
    for (int i=0;i<h->cantidadPacientes;i++) {
        if (strcmp(h->pacientes[i].cedula, cedula) == 0) {
            cout << "Error: La cédula ya está registrada.\n";
            return nullptr;
        }
    }
    
    // Validar email si se proporciona
    if (email && email[0] != '\0') {
        if (!validarEmail(email)) {
            cout << "Error: Email inválido. Formato esperado: usuario@dominio.ext\n";
            return nullptr;
        }
    }
    
    if (h->cantidadPacientes >= h->capacidadPacientes) {
        redimensionarArrayPacientes(h);
    }
    
    int idx = h->cantidadPacientes;
    Paciente& p = h->pacientes[idx];
    p.id = h->siguienteIdPaciente++;
    strncpy(p.nombre, nombre, sizeof(p.nombre)-1); 
    p.nombre[sizeof(p.nombre)-1] = '\0';
    strncpy(p.apellido, apellido, sizeof(p.apellido)-1); 
    p.apellido[sizeof(p.apellido)-1] = '\0';
    strncpy(p.cedula, cedula, sizeof(p.cedula)-1); 
    p.cedula[sizeof(p.cedula)-1] = '\0';
    p.edad = edad;
    p.sexo = toupper(sexo);
    
    // Copiar campos opcionales si se proporcionan
    if (tipoSangre && tipoSangre[0] != '\0') {
        strncpy(p.tipoSangre, tipoSangre, sizeof(p.tipoSangre)-1);
        p.tipoSangre[sizeof(p.tipoSangre)-1] = '\0';
    } else {
        p.tipoSangre[0] = '\0';
    }
    
    if (telefono && telefono[0] != '\0') {
        strncpy(p.telefono, telefono, sizeof(p.telefono)-1);
        p.telefono[sizeof(p.telefono)-1] = '\0';
    } else {
        p.telefono[0] = '\0';
    }
    
    if (direccion && direccion[0] != '\0') {
        strncpy(p.direccion, direccion, sizeof(p.direccion)-1);
        p.direccion[sizeof(p.direccion)-1] = '\0';
    } else {
        p.direccion[0] = '\0';
    }
    
    if (email && email[0] != '\0') {
        strncpy(p.email, email, sizeof(p.email)-1);
        p.email[sizeof(p.email)-1] = '\0';
    } else {
        p.email[0] = '\0';
    }
    // Copiar alergias y observaciones proporcionadas
    if (alergias && alergias[0] != '\0') {
        strncpy(p.alergias, alergias, sizeof(p.alergias)-1);
        p.alergias[sizeof(p.alergias)-1] = '\0';
    } else {
        p.alergias[0] = '\0';
    }
    if (observaciones && observaciones[0] != '\0') {
        strncpy(p.observaciones, observaciones, sizeof(p.observaciones)-1);
        p.observaciones[sizeof(p.observaciones)-1] = '\0';
    } else {
        p.observaciones[0] = '\0';
    }

    inicializarPacienteEstructura(p);
    h->cantidadPacientes++;
    
    cout << "Paciente creado exitosamente. ID: " << p.id << "\n";
    return &h->pacientes[idx];
}

Paciente* buscarPacientePorId(Hospital* h, int id) {
    for (int i=0;i<h->cantidadPacientes;i++) {
        if (h->pacientes[i].id == id) {
            return &h->pacientes[i];
        }
    }
    return nullptr;
}

Paciente* buscarPacientePorCedula(Hospital* h, const char* cedula) {
    for (int i=0;i<h->cantidadPacientes;i++) {
        if (strcmp(h->pacientes[i].cedula, cedula) == 0) {
            return &h->pacientes[i];
        }
    }
    return nullptr;
}

Paciente** buscarPacientesPorNombre(Hospital* h, const char* nombre, int* cantidad) {
    *cantidad = 0;
    for (int i=0;i<h->cantidadPacientes;i++) {
        char full[120];
        full[0] = '\0';
        strncat(full, h->pacientes[i].nombre, sizeof(full)-1);
        strncat(full, " ", sizeof(full)-1);
        strncat(full, h->pacientes[i].apellido, sizeof(full)-1);
        if (strstr(full, nombre) != nullptr) (*cantidad)++;
    }
    if (*cantidad == 0) return nullptr;
    Paciente** res = new Paciente*[*cantidad];
    int pos = 0;
    for (int i=0;i<h->cantidadPacientes;i++) {
        char full[120]; 
        full[0] = '\0';
        strncat(full, h->pacientes[i].nombre, sizeof(full)-1);
        strncat(full, " ", sizeof(full)-1);
        strncat(full, h->pacientes[i].apellido, sizeof(full)-1);
        if (strstr(full, nombre) != nullptr) {
            res[pos++] = &h->pacientes[i];
        }
    }
    return res;
}

bool actualizarPaciente(Hospital* h, int id) {
    Paciente* p = buscarPacientePorId(h, id);
    if (!p) {
        cout << "Paciente no encontrado.\n";
        return false;
    }
    
    char buffer[256];
    cout << "Dejar vacio para no cambiar.\n";
    
    cout << "Telefono actual: " << p->telefono << "\nNuevo telefono: ";
    cin.ignore(256, '\n');
    leerLinea(buffer, 15);
    if (strlen(buffer) > 0) { 
        strncpy(p->telefono, buffer, 14); 
        p->telefono[14] = '\0'; 
    }
    
    cout << "Direccion actual: " << p->direccion << "\nNueva direccion: ";
    leerLinea(buffer, 100);
    if (strlen(buffer) > 0) { 
        strncpy(p->direccion, buffer, 99); 
        p->direccion[99] = '\0'; 
    }
    
    cout << "Email actual: " << p->email << "\nNuevo email: ";
    leerLinea(buffer, 50);
    if (strlen(buffer) > 0) {
        if (validarEmail(buffer)) {
            strncpy(p->email, buffer, 49); 
            p->email[49] = '\0';
            cout << "Email actualizado correctamente.\n";
        } else {
            cout << "Error: Email inválido. Formato esperado: usuario@dominio.ext\n";
            cout << "El email no se actualizó.\n";
        }
    }
    
    cout << "Alergias actuales: " << p->alergias << "\nNuevas alergias: ";
    leerLinea(buffer, 500);
    if (strlen(buffer) > 0) { 
        strncpy(p->alergias, buffer, 499); 
        p->alergias[499] = '\0'; 
    }
    
    cout << "Observaciones actuales: " << p->observaciones << "\nNuevas observaciones: ";
    leerLinea(buffer, 500);
    if (strlen(buffer) > 0) { 
        strncpy(p->observaciones, buffer, 499); 
        p->observaciones[499] = '\0'; 
    }
    
    cout << "Paciente actualizado correctamente.\n";
    return true;
}

bool eliminarPaciente(Hospital* h, int id) {
    int idx = -1;
    for (int i=0;i<h->cantidadPacientes;i++) {
        if (h->pacientes[i].id == id) { 
            idx = i; 
            break; 
        }
    }
    if (idx == -1) {
        cout << "Paciente no encontrado.\n";
        return false;
    }
    
    Paciente& p = h->pacientes[idx];

    // Cancelar citas asociadas y remover referencias de doctores
    for (int i=0;i<h->cantidadCitas;i++) {
        if (h->citas[i].idPaciente == id && strcmp(h->citas[i].estado,"Agendada")==0) {
            strcpy(h->citas[i].estado, "Cancelada");
            h->citas[i].atendida = false;
            // remover de doctor
            Doctor* d = nullptr;
            for (int j=0;j<h->cantidadDoctores;j++) {
                if (h->doctores[j].id == h->citas[i].idDoctor) { 
                    d = &h->doctores[j]; 
                    break; 
                }
            }
            if (d) {
                for (int k=0;k<d->cantidadCitas;k++) {
                    if (d->citasAgendadas[k] == h->citas[i].id) {
                        for (int m=k;m<d->cantidadCitas-1;m++) {
                            d->citasAgendadas[m] = d->citasAgendadas[m+1];
                        }
                        d->cantidadCitas--;
                        break;
                    }
                }
            }
        }
    }
    
    // remover de doctores asignados
    for (int i=0;i<h->cantidadDoctores;i++) {
        Doctor& d = h->doctores[i];
        for (int j=0;j<d.cantidadPacientes;j++) {
            if (d.pacientesAsignados[j] == id) {
                for (int k=j;k<d.cantidadPacientes-1;k++) {
                    d.pacientesAsignados[k] = d.pacientesAsignados[k+1];
                }
                d.cantidadPacientes--;
                break;
            }
        }
    }

    // liberar memoria interna del paciente
    if (p.historial) delete[] p.historial;
    if (p.citasAgendadas) delete[] p.citasAgendadas;

    // compactar array
    for (int i=idx;i<h->cantidadPacientes-1;i++) {
        h->pacientes[i] = h->pacientes[i+1];
    }
    h->cantidadPacientes--;
    
    cout << "Paciente eliminado correctamente.\n";
    return true;
}

void listarPacientes(Hospital* h) {
    if (h->cantidadPacientes == 0) {
        cout << "No hay pacientes registrados.\n";
        return;
    }
    cout << "\nID  Nombre                Cedula         Edad  TS   Telefono        Email                         Direccion                         Consultas\n";
    cout << "-----------------------------------------------------------------------------------------------------------------------------\n";
    for (int i=0;i<h->cantidadPacientes;i++) {
        Paciente& p = h->pacientes[i];
        char full[120]; 
        full[0]='\0';
        strncat(full, p.nombre, sizeof(full)-1);
        strncat(full, " ", sizeof(full)-1);
        strncat(full, p.apellido, sizeof(full)-1);
        cout << setw(3) << p.id << "  " << setw(20) << full << "  " << setw(12) << p.cedula
             << "  " << setw(4) << p.edad << "  " << setw(4) << p.tipoSangre
             << "  " << setw(14) << p.telefono << "  " << setw(28) << p.email
             << "  " << setw(30) << p.direccion << "  " << setw(8) << p.cantidadConsultas << "\n";
        if (p.alergias[0] != '\0') {
            cout << "     Alergias: " << p.alergias << "\n";
        }
        if (p.observaciones[0] != '\0') {
            cout << "     Observaciones: " << p.observaciones << "\n";
        }
    }
}

void agregarConsultaAlHistorial(Paciente* paciente, HistorialMedico consulta) {
    if (paciente->cantidadConsultas >= paciente->capacidadHistorial) {
        int nueva = paciente->capacidadHistorial * 2;
        HistorialMedico* arr = new HistorialMedico[nueva];
        for (int i=0;i<paciente->cantidadConsultas;i++) {
            arr[i] = paciente->historial[i];
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
    if (paciente->cantidadConsultas == 0) {
        cout << "No hay consultas en el historial.\n";
        return;
    }
    
    cout << "\nHistorial de " << paciente->nombre << " " << paciente->apellido << " (ID " << paciente->id << ")\n";
    cout << "ID   Fecha       Hora  Doctor  Costo   Diagnostico\n";
    cout << "---------------------------------------------------------------\n";
    for (int i=0;i<paciente->cantidadConsultas;i++) {
        HistorialMedico& h = paciente->historial[i];
        cout << setw(4) << h.idConsulta << "  " << setw(10) << h.fecha << "  " << setw(5) << h.hora 
             << "  " << setw(6) << h.idDoctor << "  " << setw(6) << fixed << setprecision(2) << h.costo 
             << "  " << h.diagnostico << "\n";
    }
}

HistorialMedico* obtenerUltimaConsulta(Paciente* paciente) {
    if (paciente->cantidadConsultas == 0) return nullptr;
    return &paciente->historial[paciente->cantidadConsultas - 1];
}

void inicializarDoctorEstructura(Doctor& d) {
    d.pacientesAsignados = new int[CAP_PACIENTOS_DOCTOR_INICIAL];
    d.cantidadPacientes = 0;
    d.capacidadPacientes = CAP_PACIENTOS_DOCTOR_INICIAL;
    d.citasAgendadas = new int[CAP_CITAS_DOCTOR_INICIAL];
    d.cantidadCitas = 0;
    d.capacidadCitas = CAP_CITAS_DOCTOR_INICIAL;
    d.disponible = true;
}

Doctor* crearDoctor(Hospital* h, const char* nombre, const char* apellido, const char* cedula,
                   const char* especialidad, int anios, float costo) {
    if (!validarCedula(cedula)) {
        cout << "Error: Cédula profesional inválida.\n";
        return nullptr;
    }
    if (anios < 0) {
        cout << "Error: Años de experiencia no pueden ser negativos.\n";
        return nullptr;
    }
    if (costo <= 0.0f) {
        cout << "Error: Costo de consulta debe ser mayor a 0.\n";
        return nullptr;
    }
    
    // Verificar cedula única
    for (int i=0;i<h->cantidadDoctores;i++) {
        if (strcmp(h->doctores[i].cedula, cedula) == 0) {
            cout << "Error: La cédula profesional ya está registrada.\n";
            return nullptr;
        }
    }
    
    if (h->cantidadDoctores >= h->capacidadDoctores) {
        redimensionarArrayDoctores(h);
    }
    
    int idx = h->cantidadDoctores;
    Doctor& d = h->doctores[idx];
    d.id = h->siguienteIdDoctor++;
    strncpy(d.nombre, nombre, sizeof(d.nombre)-1); 
    d.nombre[sizeof(d.nombre)-1] = '\0';
    strncpy(d.apellido, apellido, sizeof(d.apellido)-1); 
    d.apellido[sizeof(d.apellido)-1] = '\0';
    strncpy(d.cedula, cedula, sizeof(d.cedula)-1); 
    d.cedula[sizeof(d.cedula)-1] = '\0';
    strncpy(d.especialidad, especialidad, sizeof(d.especialidad)-1); 
    d.especialidad[sizeof(d.especialidad)-1] = '\0';
    d.aniosExperiencia = anios;
    d.costoConsulta = costo;
    d.horarioAtencion[0] = '\0';
    d.telefono[0] = '\0';
    d.email[0] = '\0';
    inicializarDoctorEstructura(d);
    h->cantidadDoctores++;
    
    cout << "Doctor creado exitosamente. ID: " << d.id << "\n";
    return &h->doctores[idx];
}

Doctor* buscarDoctorPorId(Hospital* h, int id) {
    for (int i=0;i<h->cantidadDoctores;i++) {
        if (h->doctores[i].id == id) {
            return &h->doctores[i];
        }
    }
    return nullptr;
}

Doctor* buscarDoctorPorCedula(Hospital* h, const char* cedula) {
    for (int i=0;i<h->cantidadDoctores;i++) {
        if (strcmp(h->doctores[i].cedula, cedula) == 0) {
            return &h->doctores[i];
        }
    }
    return nullptr;
}

Doctor** buscarDoctoresPorNombre(Hospital* h, const char* nombre, int* cantidad) {
    *cantidad = 0;
    for (int i=0;i<h->cantidadDoctores;i++) {
        char full[120];
        full[0] = '\0';
        strncat(full, h->doctores[i].nombre, sizeof(full)-1);
        strncat(full, " ", sizeof(full)-1);
        strncat(full, h->doctores[i].apellido, sizeof(full)-1);
        if (strstr(full, nombre) != nullptr) (*cantidad)++;
    }
    if (*cantidad == 0) return nullptr;
    Doctor** res = new Doctor*[*cantidad];
    int pos = 0;
    for (int i=0;i<h->cantidadDoctores;i++) {
        char full[120];
        full[0] = '\0';
        strncat(full, h->doctores[i].nombre, sizeof(full)-1);
        strncat(full, " ", sizeof(full)-1);
        strncat(full, h->doctores[i].apellido, sizeof(full)-1);
        if (strstr(full, nombre) != nullptr) {
            res[pos++] = &h->doctores[i];
        }
    }
    return res;
}

Doctor** buscarDoctoresPorEspecialidad(Hospital* h, const char* especialidad, int* cantidad) {
    *cantidad = 0;
    for (int i=0;i<h->cantidadDoctores;i++) {
        if (strcmp(h->doctores[i].especialidad, especialidad) == 0) (*cantidad)++;
    }
    if (*cantidad == 0) return nullptr;
    Doctor** res = new Doctor*[*cantidad];
    int pos = 0;
    for (int i=0;i<h->cantidadDoctores;i++) {
        if (strcmp(h->doctores[i].especialidad, especialidad) == 0) {
            res[pos++] = &h->doctores[i];
        }
    }
    return res;
}

bool asignarPacienteADoctor(Doctor* d, int idPaciente) {
    for (int i=0;i<d->cantidadPacientes;i++) {
        if (d->pacientesAsignados[i] == idPaciente) {
            cout << "El paciente ya está asignado a este doctor.\n";
            return false;
        }
    }
    
    if (d->cantidadPacientes >= d->capacidadPacientes) {
        int nueva = d->capacidadPacientes * 2;
        int* arr = new int[nueva];
        for (int i=0;i<d->cantidadPacientes;i++) {
            arr[i] = d->pacientesAsignados[i];
        }
        delete[] d->pacientesAsignados;
        d->pacientesAsignados = arr;
        d->capacidadPacientes = nueva;
    }
    d->pacientesAsignados[d->cantidadPacientes++] = idPaciente;
    cout << "Paciente asignado correctamente al doctor.\n";
    return true;
}

bool removerPacienteDeDoctor(Doctor* d, int idPaciente) {
    for (int i=0;i<d->cantidadPacientes;i++) {
        if (d->pacientesAsignados[i] == idPaciente) {
            for (int k=i;k<d->cantidadPacientes-1;k++) {
                d->pacientesAsignados[k] = d->pacientesAsignados[k+1];
            }
            d->cantidadPacientes--;
            cout << "Paciente removido correctamente del doctor.\n";
            return true;
        }
    }
    cout << "Paciente no encontrado en la lista del doctor.\n";
    return false;
}

void listarDoctores(Hospital* h) {
    if (h->cantidadDoctores == 0) {
        cout << "No hay doctores registrados.\n";
        return;
    }
    cout << "\nListado de doctores:\n";
    cout << "---------------------------------------------------------------\n";
    for (int i=0;i<h->cantidadDoctores;i++) {
        Doctor& d = h->doctores[i];
        char full[200];
        full[0] = '\0';
        strncat(full, d.nombre, sizeof(full)-1);
        strncat(full, " ", sizeof(full)-1);
        strncat(full, d.apellido, sizeof(full)-1);
        cout << "ID: " << d.id << " - " << full << "\n";
        cout << "  Cedula: " << d.cedula << "  Especialidad: " << d.especialidad << "\n";
        cout << "  Anios experiencia: " << d.aniosExperiencia << "  Costo: " << fixed << setprecision(2) << d.costoConsulta << "\n";
        if (d.telefono[0] != '\0') cout << "  Telefono: " << d.telefono << "\n";
        if (d.email[0] != '\0') cout << "  Email: " << d.email << "\n";
        if (d.horarioAtencion[0] != '\0') cout << "  Horario: " << d.horarioAtencion << "\n";
        cout << "  Pacientes asignados: " << d.cantidadPacientes << "  Citas agendadas: " << d.cantidadCitas << "  Disponible: " << (d.disponible ? "Si" : "No") << "\n";
        cout << "---------------------------------------------------------------\n";
    }
}

bool eliminarDoctor(Hospital* h, int id) {
    int idx = -1;
    for (int i=0;i<h->cantidadDoctores;i++) {
        if (h->doctores[i].id == id) { 
            idx = i; 
            break; 
        }
    }
    if (idx == -1) {
        cout << "Doctor no encontrado.\n";
        return false;
    }
    
    Doctor& d = h->doctores[idx];
    
    // cancelar citas asociadas
    for (int i=0;i<h->cantidadCitas;i++) {
        if (h->citas[i].idDoctor == id && strcmp(h->citas[i].estado,"Agendada")==0) {
            strcpy(h->citas[i].estado, "Cancelada");
            h->citas[i].atendida = false;
            // remover de paciente
            Paciente* p = nullptr;
            for (int j=0;j<h->cantidadPacientes;j++) {
                if (h->pacientes[j].id == h->citas[i].idPaciente) { 
                    p = &h->pacientes[j]; 
                    break; 
                }
            }
            if (p) {
                for (int k=0;k<p->cantidadCitas;k++) {
                    if (p->citasAgendadas[k] == h->citas[i].id) {
                        for (int m=k;m<p->cantidadCitas-1;m++) {
                            p->citasAgendadas[m] = p->citasAgendadas[m+1];
                        }
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
    for (int i=idx;i<h->cantidadDoctores-1;i++) {
        h->doctores[i] = h->doctores[i+1];
    }
    h->cantidadDoctores--;
    
    cout << "Doctor eliminado correctamente.\n";
    return true;
}

bool verificarDisponibilidad(Hospital* h, int idDoctor, const char* fecha, const char* hora) {
    for (int i=0;i<h->cantidadCitas;i++) {
        if (h->citas[i].idDoctor == idDoctor && strcmp(h->citas[i].estado,"Agendada")==0) {
            if (strcmp(h->citas[i].fecha, fecha) == 0 && strcmp(h->citas[i].hora, hora) == 0) {
                return false;
            }
        }
    }
    return true;
}

Cita* agendarCita(Hospital* h, int idPaciente, int idDoctor, const char* fecha, 
                  const char* hora, const char* motivo) {
    Paciente* p = buscarPacientePorId(h, idPaciente);
    if (!p) {
        cout << "Error: Paciente no encontrado.\n";
        return nullptr;
    }
    
    Doctor* d = buscarDoctorPorId(h, idDoctor);
    if (!d) {
        cout << "Error: Doctor no encontrado.\n";
        return nullptr;
    }
    
    if (!validarFecha(fecha)) {
        cout << "Error: Fecha inválida. Formato: YYYY-MM-DD\n";
        return nullptr;
    }
    
    if (!validarHora(hora)) {
        cout << "Error: Hora inválida. Formato: HH:MM\n";
        return nullptr;
    }
    
    if (!verificarDisponibilidad(h, idDoctor, fecha, hora)) {
        cout << "Error: El doctor no está disponible en esa fecha y hora.\n";
        return nullptr;
    }
    
    if (h->cantidadCitas >= h->capacidadCitas) {
        redimensionarArrayCitas(h);
    }
    
    int idx = h->cantidadCitas;
    Cita& c = h->citas[idx];
    c.id = h->siguienteIdCita++;
    c.idPaciente = idPaciente;
    c.idDoctor = idDoctor;
    strncpy(c.fecha, fecha, 10); 
    c.fecha[10] = '\0';
    strncpy(c.hora, hora, 5); 
    c.hora[5] = '\0';
    strncpy(c.motivo, motivo, sizeof(c.motivo)-1); 
    c.motivo[sizeof(c.motivo)-1] = '\0';
    strcpy(c.estado, "Agendada");
    c.observaciones[0] = '\0';
    c.atendida = false;
    
    // agregar al paciente
    if (p->cantidadCitas >= p->capacidadCitas) {
        int nueva = p->capacidadCitas * 2;
        int* arr = new int[nueva];
        for (int i=0;i<p->cantidadCitas;i++) {
            arr[i] = p->citasAgendadas[i];
        }
        delete[] p->citasAgendadas;
        p->citasAgendadas = arr;
        p->capacidadCitas = nueva;
    }
    p->citasAgendadas[p->cantidadCitas++] = c.id;
    
    // agregar al doctor
    if (d->cantidadCitas >= d->capacidadCitas) {
        int nueva = d->capacidadCitas * 2;
        int* arr = new int[nueva];
        for (int i=0;i<d->cantidadCitas;i++) {
            arr[i] = d->citasAgendadas[i];
        }
        delete[] d->citasAgendadas;
        d->citasAgendadas = arr;
        d->capacidadCitas = nueva;
    }
    d->citasAgendadas[d->cantidadCitas++] = c.id;
    
    h->cantidadCitas++;
    cout << "Cita agendada exitosamente. ID: " << c.id << "\n";
    return &h->citas[idx];
}

bool cancelarCita(Hospital* h, int idCita) {
    int idx = -1;
    for (int i=0;i<h->cantidadCitas;i++) {
        if (h->citas[i].id == idCita) { 
            idx = i; 
            break; 
        }
    }
    if (idx == -1) {
        cout << "Cita no encontrada.\n";
        return false;
    }
    
    Cita& c = h->citas[idx];
    if (strcmp(c.estado,"Cancelada") == 0) {
        cout << "La cita ya está cancelada.\n";
        return false;
    }
    
    strcpy(c.estado, "Cancelada");
    c.atendida = false;
    
    // remover de paciente
    Paciente* p = buscarPacientePorId(h, c.idPaciente);
    if (p) {
        for (int i=0;i<p->cantidadCitas;i++) {
            if (p->citasAgendadas[i] == idCita) {
                for (int k=i;k<p->cantidadCitas-1;k++) {
                    p->citasAgendadas[k] = p->citasAgendadas[k+1];
                }
                p->cantidadCitas--;
                break;
            }
        }
    }
    
    // remover de doctor
    Doctor* d = buscarDoctorPorId(h, c.idDoctor);
    if (d) {
        for (int i=0;i<d->cantidadCitas;i++) {
            if (d->citasAgendadas[i] == idCita) {
                for (int k=i;k<d->cantidadCitas-1;k++) {
                    d->citasAgendadas[k] = d->citasAgendadas[k+1];
                }
                d->cantidadCitas--;
                break;
            }
        }
    }
    
    cout << "Cita cancelada correctamente.\n";
    return true;
}

bool atenderCita(Hospital* h, int idCita, const char* diagnostico, 
                 const char* tratamiento, const char* medicamentos) {
    Cita* c = nullptr;
    for (int i=0;i<h->cantidadCitas;i++) {
        if (h->citas[i].id == idCita) { 
            c = &h->citas[i]; 
            break; 
        }
    }
    if (!c) {
        cout << "Cita no encontrada.\n";
        return false;
    }
    
    if (strcmp(c->estado,"Agendada") != 0) {
        cout << "Error: La cita no está en estado 'Agendada'.\n";
        return false;
    }
    
    strcpy(c->estado, "Atendida");
    c->atendida = true;
    
    HistorialMedico hm;
    hm.idConsulta = h->siguienteIdConsulta++;
    strncpy(hm.fecha, c->fecha, 10); 
    hm.fecha[10] = '\0';
    strncpy(hm.hora, c->hora, 5); 
    hm.hora[5] = '\0';
    strncpy(hm.diagnostico, diagnostico, sizeof(hm.diagnostico)-1); 
    hm.diagnostico[sizeof(hm.diagnostico)-1] = '\0';
    strncpy(hm.tratamiento, tratamiento, sizeof(hm.tratamiento)-1); 
    hm.tratamiento[sizeof(hm.tratamiento)-1] = '\0';
    strncpy(hm.medicamentos, medicamentos, sizeof(hm.medicamentos)-1); 
    hm.medicamentos[sizeof(hm.medicamentos)-1] = '\0';
    hm.idDoctor = c->idDoctor;
    
    Doctor* d = buscarDoctorPorId(h, c->idDoctor);
    hm.costo = (d ? d->costoConsulta : 0.0f);
    
    Paciente* p = buscarPacientePorId(h, c->idPaciente);
    if (!p) {
        cout << "Error: Paciente no encontrado.\n";
        return false;
    }
    
    agregarConsultaAlHistorial(p, hm);
    cout << "Cita atendida y agregada al historial correctamente.\n";
    return true;
}

Cita** obtenerCitasDePaciente(Hospital* h, int idPaciente, int* cantidad) {
    *cantidad = 0;
    for (int i=0;i<h->cantidadCitas;i++) {
        if (h->citas[i].idPaciente == idPaciente) (*cantidad)++;
    }
    if (*cantidad == 0) return nullptr;
    Cita** res = new Cita*[*cantidad];
    int pos = 0;
    for (int i=0;i<h->cantidadCitas;i++) {
        if (h->citas[i].idPaciente == idPaciente) {
            res[pos++] = &h->citas[i];
        }
    }
    return res;
}

Cita** obtenerCitasDeDoctor(Hospital* h, int idDoctor, int* cantidad) {
    *cantidad = 0;
    for (int i=0;i<h->cantidadCitas;i++) {
        if (h->citas[i].idDoctor == idDoctor) (*cantidad)++;
    }
    if (*cantidad == 0) return nullptr;
    Cita** res = new Cita*[*cantidad];
    int pos = 0;
    for (int i=0;i<h->cantidadCitas;i++) {
        if (h->citas[i].idDoctor == idDoctor) {
            res[pos++] = &h->citas[i];
        }
    }
    return res;
}

Cita** obtenerCitasPorFecha(Hospital* h, const char* fecha, int* cantidad) {
    *cantidad = 0;
    for (int i=0;i<h->cantidadCitas;i++) {
        if (strcmp(h->citas[i].fecha, fecha) == 0) (*cantidad)++;
    }
    if (*cantidad == 0) return nullptr;
    Cita** res = new Cita*[*cantidad];
    int pos = 0;
    for (int i=0;i<h->cantidadCitas;i++) {
        if (strcmp(h->citas[i].fecha, fecha) == 0) {
            res[pos++] = &h->citas[i];
        }
    }
    return res;
}

void listarCitasPendientes(Hospital* h) {
    int contador = 0;
    cout << "\nCitas pendientes:\nID  Paciente  Doctor  Fecha       Hora  Motivo\n";
    cout << "------------------------------------------------\n";
    for (int i=0;i<h->cantidadCitas;i++) {
        Cita& c = h->citas[i];
        if (strcmp(c.estado,"Agendada")==0) {
            cout << setw(3) << c.id << "  " << setw(8) << c.idPaciente << "  " 
                 << setw(6) << c.idDoctor << "  " << setw(10) << c.fecha << "  " 
                 << setw(5) << c.hora << "  " << c.motivo << "\n";
            contador++;
        }
    }
    if (contador == 0) {
        cout << "No hay citas pendientes.\n";
    }
}

// FUNCIÓN DESTRUIR HOSPITAL CORREGIDA
void destruirHospital(Hospital*& h) {
    if (!h) {
        cout << "El hospital ya ha sido destruido.\n";
        return;
    }
    
    cout << "Destruyendo hospital...\n";
    
    // 1. Liberar memoria interna de pacientes
    for (int i = 0; i < h->cantidadPacientes; i++) {
        Paciente& p = h->pacientes[i];
        if (p.historial) {
            delete[] p.historial;
            p.historial = nullptr;
        }
        if (p.citasAgendadas) {
            delete[] p.citasAgendadas;
            p.citasAgendadas = nullptr;
        }
    }
    
    // 2. Liberar memoria interna de doctores
    for (int i = 0; i < h->cantidadDoctores; i++) {
        Doctor& d = h->doctores[i];
        if (d.pacientesAsignados) {
            delete[] d.pacientesAsignados;
            d.pacientesAsignados = nullptr;
        }
        if (d.citasAgendadas) {
            delete[] d.citasAgendadas;
            d.citasAgendadas = nullptr;
        }
    }
    
    // 3. Liberar arrays principales
    if (h->pacientes) {
        delete[] h->pacientes;
        h->pacientes = nullptr;
    }
    if (h->doctores) {
        delete[] h->doctores;
        h->doctores = nullptr;
    }
    if (h->citas) {
        delete[] h->citas;
        h->citas = nullptr;
    }
    
    // 4. Finalmente liberar el hospital
    delete h;
    h = nullptr;  // ¡IMPORTANTE!!!! Demasiado importante: Establecer a nullptr (media hora de errores por no hacerlo)
    
    cout << "Hospital destruido completamente.\n";
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

void mostrarMenuPrincipal() {
    cout << "\n----------------------------------------\n";
    cout << "|   SISTEMA DE GESTION HOSPITALARIO   |\n";
    cout << "----------------------------------------\n";
    cout << "1. Gestion de Pacientes\n";
    cout << "2. Gestion de Doctores\n";
    cout << "3. Gestion de Citas\n";
    cout << "4. Mostrar Datos del Hospital\n";
    cout << "5. Opciones Avanzadas\n";
    cout << "0. Salir\n";
    cout << "Elija una opcion: ";
}

void menuOpcionesAvanzadas(Hospital*& h) {
    int op = -1;
    system("pause");
    system("cls");
    cout << "\n--- Menu Opciones Avanzadas ---\n";
    cout << "1. Destruir Hospital\n";
    cout << "0. Volver\n";
    cout << "Elija: ";
    op = leerEntero();
    if (op == 1) {
        destruirHospital(h);
        cout << "Hospital destruido exitosamente.\n";
    } else if (op != 0) {
        cout << "Opcion invalida.\n";
    }
}

void menuPacientes(Hospital* h) {
    int op = -1;
    do {
        system("pause");
        system("cls");
        cout << "\n--- Menu Pacientes ---\n";
        cout << "1. Registrar paciente\n";
        cout << "2. Buscar paciente por cedula\n";
        cout << "3. Buscar paciente por nombre\n";
        cout << "4. Buscar paciente por ID\n";
        cout << "5. Ver historial\n";
        cout << "6. Actualizar paciente\n";
        cout << "7. Listar todos\n";
        cout << "8. Eliminar paciente\n";
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
                cout << "Cedula: "; 
                leerLinea(cedula, 20);
                cout << "Edad: "; 
                edad = leerEntero();
                if (edad == -1) {
                    cout << "Edad inválida.\n";
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
                crearPaciente(h, nombre, apellido, cedula, edad, sexo, tipoSangre, telefono, direccion, email, alergias, observaciones);
                break;
            }
            case 2: {
                char ced[20]; 
                cout << "Cedula: "; 
                limpiarBufferEntrada(); 
                leerLinea(ced, 20);
                Paciente* p = buscarPacientePorCedula(h, ced);
                if (p) {
                    cout << "Encontrado: " << p->nombre << " " << p->apellido << " (ID " << p->id << ")\n";
                    cout << "Cedula: " << p->cedula << "\n";
                    cout << "Edad: " << p->edad << "  Sexo: " << p->sexo << "\n";
                    cout << "Telefono: " << p->telefono << "\n";
                    cout << "Direccion: " << p->direccion << "\n";
                    cout << "Email: " << p->email << "\n";
                    cout << "Alergias: " << p->alergias << "\n";
                    cout << "Observaciones: " << p->observaciones << "\n";
                } else {
                    cout << "No encontrado.\n";
                }
                break;
            }
            case 3: {
                char nombre[50]; 
                cout << "Nombre parcial: "; 
                limpiarBufferEntrada(); 
                leerLinea(nombre, 50);
                int cnt; 
                Paciente** res = buscarPacientesPorNombre(h, nombre, &cnt);
                if (!res) {
                    cout << "No se encontraron pacientes.\n";
                } else {
                    cout << "Resultados:\n";
                    for (int i=0;i<cnt;i++) {
                        Paciente* p = res[i];
                        cout << p->id << " - " << p->nombre << " " << p->apellido << "\n";
                        cout << "  Cedula: " << p->cedula << "  Edad: " << p->edad << "  Sexo: " << p->sexo << "\n";
                        cout << "  Telefono: " << p->telefono << "\n";
                        cout << "  Direccion: " << p->direccion << "\n";
                        cout << "  Email: " << p->email << "\n";
                        cout << "  Alergias: " << p->alergias << "\n";
                        cout << "  Observaciones: " << p->observaciones << "\n";
                        cout << "---------------------------\n";
                    }
                    delete[] res;
                }
                break;
            }
            case 4: {
                int id; 
                cout << "ID paciente: "; 
                id = leerEntero();
                Paciente* p = buscarPacientePorId(h, id);
                if (!p) {
                    cout << "Paciente no encontrado.\n";
                } else {
                    cout << "ID: " << p->id << "\n";
                    cout << "Nombre: " << p->nombre << " " << p->apellido << "\n";
                    cout << "Cedula: " << p->cedula << "\n";
                    cout << "Edad: " << p->edad << "  Sexo: " << p->sexo << "\n";
                    cout << "Tipo sangre: " << p->tipoSangre << "\n";
                    cout << "Telefono: " << p->telefono << "\n";
                    cout << "Direccion: " << p->direccion << "\n";
                    cout << "Email: " << p->email << "\n";
                    cout << "Consultas: " << p->cantidadConsultas << "\n";
                    cout << "Alergias: " << p->alergias << "\n";
                    cout << "Observaciones: " << p->observaciones << "\n";
                    cout << "Activo: " << (p->activo ? "Si" : "No") << "\n";
                }
                break;
            }
            case 5: {
                int id; 
                cout << "ID paciente: "; 
                id = leerEntero();
                Paciente* p = buscarPacientePorId(h, id);
                if (!p) {
                    cout << "No encontrado.\n";
                } else {
                    mostrarHistorialMedico(p);
                }
                break;
            }
            case 6: {
                int id; 
                cout << "ID paciente: "; 
                id = leerEntero();
                if (!actualizarPaciente(h, id)) {
                    cout << "No se pudo actualizar.\n";
                }
                break;
            }
            case 7: {
                listarPacientes(h);
                break;
            }
            case 8: {
                int id; 
                cout << "ID a eliminar: "; 
                id = leerEntero();
                if (!eliminarPaciente(h, id)) {
                    cout << "No se pudo eliminar.\n";
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
        cout << "\n--- Menu Doctores ---\n";
    cout << "1. Registrar doctor\n";
    cout << "2. Buscar doctor por ID\n";
    cout << "3. Buscar doctor por cedula\n";
    cout << "4. Buscar doctores por nombre\n";
    cout << "5. Buscar doctores por especialidad\n";
    cout << "6. Asignar paciente a doctor\n";
    cout << "7. Mostrar pacientes asignados\n";
    cout << "8. Listar doctores\n";
    cout << "9. Eliminar doctor\n";
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
                    cout << "Años de experiencia inválidos.\n";
                    break;
                }
                cout << "Costo consulta: "; 
                cin >> costo;
                // Limpiar buffer y leer campos opcionales
                limpiarBufferEntrada();
                cout << "Telefono (opcional): ";
                char telefonoDoc[15]; leerLinea(telefonoDoc, 15);
                cout << "Email (opcional): ";
                char emailDoc[50]; leerLinea(emailDoc, 50);
                cout << "Horario atencion (opcional): ";
                char horarioDoc[50]; leerLinea(horarioDoc, 50);
                Doctor* nuevo = crearDoctor(h, nombre, apellido, cedula, especialidad, anios, costo);
                if (nuevo) {
                    if (telefonoDoc[0] != '\0') {
                        strncpy(nuevo->telefono, telefonoDoc, sizeof(nuevo->telefono)-1);
                        nuevo->telefono[sizeof(nuevo->telefono)-1] = '\0';
                    }
                    if (emailDoc[0] != '\0') {
                        if (validarEmail(emailDoc)) {
                            strncpy(nuevo->email, emailDoc, sizeof(nuevo->email)-1);
                            nuevo->email[sizeof(nuevo->email)-1] = '\0';
                        } else {
                            cout << "Email doctor invalido, no se guardo.\n";
                        }
                    }
                    if (horarioDoc[0] != '\0') {
                        strncpy(nuevo->horarioAtencion, horarioDoc, sizeof(nuevo->horarioAtencion)-1);
                        nuevo->horarioAtencion[sizeof(nuevo->horarioAtencion)-1] = '\0';
                    }
                }
                break;
            }
            case 2: {
                int id; 
                cout << "ID: "; 
                id = leerEntero();
                Doctor* d = buscarDoctorPorId(h, id);
                if (d) {
                    cout << "ID: " << d->id << "\n";
                    cout << "Nombre: " << d->nombre << " " << d->apellido << "\n";
                    cout << "Cedula: " << d->cedula << "\n";
                    cout << "Especialidad: " << d->especialidad << "\n";
                    cout << "Anios experiencia: " << d->aniosExperiencia << "\n";
                    cout << "Costo consulta: " << fixed << setprecision(2) << d->costoConsulta << "\n";
                    cout << "Telefono: " << d->telefono << "\n";
                    cout << "Email: " << d->email << "\n";
                    cout << "Horario: " << d->horarioAtencion << "\n";
                    cout << "Pacientes asignados: " << d->cantidadPacientes << "\n";
                    cout << "Citas agendadas: " << d->cantidadCitas << "\n";
                    cout << "Disponible: " << (d->disponible ? "Si" : "No") << "\n";
                } else {
                    cout << "No encontrado.\n";
                }
                break;
            }
            case 3: {
                // buscador por cedula
                char ced[20];
                cout << "Cedula profesional: "; limpiarBufferEntrada(); leerLinea(ced, 20);
                Doctor* dced = buscarDoctorPorCedula(h, ced);
                if (dced) {
                    cout << "ID: " << dced->id << " - " << dced->nombre << " " << dced->apellido << "\n";
                    cout << "  Especialidad: " << dced->especialidad << "  Anios: " << dced->aniosExperiencia << "\n";
                    cout << "  Telefono: " << dced->telefono << "  Email: " << dced->email << "  Horario: " << dced->horarioAtencion << "\n";
                } else {
                    cout << "No encontrado.\n";
                }
                break;
            }
            case 4: {
                // buscador por nombre parcial
                char nombreBus[50];
                cout << "Nombre parcial: "; limpiarBufferEntrada(); leerLinea(nombreBus, 50);
                int cntN;
                Doctor** resN = buscarDoctoresPorNombre(h, nombreBus, &cntN);
                if (!resN) {
                    cout << "No se encontraron doctores.\n";
                } else {
                    for (int i=0;i<cntN;i++) {
                        Doctor* dn = resN[i];
                        cout << dn->id << " - " << dn->nombre << " " << dn->apellido << "\n";
                        cout << "  Cedula: " << dn->cedula << "  Especialidad: " << dn->especialidad << "\n";
                        cout << "  Telefono: " << dn->telefono << "  Email: " << dn->email << "  Horario: " << dn->horarioAtencion << "\n";
                        cout << "---------------------------\n";
                    }
                    delete[] resN;
                }
                break;
            }
            case 5: {
                char esp[50]; 
                cout << "Especialidad: "; 
                limpiarBufferEntrada(); 
                leerLinea(esp, 50);
                int cnt; 
                Doctor** res = buscarDoctoresPorEspecialidad(h, esp, &cnt);
                if (!res) {
                    cout << "No se encontraron.\n";
                } else {
                    for (int i=0;i<cnt;i++) {
                        cout << res[i]->id << " - " << res[i]->nombre << " " << res[i]->apellido << "\n";
                    }
                    delete[] res;
                }
                break;
            }
            case 6: {
                int idDoc, idPac; 
                cout << "ID doctor: "; 
                idDoc = leerEntero();
                cout << "ID paciente: "; 
                idPac = leerEntero();
                Doctor* d = buscarDoctorPorId(h, idDoc);
                Paciente* p = buscarPacientePorId(h, idPac);
                if (!d || !p) {
                    cout << "Doctor o paciente no encontrados.\n";
                } else if (!asignarPacienteADoctor(d, idPac)) {
                    cout << "No se pudo asignar el paciente.\n";
                }
                break;
            }
            case 7: {
                int id; 
                cout << "ID doctor: "; 
                id = leerEntero();
                Doctor* d = buscarDoctorPorId(h, id);
                if (!d) {
                    cout << "Doctor no encontrado.\n";
                } else {
                    cout << "Pacientes asignados:\n";
                    for (int i=0;i<d->cantidadPacientes;i++) {
                        Paciente* p = buscarPacientePorId(h, d->pacientesAsignados[i]);
                        if (p) {
                            cout << p->id << " - " << p->nombre << " " << p->apellido << "\n";
                        }
                    }
                }
                break;
            }
            case 8: {
                listarDoctores(h);
                break;
            }
            case 9: {
                int id; 
                cout << "ID a eliminar: "; 
                id = leerEntero();
                if (!eliminarDoctor(h, id)) {
                    cout << "No se pudo eliminar.\n";
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

void menuCitas(Hospital* h) {
    int op = -1;
    do {
        system("pause");
        system("cls");
        cout << "\n--- Menu Citas ---\n";
        cout << "1. Agendar cita\n";
        cout << "2. Cancelar cita\n";
        cout << "3. Atender cita\n";
        cout << "4. Ver citas de paciente\n";
        cout << "5. Ver citas de doctor\n";
        cout << "6. Ver citas por fecha\n";
        cout << "7. Ver citas pendientes\n";
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
                cout << "ID paciente: "; 
                idPac = leerEntero();
                cout << "ID doctor: "; 
                idDoc = leerEntero();
                cout << "Fecha (YYYY-MM-DD): "; 
                limpiarBufferEntrada(); 
                leerLinea(fecha, 11);
                cout << "Hora (HH:MM): "; 
                leerLinea(hora, 6);
                cout << "Motivo: "; 
                leerLinea(motivo, 150);
                agendarCita(h, idPac, idDoc, fecha, hora, motivo);
                break;
            }
            case 2: {
                int id; 
                cout << "ID cita: "; 
                id = leerEntero();
                if (!cancelarCita(h, id)) {
                    cout << "No se pudo cancelar.\n";
                }
                break;
            }
            case 3: {
                int id; 
                cout << "ID cita: "; 
                id = leerEntero();
                char diag[200], trat[200], meds[150];
                cout << "Diagnostico: "; 
                limpiarBufferEntrada(); 
                leerLinea(diag, 200);
                cout << "Tratamiento: "; 
                leerLinea(trat, 200);
                cout << "Medicamentos: "; 
                leerLinea(meds, 150);
                if (!atenderCita(h, id, diag, trat, meds)) {
                    cout << "No se pudo atender.\n";
                }
                break;
            }
            case 4: {
                int id; 
                cout << "ID paciente: "; 
                id = leerEntero();
                int cnt; 
                Cita** res = obtenerCitasDePaciente(h, id, &cnt);
                if (!res) {
                    cout << "No hay citas.\n";
                } else {
                    for (int i=0;i<cnt;i++) {
                        cout << res[i]->id << " - " << res[i]->fecha << " " << res[i]->hora << " - " << res[i]->estado << "\n";
                    }
                    delete[] res;
                }
                break;
            }
            case 5: {
                int id; 
                cout << "ID doctor: "; 
                id = leerEntero();
                int cnt; 
                Cita** res = obtenerCitasDeDoctor(h, id, &cnt);
                if (!res) {
                    cout << "No hay citas.\n";
                } else {
                    for (int i=0;i<cnt;i++) {
                        cout << res[i]->id << " - Paciente " << res[i]->idPaciente << " - " << res[i]->fecha << " " << res[i]->hora << " - " << res[i]->estado << "\n";
                    }
                    delete[] res;
                }
                break;
            }
            case 6: {
                char fecha[11]; 
                cout << "Fecha (YYYY-MM-DD): "; 
                limpiarBufferEntrada(); 
                leerLinea(fecha, 11);
                int cnt; 
                Cita** res = obtenerCitasPorFecha(h, fecha, &cnt);
                if (!res) {
                    cout << "No hay citas.\n";
                } else {
                    for (int i=0;i<cnt;i++) {
                        cout << res[i]->id << " - Paciente " << res[i]->idPaciente << " - Doctor " << res[i]->idDoctor << " - " << res[i]->hora << " - " << res[i]->estado << "\n";
                    }
                    delete[] res;
                }
                break;
            }
            case 7: {
                listarCitasPendientes(h);
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

int main() {
    setlocale(LC_ALL, "spanish");
    

    Hospital* h = crearHospital("Hospital SALTO ANGEL", "Av. Secundaria #777", "0412-0000000");

    int opcion = -1;
    do {
        // Verificar si el hospital existe
        if (!h) {
            cout << "\nEl hospital ha sido destruido. ¿Desea crear uno nuevo? (1: Sí, 0: Salir): ";
            int respuesta = leerEntero();
            if (respuesta == 1) {
                h = crearHospital("Hospital Renacido", "Nueva Dirección", "0212-1111111");
                cout << "Nuevo hospital creado.\n";
            } else {
                opcion = 0;
            }
            continue;
        }
        
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
                menuOpcionesAvanzadas(h); 
                break;
            case 0: 
                cout << "Saliendo...\n"; 
                break;
            default: 
                cout << "Opcion invalida.\n"; 
                break;
        }
    } while (opcion != 0);

    // Solo destruir si aún existe
    if (h) {
        destruirHospital(h);
    }
    
    return 0;
}