// test_script.cpp
// Script de pruebas automático para hospital_fixed.cpp
// Compilar con: g++ -std=c++11 hospital_fixed.cpp test_script.cpp -o hospital_test

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cassert>
using namespace std;

// ------------------- Declaraciones (coinciden con hospital_fixed.cpp) -------------------

// Estructuras (deben coincidir exactamente con las definiciones en hospital_fixed.cpp)
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
    int capacidadCitas; // nombre del campo no usado aquí
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

// ------------------- Prototipos de funciones (implementadas en hospital_fixed.cpp) -------------------
// Declaraciones con enlace C++ normal (coincidir con la implementación en hospital_fixed.cpp)
// Inicialización / destrucción
Hospital* crearHospital(const char* nombre, const char* direccion, const char* telefono);
void destruirHospital(Hospital* h);

// Pacientes
Paciente* crearPaciente(Hospital* h, const char* nombre, const char* apellido, const char* cedula, int edad, char sexo);
Paciente* buscarPacientePorId(Hospital* h, int id);
Paciente* buscarPacientePorCedula(Hospital* h, const char* cedula);
Paciente** buscarPacientesPorNombre(Hospital* h, const char* nombre, int* cantidad);
bool eliminarPaciente(Hospital* h, int id);

// Doctores
Doctor* crearDoctor(Hospital* h, const char* nombre, const char* apellido, const char* cedula, const char* especialidad, int anios, float costo);
Doctor* buscarDoctorPorId(Hospital* h, int id);
Doctor** buscarDoctoresPorEspecialidad(Hospital* h, const char* especialidad, int* cantidad);
bool asignarPacienteADoctor(Doctor* d, int idPaciente);

// Citas
Cita* agendarCita(Hospital* h, int idPaciente, int idDoctor, const char* fecha, const char* hora, const char* motivo);
bool atenderCita(Hospital* h, int idCita, const char* diagnostico, const char* tratamiento, const char* medicamentos);
bool cancelarCita(Hospital* h, int idCita);

// Para verificar disponibilidad (no estrictamente necesario)
bool verificarDisponibilidad(Hospital* h, int idDoctor, const char* fecha, const char* hora);

// ------------------- Helpers del script -------------------

void printHeader(const char* s) {
    cout << "\n==== " << s << " ====\n";
}

bool espera(bool cond, const char* msgOk, const char* msgFail) {
    if (cond) {
        cout << "[PASS] " << msgOk << "\n";
        return true;
    } else {
        cout << "[FAIL] " << msgFail << "\n";
        return false;
    }
}

// ------------------- Pruebas -------------------

int main() {
    int fallos = 0;
    printHeader("Creando hospital");
    Hospital* h = crearHospital("Hospital Test", "Calle Test", "0000");
    if (!h) { cout << "No se pudo crear hospital\n"; return 1; }
    cout << "Hospital creado. CapPacientes inicial: " << h->capacidadPacientes << " CapDoctores inicial: " << h->capacidadDoctores << " CapCitas inicial: " << h->capacidadCitas << "\n";

    // 1) Crear más de 10 pacientes (debe forzar resize)
    printHeader("Creando 12 pacientes (forzar redimensionamiento de pacientes)");
    for (int i = 0; i < 12; ++i) {
        char nom[32], ape[32], ced[32];
        sprintf(nom, "Pnom%d", i+1);
        sprintf(ape, "Pape%d", i+1);
        sprintf(ced, "V-%03d", 200 + i);
        Paciente* p = crearPaciente(h, nom, ape, ced, 30 + (i%10), (i%2==0)?'M':'F');
        if (!p) {
            cout << "Fallo al crear paciente " << i+1 << " (cedula " << ced << ")\n";
            fallos++;
        }
    }
    bool t1 = h->cantidadPacientes >= 12 && h->capacidadPacientes > 10;
    if (!espera(t1, "Redimensionamiento de pacientes OK", "Redimensionamiento de pacientes FALLÓ")) fallos++;

    // 2) Crear 6 doctores (forzar redimensionamiento de doctores si hace falta)
    printHeader("Creando 6 doctores");
    for (int i = 0; i < 6; ++i) {
        char nom[32], ape[32], ced[32], esp[32];
        sprintf(nom, "Dnom%d", i+1);
        sprintf(ape, "Dape%d", i+1);
        sprintf(ced, "DR-%03d", 100 + i);
        sprintf(esp, (i%3==0)?"Cardiologia":(i%3==1)?"Pediatria":"Traumatologia");
        Doctor* d = crearDoctor(h, nom, ape, ced, esp, 5 + i, 30.0f + i*5.0f);
        if (!d) {
            cout << "Fallo al crear doctor " << i+1 << "\n";
            fallos++;
        }
    }
    bool t2 = h->cantidadDoctores >= 6 && h->capacidadDoctores >= 10;
    if (!espera(t2, "Creación doctores OK", "Creación doctores FALLÓ")) fallos++;

    // 3) Agendar 22 citas (forzar redimensionamiento de citas)
    printHeader("Agendando 22 citas para forzar resize de citas");
    int createdCitas = 0;
    int np = h->cantidadPacientes;
    int nd = h->cantidadDoctores;
    for (int i = 0; i < 22; ++i) {
        int pid = (i % np) + 1; // IDs auto-incrementales empiezan en 1
        int did = (i % nd) + 1;
        char fecha[11], hora[6], motivo[64];
        sprintf(fecha, "2025-11-%02d", (i%28)+1);
        sprintf(hora, "%02d:%02d", 8 + (i%8), (i%2)*15);
        sprintf(motivo, "Consulta %d", i+1);
        Cita* c = agendarCita(h, pid, did, fecha, hora, motivo);
        if (c) createdCitas++;
        else cout << "No se pudo agendar cita " << i+1 << " (paciente " << pid << " doctor " << did << ")\n";
    }
    bool t3 = h->cantidadCitas >= 22 && h->capacidadCitas > 20;
    if (!espera(t3, "Agendado y redimensionamiento de citas OK", "Agendado/redimensionamiento de citas FALLÓ")) fallos++;

    // 4) Atender al menos 6 citas de un mismo paciente para forzar expandir historial
    printHeader("Atender 6 citas para un mismo paciente (forzar historial)");
    int targetPacienteId = 1;
    int atendidas = 0;
    // buscar citas del paciente 1
    for (int i = 0; i < h->cantidadCitas && atendidas < 6; ++i) {
        if (h->citas[i].idPaciente == targetPacienteId) {
            bool ok = atenderCita(h, h->citas[i].id, "Dx test", "Tratamiento test", "Med test");
            if (ok) atendidas++;
        }
    }
    // buscar paciente y comprobar cantidadConsultas
    Paciente* paciente1 = buscarPacientePorId(h, targetPacienteId);
    bool t4 = (paciente1 != nullptr) && (paciente1->cantidadConsultas >= atendidas);
    if (!espera(t4, "Atención de citas e historial OK", "Atención de citas/historial FALLÓ")) fallos++;

    // 5) Asignar >5 pacientes a un doctor para forzar redimensionamiento pacientes asignados
    printHeader("Asignar >5 pacientes a Doctor ID 1");
    Doctor* doc1 = buscarDoctorPorId(h, 1);
    bool t5 = true;
    if (!doc1) { cout << "Doctor 1 no encontrado\n"; t5 = false; }
    else {
        // intentar asignar pacientes 1..7 (ignorar duplicados)
        for (int pid = 1; pid <= 7; ++pid) {
            asignarPacienteADoctor(doc1, pid);
        }
        if (doc1->cantidadPacientes <= 5) t5 = false;
    }
    if (!espera(t5, "Asignación de pacientes a doctor y resize OK", "Asignación de pacientes FALLÓ")) fallos++;

    // 6) Buscar paciente por cédula y por nombre parcial (case-insensitive) y doctores por especialidad
    printHeader("Pruebas de búsqueda (cedula/nombre/especialidad)");
    // cédula del primer paciente creado: V-200
    Paciente* pced = buscarPacientePorCedula(h, "V-200");
    bool t6a = (pced != nullptr);
    if (!espera(t6a, "Buscar por cédula OK", "Buscar por cédula FALLÓ")) fallos++;

    int cnt = 0;
    Paciente** resNombre = buscarPacientesPorNombre(h, "Pnom1", &cnt);
    bool t6b = (resNombre != nullptr && cnt >= 1);
    if (!espera(t6b, "Buscar por nombre parcial OK", "Buscar por nombre parcial FALLÓ")) fallos++;
    if (resNombre) delete[] resNombre;

    int cntD = 0;
    Doctor** resDoc = buscarDoctoresPorEspecialidad(h, "Cardiologia", &cntD);
    bool t6c = (resDoc != nullptr && cntD >= 1);
    if (!espera(t6c, "Buscar doctores por especialidad OK", "Buscar doctores por especialidad FALLÓ")) fallos++;
    if (resDoc) delete[] resDoc;

    // 7) Cancelar una cita y comprobar removido de listas de paciente/doctor (comprobamos que estado cambió)
    printHeader("Cancelar una cita (si existe)");
    int anyCitaId = -1;
    if (h->cantidadCitas > 0) anyCitaId = h->citas[0].id;
    if (anyCitaId != -1) {
        cancelarCita(h, anyCitaId);
        // buscar cita y verificar estado "Cancelada"
        bool foundCancel = false;
        for (int i = 0; i < h->cantidadCitas; ++i) {
            if (h->citas[i].id == anyCitaId && strcmp(h->citas[i].estado, "Cancelada") == 0) { foundCancel = true; break; }
        }
    if (!espera(foundCancel, "Cancelar cita OK", "Cancelar cita FALLÓ")) { fallos++; }
    } else {
        cout << "No hay citas para cancelar (saltando test)\n";
    }

    // 8) Eliminar un paciente y verificar que disminuye cantidadPacientes
    printHeader("Eliminar paciente ID 2");
    int antes = h->cantidadPacientes;
    bool okElim = eliminarPaciente(h, 2);
    bool t8 = okElim && (h->cantidadPacientes == antes - 1);
    if (!espera(t8, "Eliminar paciente OK", "Eliminar paciente FALLÓ")) fallos++;

    // 9) Prueba final: destruir hospital y no crash
    printHeader("Destruyendo hospital");
    destruirHospital(h);
    cout << "Hospital destruido (si no hay crash, OK)\n";

    // Resumen
    cout << "\n================== RESUMEN DE PRUEBAS ==================\n";
    if (fallos == 0) {
        cout << "TODAS LAS PRUEBAS PASARON CORRECTAMENTE ✅\n";
        return 0;
    } else {
        cout << "PRUEBAS FALLIDAS: " << fallos << " ❌\n";
        return 1;
    }
}
