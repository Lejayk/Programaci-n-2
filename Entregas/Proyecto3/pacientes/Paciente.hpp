#ifndef PACIENTE_HPP
#define PACIENTE_HPP

#include <cstring>
#include <ctime>

class Paciente {
private:
    int id;
    char nombre[50];
    char apellido[50];
    char cedula[20];
    int edad;
    char sexo;            // M / F / O
    char tipoSangre[5];   // A+ A- B+ B- AB+ AB- O+ O-
    char telefono[20];
    char email[50];
    char direccion[80];
    char alergias[100];
    char observaciones[200];

    int cantidadCitas;
    int citasID[20];

    // Contador de consultas e indice a la primera consulta (lista enlazada)
    int cantidadConsultas;
    int primerConsultaID;

    bool eliminado;
    time_t fechaCreacion;
    time_t fechaModificacion;

public:
    // Constructores
    Paciente();
    Paciente(const char* nombre, const char* apellido, const char* cedula);
    Paciente(const Paciente& other);

    // Getters
    int getId() const;
    const char* getNombre() const;
    const char* getApellido() const;
    const char* getCedula() const;
    int getEdad() const;
    char getSexo() const;
    const char* getTipoSangre() const;
    const char* getTelefono() const;
    const char* getEmail() const;
    const char* getDireccion() const;
    const char* getAlergias() const;
    const char* getObservaciones() const;

    int getCantidadCitas() const;
    const int* getCitasID() const;
    bool getEliminado() const;
    time_t getFechaCreacion() const;
    time_t getFechaModificacion() const;

    // Setters
    void setId(int id);
    void setNombre(const char* nombre);
    void setApellido(const char* apellido);
    void setCedula(const char* cedula);
    void setEdad(int edad);
    void setSexo(char sexo);
    void setTipoSangre(const char* tipo);
    void setTelefono(const char* telefono);
    void setEmail(const char* email);
    void setDireccion(const char* direccion);
    void setAlergias(const char* alergias);
    void setObservaciones(const char* obs);

    // Gestión de citas
    bool agregarCitaID(int idCita);
    bool eliminarCitaID(int idCita);
    bool tieneCitaID(int idCita) const;

    // Marcado lógico
    void setEliminado(bool e);

    // Presentación
    void mostrarInformacionBasica() const;
    void mostrarInformacionCompleta() const;

    // Validación completa
    bool validarDatos() const;

    // Para persistencia
    static size_t obtenerTamano();

    // Accesores para historial de consultas
    int getCantidadConsultas() const;
    int getPrimerConsultaID() const;
    void setCantidadConsultas(int cantidad);
    void setPrimerConsultaID(int primerConsultaID);
};

#endif
