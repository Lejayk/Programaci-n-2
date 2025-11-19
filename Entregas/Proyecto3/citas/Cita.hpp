#ifndef CITA_HPP
#define CITA_HPP

#include <ctime>

class Cita {
private:
    int id;
    int idPaciente;
    int idDoctor;
    int dia;
    int mes;
    int anio;
    int hora; 
    int minuto;
    char motivo[100];
    bool atendida;
    bool eliminada;
    time_t fechaCreacion;
    time_t fechaModificacion;

public:
    Cita();
    Cita(int idPaciente, int idDoctor, const char* motivo);

    int getId() const;
    int getIdPaciente() const;
    int getIdDoctor() const;
    int getDia() const;
    int getMes() const;
    int getAnio() const;
    int getHora() const;
    int getMinuto() const;
    const char* getMotivo() const;
    bool getAtendida() const;
    bool getEliminada() const;
    // Compatibility alias used by GestorArchivos
    bool getEliminado() const;

    void setId(int);
    void setFecha(int d, int m, int a, int h, int min);
    void setMotivo(const char* m);
    void setAtendida(bool a);
    void setEliminada(bool e);
    // Compatibility alias used by GestorArchivos
    void setEliminado(bool e);

    void mostrarInformacionBasica() const;
    void mostrarInformacionCompleta() const;

    bool validarDatos() const;

    static size_t obtenerTamano();
};

#endif
