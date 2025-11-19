#ifndef OPERACIONES_PACIENTES_HPP
#define OPERACIONES_PACIENTES_HPP

#include "../hospital/Hospital.hpp"
#include "Paciente.hpp"

void menuPacientes(Hospital& hospital);
void registrarPaciente(Hospital& hospital);
void buscarPacientePorID();
void buscarPacientePorCedula();
void modificarPaciente();
void eliminarPaciente();
void listarTodosPacientes();

#endif