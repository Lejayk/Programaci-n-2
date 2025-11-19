#ifndef OPERACIONES_MANTENIMIENTO_HPP
#define OPERACIONES_MANTENIMIENTO_HPP

#include "../hospital/Hospital.hpp"

void menuMantenimiento(Hospital& hospital);
void compactarTodosArchivos();
void crearRespaldo();
void restaurarRespaldo();
void verificarIntegridadReferencial();
void repararIntegridadReferencial();
void mostrarEstadisticasDetalladas();
void verificarSistemaArchivos();

#endif
