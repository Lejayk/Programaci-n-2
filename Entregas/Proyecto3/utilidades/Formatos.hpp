#ifndef FORMATOS_HPP
#define FORMATOS_HPP

class Formatos {
public:
    static void limpiarBuffer();
    static void pausar();
    static void leerLinea(char* buffer, int size);
    static int leerEntero();
    static float leerFloat();
    static void centrarTexto(const char* texto, int ancho);
};

#endif
