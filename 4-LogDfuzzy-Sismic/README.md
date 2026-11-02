# Sistema Difuso para Evaluación de Severidad Sísmica

Proyecto académico que estima una intensidad sísmica percibida a partir de magnitud, profundidad y distancia epicentral. El motor de inferencia Mamdani está implementado en C++17 con Eigen; pybind11 expone el motor a una interfaz de consola Python con visualización y exportación CSV.

> **Estado:** el motor C++ compila, el bridge construye el sistema predeterminado y una evaluación con M, h y d devuelve intensidad. Los siete tests CTest pasan en el entorno Windows validado. La sensibilidad comparativa sigue pendiente y la superficie 3D aún oculta excepciones durante sus cálculos.

## Estructura

- `cpp/include/fuzzy_core/`: interfaces públicas del motor.
- `cpp/src/`: fuzzificación, evaluación de reglas, agregación, desfusificación y fachada del sistema.
- `cpp/bindings/`: binding pybind11; el módulo se copia al frontend al compilar.
- `cpp/tests/`: pruebas de integración con GoogleTest.
- `workspace/python/fuzzy_ui/`: CLI, configuración sísmica, bridge, gráficas y exportador CSV.
- `SSD/`: arquitectura y especificaciones del sistema.

## Requisitos

- CMake 3.20 o posterior.
- Compilador C++17; en Windows, Visual Studio 2022 con MSVC.
- Eigen3 instalado y visible como paquete CMake `Eigen3Config.cmake`. En el entorno validado se utilizó vcpkg.
- Python compatible con el binding y con `numpy`, `matplotlib` y `pandas`.
- Acceso de red durante la configuración de CMake para descargar pybind11 y GoogleTest mediante FetchContent, salvo que estén disponibles en caché.

## Configuración y compilación en Windows

Desde la raíz del repositorio, configura CMake. Sustituye la ruta de ejemplo por la ruta de tu instalación de vcpkg:

```powershell
vcpkg install eigen3:x64-windows
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="C:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake"
cmake --build build --config Debug
```

Si Eigen3 ya está instalado y su paquete CMake se encuentra en `CMAKE_PREFIX_PATH`, puedes omitir `CMAKE_TOOLCHAIN_FILE`.

El build genera `fuzzy_core` y lo copia automáticamente a `workspace/python/fuzzy_ui/`. No es necesario copiar el `.pyd` manualmente.

## Pruebas

```powershell
ctest --test-dir build -C Debug --output-on-failure
```

CTest registra actualmente la prueba `fuzzy_system`, que contiene cuatro pruebas de integración y tres pruebas para funciones trapezoidales. Los siete casos pasaron en el entorno Windows validado.

## Frontend Python

Instala las dependencias en el entorno Python que se utilizará para ejecutar el módulo:

```powershell
python -m pip install numpy matplotlib pandas
cd workspace\python
python -m fuzzy_ui.main
```

El bridge importa el módulo nativo desde el paquete y el motor admite los trapezoides de hombro de la configuración. La CLI permite calcular, visualizar, exportar, mostrar reglas e historial. El análisis de sensibilidad todavía es informativo y no compara configuraciones.

## Documentación

- [Arquitectura](SSD/ARCH_DESIGN.md)
- [Especificación del motor C++](SSD/SPECS/spec-01.md)
- [Especificación del frontend Python](SSD/SPECS/spec-02.md)
