#ifndef HISTORIAL_MEDICO_HPP
#define HISTORIAL_MEDICO_HPP

#include <cstring>
#include <ctime>

class HistorialMedico {
private:
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

public:
    // Constructores
    HistorialMedico();
    HistorialMedico(int pacienteID, int doctorID, const char* diagnostico, const char* tratamiento, const char* medicamentos, float costo);
    HistorialMedico(const HistorialMedico& other);
    
    // Getters
    int getId() const;
    int getPacienteID() const;
    const char* getFecha() const;
    const char* getHora() const;
    const char* getDiagnostico() const;
    const char* getTratamiento() const;
    const char* getMedicamentos() const;
    int getDoctorID() const;
    float getCosto() const;
    int getSiguienteConsultaID() const;
    bool getEliminado() const;
    time_t getFechaRegistro() const;
    
    // Setters con validaciones
    void setId(int id);
    void setPacienteID(int pacienteID);
    void setDoctorID(int doctorID);
    void setFecha(const char* fecha);
    void setHora(const char* hora);
    void setDiagnostico(const char* diagnostico);
    void setTratamiento(const char* tratamiento);
    void setMedicamentos(const char* medicamentos);
    void setCosto(float costo);
    void setSiguienteConsultaID(int siguienteConsultaID);
    void setEliminado(bool eliminado);
    
    // Métodos de validación
    bool validarDatos() const;
    bool validarFecha(const char* fecha) const;
    bool validarHora(const char* hora) const;
    
    // Métodos de presentación
    void mostrarInformacionBasica() const;
    void mostrarInformacionCompleta() const;
    
    // Método estático para tamaño
    static size_t obtenerTamano();
};

#endif