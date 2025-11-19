#ifndef OPERACIONES_DOCTORES_HPP
#define OPERACIONES_DOCTORES_HPP

#include "../hospital/Hospital.hpp"

void menuDoctores(Hospital& hospital);

void registrarDoctor(Hospital& hospital);
void editarDoctor(Hospital& hospital);
void buscarDoctorPorID();
void listarDoctores();
void eliminarDoctor();

#endif
