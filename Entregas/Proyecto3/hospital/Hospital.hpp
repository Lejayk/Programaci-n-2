#ifndef HOSPITAL_HPP
#define HOSPITAL_HPP

#include <cstring>
#include <ctime>

class Hospital {
private:
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

public:
    Hospital();
    Hospital(const char* nombre, const char* direccion, const char* telefono);
    
    // Getters
    const char* getNombre() const;
    const char* getDireccion() const;
    const char* getTelefono() const;
    int getSiguienteIDPaciente() const;
    int getSiguienteIDDoctor() const;
    int getSiguienteIDCita() const;
    int getSiguienteIDConsulta() const;
    int getTotalPacientesRegistrados() const;
    int getTotalDoctoresRegistrados() const;
    int getTotalCitasAgendadas() const;
    int getTotalConsultasRealizadas() const;
    
    // Setters
    void setNombre(const char* nombre);
    void setDireccion(const char* direccion);
    void setTelefono(const char* telefono);
    void setSiguienteIDPaciente(int id);
    void setSiguienteIDDoctor(int id);
    void setSiguienteIDCita(int id);
    void setSiguienteIDConsulta(int id);
    void setTotalPacientesRegistrados(int total);
    void setTotalDoctoresRegistrados(int total);
    void setTotalCitasAgendadas(int total);
    void setTotalConsultasRealizadas(int total);
    
    // Métodos de gestión de IDs
    int generarIDPaciente();
    int generarIDDoctor();
    int generarIDCita();
    int generarIDConsulta();
    
    // Métodos de estadísticas
    void incrementarPacientesRegistrados();
    void incrementarDoctoresRegistrados();
    void incrementarCitasAgendadas();
    void incrementarConsultasRealizadas();
    
    void mostrarInformacion() const;
    static size_t obtenerTamano();
};

#endif