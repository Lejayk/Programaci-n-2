#include <iostream>
using namespace std;

class hora {
private: //Atributos
    int horas;
    int minutos;
    int segundos;
    public: //Metodos
    hora(int h, int m, int s); //Constructor    
    hora(long hora); //Constructor desde segundos

    void mostrarHora();
};

//Constructor 1
hora::hora(int h, int m, int s) {
    horas = h;
    minutos = m;
    segundos = s;
}

void hora::mostrarHora() {
    cout << "La hora es: " << horas << ":" << minutos << ":" << segundos << endl;
}
//Constructor 2

hora::hora(long hora) {
    horas = int (hora / 3600);
    minutos = int ((hora % 3600) / 60);
    segundos = int (hora % 60);
}
int main () {
    hora ahoramismo(14, 30, 45);
    hora ahoramismo2(34125);
    ahoramismo.mostrarHora();
    ahoramismo2.mostrarHora();
}