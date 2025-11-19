#ifndef DOCTOR_HPP
#define DOCTOR_HPP

#include <cstring>
#include <ctime>

class Doctor {
private:
    int id;
    char nombre[50];
    char apellido[50];
    char cedulaProfesional[20];
    char especialidad[50];
    int aniosExperiencia;
    float costoConsulta;
    char horarioAtencion[50];
    char telefono[20];
    char email[50];
    bool disponible;
    int cantidadPacientes;
    int pacientesIDs[50];
    int cantidadCitas;
    int citasIDs[30];
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaModificacion;

public:
    // Constructores
    Doctor();
    Doctor(const char* nombre, const char* apellido, const char* cedulaProfesional, const char* especialidad);
    Doctor(const Doctor& other);
    
    // Getters
    int getId() const;
    const char* getNombre() const;
    const char* getApellido() const;
    const char* getCedulaProfesional() const;
    const char* getEspecialidad() const;
    int getAniosExperiencia() const;
    float getCostoConsulta() const;
    const char* getHorarioAtencion() const;
    const char* getTelefono() const;
    const char* getEmail() const;
    bool getDisponible() const;
    int getCantidadPacientes() const;
    const int* getPacientesIDs() const;
    int getCantidadCitas() const;
    const int* getCitasIDs() const;
    bool getEliminado() const;
    time_t getFechaCreacion() const;
    time_t getFechaModificacion() const;
    
    // Setters con validaciones
    void setId(int id);
    void setNombre(const char* nombre);
    void setApellido(const char* apellido);
    void setCedulaProfesional(const char* cedulaProfesional);
    void setEspecialidad(const char* especialidad);
    void setAniosExperiencia(int aniosExperiencia);
    void setCostoConsulta(float costoConsulta);
    void setHorarioAtencion(const char* horarioAtencion);
    void setTelefono(const char* telefono);
    void setEmail(const char* email);
    void setDisponible(bool disponible);
    void setEliminado(bool eliminado);
    
    // Métodos de gestión de pacientes
    bool agregarPacienteID(int idPaciente);
    bool eliminarPacienteID(int idPaciente);
    bool tienePacienteID(int idPaciente) const;
    
    // Métodos de gestión de citas
    bool agregarCitaID(int idCita);
    bool eliminarCitaID(int idCita);
    bool tieneCitaID(int idCita) const;
    
    // Métodos de presentación
    void mostrarInformacionBasica() const;
    void mostrarInformacionCompleta() const;
    
    // Métodos de validación
    bool validarDatos() const;
    bool estaDisponible() const;
    
    // Método estático para tamaño
    static size_t obtenerTamano();
};

#endif