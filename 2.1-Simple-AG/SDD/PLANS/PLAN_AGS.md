# Plan de Implementación — Algoritmo Genético Simple (AGS)


---

## Fase 1: Preparación del entorno

| Paso | Acción | Comando / Acción | Estado |
|------|--------|------------------|--------|
| 1.1 | Crear estructura de carpetas | Según `SDD/ARCH-DESINGH.md` | ⬜ |
| 1.2 | Crear `CMakeLists.txt` | CMake 3.16+, target C++17 | ⬜ |
| 1.3 | Verificar `python/requirements.txt` | numpy, matplotlib, pandas | ⬜ |
| 1.4 | Verificar compilador | `g++ --version` ≥ 11 | ⬜ |

**Checkpoint:** Estructura de carpetas lista, build system funcional.

---

## Fase 2: Implementar módulos C++

>Implementaciones tecnicas segun SPEC-001.md 

### 2.1 Módulo `utils` — Conversión binario ↔ fenotipo

**Archivos:** `src/utils.hpp` + `src/utils.cpp`

**Tareas:**
1. Implementar `binarioAEntero(const std::vector<int>& bits)`:
   - Suma ponderada: $\sum \text{bit}_i \cdot 2^{L-1-i}$.
   - Test: `[1,1,1,1,1,1]` → `63`.
2. Implementar `mapeoReal(int entero, int L, int precision)`:
   - $x = 0 + \text{entero} \cdot \frac{63}{2^L - 1}$.
   - Redondear a `precision` decimales.
   - Test: L=13, entero=8191 → x ≈ 63.00.

### 2.2 Módulo `ag_core` — Población, fitness, inicialización

**Archivos:** `src/ag_core.hpp` + `src/ag_core.cpp`

**Tareas:**
1. Definir `struct Individuo` y `struct Poblacion`.
2. Implementar `inicializarPoblacion()`:
   - Generar bits aleatorios con `std::uniform_int_distribution<int>(0,1)`.
   - Decodificar y evaluar fitness.
3. Implementar `decodificarCromosoma()`:
   - Llamar a `binarioAEntero()` y luego mapeo según modo.

### 2.3 Módulo `operadores` — Ruleta, cruce, mutación

**Archivos:** `src/operadores.hpp` + `src/operadores.cpp`

**Tareas:**
1. Implementar `seleccionRuleta()`:
   - Calcular probabilidades acumuladas.
   - Usar `std::upper_bound` para búsqueda eficiente.
2. Implementar `cruceUnPunto()`:
   - Emparejar consecutivamente.
   - Punto de corte aleatorio en `[1, L-1]`.
   - Intercambiar segmentos.
3. Implementar `mutarPoblacion()`:
   - Para cada bit: `std::uniform_real_distribution<double>(0,1)`.
   - Flip si `r < Pm`.

### 2.4 Módulo `exportador`

**Archivos:** `src/exportador.hpp` + `src/exportador.cpp`

**Tareas:**
1. Implementar `exportarMetricas()`:
   - CSV con encabezado y 6 decimales.

### 2.5 Módulo `main.cpp`

**Archivo:** `src/main.cpp`

**Tareas:**
1. Definir los 6 experimentos (ver SPEC-001 sección 4.2).
2. Implementar `ejecutarAGS()`:
   - Loop de 100 generaciones.
   - Registrar métricas por generación.
3. Llamar a `exportarMetricas()` para cada experimento.
4. Logging básico a consola.

---

## Fase 3: Build system

**Archivo:** `CMakeLists.txt`

**Tareas:**
1. `cmake_minimum_required(VERSION 3.16)`.
2. `set(CMAKE_CXX_STANDARD 17)`.
3. Target `ags_maximizador` con todos los `.cpp`.
4. Flags: `-Wall -Wextra -O2`.

---

## Fase 4: Script Python

>Implementaciones tecnicas segun SPEC-002.md 

**Archivo:** `python/visualizar_ag.py`

**Tareas:**
1. `leer_metricas()` — pandas read_csv.
2. `graficar_convergencia()` — matplotlib, mejor + promedio.
3. `graficar_comparativa()` — múltiples curvas en un solo plot.
4. Generar las 4 figuras (ver SPEC-002 sección 6).

---

## Fase 5: Compilación y pruebas

| Paso | Acción |
|------|--------|
| 5.1 | `mkdir build && cd build` |
| 5.2 | `cmake ..` |
| 5.3 | `cmake --build . -j$(nproc)` |
| 5.4 | Ejecutar: `./ags_maximizador` |
| 5.5 | Verificar 6 CSV en `resultados/` |
| 5.6 | Ejecutar: `python python/visualizar_ag.py` |
| 5.7 | Verificar 4 PNG en `resultados/graficos/` |

---

## Checkpoints de validación

| Checkpoint | Cómo verificar |
|------------|----------------|
| [x] Decodificación 6 bits | `[1,1,1,1,1,1]` → x = 63 |
| [x] Decodificación 13 bits | Rango [0, 63], error < 0.01 |
| [x] Ruleta | Suma prob = 1.0 |
| [x] Cruce | Hijos diferentes de padres (con Pc=1.0) |
| [x] Mutación | Bits flipados observados |
| [x] Convergencia | Mejor fitness → ~3969 |
| [x]CSV exportado | 100 filas + encabezado |
| [x]Gráficos | 4 PNG generados |

---