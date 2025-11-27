#ifndef VALIDACIONES_HPP
#define VALIDACIONES_HPP

class Validaciones {
public:
    static bool validarCedula(const char* cedula);
    static bool validarCedulaProfesional(const char* cedula);
    static bool validarEmail(const char* email);
    static bool validarTelefono(const char* telefono);
    static bool validarFecha(const char* fecha);
    static bool validarHora(const char* hora);
    static bool validarRango(int valor, int min, int max);
    static bool validarRangoFloat(float valor, float min, float max);
    static bool validarTipoSangre(const char* tipo);
    static bool validarSexo(char sexo);
};

#endif
