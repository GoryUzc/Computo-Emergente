# SPEC-001: Implementación C++ del Núcleo del Algoritmo Genético Bohachevsky

---

## 1. Alcance

Este documento especifica la implementación en C++17 del motor de cómputo del Algoritmo Genético para minimizar la función de Bohachevsky. Incluye estructuras de datos, operadores genéticos, renormalización y exportación a CSV.

**Dependencia:** ARCH_DESIGN.md 

---

## 2. Estructuras de Datos

### 2.1 Individuo

```cpp
struct Individuo {
    std::bitset<32> cromosoma;  // [0..15] = x, [16..31] = y
    double fitness;              // f(x,y) evaluado
};
```

**Invariantes:**
- `cromosoma` siempre contiene 32 bits válidos
- `fitness` se calcula inmediatamente después de cualquier modificación del cromosoma

### 2.2 Población

```cpp
using Poblacion = std::vector<Individuo>;
```

**Tamaño fijo:** 100 individuos (constante `TAM_POBLACION = 100`).

### 2.3 Métricas por Generación

```cpp
struct MetricasGen {
    int simulacion;
    int generacion;
    std::string ag_type;
    double pc;
    double pm;
    double best_fitness;
    double avg_fitness;
    double std_fitness;
    double best_x;
    double best_y;
};
```

---

## 3. Módulos y Funciones

### 3.1 utils.hpp / utils.cpp

| Función | Firma | Descripción |
|---------|-------|-------------|
| `decodificar_variable` | `double decodificar_variable(const std::bitset<32>& crom, int inicio, int fin)` | Extrae bits [inicio, fin] y mapea a [-1, 1] |
| `decodificar_x` | `double decodificar_x(const std::bitset<32>& crom)` | Wrapper: decodifica x (bits 0-15) |
| `decodificar_y` | `double decodificar_y(const std::bitset<32>& crom)` | Wrapper: decodifica y (bits 16-31) |
| `bohachevsky` | `double bohachevsky(double x, double y)` | Evalúa f(x,y) = x² + 2y² - 0.3cos(3πx) - 0.4cos(4πy) + 0.7 |
| `crear_generador` | `std::mt19937 crear_generador(int seed)` | Factory de generadores con semilla |

**Algoritmo decodificar_variable:**
```
1. Extraer sub-bits del cromosoma desde 'inicio' hasta 'fin'
2. Convertir a entero sin signo (valor_entero)
3. n_bits = fin - inicio + 1
4. max_valor = 2^n_bits - 1
5. return -1.0 + valor_entero * (2.0 / max_valor)
```

### 3.2 ag_core.hpp / ag_core.cpp

| Función | Firma | Descripción |
|---------|-------|-------------|
| `inicializar_poblacion` | `Poblacion inicializar_poblacion(std::mt19937& gen)` | Genera 100 individuos aleatorios, evalúa fitness |
| `evaluar_poblacion` | `void evaluar_poblacion(Poblacion& pop)` | Recalcula fitness de toda la población |
| `obtener_metricas` | `MetricasGen obtener_metricas(const Poblacion& pop, int sim, int gen, const std::string& type, double pc, double pm)` | Calcula métricas estadísticas de la población |
| `encontrar_mejor` | `int encontrar_mejor(const Poblacion& pop)` | Índice del individuo con menor fitness |
| `encontrar_peor` | `int encontrar_peor(const Poblacion& pop)` | Índice del individuo con mayor fitness |

**Algoritmo inicializar_poblacion:**
```
1. Crear vector<Individuo> de tamaño TAM_POBLACION
2. Para cada individuo i:
   a. Para cada bit j de 0 a 31:
      - Generar aleatorio uniforme {0, 1}
      - Asignar a cromosoma[j]
   b. Calcular x = decodificar_x(cromosoma)
   c. Calcular y = decodificar_y(cromosoma)
   d. fitness = bohachevsky(x, y)
3. Retornar población
```

### 3.3 operadores.hpp / operadores.cpp

| Función | Firma | Descripción |
|---------|-------|-------------|
| `seleccion_ranking` | `Poblacion seleccionar_padres(const Poblacion& pop, std::mt19937& gen)` | Selección proporcional al rango |
| `cruce_uniforme` | `std::pair<Individuo, Individuo> cruce_uniforme(const Individuo& p1, const Individuo& p2, double pc, std::mt19937& gen)` | Cruce uniforme con probabilidad pc |
| `mutacion` | `void mutacion(Individuo& ind, double pm, std::mt19937& gen)` | Flip bit con probabilidad pm |
| `aplicar_elitismo` | `void aplicar_elitismo(Poblacion& nueva, const Individuo& mejor)` | Reemplaza el peor de la nueva población por el mejor anterior |
| `evolucionar_generacion` | `Poblacion evolucionar_generacion(const Poblacion& actual, double pc, double pm, std::mt19937& gen)` | Pipeline completo: selección → cruce → mutación → elitismo |

**Algoritmo seleccion_ranking:**
```
1. Crear copia de población
2. Ordenar copia por fitness ascendente (menor = mejor)
3. Calcular probabilidades: p[i] = 2*rango[i] / (n*(n+1))
   donde rango[i] = i+1 (1=peor, n=mejor)
4. Calcular probabilidades acumuladas q[i] = sum(p[0..i])
5. Para k = 0..n-1:
   a. Generar r ~ U[0,1]
   b. Buscar índice i tal que q[i-1] < r ≤ q[i]
   c. Agregar pop[índice_original_del_ordenado[i]] a padres
6. Retornar padres
```

**Algoritmo cruce_uniforme:**
```
1. Generar r ~ U[0,1]
2. Si r ≥ pc: retornar copias idénticas de p1 y p2
3. Si r < pc:
   a. Crear hijo1, hijo2
   b. Para cada bit j de 0 a 31:
      - Generar r_bit ~ U[0,1]
      - Si r_bit < 0.5: hijo1[j] = p1[j], hijo2[j] = p2[j]
      - Si no: hijo1[j] = p2[j], hijo2[j] = p1[j]
   c. Evaluar fitness de ambos hijos
   d. Retornar par (hijo1, hijo2)
```

**Algoritmo mutacion:**
```
1. Para cada bit j de 0 a 31:
   a. Generar r ~ U[0,1]
   b. Si r < pm:
      - ind.cromosoma[j] = !ind.cromosoma[j]
2. Recalcular fitness: x, y = decodificar, fitness = bohachevsky(x,y)
```

**Algoritmo aplicar_elitismo:**
```
1. Encontrar índice del peor individuo en 'nueva'
2. nueva[peor] = mejor (copia completa)
```

**Algoritmo evolucionar_generacion:**
```
1. Guardar índice del mejor individuo de 'actual'
2. padres = seleccion_ranking(actual, gen)
3. nueva = vector vacío
4. Para i = 0, 2, 4, ... hasta n-2:
   a. (h1, h2) = cruce_uniforme(padres[i], padres[i+1], pc, gen)
   b. mutacion(h1, pm, gen)
   c. mutacion(h2, pm, gen)
   d. Agregar h1 y h2 a 'nueva'
5. aplicar_elitismo(nueva, actual[mejor_idx])
6. Retornar nueva
```

### 3.4 renormalizacion.hpp / renormalizacion.cpp

| Función | Firma | Descripción |
|---------|-------|-------------|
| `renormalizar_lineal` | `std::vector<double> renormalizar_lineal(const std::vector<double>& fitness)` | f' = a·f + b, mapea [f_min, f_max] → [f_max, f_min] |
| `renormalizar_ventana` | `std::vector<double> renormalizar_ventana(const std::vector<double>& fitness)` | f' = f_max - f_i + c |

**Algoritmo renormalizar_lineal:**
```
1. Encontrar f_min y f_max del vector fitness
2. Si f_max == f_min: retornar vector de 1.0 (evitar división por cero)
3. a = (f_max - f_min) / (f_min - f_max) = -1.0
   b = f_max - a * f_min = f_max + f_min
4. Para cada f_i: f'_i = a * f_i + b
   Verificar que f'_i > 0 para todo i
5. Retornar vector de f'
```

**Nota:** Para minimización, el mapeo es:
- f_min (mejor) → f_max (mayor valor renormalizado)
- f_max (peor) → f_min (menor valor renormalizado)

**Algoritmo renormalizar_ventana:**
```
1. Encontrar f_max del vector fitness
2. c = 0.01 (constante de seguridad)
3. Para cada f_i: f'_i = f_max - f_i + c
4. Retornar vector de f'
```

### 3.5 exportador.hpp / exportador.cpp

| Función | Firma | Descripción |
|---------|-------|-------------|
| `exportar_csv` | `void exportar_csv(const std::vector<MetricasGen>& metricas, const std::string& filename)` | Escribe todas las métricas a CSV |
| `acumular_metricas` | `void acumular_metricas(std::vector<MetricasGen>& acumulador, const MetricasGen& m)` | Agrega métrica al vector acumulador |

**Formato CSV:**
```csv
simulation,generation,ag_type,pc,pm,best_fitness,avg_fitness,std_fitness,best_x,best_y
0,0,simple,0.7,0.005,2.4567,3.8912,0.4321,0.1234,-0.5678
```

---

## 4. Flujo Principal (main.cpp)

```cpp
int main(int argc, char* argv[]) {
    // 1. Parsear argumentos: --type, --pc, --pm, --sims, --seed, --grid
    // 2. Si --grid: iterar sobre PC × PM
    // 3. Para cada configuración:
    //    a. Para s = 0..n_sims-1:
    //       - gen = crear_generador(seed + s)
    //       - pop = inicializar_poblacion(gen)
    //       - metricas.push_back(obtener_metricas(pop, s, 0, type, pc, pm))
    //       - Para g = 1..G_max:
    //         * pop = evolucionar_generacion(pop, pc, pm, gen)
    //         * metricas.push_back(obtener_metricas(pop, s, g, type, pc, pm))
    // 4. exportar_csv(metricas, "resultados/results.csv")
    // 5. Retornar 0
}
```
> Agregar std::count para visualizar por consola la ejecucion del algorimo y cualquier error. 

---

## 5. Constantes y Configuración

```cpp
constexpr int TAM_POBLACION = 100;
constexpr int MAX_GENERACIONES = 100;
constexpr int NUM_SIMULACIONES = 30;
constexpr int BITS_X = 16;
constexpr int BITS_Y = 16;
constexpr int BITS_TOTAL = 32;
constexpr double PC_DEFAULT = 0.7;
constexpr double PM_DEFAULT = 0.005;
constexpr int SEED_DEFAULT = 42;
constexpr double C_VENTANA = 0.01;
```

---

## 6. Criterios de Aceptación

| Criterio | Verificación | Estado |
|----------|-------------|--------|
| Compilación sin warnings | `g++ -std=c++17 -Wall -Wextra -O2` | [ ] |
| Decodificación correcta | x=-1 → bits=0, x=1 → bits=65535 | [ ] |
| Fitness Bohachevsky | f(0,0) = 0.7 - 0.3 - 0.4 + 0.7 = 1.0? No, verificar: 0 + 0 - 0.3 - 0.4 + 0.7 = 0.0 ✓ | [ ] |
| Ranking suma 1 | Σ p_i = 1.0 | [ ] |
| Cruce con pc=0 | Sin cambios en padres | [ ] |
| Mutación con pm=0 | Sin cambios en bits | [ ] |
| Elitismo preserva mejor | Mejor fitness nunca empeora entre gens | [ ] |
| Renorm. lineal todos positivos | min(f') > 0 | [ ] |
| Renorm. ventana todos positivos | min(f') > 0 | [ ] |
| CSV con 30×101 = 3030 líneas | Contar líneas de salida | [ ] |

---

## 7. Dependencias

- `<bitset>` — Cromosomas
- `<vector>` — Poblaciones
- `<random>` — Generación aleatoria (mt19937, uniform_real_distribution)
- `<algorithm>` — Ordenamiento, búsqueda
- `<cmath>` — cos(), M_PI
- `<fstream>` — Exportación CSV
- `<sstream>` — Formateo de strings
- `<string>` — Tipos de texto
- `<iostream>` — Debug (opcional)

**Sin dependencias externas.**
