#include "Validaciones.hpp"
#include <cstring>
#include <cctype>
#include <cstdio>
#include <cstdlib>

bool Validaciones::validarCedula(const char* cedula) {
    if (!cedula || strlen(cedula) < 5) return false;
    return true;
}

bool Validaciones::validarCedulaProfesional(const char* cedula) {
    if (!cedula) return false;
    int len = (int)strlen(cedula);
    if (len < 3) return false;
    
    // Formato V-12345678 o E-12345678
    char primera = cedula[0];
    if (primera != 'V' && primera != 'v' && primera != 'E' && primera != 'e') return false;
    if (cedula[1] != '-') return false;
    
    for (int i = 2; i < len; i++) {
        if (!isdigit((unsigned char)cedula[i])) return false;
    }
    return true;
}

bool Validaciones::validarEmail(const char* email) {
    if (!email || strlen(email) == 0) return true; // Permitir vacio si es opcional, pero aqui validamos formato si existe
    if (strlen(email) < 5) return false;
    
    const char* at = strchr(email, '@');
    if (!at) return false;
    if (strchr(at + 1, '@')) return false;
    if (at == email) return false;
    if (*(at + 1) == '\0') return false;
    
    const char* dot = strchr(at, '.');
    if (!dot) return false;
    if (dot == at + 1) return false;
    
    const char* lastDot = strrchr(email, '.');
    if (!lastDot || strlen(lastDot) <= 1) return false;
    
    return true;
}

bool Validaciones::validarTelefono(const char* telefono) {
    if (!telefono || strlen(telefono) < 7) return false;
    int digitCount = 0;
    for (int i = 0; telefono[i] != '\0'; i++) {
        if (isdigit(telefono[i])) digitCount++;
    }
    return digitCount >= 7;
}

bool Validaciones::validarFecha(const char* fecha) {
    if (!fecha || strlen(fecha) != 10) return false;
    
    // YYYY-MM-DD
    if (fecha[4] != '-' || fecha[7] != '-') return false;
    
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit(fecha[i])) return false;
    }
    
    int year = atoi(fecha);
    int month = atoi(fecha + 5);
    int day = atoi(fecha + 8);
    
    if (year < 1900 || year > 2100) return false;
    if (month < 1 || month > 12) return false;
    
    int diasMes = 31;
    if (month == 4 || month == 6 || month == 9 || month == 11) diasMes = 30;
    else if (month == 2) diasMes = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 29 : 28;
    
    return day >= 1 && day <= diasMes;
}

bool Validaciones::validarHora(const char* hora) {
    if (!hora || strlen(hora) != 5) return false;
    
    // HH:MM
    if (hora[2] != ':') return false;
    
    if (!isdigit(hora[0]) || !isdigit(hora[1]) || !isdigit(hora[3]) || !isdigit(hora[4])) return false;
    
    int hh = atoi(hora);
    int mm = atoi(hora + 3);
    
    return hh >= 0 && hh <= 23 && mm >= 0 && mm <= 59;
}

bool Validaciones::validarRango(int valor, int min, int max) {
    return valor >= min && valor <= max;
}

bool Validaciones::validarRangoFloat(float valor, float min, float max) {
    return valor >= min && valor <= max;
}

bool Validaciones::validarTipoSangre(const char* tipo) {
    if (!tipo) return false;
    const char* validos[] = {"A+","A-","B+","B-","AB+","AB-","O+","O-"};
    for (const char* v : validos) {
        if (strcmp(tipo, v) == 0) return true;
    }
    return false;
}

bool Validaciones::validarSexo(char sexo) {
    sexo = toupper(sexo);
    return sexo == 'M' || sexo == 'F' || sexo == 'O';
}
