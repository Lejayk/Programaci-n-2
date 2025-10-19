#include <iostream>
#include <cstring>
#include <iomanip>
#include <ctime>
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
void mostrarMenuPrincipal() {
    cout << "╔════════════════════════════════════════════╗\n";
    cout << "║      SISTEMA DE GESTIÓN HOSPITALARIA      ║\n";
    cout << "╠════════════════════════════════════════════╣\n";
    cout << "║ 1. Gestión de Pacientes                   ║\n";
    cout << "║ 2. Gestión de Doctores                    ║\n";
    cout << "║ 3. Gestión de Citas                       ║\n";
    cout << "║ 0. Salir                                  ║\n";
    cout << "╚════════════════════════════════════════════╝\n";
    cout << "Seleccione una opción: ";
}
Hospital* crearHospital(const char* nombreHospital, const char* direccion, const char* telefono) {
    Hospital* h = new Hospital; // Reserva de memoria dinámica para la estructura principal

    // Copia segura de los datos básicos
    strncpy(h->nombre, nombreHospital, sizeof(h->nombre) - 1);
    h->nombre[sizeof(h->nombre) - 1] = '\0';
    
    strncpy(h->direccion, direccion, sizeof(h->direccion) - 1);
    h->direccion[sizeof(h->direccion) - 1] = '\0';
    
    strncpy(h->telefono, telefono, sizeof(h->telefono) - 1);
    h->telefono[sizeof(h->telefono) - 1] = '\0';

    // Capacidades iniciales según requisitos del proyecto
    h->capacidadPacientes = 10;
    h->capacidadDoctores = 10;
    h->capacidadCitas = 20;

    // Inicializa contadores
    h->cantidadPacientes = 0;
    h->cantidadDoctores = 0;
    h->cantidadCitas = 0;

    // Inicializa contadores de ID automáticos
    h->siguienteIdPaciente = 1;
    h->siguienteIdDoctor = 1;
    h->siguienteIdCita = 1;
    h->siguienteIdConsulta = 1;

    // Reserva memoria para los arrays dinámicos principales
    h->pacientes = new Paciente[h->capacidadPacientes];
    h->doctores = new Doctor[h->capacidadDoctores];
    h->citas = new Cita[h->capacidadCitas];

    return h;

    bool ValidarCedula(const char* cedula) {
        if (!cedula) return false;
        int len = (int)strlen(cedula);
        if (len == 0 || len > 19) return false;
        return true;

}


int main(){
    Hospital* hospital = crearHospital("Hospital Bludsiano", "Av. PisPas Jhon Eate, Noreste", "777-7777");
    int opcion;

    do{
        mostrarMenuPrincipal();
        cin >> opcion;
        cin.ignore();

        switch(opcion){
            case 1:
                mostarDatosHospital(hospital);
                cout << "presione enter para continuar...";
                cin.get();
                break;
            case 2:

            case 0:
                cout << "Saliendo del programa..." << endl;
                break;
        }

    } while (opcion != 0);

    return 0;
}
