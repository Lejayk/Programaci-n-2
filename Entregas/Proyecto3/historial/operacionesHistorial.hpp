#ifndef OPERACIONES_HISTORIAL_HPP
#define OPERACIONES_HISTORIAL_HPP

#include "../hospital/Hospital.hpp"

void menuHistorial(Hospital& hospital);

void crearRegistroHistorial(Hospital& hospital);
void buscarHistorialPorID();
void listarHistorial();
void eliminarHistorial();

#endif
