#ifndef OPERACIONES_CITAS_HPP
#define OPERACIONES_CITAS_HPP

#include "../hospital/Hospital.hpp"

void menuCitas(Hospital& hospital);

void crearCita(Hospital& hospital);
void listarCitas();
void buscarCitaPorID();
void cancelarCita();
void marcarCitaAtendida();

#endif
