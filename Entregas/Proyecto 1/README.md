# Sistema de Gestión Hospitalaria

## 1. Descripción del Proyecto

### Qué es y por qué existe

Este proyecto es un sistema ligero, robusto y educativo para la **gestión administrativa de un hospital** escrito en C++. Nació con la intención de servir tanto como herramienta de práctica para estudiantes de programación que desean comprender gestión manual de memoria y estructuras de datos, como de prototipo funcional para equipos que necesiten un registro simple y autocontenido de pacientes, doctores y citas.

### Objetivo General

Proveer una aplicación de consola que permita administrar pacientes, doctores, citas y historiales médicos con funcionalidades completas de creación, consulta, edición y eliminación, manteniendo integridad referencial y control de memoria explícito.

### Qué es capaz de hacer

* Registrar pacientes con verificación básica (cédula, email, edad) y almacenar su historial médico de consultas.
* Administrar doctores: registro con especialidad, costo de consulta y asignación de pacientes.
* Agendar, cancelar y atender citas, manteniendo referencias cruzadas entre pacientes y doctores.
* Mostrar listados claros y detallados (pacientes, doctores, citas pendientes), con datos enriquecidos como alergias y observaciones.
* Manejo dinámico de memoria: los arreglos crecen de forma controlada (estrategia de redimensionamiento), sin depender de contenedores externos.
* Control fino de estados ("Agendada", "Cancelada", "Atendida") y generación automática de IDs únicos para cada entidad.

Con este sistema puedes simular procesos administrativos reales (por ejemplo: reasignar pacientes, cancelar lexiblemente citas, obtener historiales completos) y utilizarlo como base para futuras extensiones: persistencia a disco, interfaz gráfica o migración a contenedores STL.

---

## 2. Instrucciones de Compilación

```bash
g++ -std=c++11 SistemaDeGestionHospitalario.cpp -o SDGH
```

---

## 3. Instrucciones de Ejecución

```bash
./SDGH
```

---

## 4. Estructuras de Datos

### `struct HistorialMedico`

Contiene los datos de cada consulta médica: `idConsulta`, `fecha`, `hora`, `diagnostico`, `tratamiento`, `medicamentos`, `idDoctor` y `costo`.

### `struct Paciente`

Representa a un paciente con datos personales (nombre, cédula, edad, sexo), información de contacto, `historial` (array dinámico de `HistorialMedico`), `citasAgendadas` (IDs) y campos adicionales como `alergias` y `observaciones`.

### `struct Doctor`

Almacena la información de un doctor, incluyendo `especialidad`, `aniosExperiencia`, `costoConsulta`, listas dinámicas de `pacientesAsignados` y `citasAgendadas`, y banderas como `disponible`.

### `struct Cita`

Estructura que representa una cita médica: enlaza `idPaciente` y `idDoctor` con `fecha`, `hora`, `motivo`, `estado` y `observaciones`.

### `struct Hospital`

Entidad principal del sistema. Contiene arrays dinámicos de `Pacientes`, `Doctores` y `Citas`, así como contadores (`cantidad*`), capacidades (`capacidad*`) y generadores de IDs automáticos (`siguienteId*`).

#### Relaciones Entre Estructuras

* Un `Hospital` posee múltiples `Pacientes`, `Doctores` y `Citas`.
* Cada `Cita` enlaza un `Paciente` y un `Doctor` mediante sus IDs.
* Cada `Paciente` contiene su propio `HistorialMedico` y lista de IDs de citas.
* Cada `Doctor` mantiene listas de pacientes asignados y citas agendadas (por ID), facilitando búsquedas y mantenimiento de integridad.

---

## 5. Funciones Principales

### Módulo General

* `crearHospital()` → Inicializa la estructura principal del hospital.
* `destruirHospital()` → Libera toda la memoria dinámica del sistema y deja el puntero a `nullptr`.

### Módulo de Pacientes

* `crearPaciente()` → Registra un paciente (validación básica incluida).
* `buscarPacientePorId()`, `buscarPacientePorCedula()`, `buscarPacientesPorNombre()` → Búsquedas por distintos criterios.
* `actualizarPaciente()` → Modifica datos de contacto y observaciones.
* `eliminarPaciente()` → Elimina al paciente, cancela sus citas y libera memoria asociada.
* `listarPacientes()` → Muestra los registros en forma tabular.

### Módulo de Doctores

* `crearDoctor()` → Registra un doctor con validaciones.
* `buscarDoctorPorId()`, `buscarDoctorPorCedula()`, `buscarDoctoresPorEspecialidad()` → Búsquedas.
* `asignarPacienteADoctor()` / `removerPacienteDeDoctor()` → Gestión de asignaciones.
* `eliminarDoctor()` → Elimina al doctor, cancela sus citas y libera recursos.

### Módulo de Citas

* `agendarCita()` → Valida fecha/hora y disponibilidad; actualiza referencias del paciente y doctor.
* `cancelarCita()` → Cancela y elimina referencias cruzadas.
* `atenderCita()` → Marca como atendida y registra la consulta en el historial del paciente.
* `listarCitasPendientes()` → Presenta todas las citas en estado "Agendada".

### Validaciones y Utilidades

* `validarCedula()`, `validarEmail()`, `validarFecha()`, `validarHora()` → Reglas básicas de formato.
* `leerLinea()`, `leerEntero()` → Lectura segura desde `stdin`.
* `redimensionarArray*()` → Funciones que duplican capacidad cuando se requiere.

---

## 6. Decisiones de Diseño

### Elección de Arrays Dinámicos

Se emplean arrays dinámicos para mostrar y controlar manualmente la asignación y liberación de memoria (`new`/`delete[]`), lo cual es útil en contextos educativos y en entornos donde se desee evitar dependencias externas o comprender cómo funciona la administración de memoria en C++.

### Estrategia de Redimensionamiento

La estrategia elegida es la **duplicación de capacidad**: cuando un array alcanza su tope, se asigna un nuevo array con el doble de la capacidad previa y se copian los elementos. Esto reduce la frecuencia de realocaciones manteniendo buen rendimiento amortizado.

### Manejo de Memoria

* Todas las estructuras que contienen arrays internos inicializan dichos arrays al crear la entidad.
* Al eliminar entidades (pacientes o doctores), sus arrays internos se liberan con `delete[]` y se compactan los arrays principales.
* `destruirHospital()` asegura la liberación completa de memoria antes de finalizar la ejecución.

---

## 7. Uso y Ejemplo de Interacción (flujo típico)

1. Compila y ejecuta el binario:

```bash
g++ -std=c++11 hospital.cpp -o hospital
./hospital
```

2. En el menú principal selecciona "Gestión de Pacientes" para registrar un nuevo paciente.
3. Registra doctores desde "Gestión de Doctores".
4. Ve a "Gestión de Citas" para agendar una consulta (valida fecha y hora).
5. Usa "Atender cita" para marcar la cita como atendida y generar un registro en el historial médico del paciente.
6. Consulta el historial del paciente para revisar diagnósticos y tratamientos anteriores.

### Ejemplo de caso de uso real

* Un recepcionista registra al paciente y su información básica.
* El administrador asigna al paciente a un doctor y programa la cita.
* Tras la atención, el médico introduce diagnóstico y tratamiento; el sistema guarda la entrada en el historial y actualiza estados.

---

**Autor:** Leandro Jay

**Versión:** 1.0

