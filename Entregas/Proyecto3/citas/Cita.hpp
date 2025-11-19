#ifndef CITA_HPP
#define CITA_HPP

#include <cstring>
#include <ctime>

class Cita {
private:
    int id;
    int pacienteID;
    int doctorID;
    char fecha[11]; // YYYY-MM-DD
    char hora[6];   // HH:MM
    char motivo[150];
    char estado[20];
    char observaciones[200];
    bool atendida;
    int consultaID;
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaModificacion;

public:
    // Constructores
    Cita();
    Cita(int pacienteID, int doctorID, const char* fecha, const char* hora, const char* motivo);
    Cita(const Cita& other);
    
    // Getters
    int getId() const;
    int getPacienteID() const;
    int getDoctorID() const;
    const char* getFecha() const;
    const char* getHora() const;
    const char* getMotivo() const;
    const char* getEstado() const;
    const char* getObservaciones() const;
    bool getAtendida() const;
    int getConsultaID() const;
    bool getEliminado() const;
    time_t getFechaCreacion() const;
    time_t getFechaModificacion() const;
    
    // Setters con validaciones
    void setId(int id);
    void setPacienteID(int pacienteID);
    void setDoctorID(int doctorID);
    void setFecha(const char* fecha);
    void setHora(const char* hora);
    void setMotivo(const char* motivo);
    void setEstado(const char* estado);
    void setObservaciones(const char* observaciones);
    void setAtendida(bool atendida);
    void setConsultaID(int consultaID);
    void setEliminado(bool eliminado);
    
    // Métodos de validación
    bool validarFecha(const char* fecha) const;
    bool validarHora(const char* hora) const;
    bool validarDatos() const;
    
    // Métodos de estado
    bool estaAgendada() const;
    bool estaCancelada() const;
    bool estaAtendida() const;
    
    // Métodos de presentación
    void mostrarInformacionBasica() const;
    void mostrarInformacionCompleta() const;
    
    // Método estático para tamaño
    static size_t obtenerTamano();
};

#endif