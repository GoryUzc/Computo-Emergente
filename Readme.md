# Cómputo Emergente

Repositorio académico de proyectos, implementaciones y recursos para la materia Cómputo Emergente. Cada carpeta contiene una herramienta o tema desarrollado durante el curso, con código fuente, documentación y especificaciones técnicas independientes.

## Proyectos

Cada proyecto es autónomo: posee su propio `CMakeLists.txt`, puede compilarse de forma independiente y no depende de los demás.

| # | Carpeta | Tema | Descripción | Estado |
|---|---------|------|-------------|--------|
| 1.1 | `AutomataWolfram` | Autómatas Celulares 1D | Simulador gráfico de las reglas de Wolfram (30, 90 y 110) con SFML 3. Incluye sistema de capturas de pantalla y clasificación automática de comportamientos. | ✅ Completado |
| 1.2 | `MedidasCuantitativas-AC` | Medidas Cuantitativas | Análisis cuantitativo de autómatas celulares mediante 6 medidas (densidad, hamming, λ de Langton, entropía espacial/temporal, exponente de Lyapunov) con exportación CSV. | ✅ Completado |
| 1.3 | `ProcesamientoImagenes-AC` | Procesamiento de Imágenes | Detector de bordes basado en autómata celular 2D con vecindad de Moore. Incluye comparador visual y métricas de evaluación. | ✅ Completado |
| 1.4 | `1.4-RevisionTrabajoInvestigacion-AC` | Revisión de Investigación | Análisis del artículo *"Training Cellular Automata for Image Edge Detection"* de Shukla (2016): revisión de metodología SFFS, métricas de error y conclusiones críticas. | ✅ Completado |

## Cómo compilar cualquier proyecto

Todos los proyectos siguen la misma estructura de compilación con CMake y vcpkg.

### En Windows con Visual Studio 2022

Desde `x64 Native Tools Command Prompt for VS 2022`:

```bash
cd "1.1 -  AutomataWolfram"
cmake -B build -S . -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE=C:/Users/User/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Debug
```

```bash
cd "../1.2 - MedidasCuantitativas-AC"
cmake -B build -S . -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE=C:/Users/User/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Debug
```

```bash
cd "../1.3-ProcesamientoImagenes-AC"
cmake -B build -S . -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE=C:/Users/User/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Debug
```

```bash
cd "../1.4-RevisionTrabajoInvestigacion-AC"
cmake -B build -S . -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE=C:/Users/User/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Debug
```

### Con la extensión CMake Tools de VS Code

- Abre la carpeta del proyecto.
- Presiona `F7` para compilar.
- Presiona `Shift+F5` para ejecutar.

## Recursos

### Autómatas Celulares
- **A New Kind of Science** — Wolfram, S. (2002)
- **Universality in Elementary Cellular Automata** — Cook, M. (2004)

### Procesamiento de Imágenes
- **Digital Image Processing** — Gonzalez, R. C. & Woods, R. E.
- **SFML Documentation** — https://www.sfml-dev.org/documentation/3.0

### Revisión de Investigación
- **Training Cellular Automata for Image Edge Detection** — Shukla, A. P. (2016). Romanian Journal of Information Science and Technology, 19(4), 338–359. http://romjist.ro/content/pdf/shukla.pdf
- **Handbook of Nature-Inspired Computing** — Rosin, P. L., & Ioannidis, E. (2003)
- **Cellular Automata in Image Processing** — Popovici, A., & Popovici, D. (2002)

## Licencia

Uso académico. Libre para fines educativos.
