#ifndef OPERACIONES_DOCTORES_HPP
#define OPERACIONES_DOCTORES_HPP

#include "../hospital/Hospital.hpp"
#include "Doctor.hpp"

void menuDoctores(Hospital& hospital);
void registrarDoctor(Hospital& hospital);
void buscarDoctorPorID();
void buscarDoctorPorCedula();
void modificarDoctor();
void eliminarDoctor();
void listarTodosDoctores();
void listarDoctoresPorEspecialidad();
void asignarPacienteADoctor();
void removerPacienteDeDoctor();
void listarPacientesDeDoctor();

#endif