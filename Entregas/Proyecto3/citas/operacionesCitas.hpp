#ifndef OPERACIONES_CITAS_HPP
#define OPERACIONES_CITAS_HPP

#include "../hospital/Hospital.hpp"
#include "Cita.hpp"

void menuCitas(Hospital& hospital);
void agendarCita(Hospital& hospital);
void cancelarCita();
void atenderCita(Hospital& hospital);
void buscarCitaPorID();
void listarCitasPorPaciente();
void listarCitasPorDoctor();
void listarCitasPorFecha();
void listarCitasPendientes();
void modificarCita();

#endif