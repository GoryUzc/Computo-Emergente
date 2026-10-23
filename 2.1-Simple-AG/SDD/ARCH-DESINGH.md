# ARQUITECTURA Y DISEÑO: Proyecto — Algoritmo Genético Simple (AGS)

---

## 1. Stack

| Capa | Tecnología | Versión | Propósito |
|------|-----------|---------|-----------|
| **Cálculo numérico** | C++17 | ISO/IEC 14882:2017 | Núcleo: AGS, operadores genéticos |
| **Visualización** | Python 3.10+ | 3.10+ | Gráficas de convergencia y comparativas |
| **Gráficos** | matplotlib | 3.7+ | Renderizado de figuras |
| **Build** | CMake / g++ | 3.16+ / 11+ | Compilación |

---

## 2. Objetivo

Implementar un Algoritmo Genético Simple (AGS) para maximizar la función $F(x) = x^2$ en dos variantes:

1. **Versión entera:** $x \in [0, 63]$ (6 bits), población $n=20$, 100 generaciones.
2. **Versión real:** $x \in [0, 63]$ con precisión 2 decimales (13 bits), población $n=50$.

Evaluar el impacto de los parámetros $P_c$ (probabilidad de cruce) y $P_m$ (probabilidad de mutación) en la convergencia.

**Flujo general:**

```
┌──────────────────────────────────────────────────────────────┐
│                    INICIO                                    │
└──────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌──────────────────────────────────────────────────────────────┐
│  INICIALIZAR: Generar n individuos aleatorios               │
│  EVALUAR: Calcular fitness de cada individuo                │
└──────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌──────────────────────────────────────────────────────────────┐
│  REGISTRAR: mejor_fitness, promedio_fitness (gen actual)    │
└──────────────────────────────────────────────────────────────┘
                            │
                            ▼
                    ┌───────────────┐
                    │ gen < 100 ?   │──NO──┐
                    └───────────────┘      │
                         │SÍ              │
                         ▼                ▼
┌─────────────────────────────────┐    ┌──────────────┐
│ CREAR NUEVA POBLACIÓN:          │    │  EXPORTAR    │
│                                 │    │  RESULTADOS  │
│  Para i = 0..n-1:               │    │  (CSV)       │
│    1. Seleccionar padre1        │    └──────────────┘
│    2. Seleccionar padre2        │
│    3. Cruzar con prob Pc        │
│    4. Mutar con prob Pm         │
│    5. Agregar hijo              │
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
```

---

## 3. Conceptos Fundamentales

### 3.1 Representación Binaria

| Versión | Dominio | Precisión | Valores discretos | Bits necesarios ($L$) |
|---------|---------|-----------|-------------------|----------------------|
| Entera | $[0, 63]$ | 1 | 64 | $\lceil \log_2(64) \rceil = 6$ |
| Real | $[0, 63]$ | 0.01 | 6301 | $\lceil \log_2(6301) \rceil = 13$ |

**Mapeo fenotipo:**
- Entera: $x = \text{valor\_entero}(\text{cromosoma})$
- Real: $x = x_{\min} + \text{valor\_entero}(\text{cromosoma}) \cdot \frac{x_{\max} - x_{\min}}{2^L - 1}$

### 3.2 Operadores Genéticos

| Operador | Descripción | Parámetro |
|----------|-------------|-----------|
| **Selección** | Rueda de ruleta proporcional al fitness | — |
| **Cruce** | Cruce de un punto (single-point crossover) | $P_c \approx 0.8$ |
| **Mutación** | Flip bit con probabilidad por bit | $P_m \approx 0.002$ |
| **Sustitución** | Total: la población anterior es reemplazada completamente | — |

### 3.3 Rueda de Ruleta

La probabilidad de selección del individuo $i$ es:

$$p_i = \frac{f_i}{\sum_{j=1}^{n} f_j}$$

Donde $f_i = x_i^2$ es el fitness. Se acumulan las probabilidades en un vector de probabilidades acumuladas y se genera un número aleatorio $r \in [0,1]$ para seleccionar el individuo cuya probabilidad acumulada cubre $r$.

### 3.4 Cruce de Un Punto

Dados dos padres, se selecciona un punto de corte aleatorio $k \in [1, L-1]$:
- Hijo 1: bits $[0, k)$ del padre 1 + bits $[k, L)$ del padre 2
- Hijo 2: bits $[0, k)$ del padre 2 + bits $[k, L)$ del padre 1

Se aplica con probabilidad $P_c$. Si no se aplica, los hijos son copias idénticas de los padres.

### 3.5 Mutación

Para cada bit de cada cromosoma de la descendencia, se invierte (flip) con probabilidad $P_m$:

$$\text{bit}_j \leftarrow 1 - \text{bit}_j \quad \text{con probabilidad } P_m$$

---

## 4. Implementación Final

```
ENTRADA: n (tamaño población), L (longitud cromosoma), Pc, Pm, G_max (generaciones)
SALIDA:  Mejor fitness por generación, fitness promedio por generación, mejor individuo final

1. INICIALIZAR
   a. Para i = 1..n: generar cromosoma aleatorio de L bits
   b. Evaluar fitness f_i = x_i^2 para cada individuo

2. PARA cada generación g = 1..G_max:
   a. SELECCIÓN (Ruleta)
      i.   Calcular fitness total F_total = Σ f_i
      ii.  Calcular probabilidades acumuladas q_i = Σ_{j=1}^{i} f_j / F_total
      iii. Para k = 1..n:
           - Generar r ~ U[0,1]
           - Seleccionar individuo i tal que q_{i-1} < r ≤ q_i
           - Agregar a pool de padres

   b. CRUCE (Un punto, probabilidad Pc)
      i.   Emparejar padres consecutivos: (1,2), (3,4), ..., (n-1,n)
      ii.  Para cada par:
           - Generar r ~ U[0,1]
           - Si r < Pc:
                * Elegir punto de corte k ~ U{1, 2, ..., L-1}
                * Generar 2 hijos por intercambio de segmentos
           - Si no: hijos = copias de los padres

   c. MUTACIÓN (Probabilidad Pm por bit)
      i.   Para cada bit de cada cromosoma descendiente:
           - Generar r ~ U[0,1]
           - Si r < Pm: flip del bit

   d. SUSTITUCIÓN TOTAL
      i.   Población actual ← Población descendiente
      ii.  Evaluar fitness de la nueva población
      iii. Registrar: mejor fitness, fitness promedio

3. EXPORTAR
   a. Guardar CSV: generación, mejor_fitness, fitness_promedio
   b. Invocar script Python para generar gráficas
```

---

## 5. Estructura de Carpetas

```
2.1-SIMPLE-AG/
├── CMakeLists.txt
├── README.md
├── docs/
│   └── Analisis_resultados.tex
├── SDD/
│   ├── SPECS/
│   │   ├── spec-001.md    # Implementación C++ del AGS
│   │   └── spec-002.md    # Script Python de visualización
│   ├── PLANS/
│   │   └── plan_ag.md     # Plan de desarrollo
│   └── ARCH_DESIGN.md     # Arquitectura, diseño y teoría
├── src/
│   ├── main.cpp           # Orquestador y parámetros experimentales
│   ├── ag_core.hpp/cpp    # Población, fitness, evolución
│   ├── operadores.hpp/cpp # Ruleta, cruce, mutación, sustitución
│   ├── utils.hpp/cpp      # Conversión binario-entero, binario-real
│   └── exportador.hpp/cpp # CSV de métricas por generación
├── resultados/
│   ├── experimento_base.csv
│   ├── experimento_pc06.csv
│   ├── experimento_pc04.csv
│   ├── experimento_pm001.csv
│   ├── experimento_pm01.csv
│   ├── experimento_real.csv
│   └── graficos/
│       ├── convergencia_base.png
│       ├── comparativa_pc.png
│       ├── comparativa_pm.png
│       └── convergencia_real.png
└── python/
    ├── visualizar_ag.py
    └── requirements.txt
```

---

## 6. Métricas y Criterios de Aceptación

| Criterio | Cómo se verifica | Estado |
|----------|-----------------|--------|
| **Representación correcta** | 6 bits para entero, 13 bits para real | ⬜ |
| **Fitness correcto** | $f_i = x_i^2$ con $x_i$ decodificado correctamente | ⬜ |
| **Ruleta proporcional** | Suma de probabilidades = 1, selección estocástica | ⬜ |
| **Cruce un punto** | Punto de corte en [1, L-1], 2 hijos por par | ⬜ |
| **Mutación por bit** | Tasa observada ≈ $P_m$ en muestra grande | ⬜ |
| **Sustitución total** | Nueva población reemplaza completamente la anterior | ⬜ |
| **Convergencia** | Mejor fitness tiende a $63^2 = 3969$ en ≤100 gen | ⬜ |
| **Gráficos** | 6 figuras generadas correctamente | ⬜ |
| **Variación Pc** | Curvas comparables para Pc=0.8, 0.6, 0.4 | ⬜ |
| **Variación Pm** | Curvas comparables para Pm=0.002, 0.01, 0.1 | ⬜ |
| **Real precisión** | x decodificado con error < 0.01 respecto a teórico | ⬜ |

---

## 7. Procedimiento de Compilación y Ejecución

### 7.1 Compilación (Linux/macOS)

```bash
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
./ags_maximizador
```

### 7.2 Ejecución del script Python

```bash
pip install -r python/requirements.txt
python python/visualizar_ag.py
```

---

## 8. Referencias

1. **Goldberg, D. E.** (1989). *Genetic Algorithms in Search, Optimization, and Machine Learning*. Addison-Wesley.
2. **Michalewicz, Z.** (1996). *Genetic Algorithms + Data Structures = Evolution Programs*. Springer.
3. **Trefethen, L. N., & Bau III, D.** (1997). *Numerical Linear Algebra*. SIAM. (Para generadores aleatorios y análisis de convergencia).
4. **Eigen library** https://libeigen.gitlab.io/
