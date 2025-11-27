#include "Formatos.hpp"
#include <iostream>
#include <cstring>
#include <limits>

using namespace std;

void Formatos::limpiarBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void Formatos::pausar() {
    cout << "\nPresione Enter para continuar...";
    cin.get();
}

void Formatos::leerLinea(char* buffer, int size) {
    cin.getline(buffer, size);
    if (cin.fail()) {
        cin.clear();
        limpiarBuffer();
    }
    // Eliminar salto de linea si existe (getline ya lo hace pero por seguridad)
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n') buffer[len-1] = '\0';
}

int Formatos::leerEntero() {
    int x;
    while (!(cin >> x)) {
        cin.clear();
        limpiarBuffer();
        cout << "Entrada invalida. Ingrese un numero entero: ";
    }
    limpiarBuffer(); // Consumir el enter que queda
    return x;
}

float Formatos::leerFloat() {
    float x;
    while (!(cin >> x)) {
        cin.clear();
        limpiarBuffer();
        cout << "Entrada invalida. Ingrese un numero decimal: ";
    }
    limpiarBuffer();
    return x;
}

void Formatos::centrarTexto(const char* texto, int ancho) {
    int len = (int)strlen(texto);
    int padding = (ancho - len) / 2;
    if (padding < 0) padding = 0;
    for (int i = 0; i < padding; i++) cout << " ";
    cout << texto;
}
