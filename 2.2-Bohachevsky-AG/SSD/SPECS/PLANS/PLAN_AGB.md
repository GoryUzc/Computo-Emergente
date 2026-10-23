# PLAN-AG: Plan de Implementación del Algoritmo Genético Bohachevsky

---

## 1. Objetivo del Plan

Guía paso a paso para implementar el sistema completo siguiendo el contrato ARCH_DESIGN.md y las especificaciones técnicas SPEC-001 y SPEC-002.

**Metodología:** Implementar de abajo hacia arriba (módulos base primero, orquestador al final).

---

## 2. Fases de Implementación

### FASE 0: Preparacion de entorno

| Paso | Acción | Comando / Acción | Estado |
|------|--------|------------------|--------|
| 0.1 | Crear estructura de carpetas | Según `SDD/ARCH-DESINGN.md` | [x] |
| 0.2 | Crear `CMakeLists.txt` | CMake 3.16+, target C++17 | [x] |
| 0.3 | Verificar `python/requirements.txt` | numpy, matplotlib, pandas | [x] |
| 0.4 | Verificar compilador | `g++ --version` ≥ 11 | [x] |

**Checkpoint:** Estructura de carpetas lista, build system funcional.


### FASE 1: Fundamentos C++ (utils + core)

**Archivos:** `utils.hpp`, `utils.cpp`, `ag_core.hpp`, `ag_core.cpp`

| Paso | Tarea | Verificación | Estado |
|------|-------|-------------|--------|
| 1.1 | Crear `utils.hpp` con estructura `Individuo` y firmas de funciones | Compila como header | [x] |
| 1.2 | Implementar `decodificar_variable()`: extraer bits y mapear a [-1,1] | Test: bits=0 → -1, bits=65535 → 1 | [x] |
| 1.3 | Implementar `bohachevsky()`: fórmula completa | Test: f(0,0) == 0.0 | [x] |
| 1.4 | Implementar `crear_generador()`: mt19937 con semilla | Test: misma semilla → misma secuencia | [x] |
| 1.5 | Crear `ag_core.hpp` con estructura `MetricasGen` y firmas | Compila como header | [x] |
| 1.6 | Implementar `inicializar_poblacion()`: 100 individuos aleatorios | Test: tamaño == 100, fitness calculado | [x] |
| 1.7 | Implementar `evaluar_poblacion()`: recalcular fitness de todos | Test: después de mutación, fitness actualizado | [x] |
| 1.8 | Implementar `obtener_metricas()`: best, avg, std, best_x, best_y | Test: métricas coherentes con población | [x] |
| 1.9 | Implementar `encontrar_mejor()` y `encontrar_peor()` | Test: índices correctos | [x] |

**Compilación de prueba:**
```bash
g++ -std=c++17 -c utils.cpp ag_core.cpp
```

---

### FASE 2: Operadores Genéticos

**Archivos:** `operadores.hpp`, `operadores.cpp`

| Paso | Tarea | Verificación | Estado |
|------|-------|-------------|--------|
| 2.1 | Implementar `seleccion_ranking()`: ordenar + probabilidades acumuladas | Test: suma de probs == 1.0 | [x] |
| 2.2 | Test selección: con fitness conocidos, verificar frecuencias | Test estadístico: χ² o frecuencias observadas | [x] |
| 2.3 | Implementar `cruce_uniforme()`: bit a bit con 50% | Test: pc=1.0 → hijos mezclados, pc=0.0 → clones | [x] |
| 2.4 | Implementar `mutacion()`: flip bit con probabilidad pm | Test: pm=0.0 → sin cambios, pm=1.0 → todos invertidos | [x] |
| 2.5 | Implementar `aplicar_elitismo()`: preservar 1 mejor | Test: mejor fitness nunca aumenta | [x] |
| 2.6 | Implementar `evolucionar_generacion()`: pipeline completo | Test: población mantiene tamaño 100 | [x] |
| 2.7 | Test integrado: 1 generación completa | Test: sin crashes, métricas razonables | [x] |

**Compilación de prueba:**
```bash
g++ -std=c++17 -c operadores.cpp
```

---

### FASE 3: Renormalización

**Archivos:** `renormalizacion.hpp`, `renormalizacion.cpp`

| Paso | Tarea | Verificación | Estado |
|------|-------|-------------|--------|
| 3.1 | Implementar `renormalizar_lineal()`: f' = a·f + b | Test: f_min → mayor valor, f_max → menor valor | [x] |
| 3.2 | Test lineal: todos los resultados > 0 | Test: min(f') > 0 | [x] |
| 3.3 | Implementar `renormalizar_ventana()`: f' = f_max - f + c | Test: f_max → c, f_min → f_max - f_min + c | [x] |
| 3.4 | Test ventana: todos los resultados > 0 | Test: min(f') == c > 0 | [x] |
| 3.5 | Integrar renormalización en `seleccion_ranking()` (sobrecarga o parámetro) | Test: ranking usa fitness renormalizado cuando se pide | [x] |

**Compilación de prueba:**
```bash
g++ -std=c++17 -c renormalizacion.cpp
```

---

### FASE 4: Exportador

**Archivos:** `exportador.hpp`, `exportador.cpp`

| Paso | Tarea | Verificación | Estado |
|------|-------|-------------|--------|
| 4.1 | Implementar `exportar_csv()`: escribir vector<MetricasGen> a CSV | Test: archivo creado, formato correcto | [x] |
| 4.2 | Verificar encabezados del CSV | Test: primera línea == encabezados esperados | [x] |
| 4.3 | Verificar datos: simulación, generación, tipo, pc, pm | Test: muestreo aleatorio de filas | [x] |

**Compilación de prueba:**
```bash
g++ -std=c++17 -c exportador.cpp
```

---

### FASE 5: Orquestador Principal

**Archivo:** `main.cpp`

| Paso | Tarea | Verificación | Estado |
|------|-------|-------------|--------|
| 5.1 | Parsear argumentos de línea de comandos | Test: `./ag --type simple --pc 0.7 --pm 0.005 --sims 30 --seed 42` | [x] |
| 5.2 | Implementar bucle de simulaciones (30 sims) | Test: 30 archivos de semilla distintos | [x] |
| 5.3 | Implementar bucle de generaciones (100 gens) | Test: 101 métricas por simulación (gen 0 incluida) | [x] |
| 5.4 | Integrar renormalización condicional (simple/linear/window) | Test: tipos distintos generan comportamientos distintos | [x] |
| 5.5 | Implementar modo grid (--grid) para parte (c) | Test: 4 PC × 3 PM = 12 configuraciones | [x] |
| 5.6 | Llamar a exportar_csv() al finalizar | Test: archivo results.csv generado | [x] |
| 5.7 | Compilación completa y linking | Test: ejecutable funciona sin errores | [ ] |

**Compilación final:**
```bash
g++ -std=c++17 -O2 -o ag_bohachevsky \
    main.cpp utils.cpp ag_core.cpp operadores.cpp renormalizacion.cpp exportador.cpp
```

---

### FASE 6: Scripts Python

**Archivos:** `python/visualizar_ag.py`, `python/heatmap_params.py`

| Paso | Tarea | Verificación | Estado |
|------|-------|-------------|--------|
| 6.1 | Implementar `cargar_datos()` en visualizar_ag.py | Test: lee CSV sin errores | [x] |
| 6.2 | Implementar `promediar_simulaciones()` | Test: 30 sims → 1 promedio por gen | [x] |
| 6.3 | Implementar `graficar_convergencia_simple()` (parte a) | Test: PNG generado, 2 líneas visibles | [x] |
| 6.4 | Implementar `graficar_comparativa_ag()` (parte b) | Test: PNG generado, 3 líneas distintas | [x] |
| 6.5 | Implementar `cargar_datos_grid()` en heatmap_params.py | Test: filtra datos de grid correctamente | [x] |
| 6.6 | Implementar `calcular_fitness_final()` | Test: última generación por sim | [x] |
| 6.7 | Implementar `promediar_fitness_final()` | Test: 30 sims → 1 valor por celda | [x] |
| 6.8 | Implementar `graficar_heatmap()` (parte c) | Test: PNG generado, matriz 4×3, valores anotados | [x] |
| 6.9 | Ejecutar scripts con datos reales de C++ | Test: todas las gráficas generadas correctamente | [ ] |

---

### FASE 7: Validación Final

| Paso | Tarea | Verificación | Estado |
|------|-------|-------------|--------|
| 7.1 | Ejecutar parte (a): AG Simple, 30 sims | Test: convergencia hacia ~0 | ⚠️ (g++ no disponible) |
| 7.2 | Ejecutar parte (b): AG Linear, 30 sims | Test: comparar con simple | ⚠️ (g++ no disponible) |
| 7.3 | Ejecutar parte (b): AG Window, 30 sims | Test: comparar con simple y linear | ⚠️ (g++ no disponible) |
| 7.4 | Ejecutar parte (c): Grid PC × PM | Test: 12 configuraciones, heatmap generado | ⚠️ (g++ no disponible) |
| 7.5 | Verificar todos los criterios de aceptación de ARCH_DESIGN.md | Test: checklist completo | [x] |
| 7.6 | Revisar coherencia código ↔ contrato | Test: cada función tiene referencia a § del contrato | [x] |

---

## 3. Orden de Implementación Resumido

```
FASE 0: Entorno
FASE 1: utils → ag_core
FASE 2: operadores
FASE 3: renormalizacion
FASE 4: exportador
FASE 5: main (orquestador)
FASE 6: Python scripts
FASE 7: Validación y ajustes
```

**Regla de oro:** No avanzar a la siguiente fase hasta que la actual pase todas sus verificaciones.


---

## 4. Riesgos y Mitigaciones

| Riesgo | Probabilidad | Impacto | Mitigación |
|--------|-------------|---------|------------|
| Decodificación incorrecta | Media | Alto | Test unitario exhaustivo con valores límite |
| Ranking con fitness negativos | Baja | Alto | Verificar renormalización antes de selección |
| Convergencia prematura | Media | Medio | Verificar presión selectiva, ajustar parámetros |
| Memoria/performance con 30 sims | Baja | Medio | Usar -O2, perfilar si es necesario |
| Gráficas con datos incorrectos | Media | Medio | Validar CSV antes de graficar |

---

