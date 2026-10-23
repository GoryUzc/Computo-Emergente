# ARQUITECTURA Y DISEÑO: Proyecto — Algoritmo Genético Bohachevsky (AGB)

---

## 1. Stack

| Capa | Tecnología | Versión | Propósito |
|------|-----------|---------|-----------|
| **Cálculo numérico** | C++17 | ISO/IEC 14882:2017 | Núcleo: AGS, operadores genéticos, renormalización |
| **Visualización** | Python 3.10+ | 3.10+ | Gráficas de convergencia, comparativas y heatmaps |
| **Gráficos** | matplotlib | 3.7+ | Renderizado de figuras |
| **Análisis** | pandas, numpy | 1.5+, 1.24+ | Procesamiento de resultados y estadísticas |
| **Build** | g++ / make | 11+ | Compilación |

---

## 2. Objetivo

Implementar un Algoritmo Genético para **minimizar** la función de Bohachevsky:

$$f(x,y) = x^2 + 2y^2 - 0.3\cos(3\pi x) - 0.4\cos(4\pi y) + 0.7$$

donde $x, y \in [-1, 1]$. Esta función tiene un mínimo global en $f(0,0) = 0$ y múltiples mínimos locales.

**Codificación:** 16 bits por variable (x, y) → individuos de 32 bits.

**Población:** $n = 100$, **Generaciones máximas:** $G_{max} = 100$, **Simulaciones:** 30 con inicializaciones distintas.

**Variantes a evaluar:**

1. **AG Simple:** Selección por Ranking, Cruce Uniforme, Mutación bit-flip.
2. **AG Modificado:** AG Simple + Renormalización Lineal.
3. **AG Modificado:** AG Simple + Renormalización por Ventana.

**Análisis paramétrico (parte c):**
- $P_c \in \{0.6, 0.7, 0.8, 0.9\}$
- $P_m \in \{0.001, 0.005, 0.01\}$

**Flujo general:**

```
┌──────────────────────────────────────────────────────────────┐
│                    INICIO                                    │
└──────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌──────────────────────────────────────────────────────────────┐
│  CONFIGURAR: ag_type, pc, pm, base_seed, n_sims=30          │
└──────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌──────────────────────────────────────────────────────────────┐
│  PARA cada simulación s = 0..29:                            │
│    seed = base_seed + s                                     │
│    INICIALIZAR: Generar 100 individuos aleatorios (32 bits) │
│    EVALUAR: Calcular f(x,y) para cada individuo             │
└──────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌──────────────────────────────────────────────────────────────┐
│  REGISTRAR: mejor_fitness, avg_fitness, std_fitness        │
│             best_x, best_y (gen actual)                     │
└──────────────────────────────────────────────────────────────┘
                            │
                            ▼
                    ┌───────────────┐
                    │ gen < 100 ?   │──NO──┐
                    └───────────────┘      │
                         │SÍ              │
                         ▼                ▼
┌─────────────────────────────────┐    ┌──────────────┐
│ CREAR NUEVA POBLACIÓN:          │    │  SIGUIENTE   │
│                                 │    │  SIMULACIÓN  │
│  1. Aplicar elitismo (1 mejor)  │    │  (o EXPORTAR │
│  2. Para i = 0..n-1:            │    │   SI ÚLTIMA) │
│     a. Seleccionar padre1       │    └──────────────┘
│     b. Seleccionar padre2       │
│     c. Cruzar uniforme con Pc   │
│     d. Mutar con Pm             │
│     e. Agregar hijo             │
└─────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────┐
│ SUSTITUCIÓN:                    │
│   población = nueva_población   │
│   evaluar()                     │
│   gen++                         │
└─────────────────────────────────┘
                 │
                 └──────────────► (volver al loop)
                            │
                            ▼
┌──────────────────────────────────────────────────────────────┐
│  EXPORTAR: results.csv con métricas de todas las sims       │
│  INVOCAR: script Python para generar gráficas               │
└──────────────────────────────────────────────────────────────┘
```

---

## 3. Conceptos Fundamentales

### 3.1 Representación Binaria

| Variable | Dominio | Bits | Valores discretos | Resolución |
|----------|---------|------|-------------------|------------|
| x | $[-1, 1]$ | 16 | 65,536 | $\Delta x = \frac{2}{2^{16}-1} \approx 3.05 \times 10^{-5}$ |
| y | $[-1, 1]$ | 16 | 65,536 | $\Delta y = \frac{2}{2^{16}-1} \approx 3.05 \times 10^{-5}$ |

**Cromosoma total:** 32 bits `[x(16 bits) | y(16 bits)]`

**Mapeo fenotipo (decodificación):**

$$v = v_{min} + \text{valor\_entero}(\text{cromosoma}) \cdot \frac{v_{max} - v_{min}}{2^{16} - 1}$$

Donde $v \in \{x, y\}$, $v_{min} = -1$, $v_{max} = 1$.

### 3.2 Función de Bohachevsky

$$f(x,y) = x^2 + 2y^2 - 0.3\cos(3\pi x) - 0.4\cos(4\pi y) + 0.7$$

**Propiedades:**
- Mínimo global: $f(0,0) = 0$
- Múltiples mínimos locales (ondulatoria por términos coseno)
- Dominio acotado: $[-1, 1] \times [-1, 1]$

### 3.3 Operadores Genéticos

| Operador | Descripción | Parámetro |
|----------|-------------|-----------|
| **Selección** | Ranking (ordenamiento por fitness, probabilidad proporcional al rango) | — |
| **Cruce** | Cruce Uniforme (cada bit se hereda de padre1 o padre2 con 50%) | $P_c \approx 0.7$ |
| **Mutación** | Flip bit con probabilidad por bit | $P_m \approx 0.005$ |
| **Elitismo** | Preservar 1 mejor individuo sin modificar | — |
| **Sustitución** | Total: población anterior reemplazada por descendencia | — |

### 3.4 Selección por Ranking

1. Ordenar población por fitness (ascendente, porque minimizamos)
2. Asignar rangos: peor = 1, mejor = n
3. Probabilidad de selección proporcional al rango:

$$p_i = \frac{r_i}{\sum_{j=1}^{n} r_j} = \frac{2 \cdot r_i}{n(n+1)}$$

4. Construir vector de probabilidades acumuladas
5. Generar $r \sim U[0,1]$ y seleccionar individuo correspondiente

### 3.5 Cruce Uniforme

Dados dos padres, para cada bit del cromosoma:
- Con probabilidad 0.5: hijo toma bit del padre 1
- Con probabilidad 0.5: hijo toma bit del padre 2

Se aplica con probabilidad $P_c$ por pareja. Si no se aplica, los hijos son copias idénticas de los padres.

### 3.6 Mutación

Para cada bit de cada cromosoma descendiente, se invierte (flip) con probabilidad $P_m$:

$$\text{bit}_j \leftarrow 1 - \text{bit}_j \quad \text{con probabilidad } P_m$$

### 3.7 Renormalización Lineal (AG Modificado)

Transformación lineal del fitness para controlar la presión selectiva:

$$f'_i = a \cdot f_i + b$$

Donde $a < 0$ (porque minimizamos, convertimos a maximización), y $a, b$ se calculan para mapear:
- $f_{max}$ (peor fitness) → rango mínimo
- $f_{min}$ (mejor fitness) → rango máximo

Asegurando que $f'_i > 0$ para todos los individuos.

### 3.8 Renormalización por Ventana (AG Modificado)

$$f'_i = f_{max} - f_i + c$$

Donde $c > 0$ es una constante de seguridad (ej. $c = 0.01$) que evita que el peor individuo tenga probabilidad de selección cero.

---

## 4. Implementación Final

```
ENTRADA: ag_type, n=100, L=32, Pc, Pm, G_max=100, n_sims=30, base_seed
SALIDA:  results.csv con métricas por generación y simulación

1. PARA cada simulación s = 0..n_sims-1:
   a. seed = base_seed + s
   b. INICIALIZAR
      i.   Para i = 1..n: generar cromosoma aleatorio de L bits
      ii.  Evaluar fitness f_i = f(x_i, y_i) para cada individuo

   c. PARA cada generación g = 1..G_max:
      i.   REGISTRAR métricas: best_fitness, avg_fitness, std_fitness, best_x, best_y

      ii.  SI ag_type != "simple":
           - Aplicar renormalización (lineal o ventana) a fitness

      iii. SELECCIÓN (Ranking)
           - Ordenar población por fitness
           - Calcular probabilidades acumuladas por rango
           - Generar pool de padres (n individuos)

      iv.  CRUCE (Uniforme, probabilidad Pc)
           - Emparejar padres consecutivos
           - Para cada par: aplicar cruce uniforme con prob Pc

      v.   MUTACIÓN (Probabilidad Pm por bit)
           - Para cada bit de cada cromosoma: flip con prob Pm

      vi.  ELITISMO
           - Identificar mejor individuo de población anterior
           - Reemplazar peor individuo de nueva población por el mejor

      vii. SUSTITUCIÓN TOTAL
           - Población actual ← nueva población
           - Evaluar fitness
           - gen++

2. EXPORTAR
   a. Guardar CSV: simulation, generation, ag_type, pc, pm, 
                  best_fitness, avg_fitness, std_fitness, best_x, best_y
   b. Invocar script Python para generar gráficas
```

---

## 5. Estructura de Carpetas

```
Tarea02-AG-Bohachevsky/
├── SDD/
│   ├── ARCH_DESIGN.md          # Arquitectura, diseño y teoría
│   ├── SPECS/
│   │   ├── spec-001.md         # Implementación C++ del núcleo AG
│   │   └── spec-002.md         # Script Python de visualización
│   └── PLANS/
│       └── plan_ag.md          # Plan de desarrollo paso a paso
├── src/
│   ├── main.cpp                # Orquestador: parámetros, 30 sims, exportación
│   ├── ag_core.hpp / .cpp      # Población, individuo, evaluación Bohachevsky
│   ├── operadores.hpp / .cpp   # Ranking, cruce uniforme, mutación, elitismo
│   ├── renormalizacion.hpp / .cpp  # Lineal y Ventana
│   ├── utils.hpp / .cpp        # Decodificación binario→real, semillas
│   └── exportador.hpp / .cpp   # CSV de métricas por generación
├── resultados/
│   ├── results.csv             # Salida principal de C++
│   ├── graficos/
│   │   ├── convergencia_simple.png
│   │   ├── convergencia_linear.png
│   │   ├── convergencia_window.png
│   │   ├── comparativa_ag.png
│   │   └── heatmap_pc_pm.png
│   └── analisis/
└── python/
    ├── visualizar_ag.py        # Gráficas de convergencia y comparativas
    ├── heatmap_params.py       # Heatmap PC × PM (parte c)
    └── requirements.txt
```

---

## 6. Métricas y Criterios de Aceptación

| Criterio | Cómo se verifica | Estado |
|----------|-----------------|--------|
| **Representación correcta** | 32 bits (16+16), decodificación a [-1,1] | [x] |
| **Fitness correcto** | $f(x,y)$ coincide con fórmula de Bohachevsky | [x] |
| **Mínimo conocido** | Mejor fitness tiende a 0 en ≤100 gen | ⚠️ (g++ no disponible) |
| **Ranking** | Probabilidades proporcionales al rango, suma = 1 | [x] |
| **Cruce uniforme** | Cada bit heredado de padre1 o padre2 con 50% | [x] |
| **Mutación por bit** | Tasa observada ≈ $P_m$ en muestra grande | ⚠️ (g++ no disponible) |
| **Elitismo** | 1 mejor individuo preservado entre generaciones | [x] |
| **Renorm. Lineal** | $f' = a·f + b$ con $a < 0$, todos $f' > 0$ | [x] |
| **Renorm. Ventana** | $f' = f_{max} - f + c$ con $c > 0$ | [x] |
| **30 simulaciones** | Semillas distintas, resultados promediados | [x] |
| **Gráficas parte (a)** | Convergencia AG Simple: mejor y promedio | [x] |
| **Gráficas parte (b)** | Comparativa Simple vs Linear vs Window | [x] |
| **Gráficas parte (c)** | Heatmap PC × PM con fitness final | [x] |
| **CSV completo** | 30 sims × 101 gen × 3 tipos × 4 PC × 3 PM | [x] |

---

## 7. Procedimiento de Compilación y Ejecución

### 7.1 Compilación (Linux/macOS)

```bash
cd src
g++ -std=c++17 -O2 -o ag_bohachevsky \
    main.cpp ag_core.cpp operadores.cpp \
    renormalizacion.cpp utils.cpp exportador.cpp
```

### 7.2 Ejecución del AG (C++)

```bash
# Parte (a) — AG Simple
./ag_bohachevsky --type simple --pc 0.7 --pm 0.005 --sims 30 --seed 42

# Parte (b) — AG Modificado (Linear)
./ag_bohachevsky --type linear --pc 0.7 --pm 0.005 --sims 30 --seed 42

# Parte (b) — AG Modificado (Window)
./ag_bohachevsky --type window --pc 0.7 --pm 0.005 --sims 30 --seed 42

# Parte (c) — Variación de parámetros (se ejecuta internamente)
./ag_bohachevsky --type linear --grid --seed 42
```

### 7.3 Ejecución del script Python

```bash
python python/visualizar_ag.py
python python/heatmap_params.py
```

---

## 8. Referencias

1. **Goldberg, D. E.** (1989). *Genetic Algorithms in Search, Optimization, and Machine Learning*. Addison-Wesley.
2. **Michalewicz, Z.** (1996). *Genetic Algorithms + Data Structures = Evolution Programs*. Springer.
3. **Bohachevsky, I. O., Johnson, M. E., & Stein, M. L.** (1986). Generalized simulated annealing for function optimization. *Technometrics*, 28(3), 209-217.
4. **De Jong, K. A.** (1975). *An Analysis of the Behavior of a Class of Genetic Adaptive Systems*. PhD Thesis, University of Michigan.
