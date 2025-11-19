#ifndef OPERACIONES_HISTORIAL_HPP
#define OPERACIONES_HISTORIAL_HPP

#include "../hospital/Hospital.hpp"
#include "HistorialMedico.hpp"

void menuHistorial(Hospital& hospital);
void registrarConsulta(Hospital& hospital);
void verHistorialPaciente();
void buscarConsultaPorID();
void listarTodasConsultas();
void eliminarConsulta();
void repararListaHistorial();

#endif