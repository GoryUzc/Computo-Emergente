# Cómputo Emergente

Repositorio académico de proyectos, implementaciones y recursos para la materia Cómputo Emergente. Cada carpeta contiene una herramienta o tema desarrollado durante el curso, con código fuente, documentación y especificaciones técnicas independientes.

## Proyectos

Cada proyecto es autónomo: posee su propio `CMakeLists.txt`, puede compilarse de forma independiente y no depende de los demás.

| # | Carpeta | Tema | Descripción | Estado |
|---|---------|------|-------------|--------|
| 1.1 | `AutomataWolfram` | Autómatas Celulares 1D | Simulador gráfico de las reglas de Wolfram (128, 90, 210 y 110) con SFML 3. Incluye sistema de capturas de pantalla y clasificación automática de comportamientos. | ✅ Completado |

## Cómo compilar cualquier proyecto

### En Windows con Visual Studio 2022

Desde `x64 Native Tools Command Prompt for VS 2022`:

```bash
cd "01-Automata-Wolfram"
cmake -B build -S . -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE=C:/Users/User/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Debug
```

### Con la extensión CMake Tools de VS Code

- Abre la carpeta del proyecto.
- Presiona `F7` para compilar.
- Presiona `Shift+F5` para ejecutar.

## Recursos

- **A New Kind of Science** — Wolfram, S. (2002)
- **Universality in Elementary Cellular Automata** — Cook, M. (2004)
- **SFML Documentation** — https://www.sfml-dev.org/documentation/3.0

## Licencia

Uso académico. Libre para fines educativos.
