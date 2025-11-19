// Doctor.hpp - clean class declaration

#ifndef DOCTOR_HPP
#define DOCTOR_HPP

#include <cstring>
#include <ctime>

class Doctor {
private:
    int id;
    bool eliminado;
    float costoConsulta;
    char nombre[50];
    char apellido[50];
    char cedula[20];
    char especialidad[50];
    int cantidadCitas;
    int citasID[20];
    time_t fechaCreacion;
    time_t fechaModificacion;

public:
    Doctor();
    Doctor(const char* n, const char* a, const char* c, const char* e, float costo);
    Doctor(const Doctor& o);

    int getId() const;
    const char* getNombre() const;
    const char* getApellido() const;
    const char* getCedula() const;
    const char* getEspecialidad() const;
    float getCostoConsulta() const;
    bool getEliminado() const;
    int getCantidadCitas() const;
    const int* getCitasID() const;

    time_t getFechaCreacion() const;
    time_t getFechaModificacion() const;

    void setId(int id);
    void setNombre(const char* n);
    void setApellido(const char* a);
    void setCedula(const char* c);
    void setEspecialidad(const char* e);
    void setCostoConsulta(float c);
    void setEliminado(bool e);

    bool agregarCitaID(int idCita);
    bool eliminarCitaID(int idCita);
    bool tieneCitaID(int idCita) const;

    void mostrarInformacionBasica() const;
    void mostrarInformacionCompleta() const;

    bool validarDatos() const;

    static size_t obtenerTamano();
};

#endif


