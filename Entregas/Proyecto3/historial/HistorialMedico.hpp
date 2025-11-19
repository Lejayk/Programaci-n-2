#ifndef HISTORIAL_MEDICO_HPP
#define HISTORIAL_MEDICO_HPP

#include <ctime>

class HistorialMedico {
private:
    int id;
    int idPaciente;
    int idDoctor; 

    char descripcion[300];
    char diagnostico[200];
    char tratamiento[200];

    int dia;
    int mes;
    int anio;

    bool eliminado;
    time_t fechaCreacion;
    time_t fechaModificacion;

public:
    HistorialMedico();
    HistorialMedico(int idPaciente, int idDoctor);

    int getId() const;
    int getIdPaciente() const;
    int getIdDoctor() const;
    const char* getDescripcion() const;
    const char* getDiagnostico() const;
    const char* getTratamiento() const;
    int getDia() const;
    int getMes() const;
    int getAnio() const;
    bool getEliminado() const;

    void setId(int id);
    void setDescripcion(const char* d);
    void setDiagnostico(const char* diag);
    void setTratamiento(const char* t);
    void setFecha(int d, int m, int a);
    void setEliminado(bool e);

    bool validarDatos() const;

    void mostrarInformacionBasica() const;
    void mostrarInformacionCompleta() const;

    static size_t obtenerTamano();
};

#endif
