# SPEC-001 — Implementación C++: Algoritmo Genético Simple (AGS)

> Especificación formal de la implementación C++ del AGS para maximización de $F(x) = x^2$.
>
> **Estado:** Especificación para implementación.

---

## 1. Módulo `ag_core` — Población y evaluación

### 1.1 Ficheros

| Fichero | Tipo | Líneas estimadas |
|---------|------|-----------------|
| `src/ag_core.hpp` | Header (declaraciones + structs) | ~40 |
| `src/ag_core.cpp` | Implementación | ~80 |

### 1.2 Dependencias

- `<vector>`, `<string>`, `<random>`, `<algorithm>`, `<numeric>`, `<cmath>`
- `<fstream>`, `<iomanip>` para exportación

### 1.3 Estructuras de datos

```cpp

struct Individuo {
    std::vector<int> cromosoma;  
    double fitness;              
    double x_fenotipo;            
};

struct Poblacion {
    std::vector<Individuo> individuos;
    int n;                        
    int L;                        
    bool es_real;                 
    int precision;               
};

struct MetricasGeneracion {
    int generacion;
    double mejor_fitness;
    double fitness_promedio;
    double mejor_x;
};
```

### 1.4 API

#### `Poblacion inicializarPoblacion(int n, int L, bool es_real, int precision)`

**Contrato:**
- **Entrada:** `n` — tamaño población, `L` — longitud cromosoma, `es_real` — modo real/entero, `precision` — decimales.
- **Proceso:** Genera `n` cromosomas aleatorios de `L` bits cada uno usando `std::mt19937`. Calcula fenotipo y fitness para cada individuo.
- **Salida:** `Poblacion` inicializada.
- **Nota:** Para modo real, la precisión determina el mapeo: $x = x_{\min} + \text{entero}(\text{cromosoma}) \cdot \frac{x_{\max} - x_{\min}}{2^L - 1}$.

#### `double decodificarCromosoma(const std::vector<int>& cromosoma, bool es_real, int precision)`

**Contrato:**
- **Entrada:** cromosoma de bits, modo, precisión.
- **Proceso:** Convierte binario a entero (valor crudo), luego mapea al dominio.
- **Salida:** Valor fenotípico $x \in [0, 63]$.
- **Fórmula entero:** $x = \sum_{i=0}^{L-1} \text{bit}_i \cdot 2^{L-1-i}$
- **Fórmula real:** $x = 0 + \text{entero} \cdot \frac{63}{2^L - 1}$, luego redondeo a `precision` decimales.

#### `double evaluarFitness(double x)`

**Contrato:**
- **Entrada:** valor fenotípico $x$.
- **Salida:** $f(x) = x^2$.

---

## 2. Módulo `operadores` — Ruleta, cruce, mutación

### 2.1 Ficheros

| Fichero | Tipo | Líneas estimadas |
|---------|------|-----------------|
| `src/operadores.hpp` | Header | ~30 |
| `src/operadores.cpp` | Implementación | ~100 |

### 2.2 Dependencias

- `<vector>`, `<random>`, `<algorithm>`, `<numeric>`

### 2.3 API

#### `std::vector<Individuo> seleccionRuleta(const Poblacion& poblacion, std::mt19937& rng)`

**Contrato:**
- **Entrada:** población actual, generador aleatorio.
- **Proceso:**
  1. Calcular fitness total $F_{total} = \sum f_i$.
  2. Calcular probabilidades acumuladas $q_i = \sum_{j=1}^{i} f_j / F_{total}$.
  3. Para cada slot de la nueva población:
     - Generar $r \sim U[0,1]$.
     - Encontrar $i$ tal que $q_{i-1} < r \leq q_i$ (búsqueda lineal o `std::upper_bound`).
     - Copiar el individuo $i$ al pool de padres.
- **Salida:** Vector de `n` individuos seleccionados (con posibles repeticiones).


#### `std::vector<Individuo> cruceUnPunto(const std::vector<Individuo>& padres, double Pc, std::mt19937& rng)`

**Contrato:**
- **Entrada:** pool de padres (tamaño `n`), probabilidad de cruce `Pc`, RNG.
- **Proceso:**
  1. Emparejar consecutivamente: $(0,1), (2,3), \dots, (n-2, n-1)$. Se asume `n` par.
  2. Para cada par:
     - Generar $r \sim U[0,1]$.
     - Si $r < P_c$: elegir punto de corte $k \sim U\{1, 2, \dots, L-1\}$.
       - Hijo 1: bits $[0, k)$ del padre 1 + bits $[k, L)$ del padre 2.
       - Hijo 2: bits $[0, k)$ del padre 2 + bits $[k, L)$ del padre 1.
     - Si no: hijos = copias idénticas de los padres.
  3. Recalcular fenotipo y fitness para cada hijo.
- **Salida:** Vector de `n` hijos.

#### `void mutarPoblacion(std::vector<Individuo>& poblacion, double Pm, std::mt19937& rng)`

**Contrato:**
- **Entrada:** población (por referencia), probabilidad de mutación por bit `Pm`, RNG.
- **Proceso:** Para cada bit de cada cromosoma:
  - Generar $r \sim U[0,1]$.
  - Si $r < P_m$: `bit = 1 - bit` (flip).
- **Postcondición:** Después de mutar, recalcular fenotipo y fitness de cada individuo.

---

## 3. Módulo `exportador` — Métricas por generación

### 3.1 Ficheros

| Fichero | Tipo | Líneas estimadas |
|---------|------|-----------------|
| `src/exportador.hpp` | Header | ~15 |
| `src/exportador.cpp` | Implementación | ~30 |

### 3.2 API

#### `void exportarMetricas(const std::string& ruta, const std::vector<MetricasGeneracion>& metricas)`

**Contrato:**
- **Entrada:** ruta del archivo CSV, vector de métricas por generación.
- **Salida:** Archivo CSV con columnas: `generacion,mejor_fitness,promedio_fitness,mejor_x`.
- **Formato:** 6 decimales de precisión, encabezado incluido.

---

## 4. Módulo `main` — Orquestador experimental

### 4.1 Fichero

| Fichero | Tipo | Líneas estimadas |
|---------|------|-----------------|
| `src/main.cpp` | Punto de entrada | ~120 |

### 4.2 Experimentos a ejecutar

| ID | Experimento | n | L | Pc | Pm | G_max | Modo |
|----|-------------|---|---|----|----|-------|------|
| 1 | Base (entero) | 20 | 6 | 0.8 | 0.002 | 100 | Entero |
| 2 | Pc bajo | 20 | 6 | 0.6 | 0.002 | 100 | Entero |
| 3 | Pc muy bajo | 20 | 6 | 0.4 | 0.002 | 100 | Entero |
| 4 | Pm medio | 20 | 6 | 0.8 | 0.01 | 100 | Entero |
| 5 | Pm alto | 20 | 6 | 0.8 | 0.1 | 100 | Entero |
| 6 | Real precisión 2 decimales | 50 | 13 | 0.8 | 0.002 | 100 | Real |

### 4.3 Nombres de archivos de salida (coincidentes con ARCH_DESIGN.md)

```cpp
// Pseudocódigo del main — nombres de archivos CSV
exportarMetricas("resultados/experimento_base.csv", metricas_base);
exportarMetricas("resultados/experimento_pc06.csv", metricas_pc06);
exportarMetricas("resultados/experimento_pc04.csv", metricas_pc04);
exportarMetricas("resultados/experimento_pm001.csv", metricas_pm01);   // Pm = 0.01
exportarMetricas("resultados/experimento_pm01.csv", metricas_pm10);    // Pm = 0.1
exportarMetricas("resultados/experimento_real.csv", metricas_real);
```

### 4.4 Flujo de ejecución

```cpp

int main() {
    std::vector<Experimento> experimentos = {
        {"experimento_base", 20, 6, 0.8, 0.002, 100, false, 0},
        {"experimento_pc06", 20, 6, 0.6, 0.002, 100, false, 0},
        {"experimento_pc04", 20, 6, 0.4, 0.002, 100, false, 0},
        {"experimento_pm001", 20, 6, 0.8, 0.01, 100, false, 0},   // Pm = 0.01
        {"experimento_pm01", 20, 6, 0.8, 0.1, 100, false, 0},      // Pm = 0.1
        {"experimento_real", 50, 13, 0.8, 0.002, 100, true, 2}
    };

    for (auto& exp : experimentos) {
        auto metricas = ejecutarAGS(exp);
        exportarMetricas("resultados/" + exp.nombre + ".csv", metricas);
    }
}
```

### 4.5 Función `ejecutarAGS`

```cpp
std::vector<MetricasGeneracion> ejecutarAGS(const Experimento& exp) {
    std::mt19937 rng(std::random_device{}());
    auto poblacion = inicializarPoblacion(exp.n, exp.L, exp.es_real, exp.precision);
    std::vector<MetricasGeneracion> metricas;

    for (int g = 0; g < exp.G_max; ++g) {
        auto padres = seleccionRuleta(poblacion, rng);
        auto hijos = cruceUnPunto(padres, exp.Pc, rng);
        mutarPoblacion(hijos, exp.Pm, rng);
        poblacion.individuos = hijos;  // Sustitución total

        // Recalcular fitness post-mutación
        for (auto& ind : poblacion.individuos) {
            ind.x_fenotipo = decodificarCromosoma(ind.cromosoma, exp.es_real, exp.precision);
            ind.fitness = evaluarFitness(ind.x_fenotipo);
        }

        // Registrar métricas
        double mejor = -1, suma = 0, mejor_x = 0;
        for (const auto& ind : poblacion.individuos) {
            suma += ind.fitness;
            if (ind.fitness > mejor) {
                mejor = ind.fitness;
                mejor_x = ind.x_fenotipo;
            }
        }
        metricas.push_back({g, mejor, suma / exp.n, mejor_x});
    }
    return metricas;
}
```

---

## 5. Criterios de Aceptación (verificación técnica)

| Criterio | Test | Umbral | Estado |
|----------|------|--------|--------|
| Decodificación entera | $x$ para cromosoma `[1,1,1,1,1,1]` | $x = 63$ | ⬜ |
| Decodificación real (13 bits) | Rango de $x$ | $[0, 63]$ con error $< 0.01$ | ⬜ |
| Fitness máximo teórico | Mejor fitness alcanzable | $3969$ ($63^2$) | ⬜ |
| Ruleta | Suma de probabilidades | $1.0$ (tolerancia $10^{-9}$) | ⬜ |
| Cruce | Con $P_c = 1.0$, variabilidad entre padres e hijos | $> 0$ | ⬜ |
| Mutación | Con $P_m = 1.0$, todos los bits flipados | 100\% cambio | ⬜ |
| Convergencia base | Mejor fitness en gen 100 | $> 3500$ (típico) | ⬜ |
| CSV exportado | Formato correcto | 100 filas + encabezado | ⬜ |

---

## 6. Referencias

1. **Goldberg, D. E.** (1989). *Genetic Algorithms in Search, Optimization, and Machine Learning*. Addison-Wesley.
2. **Michalewicz, Z.** (1996). *Genetic Algorithms + Data Structures = Evolution Programs*. Springer.
3. **Eigen Documentation.** *Main Page.* https://libeigen.gitlab.io/ — Documentación oficial de la biblioteca Eigen para álgebra lineal en C++.
