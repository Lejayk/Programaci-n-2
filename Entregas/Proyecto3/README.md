# 🏥 Sistema de Gestión Hospitalaria (Proyecto 3 – Programación II)

Este proyecto implementa un **Sistema de Gestión Hospitalaria completo**, utilizando **Programación Orientada a Objetos (POO)**, archivos binarios y una arquitectura modular organizada en carpetas.  
Permite administrar **Pacientes, Doctores, Citas e Historial Médico**, con persistencia en archivos y un menú interactivo.

---

## 🚀 Características Principales

- Gestión completa de:
  - Pacientes
  - Doctores
  - Citas
  - Historial Médico
- Menús interactivos por módulos.
- Validaciones según los requerimientos del Proyecto 2.
- Persistencia mediante **archivos binarios** (`*.dat`).
- Eliminación lógica.
- Arquitectura basada en: 
  - Clases responsables y encapsuladas
  - Módulos separados
  - Vectores dinámicos (`std::vector`)
  - Templates para manejo genérico de archivos
- Carga automática de datos al iniciar el programa.
- Guarda cambios en tiempo real.

---

## 📁 Estructura del Proyecto

Proyecto3/
│ main.cpp
│ Makefile
│
├── pacientes/
│ ├── Paciente.hpp
│ ├── Paciente.cpp
│ ├── operacionesPacientes.hpp
│ └── operacionesPacientes.cpp
│
├── doctores/
│ ├── Doctor.hpp
│ ├── Doctor.cpp
│ ├── operacionesDoctores.hpp
│ └── operacionesDoctores.cpp
│
├── citas/
│ ├── Cita.hpp
│ ├── Cita.cpp
│ ├── operacionesCitas.hpp
│ └── operacionesCitas.cpp
│
├── historial/
│ ├── HistorialMedico.hpp
│ ├── HistorialMedico.cpp
│ ├── operacionesHistorial.hpp
│ └── operacionesHistorial.cpp
│
├── hospital/
│ ├── Hospital.hpp
│ ├── Hospital.cpp
│ └── menuPrincipal.cpp
│
└── persistencia/
├── GestorArchivos.hpp
└── GestorArchivos.cpp

---

## 🧱 Arquitectura del Sistema

### 📌 1. **Hospital**
- Clase central del sistema.
- Contiene:
  - `vector<Paciente>`
  - `vector<Doctor>`
  - `vector<Cita>`
  - `vector<HistorialMedico>`
- Carga datos desde archivos al iniciar el programa.
- Guarda datos cuando se crean nuevos elementos.
- Administra IDs máximos.

### 📌 2. **Entidades (Paciente, Doctor, Cita, Historial):**
- Cada clase representa un módulo del sistema.
- Contienen:
  - Atributos privados
  - Getters/Setters validados
  - Fecha de modificación
  - Eliminación lógica

### 📌 3. **Operaciones**
- Cada módulo tiene su archivo de operaciones:
  - `menuPacientes()`
  - `menuDoctores()`
  - `menuCitas()`
  - `menuHistorial()`
- Manipulan directamente los vectores del Hospital mediante referencia.

### 📌 4. **Persistencia (GestorArchivos)**
- Usa **templates** para leer/escribir objetos genéricos.
- Guarda en binario mediante:
  - `guardarEnArchivo<T>()`
- Carga mediante:
  - `cargarDesdeArchivo<T>()`
- Evita duplicar código entre pacientes/doctores/citas/historial.

---

## ⚙️ Compilación con Makefile (Windows o Linux)

### ✔️ Compilar
En PowerShell o terminal:

### De preferencia usar el ejecutable ya ajunto en la carpeta! Encarecidamente, Leandro.