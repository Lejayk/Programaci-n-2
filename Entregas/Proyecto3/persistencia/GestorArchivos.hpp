#ifndef GESTOR_ARCHIVOS_HPP
#define GESTOR_ARCHIVOS_HPP

#include "../hospital/Hospital.hpp"
#include "../pacientes/Paciente.hpp"
#include "../doctores/Doctor.hpp"
#include "../citas/Cita.hpp"
#include "../historial/HistorialMedico.hpp"
#include <vector>
#include <fstream>

struct ArchivoHeader {
    int cantidadRegistros;
    int proximoID;
    int registrosActivos;
    int version;
};

class GestorArchivos {
private:
    static constexpr const char* PACIENTES_BIN = "datos/pacientes.bin";
    static constexpr const char* DOCTORES_BIN = "datos/doctores.bin";
    static constexpr const char* CITAS_BIN = "datos/citas.bin";
    static constexpr const char* HISTORIALES_BIN = "datos/historiales.bin";
    static constexpr const char* HOSPITAL_BIN = "datos/hospital.bin";
    static constexpr int VERSION_ARCHIVO = 1;

    // Métodos auxiliares privados
    static bool crearDirectorioDatos();
    static long calcularPosicion(int indice, size_t tamRegistro);
    static bool inicializarArchivo(const char* nombreArchivo);

public:
    // Inicialización del sistema
    static bool inicializarSistemaArchivos();
    
    // Operaciones de header
    static ArchivoHeader leerHeader(const char* nombreArchivo);
    static bool actualizarHeader(const char* nombreArchivo, const ArchivoHeader& header);
    
    // Operaciones para Hospital
    static bool guardarHospital(const Hospital& hospital);
    static bool cargarHospital(Hospital& hospital);
    
    // Operaciones para Pacientes
    static bool guardarPaciente(Paciente& paciente);
    static Paciente leerPacientePorIndice(int indice);
    static Paciente buscarPacientePorID(int id);
    static Paciente buscarPacientePorCedula(const char* cedula);
    static bool eliminarPacienteLogico(int id);
    static std::vector<Paciente> listarPacientesActivos();
    
    // Operaciones para Doctores
    static bool guardarDoctor(Doctor& doctor);
    static Doctor leerDoctorPorIndice(int indice);
    static Doctor buscarDoctorPorID(int id);
    static bool eliminarDoctorLogico(int id);
    static std::vector<Doctor> listarDoctoresActivos();
    
    // Operaciones para Citas
    static bool guardarCita(Cita& cita);
    static Cita leerCitaPorIndice(int indice);
    static Cita buscarCitaPorID(int id);
    static bool eliminarCitaLogico(int id);
    static std::vector<Cita> listarCitasActivas();
    
    // Operaciones para Historial Medico
    static bool guardarHistorial(HistorialMedico& historial);
    static HistorialMedico leerHistorialPorIndice(int indice);
    static HistorialMedico buscarHistorialPorID(int id);
    static bool eliminarHistorialLogico(int id);
    static std::vector<HistorialMedico> listarHistorialesActivos();
    
    // Mantenimiento
    static bool compactarArchivo(const char* nombreArchivo, size_t tamRegistro);
};

#endif