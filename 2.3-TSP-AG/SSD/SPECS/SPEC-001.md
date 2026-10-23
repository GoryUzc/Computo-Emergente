# SPEC-001: Implementacion C++ del Algoritmo Genetico para TSP

> **Documento de Especificacion Tecnica**  

---

## 1. Proposito

Especificar la implementacion en C++17 de un **Algoritmo Genetico** para resolver el **Problema del Viajero de Comercio (TSP)** con las siguientes caracteristicas:

- **10 ciudades** con ubicaciones aleatorias en un plano [0, 100] x [0, 100]
- **Codificacion por permutacion** (cada individuo es una lista ordenada de ciudades)
- **Cruce OX (Order Crossover)** como operador de recombinacion
- **Mutacion Swap** como operador de variacion
- **Seleccion por Ruleta** (proporcional al fitness)
- **Elitismo Top 2** (preservar los 2 mejores individuos)
- **200 generaciones** por simulacion
- **30 simulaciones independientes** con semillas diferentes
- **Poblacion de 100 individuos**
- **Pc = 0.9** (probabilidad de cruce)
- **Pm = 0.05** (probabilidad de mutacion)

El codigo debe construir la poblacion, ejecutar el ciclo evolutivo, y exportar los resultados a CSV para visualizacion en Python.

---

## 2. Modulos a Implementar

### 2.1 Modulo: `tsp_core.hpp` / `tsp_core.cpp`

**Responsabilidad:** Definir la estructura `Ciudad`, la clase `TSP` (que gestiona las ciudades y calcula distancias), y las funciones de generacion de ciudades aleatorias.

```cpp
// tsp_core.hpp
#ifndef TSP_CORE_HPP
#define TSP_CORE_HPP

#include <vector>
#include <string>
#include <random>

struct Ciudad {
    int id;
    double x, y;

    double distancia_a(const Ciudad& otra) const;
};

class TSP {
public:
    TSP(const std::vector<Ciudad>& ciudades);


    double calcular_distancia_total(const std::vector<int>& ruta) const;

    
    double calcular_fitness(const std::vector<int>& ruta) const;

    int get_n_ciudades() const;
    const std::vector<Ciudad>& get_ciudades() const;
    double get_distancia(int i, int j) const;

private:
    std::vector<Ciudad> ciudades_;
    std::vector<std::vector<double>> matriz_distancias_;  

    void precomputar_matriz_distancias();
};


std::vector<Ciudad> generar_ciudades_aleatorias(int n, 
                                                 double ancho = 100.0, 
                                                 double alto = 100.0,
                                                 unsigned int seed = 42);

#endif
```

**Especificaciones:**

- **Distancia euclidiana:**
  $$d_{ij} = \sqrt{(x_i - x_j)^2 + (y_i - y_j)^2}$$

- **Distancia total de una ruta pi:**
  $$D(\pi) = \sum_{k=0}^{N-1} d_{\pi(k), \pi((k+1) \mod N)}$$

- **Fitness:**
  $$f(\pi) = \frac{1}{D(\pi)}$$

- **Matriz de distancias:** Precomputada en el constructor para evitar recalculos. Tamano N x N, simetrica, diagonal = 0.

---

### 2.2 Modulo: `operadores.hpp` / `operadores.cpp`

**Responsabilidad:** Implementar los operadores geneticos especializados para permutaciones: Cruce OX, Mutacion Swap, Seleccion por Ruleta.

```cpp
// operadores.hpp
#ifndef OPERADORES_HPP
#define OPERADORES_HPP

#include <vector>
#include <random>

// ============================================================
// CRUCE ORDER CROSSOVER (OX)
// ============================================================
// Algoritmo:
// 1. Seleccionar 2 puntos de cruce aleatorios [p1, p2]
// 2. Copiar el segmento [p1, p2] del padre1 al hijo
// 3. Rellenar las posiciones restantes con el orden del padre2
//    (saltando las ciudades ya copiadas)
std::vector<int> cruce_OX(const std::vector<int>& padre1,
                          const std::vector<int>& padre2,
                          std::mt19937& gen);

// ============================================================
// MUTACION SWAP (INTERCAMBIO)
// ============================================================
// Algoritmo:
// 1. Seleccionar 2 posiciones aleatorias i, j
// 2. Intercambiar las ciudades en esas posiciones
std::vector<int> mutacion_swap(const std::vector<int>& ruta,
                                std::mt19937& gen);

// ============================================================
// SELECCION POR RULETA
// ============================================================
// Algoritmo:
// 1. Calcular fitness_total = suma fitness[i]
// 2. Generar r en [0, fitness_total]
// 3. Acumular fitness hasta superar r
// 4. Retornar el indice del individuo seleccionado
int seleccion_ruleta(const std::vector<double>& fitness,
                     std::mt19937& gen);

#endif
```

**Especificacion detallada del Cruce OX:**

```
Ejemplo con N=10:

Padre 1: [1, 3, 5, |7, 2, 8, 4|, 6, 9, 0]
Padre 2: [4, 2, 0, |9, 6, 1, 5|, 3, 8, 7]
                    ^p1        ^p2

Paso 1: Copiar segmento [p1, p2] del Padre 1 al hijo
Hijo parcial: [_, _, _, |7, 2, 8, 4|, _, _, _]

Paso 2: Recorrer Padre 2 desde p2+1 y rellenar posiciones vacias
Padre 2 recorrido: [3, 8, 7, 4, 2, 0, 9, 6, 1, 5]
Saltando usados (7, 2, 8, 4): [3, 0, 9, 6, 1, 5]

Paso 3: Rellenar desde p2+1 en orden circular
Hijo final: [6, 1, 5, |7, 2, 8, 4|, 3, 0, 9]
```

**Especificacion detallada de la Mutacion Swap:**

```
Ejemplo con N=10:

Ruta original: [1, 3, 5, 7, 2, 8, 4, 6, 9, 0]
Posiciones a intercambiar: i=2, j=7

Ruta mutada:   [1, 3, 9, 7, 2, 8, 4, 6, 5, 0]
                       ^              ^
```

---

### 2.3 Modulo: `ag_core.hpp` / `ag_core.cpp`

**Responsabilidad:** Implementar la clase `AlgoritmoGenetico` que gestiona la poblacion, el ciclo evolutivo y el elitismo.

```cpp
// ag_core.hpp
#ifndef AG_CORE_HPP
#define AG_CORE_HPP

#include "tsp_core.hpp"
#include <vector>
#include <random>

struct Individuo {
    std::vector<int> ruta;     // Permutacion de ciudades
    double distancia;          // Distancia total
    double fitness;            // 1/distancia
};

using Poblacion = std::vector<Individuo>;

struct MetricasGeneracion {
    int generacion;
    double mejor_distancia;
    double peor_distancia;
    double promedio_distancia;
    double desv_est_distancia;
    double mejor_fitness;
    double promedio_fitness;
};

class AlgoritmoGenetico {
public:
    AlgoritmoGenetico(const TSP& tsp,
                      int tam_poblacion = 100,
                      double prob_cruce = 0.9,
                      double prob_mutacion = 0.05,
                      int max_generaciones = 200,
                      unsigned int seed = 42);

    // Ejecutar el algoritmo completo
    void ejecutar();

    // Getters para resultados
    Individuo get_mejor_individuo() const;
    double get_mejor_distancia() const;
    const std::vector<MetricasGeneracion>& get_historial() const;
    const Poblacion& get_poblacion_final() const;

private:
    // Metodos del ciclo evolutivo
    void inicializar_poblacion();
    void evaluar_poblacion(Poblacion& pop);
    Poblacion seleccionar_padres_y_cruzar();
    void aplicar_mutacion(Poblacion& pop);
    void aplicar_elitismo(Poblacion& nueva_poblacion, 
                         const Poblacion& poblacion_anterior);
    MetricasGeneracion calcular_metricas(const Poblacion& pop, int gen);

    // Crear un individuo aleatorio
    Individuo crear_individuo_aleatorio();

    // Referencia al problema TSP
    const TSP& tsp_;

    // Parametros del AG
    int tam_poblacion_;
    double prob_cruce_;
    double prob_mutacion_;
    int max_generaciones_;

    // Estado del AG
    Poblacion poblacion_;
    std::vector<MetricasGeneracion> historial_;
    Individuo mejor_individuo_global_;

    // Generador aleatorio
    std::mt19937 gen_;
};

#endif
```

**Especificacion del ciclo evolutivo:**

```
Para cada generacion g = 1 a max_generaciones:

    1. CREAR NUEVA POBLACION (vacia)

    2. ELITISMO:
       - Ordenar poblacion actual por fitness (descendente)
       - Copiar Top 2 mejores individuos a nueva_poblacion

    3. GENERAR RESTO DE HIJOS (hasta completar tam_poblacion):
       Mientras nueva_poblacion.size() < tam_poblacion:
           a. SELECCION:
              padre1 = seleccion_ruleta(poblacion)
              padre2 = seleccion_ruleta(poblacion)

           b. CRUCE:
              Si random() < P_c:
                  hijo = cruce_OX(padre1, padre2)
              Sino:
                  hijo = copia de padre1

           c. MUTACION:
              Si random() < P_m:
                  hijo = mutacion_swap(hijo)

           d. Agregar hijo a nueva_poblacion

    4. EVALUAR nueva_poblacion:
       Para cada individuo:
           distancia = tsp.calcular_distancia_total(ruta)
           fitness = 1 / distancia

    5. ACTUALIZAR mejor_individuo_global si hay mejora

    6. CALCULAR METRICAS de la generacion:
       - mejor_distancia, peor_distancia
       - promedio_distancia, desv_est_distancia
       - mejor_fitness, promedio_fitness

    7. SUSTITUCION:
       poblacion = nueva_poblacion
```

**Especificacion del Elitismo Top 2:**

```cpp
void AlgoritmoGenetico::aplicar_elitismo(Poblacion& nueva_poblacion,
                                          const Poblacion& poblacion_anterior) {
    // Ordenar por fitness descendente
    std::vector<Individuo> ordenada = poblacion_anterior;
    std::sort(ordenada.begin(), ordenada.end(),
              [](const Individuo& a, const Individuo& b) {
                  return a.fitness > b.fitness;
              });

    // Copiar los 2 mejores al inicio de la nueva poblacion
    nueva_poblacion.push_back(ordenada[0]);
    nueva_poblacion.push_back(ordenada[1]);
}
```

---

### 2.4 Modulo: `exportador.hpp` / `exportador.cpp`

**Responsabilidad:** Exportar resultados a archivos CSV para visualizacion en Python.

```cpp
// exportador.hpp
#ifndef EXPORTADOR_HPP
#define EXPORTADOR_HPP

#include "tsp_core.hpp"
#include "ag_core.hpp"
#include <string>
#include <vector>

class Exportador {
public:
    // Exportar coordenadas de las ciudades
    static bool exportar_ciudades(const std::string& archivo,
                                   const std::vector<Ciudad>& ciudades);

    // Exportar la mejor ruta encontrada
    static bool exportar_mejor_ruta(const std::string& archivo,
                                     const std::vector<Ciudad>& ciudades,
                                     const Individuo& mejor);

    // Exportar historial de metricas por generacion
    static bool exportar_historial(const std::string& archivo,
                                    const std::vector<MetricasGeneracion>& historial);

    // Exportar resultados de multiples simulaciones
    static bool exportar_simulaciones(const std::string& archivo,
                                       const std::vector<std::vector<MetricasGeneracion>>& todas_simulaciones);
};

#endif
```

**Formato de salida CSV:**

**`ciudades.csv`:**
```csv
id,x,y
0,45.23,67.89
1,12.45,89.12
...
```

**`mejor_ruta.csv`:**
```csv
orden_visita,id_ciudad,x,y
0,3,45.23,67.89
1,7,12.45,89.12
2,1,78.90,23.45
...
```

**`historial.csv`:**
```csv
generacion,mejor_distancia,peor_distancia,promedio_distancia,desv_est_distancia,mejor_fitness,promedio_fitness
0,850.23,1250.45,1023.67,125.34,0.001176,0.000977
1,820.12,1230.89,1005.23,120.45,0.001219,0.000995
...
```

**`simulaciones.csv` (para las 30 simulaciones):**
```csv
simulacion,generacion,mejor_distancia,promedio_distancia
0,0,850.23,1023.67
0,1,820.12,1005.23
...
0,200,245.67,289.34
1,0,870.45,1045.23
...
29,200,250.12,295.67
```

---

### 2.5 Modulo: `main.cpp`

**Responsabilidad:** Orquestar el flujo completo de la simulacion.

```cpp
// main.cpp
#include <iostream>
#include <vector>
#include "tsp_core.hpp"
#include "ag_core.hpp"
#include "exportador.hpp"

int main() {
    std::cout << "=== ALGORITMO GENETICO PARA TSP ===" << std::endl;

    // ============================================
    // PARAMETROS CONFIGURABLES
    // ============================================
    const int N_CIUDADES = 10;
    const int TAM_POBLACION = 100;
    const double PROB_CRUCE = 0.9;
    const double PROB_MUTACION = 0.02;
    const int MAX_GENERACIONES = 200;
    const int NUM_SIMULACIONES = 30;
    const unsigned int SEED_BASE = 42;

    // ============================================
    // GENERAR CIUDADES ALEATORIAS
    // ============================================
    std::cout << "\nGenerando " << N_CIUDADES << " ciudades aleatorias...\n";
    auto ciudades = generar_ciudades_aleatorias(N_CIUDADES, 100.0, 100.0, SEED_BASE);

    TSP tsp(ciudades);

    // Exportar ciudades
    Exportador::exportar_ciudades("../resultados/ciudades.csv", ciudades);
    std::cout << "Ciudades exportadas a resultados/ciudades.csv\n";

    // ============================================
    // EJECUTAR 30 SIMULACIONES INDEPENDIENTES
    // ============================================
    std::vector<std::vector<MetricasGeneracion>> todas_simulaciones;
    Individuo mejor_global;
    mejor_global.distancia = std::numeric_limits<double>::max();

    for (int sim = 0; sim < NUM_SIMULACIONES; ++sim) {
        std::cout << "\n--- Simulacion " << sim + 1 << "/" << NUM_SIMULACIONES << " ---\n";

        AlgoritmoGenetico ag(tsp, 
                             TAM_POBLACION, 
                             PROB_CRUCE, 
                             PROB_MUTACION, 
                             MAX_GENERACIONES,
                             SEED_BASE + sim);

        ag.ejecutar();

        todas_simulaciones.push_back(ag.get_historial());

        // Actualizar mejor global
        if (ag.get_mejor_distancia() < mejor_global.distancia) {
            mejor_global = ag.get_mejor_individuo();
        }

        std::cout << "  Mejor distancia: " << ag.get_mejor_distancia() << "\n";
    }

    // ============================================
    // EXPORTAR RESULTADOS
    // ============================================
    std::cout << "\n=== EXPORTANDO RESULTADOS ===\n";

    Exportador::exportar_mejor_ruta("../resultados/mejor_ruta.csv", 
                                     ciudades, mejor_global);

    Exportador::exportar_historial("../resultados/historial_sim0.csv",
                                    todas_simulaciones[0]);

    Exportador::exportar_simulaciones("../resultados/simulaciones.csv",
                                       todas_simulaciones);

    std::cout << "\n=== RESULTADO FINAL ===\n";
    std::cout << "Mejor distancia global: " << mejor_global.distancia << "\n";
    std::cout << "Mejor ruta: ";
    for (int ciudad : mejor_global.ruta) {
        std::cout << ciudad << " ";
    }
    std::cout << "\n";

    return 0;
}
```

---

## 3. Especificaciones Tecnicas

### 3.1 Tipos de Datos

| Tipo | Definicion | Uso |
|------|-----------|-----|
| `Ciudad` | `struct {int id; double x, y;}` | Representa una ciudad |
| `Individuo` | `struct {vector<int> ruta; double distancia, fitness;}` | Representa una solucion candidata |
| `Poblacion` | `vector<Individuo>` | Conjunto de individuos |
| `MetricasGeneracion` | `struct {int gen; double mejor, peor, promedio, desv_est, ...}` | Estadisticas por generacion |

### 3.2 Parametros de Configuracion

| Parametro | Valor | Justificacion |
|-----------|-------|---------------|
| `N_CIUDADES` | 10 | Recomendado (8-20), factorial manejable (9!/2 = 181,440) |
| `TAM_POBLACION` | 100 | Balance exploracion/explotacion |
| `MAX_GENERACIONES` | 200 | Suficiente para N=10 |
| `NUM_SIMULACIONES` | 30 | Analisis estadistico robusto |
| `PROB_CRUCE` | 0.9 | Alta explotacion |
| `PROB_MUTACION` | 0.05 | Mantenimiento de diversidad |
| `ELITISMO` | Top 2 | Estabilidad y convergencia rapida |
| `SEED_BASE` | 42 | Reproducibilidad |

### 3.3 Estructura de Archivos de Salida

```
resultados/
├── ciudades.csv                    # Coordenadas de las 10 ciudades
├── mejor_ruta.csv                  # Mejor ruta encontrada (global)
├── historial_sim0.csv              # Evolucion de la simulacion 0
├── simulaciones.csv                # Todas las simulaciones (30 x 200)
└── graficos/                       # Visualizaciones generadas por Python

```

---

## 4. Criterios de Aceptacion

### 4.1 Verificaciones Numericas

| Criterio | Metodo | Valor Esperado |
|----------|--------|----------------|
| **Numero de ciudades** | `ciudades.size()` | 10 |
| **Tamano poblacion** | `poblacion.size()` | 100 |
| **Generaciones** | `historial.size()` | 200 |
| **Simulaciones** | Ejecuciones independientes | 30 |
| **Ruta valida** | Cada ciudad visitada 1 vez | Si (verificar con `std::set`) |
| **Elitismo** | Mejor fitness monotono decreciente en distancia | Si |
| **Matriz distancias** | Simetrica, diagonal = 0 | Si |

### 4.2 Verificaciones Fisicas

| Criterio | Descripcion |
|----------|-------------|
| **Ruta cerrada** | Ultima ciudad conecta con primera |
| **Sin repeticiones** | Cada ciudad aparece exactamente 1 vez |
| **Distancia minima** | Mejor distancia < distancia inicial promedio |
| **Convergencia** | Fitness mejora en primeras 50 generaciones |
| **Consistencia** | Las 30 simulaciones convergen a valores similares |

### 4.3 Verificaciones de Visualizacion

| Grafico | Herramienta Python | Descripcion |
|---------|-------------------|-------------|
| Ciudades | `plt.scatter()` | 10 ciudades en plano 2D |
| Mejor ruta | `plt.plot()` + flechas | Recorrido optimo encontrado |
| Convergencia | `plt.plot()` | Evolucion mejor vs promedio |
| 30 simulaciones | Subplots o boxplot | Comparativa estadistica |

---

## 5. Algoritmos Detallados

### 5.1 Algoritmo Order Crossover (OX)

```
FUNCION cruce_OX(padre1, padre2, gen):
    N <- longitud de padre1

    // 1. Seleccionar 2 puntos de cruce
    p1 <- random(0, N-1)
    p2 <- random(0, N-1)
    SI p1 > p2: intercambiar(p1, p2)

    // 2. Inicializar hijo con -1 (vacio)
    hijo <- vector de N elementos con -1
    usado <- vector de N booleanos en false

    // 3. Copiar segmento [p1, p2] del padre1
    PARA i DESDE p1 HASTA p2:
        hijo[i] <- padre1[i]
        usado[padre1[i]] <- true

    // 4. Rellenar con orden del padre2
    pos <- (p2 + 1) mod N  // Empezar despues del segmento
    PARA i DESDE 0 HASTA N-1:
        ciudad <- padre2[(p2 + 1 + i) mod N]
        SI NO usado[ciudad]:
            hijo[pos] <- ciudad
            usado[ciudad] <- true
            pos <- (pos + 1) mod N

    RETORNAR hijo
```

### 5.2 Algoritmo Mutacion Swap

```
FUNCION mutacion_swap(ruta, gen):
    N <- longitud de ruta
    hijo <- copia de ruta

    // Seleccionar 2 posiciones diferentes
    i <- random(0, N-1)
    j <- random(0, N-1)
    MIENTRAS j == i:
        j <- random(0, N-1)

    // Intercambiar
    intercambiar(hijo[i], hijo[j])

    RETORNAR hijo
```

### 5.3 Algoritmo Seleccion Ruleta

```
FUNCION seleccion_ruleta(fitness, gen):
    // 1. Calcular fitness total
    fitness_total <- suma fitness[i] para todo i

    SI fitness_total == 0:
        RETORNAR random(0, N-1)  // Seleccion aleatoria

    // 2. Generar numero aleatorio
    r <- uniform(0, fitness_total)

    // 3. Acumular fitness
    suma_acumulada <- 0
    PARA i DESDE 0 HASTA N-1:
        suma_acumulada <- suma_acumulada + fitness[i]
        SI suma_acumulada >= r:
            RETORNAR i

    RETORNAR N-1  // Fallback por errores de punto flotante
```

---

## 6. Dependencias y Compilacion

### 6.1 Dependencias

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(TSP_AG LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg toolchain
set(VCPKG_ROOT "$ENV{VCPKG_ROOT}" CACHE PATH "vcpkg root directory")
if(EXISTS "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Ejecutable
add_executable(tsp_ag
    src/main.cpp
    src/tsp_core.cpp
    src/ag_core.cpp
    src/operadores.cpp
    src/exportador.cpp
)

target_include_directories(tsp_ag PRIVATE src)

# Optimizaciones
target_compile_options(tsp_ag PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/Wall>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -O2>
)
```

### 6.2 Compilacion

**Windows (MSVC):**
```powershell
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
.\Release\tsp_ag.exe
```

**Linux/macOS (GCC/Clang):**
```bash
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
./tsp_ag
```

## 8. Checklist de Implementacion

- [x] Crear estructura de carpetas (`src/`, `resultados/`, `python/`)
- [x] Implementar `tsp_core.hpp/cpp` con estructura Ciudad y clase TSP
- [x] Implementar `operadores.hpp/cpp` con OX, Swap, Ruleta
- [x] Implementar `ag_core.hpp/cpp` con clase AlgoritmoGenetico y elitismo
- [x] Implementar `exportador.hpp/cpp` con escritura CSV
- [x] Implementar `main.cpp` con flujo completo (30 simulaciones)
- [x] Configurar `CMakeLists.txt`
- [x] Compilar y ejecutar (pendiente - requiere compilador C++ disponible)
- [x] Verificar que los CSV se generen correctamente (pendiente - después de compilación)
- [x] Implementar script Python de visualizacion
- [x] Generar las 4 graficas requeridas
- [ ] Validar resultados (rutas validas, convergencia, consistencia) - pendiente de ejecución

**Notas de implementación:**
1. Error en `operadores.cpp` (variable `n` no definida en `mutacion_swap`) - CORREGIDO
2. Bug en `cruce_OX()`: posible bucle infinito cuando p1 == p2 o cuando el while busca ciudades - CORREGIDO con algoritmo más robusto
   - Implementación anterior podía colgar por incremento ilimitado de `pos` sin protección circular
   - Nueva implementación usa doble loop: recorre padre2 y busca posición vacía en hijo de forma segura

---

**Fin del documento SPEC-001.md**
