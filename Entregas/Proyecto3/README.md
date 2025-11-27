# 🏥 Sistema de Gestión Hospitalaria v3 (POO y Modularización)

Este proyecto implementa un **Sistema de Gestión Hospitalaria completo**, evolucionando desde una estructura monolítica hacia una arquitectura **Modular y Orientada a Objetos**, cumpliendo estrictamente con los principios de encapsulamiento, abstracción y responsabilidad única.

El sistema permite administrar **Pacientes, Doctores, Citas e Historial Médico**, garantizando la persistencia de datos mediante archivos binarios y ofreciendo herramientas de mantenimiento y validación robustas.

---

## 🚀 Características Principales

### 🔹 Arquitectura Modular
El código está organizado por dominios, separando claramente las responsabilidades:
- **Entidades:** Clases encapsuladas (`Paciente`, `Doctor`, etc.) con atributos privados y métodos públicos.
- **Operaciones:** Lógica de negocio y menús separados de las definiciones de clases.
- **Persistencia:** Módulo centralizado (`GestorArchivos`) para todas las operaciones de E/S binaria.
- **Utilidades:** Validaciones y formatos reutilizables.

### 🔹 Funcionalidades
- **Gestión de Pacientes:** Registro, búsqueda (ID/Nombre/Cédula), modificación, eliminación lógica, historial de consultas.
- **Gestión de Doctores:** Registro, búsqueda, gestión de disponibilidad, especialidades.
- **Gestión de Citas:** Agendamiento, cancelación, control de estado (Atendida/No asistió).
- **Historial Médico:** Registro de consultas, diagnósticos, tratamientos y costos.
- **Mantenimiento:** Compactación de archivos (física), copias de seguridad, verificación de integridad y estadísticas de uso.

### 🔹 Persistencia Robusta
- Uso de archivos binarios con acceso aleatorio (`seekg`, `seekp`).
- Headers de archivo para control de metadatos (cantidad de registros, próximo ID, versión).
- Sistema de IDs autoincrementales persistentes.
- Inicialización segura que previene pérdida de datos.

---

## 📁 Estructura del Proyecto

```
Proyecto3/
│
├── hospital/                   # Información general del hospital
│   ├── Hospital.hpp
│   └── Hospital.cpp
│
├── pacientes/                  # Módulo de Pacientes
│   ├── Paciente.hpp            # Clase Entidad
│   ├── Paciente.cpp
│   ├── operacionesPacientes.hpp # Lógica de menús y negocio
│   └── operacionesPacientes.cpp
│
├── doctores/                   # Módulo de Doctores
│   ├── Doctor.hpp
│   ├── Doctor.cpp
│   ├── operacionesDoctores.hpp
│   └── operacionesDoctores.cpp
│
├── citas/                      # Módulo de Citas
│   ├── Cita.hpp
│   ├── Cita.cpp
│   ├── operacionesCitas.hpp
│   └── operacionesCitas.cpp
│
├── historial/                  # Módulo de Historial Médico
│   ├── HistorialMedico.hpp
│   ├── HistorialMedico.cpp
│   ├── operacionesHistorial.hpp
│   └── operacionesHistorial.cpp
│
├── persistencia/               # Motor de Archivos Binarios
│   ├── GestorArchivos.hpp
│   └── GestorArchivos.cpp
│
├── utilidades/                 # Herramientas Transversales
│   ├── Validaciones.hpp        # Validaciones estáticas (Email, Cédula, Fechas)
│   ├── Validaciones.cpp
│   ├── Formatos.hpp            # Helpers de entrada/salida
│   └── Formatos.cpp
│
├── mantenimiento/              # Herramientas de Sistema
│   ├── operacionesMantenimiento.hpp
│   └── operacionesMantenimiento.cpp
│
├── datos/                      # Archivos binarios generados (BD)
│   ├── pacientes.bin
│   ├── doctores.bin
│   ├── citas.bin
│   ├── historiales.bin
│   └── hospital.bin
│
├── main.cpp                    # Punto de entrada (Menú Principal)
└── README.md                   # Documentación
```

---

## 🛠️ Compilación y Ejecución

### Requisitos
- Compilador C++ compatible con C++17 (GCC/MinGW recomendado).
- Entorno Windows (para comandos de consola específicos) o Linux (adaptable).

### Compilación Manual (MinGW/G++)
Ejecute el siguiente comando en la raíz del proyecto para compilar todos los módulos:

```powershell
g++ -std=c++17 -O2 -I. -o SistemaHospital.exe main.cpp hospital/Hospital.cpp pacientes/Paciente.cpp pacientes/operacionesPacientes.cpp doctores/Doctor.cpp doctores/operacionesDoctores.cpp citas/Cita.cpp citas/operacionesCitas.cpp historial/HistorialMedico.cpp historial/operacionesHistorial.cpp persistencia/GestorArchivos.cpp mantenimiento/operacionesMantenimiento.cpp utilidades/Validaciones.cpp utilidades/Formatos.cpp
```

### Ejecución
```powershell
.\SistemaHospital.exe
```

---

## ✅ Cumplimiento de Pautas (Proyecto 3)

1.  **POO Pura:** Transformación de `struct` a `class` con atributos privados y métodos de acceso (`getters`/`setters`).
2.  **Validaciones Encapsuladas:** Los `setters` incluyen lógica de validación robusta (rangos, formatos, longitud) utilizando la clase auxiliar `Validaciones`.
3.  **Separación de Responsabilidades:**
    *   Las clases de entidad (`Paciente`, `Doctor`) **NO** gestionan archivos ni menús.
    *   `GestorArchivos` centraliza **TODA** la interacción con el disco.
    *   `main.cpp` es ligero y solo delega a los módulos de operaciones.
4.  **Código Limpio:** Uso de constantes, nombres descriptivos y eliminación de código duplicado mediante el módulo `utilidades`.

---


