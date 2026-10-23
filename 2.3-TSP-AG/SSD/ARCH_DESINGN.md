# ARQUITECTURA Y DISENO: Proyecto - Algoritmo Genetico para TSP

> **Problema:** Traveling Salesman Problem (TSP) mediante Algoritmo Genetico

---

## 1. Stack

| Capa | Tecnologia | Version | Proposito |
|------|-----------|---------|-----------|
| **Calculo numerico** | C++17 | ISO/IEC 14882:2017 | Nucleo: AG, operadores, evaluacion |
| **Generacion aleatoria** | `<random>` (STL) | C++11+ | Mersenne Twister para reproducibilidad |
| **Exportacion** | `std::ofstream` (STL) | C++11+ | Escritura de resultados CSV |
| **Build** | CMake | 3.16+ | Generacion de targets |
| **Visualizacion** | Python 3.10+ | 3.10+ | Rutas, convergencia, evolucion |
| **Graficos** | matplotlib | 3.7+ | Renderizado de figuras |
| **Analisis datos** | numpy, pandas | 2.0+ | Carga y procesamiento de CSV |

**Compilador:** Visual Studio 2022 (MSVC) / GCC 11+ / Clang 14+

**Nota:** El proyecto es 100% autocontenido usando solo la libreria estandar de C++17. No se requieren dependencias externas.

---

## 2. Objetivo

Implementar un **Algoritmo Genetico** para resolver el **Problema del Viajero de Comercio (TSP)** en un conjunto de 10 ciudades con ubicaciones aleatorias, encontrando el recorrido de longitud minima que visite todas las ciudades y regrese al punto de partida.

**Funcion objetivo (minimizar):**

$$\min_{\pi} D(\pi) = \sum_{i=1}^{N} d_{\pi(i), \pi(i+1)} \quad \text{con } \pi(N+1) = \pi(1)$$

**Fitness (maximizar):**

$$f(\pi) = \frac{1}{D(\pi)}$$

**Flujo:**

```
Ciudades (x,y)  ->  Matriz distancias  ->  Poblacion inicial  ->  Evaluacion  ->  Seleccion (Ruleta)  ->  Cruce (OX)  ->  Mutacion (Swap)  ->  Elitismo (Top 2)  ->  CSV  ->  Python  ->  Visualizaciones
```

---

## 3. Conceptos Fundamentales

### 3.1 El Problema del Viajero de Comercio (TSP)

| Propiedad | Valor |
|-----------|-------|
| **Tipo** | Optimizacion combinatoria |
| **Complejidad** | NP-completo |
| **Espacio de busqueda** | $(N-1)!/2$ rutas posibles |
| **Para N=10** | $9!/2 = 181,440$ rutas |
| **Restriccion** | Cada ciudad visitada exactamente una vez |
| **Objetivo** | Minimizar distancia total del recorrido |

**Complejidad factorial:** Para $N$ ciudades, el numero de rutas posibles crece factorialmente:

- $N=5$: 12 rutas
- $N=10$: 181,440 rutas
- $N=15$: 43,589,145,600 rutas
- $N=20$: $6.08 \times 10^{16}$ rutas

### 3.2 Representacion del Individuo (Codificacion por Permutacion)

Cada individuo es una **permutacion** de las $N$ ciudades:

```
Individuo: [3, 7, 1, 5, 2, 9, 4, 8, 6, 0]
           |                              |
           Ciudad 3 (inicio)        Ciudad 0 (regreso a 3)
```

**Ventajas:**

- Representa directamente una ruta valida
- No hay individuos invalidos (no se repiten ciudades)
- Facil de visualizar y evaluar

**Desventajas:**

- Los operadores clasicos (cruce 1-punto, mutacion bit-flip) generan rutas invalidas
- Requiere operadores especializados

### 3.3 Generacion de Ciudades

Las 10 ciudades se generan aleatoriamente en un plano $[0, 100] \times [0, 100]$:

```cpp
struct Ciudad {
    int id;
    double x, y;
};
```

**Distancia euclidiana:**

$$d_{ij} = \sqrt{(x_i - x_j)^2 + (y_i - y_j)^2}$$

### 3.4 Seleccion por Ruleta (Roulette Wheel)

La probabilidad de seleccion del individuo $i$ es:

$$P_i = \frac{f_i}{\sum_{j=1}^{n} f_j}$$

Donde $f_i = 1/D_i$ es el fitness inverso a la distancia.

**Algoritmo de seleccion:**

```
1. Calcular fitness_total = suma f_j
2. Generar numero aleatorio r en [0, fitness_total]
3. Acumular fitness: suma = 0
4. Para cada individuo i:
   suma += f_i
   Si suma >= r: retornar i
```

### 3.5 Cruce Order Crossover (OX)

**Idea:** Copiar un segmento del padre 1, rellenar el resto con el orden del padre 2.

```
Padre 1: [1, 2, 3, |4, 5, 6, 7|, 8, 9]
Padre 2: [9, 3, 7, |8, 2, 4, 1|, 5, 6]

Paso 1: Copiar segmento del Padre 1
Hijo:    [_, _, _, |4, 5, 6, 7|, _, _]

Paso 2: Rellenar con orden del Padre 2 (saltando los ya usados)
Padre 2 sin usados: [9, 3, 8, 2, 1]
Hijo:    [9, 3, 8, |4, 5, 6, 7|, 2, 1]  <- VALIDO!
```

**Preserva:** El orden relativo de las ciudades del padre 2.

### 3.6 Mutacion Swap (Intercambio)

Intercambia dos ciudades al azar:

```
Antes: [1, 2, 3, 4, 5, 6]
Swap(2, 5):
Despues: [1, 5, 3, 4, 2, 6]
```

**Ventaja:** Simple, siempre produce rutas validas.

### 3.7 Elitismo (Top 2)

Los 2 mejores individuos de la generacion $t$ se copian **intactos** a la generacion $t+1$:

```
Generacion t:   [Ind1(fit=0.01), Ind2(fit=0.02), ..., Ind100]
                | (Top 2 se preservan)
Generacion t+1: [Ind1, Ind2, 98 nuevos hijos]
```

**Beneficios:**

- Garantiza mejora monotona del mejor fitness
- Evita perdida de soluciones excelentes
- Acelera convergencia

### 3.8 Parametros Configurados

| Parametro | Valor | Justificacion |
|-----------|-------|---------------|
| **N ciudades** | 10 | Recomendado (8-20), factorial manejable |
| **Poblacion** | 100 | Balance exploracion/explotacion |
| **Generaciones** | 200 | Suficiente para N=10 |
| **Simulaciones** | 30 | Analisis estadistico robusto |
| **Prob. Cruce (Pc)** | 0.9 | Alta explotacion |
| **Prob. Mutacion (Pm)** | 0.01 | Mantenimiento de diversidad |
| **Elitismo** | Top 2 | Estabilidad y convergencia rapida |
| **Seleccion** | Ruleta | Clasica, proporcional al fitness |

---

## 4. Algoritmo General

```
ENTRADA:  N ciudades, poblacion, generaciones, Pc, Pm
SALIDA:   resultados/mejor_ruta.csv, resultados/historial.csv

1. INICIALIZACION
   a. Generar N ciudades aleatorias en [0, 100] x [0, 100]
   b. Calcular matriz de distancias D[N][N]
   c. Crear poblacion inicial de permutaciones aleatorias
   d. Evaluar fitness de cada individuo

2. CICLO PRINCIPAL (generaciones)
   Para gen = 1..generaciones:

   a. SELECCION (n veces)
      Para cada hijo:
         padre1 = seleccionar_ruleta()
         padre2 = seleccionar_ruleta()

         Si random() < Pc:
            hijo = cruce_OX(padre1, padre2)
         Sino:
            hijo = padre1

         Si random() < Pm:
            hijo = mutacion_swap(hijo)

         Agregar hijo a nueva_poblacion

   b. ELITISMO
      Identificar Top 2 mejores de poblacion actual
      Reemplazar 2 peores de nueva_poblacion con Top 2

   c. EVALUAR NUEVA POBLACION
      Para cada individuo:
         distancia = calcular_distancia_total(ruta)
         fitness = 1 / distancia

      mejor_fitness = max(fitness)
      promedio_fitness = mean(fitness)

      Registrar en historial: gen, mejor_distancia, promedio_distancia

   d. SUSTITUCION
      poblacion = nueva_poblacion

3. EXPORTAR RESULTADOS
   a. Guardar mejor_ruta.csv con coordenadas de la mejor ruta
   b. Guardar historial.csv con evolucion del fitness
```

---

## 5. Estructura de Carpetas

```
2.3-TSP-AG/
├── CMakeLists.txt                       # Build system (CMake 3.16+)
├── README.md                            # Instrucciones de uso y compilacion
├── SDD/                                 # Specification-Driven Development
│   ├── SPECS/
│   │   ├── spec-001.md              # Implementacion C++: modulos, API
│   │   └── spec-002.md              # Script Python de visualizacion
│   └── ARCH_DESIGN.md               # Arquitectura, diseno y teoria (este archivo)
├── src/
│   ├── main.cpp                     # Orquestador del flujo
│   ├── tsp_core.hpp/cpp             # Estructura Ciudad, TSP, calculo distancias
│   ├── ag_core.hpp/cpp              # Clase AlgoritmoGenetico, poblacion
│   ├── operadores.hpp/cpp           # Cruce OX, Mutacion Swap
│   └── exportador.hpp/cpp           # Escritura de CSV
├── resultados/
│   ├── ciudades.csv                 # Coordenadas de las 10 ciudades
│   ├── mejor_ruta.csv               # Mejor ruta encontrada
│   ├── historial.csv                # Evolucion del fitness por generacion
│   └── graficos/                    # Visualizaciones generadas
│       ├── 01_ciudades.png
│       ├── 02_mejor_ruta.png
│       ├── 03_convergencia.png
│       └── 04_evolucion_30sims.png
├── python/
│   ├── visualizar_tsp.py            # Script principal de visualizacion
│   └── requirements.txt             # Dependencias: numpy, matplotlib, pandas
└── build/                           # Directorio de compilacion (generado)
```

---

## 6. Metricas y Criterios de Aceptacion

### 6.1 Verificaciones Numericas

| Criterio | Metodo | Valor Esperado |
|----------|--------|----------------|
| **Numero de ciudades** | `ciudades.size()` | 10 |
| **Tamano poblacion** | `poblacion.size()` | 100 |
| **Generaciones** | `historial.size()` | 200 |
| **Simulaciones** | Ejecuciones independientes | 30 |
| **Ruta valida** | Cada ciudad visitada 1 vez | Si |
| **Elitismo** | Mejor fitness monotono | Si |

### 6.2 Verificaciones Fisicas

| Criterio | Descripcion |
|----------|-------------|
| **Ruta cerrada** | Ultimo ciudad conecta con primera |
| **Sin repeticiones** | Cada ciudad aparece exactamente 1 vez |
| **Distancia minima** | Mejor distancia < distancia inicial promedio |
| **Convergencia** | Fitness mejora en primeras 50 generaciones |

### 6.3 Verificaciones de Visualizacion

| Grafico | Herramienta Python | Descripcion |
|---------|-------------------|-------------|
| Ciudades | `plt.scatter()` | 10 ciudades en plano 2D |
| Mejor ruta | `plt.plot()` + flechas | Recorrido optimo encontrado |
| Convergencia | `plt.plot()` | Evolucion mejor vs promedio |
| 30 simulaciones | Subplots | Comparativa estadistica |

---

## 7. Procedimiento de Compilacion y Ejecucion

### 7.1 Prerrequisitos

| Herramienta | Version minima | Comentario |
|-------------|----------------|------------|
| **CMake** | 3.16+ | Build system |
| **Compilador C++** | GCC 11+, Clang 14+, o MSVC 2022 | C++17 requerido |
| **Python** | 3.10+ | Visualizacion |

### 7.2 Compilacion (Windows - MSVC)

```powershell
# 1. Crear directorio de compilacion
mkdir build
cd build

# 2. Configurar con CMake
cmake .. -G "Visual Studio 17 2022"

# 3. Compilar en Release
cmake --build . --config Release

# 4. Ejecutar
.\Release\tsp_ag.exe
```

### 7.3 Compilacion (Linux/macOS - GCC/Clang)

```bash
# 1. Crear directorio de compilacion
mkdir build && cd build

# 2. Configurar con CMake
cmake ..

# 3. Compilar
cmake --build . -j$(nproc)

# 4. Ejecutar
./tsp_ag
```

### 7.4 Ejecucion del script Python

```bash
# Instalar dependencias
pip install -r python/requirements.txt

# Ejecutar visualizacion
cd python
python visualizar_tsp.py
```

### 7.5 Salida esperada

```
=== ALGORITMO GENETICO PARA TSP ===
Ciudades: 10
Poblacion: 100
Generaciones: 200
Simulaciones: 30

Generacion 0:   Mejor = 450.23, Promedio = 620.45
Generacion 50:  Mejor = 280.12, Promedio = 380.67
Generacion 100: Mejor = 245.89, Promedio = 310.23
Generacion 150: Mejor = 238.45, Promedio = 295.12
Generacion 200: Mejor = 235.67, Promedio = 289.34

Mejor ruta encontrada: [3, 7, 1, 5, 2, 9, 4, 8, 6, 0]
Distancia total: 235.67

Resultados exportados a resultados/
```

---

## 8. Entregables

### 8.1 Visualizacion de Ciudades

**Tipo:** Scatter plot  
**Proposito:** Mostrar las 10 ciudades en el plano  
**Herramienta Python:** `matplotlib.pyplot.scatter()`

### 8.2 Visualizacion de la Mejor Ruta

**Tipo:** Grafico de lineas con flechas  
**Proposito:** Mostrar el recorrido optimo encontrado  
**Herramienta Python:** `plt.plot()` + `plt.annotate()` con flechas

### 8.3 Grafico de Convergencia

**Tipo:** Grafico de lineas  
**Proposito:** Mostrar evolucion del mejor fitness vs promedio  
**Herramienta Python:** `plt.plot()` con 2 series

### 8.4 Analisis Estadistico (30 Simulaciones)

**Tipo:** Subplots o boxplot  
**Proposito:** Comparar rendimiento entre simulaciones  
**Herramienta Python:** `plt.subplots()` o `plt.boxplot()`

---

## 9. Consideraciones de Implementacion

### 9.1 Estructura de Datos

```cpp
struct Ciudad {
    int id;
    double x, y;
};

struct Individuo {
    std::vector<int> ruta;     // Permutacion de ciudades
    double distancia;          // Distancia total
    double fitness;            // 1/distancia
};

using Poblacion = std::vector<Individuo>;
```

### 9.2 Matriz de Distancias

```cpp
std::vector<std::vector<double>> matriz_distancias(N, std::vector<double>(N));

for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
        matriz_distancias[i][j] = calcular_distancia(ciudades[i], ciudades[j]);
    }
}
```

### 9.3 Operadores Geneticos

**Cruce OX:**

```cpp
std::vector<int> cruce_OX(const std::vector<int>& p1, 
                          const std::vector<int>& p2);
```

**Mutacion Swap:**

```cpp
std::vector<int> mutacion_swap(const std::vector<int>& ruta);
```

### 9.4 Elitismo

```cpp
void aplicar_elitismo(Poblacion& poblacion, 
                      const Poblacion& nueva_poblacion, 
                      int top_n = 2);
```

---

## 10. Referencias

1. **Goldberg, D. E.** (1989). *Genetic Algorithms in Search, Optimization, and Machine Learning.* Addison-Wesley. -> Fundamentos de AG, operadores.

2. **Holland, J. H.** (1975). *Adaptation in Natural and Artificial Systems.* University of Michigan Press. -> Teoria de esquemas.

3. **Mitchell, M.** (1996). *An Introduction to Genetic Algorithms.* MIT Press. -> Implementaciones practicas, TSP.

4. **Oliver, I. M., Smith, D. J., & Holland, J. R. C.** (1987). *A study of the crossover operators in genetic algorithms and its application to the travelling salesman problem.* -> Analisis de operadores para TSP.

5. **Lawler, E. L., et al.** (1985). *The Traveling Salesman Problem: A Guided Tour of Combinatorial Optimization.* Wiley. -> Teoria del TSP.

---

**Fin del documento ARCH_DESIGN.md**

---

## 11. Estado de Implementación (Actualizado 2026-07-01)

### 11.1 Módulos C++ Implementados
- [x] `tsp_core.hpp/cpp` - Estructura Ciudad y clase TSP con matriz de distancias
- [x] `operadores.hpp/cpp` - Cruce OX, Mutación Swap, Selección Ruleta (revisado y corregido)
- [x] `ag_core.hpp/cpp` - Clase AlgoritmoGenetico con elitismo Top 2
- [x] `exportador.hpp/cpp` - Exportación a CSV
- [x] `main.cpp` - Orquestador 30 simulaciones

### 11.2 Script Python Implementado
- [x] `visualizar_tsp.py` - Clase VisualizadorTSP con 4 gráficas
- [x] `requirements.txt` - Dependencias numpy, pandas, matplotlib

### 11.3 Bugs Corregidos
1. `operadores.cpp`: Variable `n` no definida en `mutacion_swap()` - CORREGIDO
2. `operadores.cpp`: Posible bucle infinito en `cruce_OX()` - CORREGIDO con algoritmo robusto
