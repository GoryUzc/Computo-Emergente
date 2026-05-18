# Desarrollo Formal de las Medidas Cuantitativas — Autómatas Celulares 1D

> **Proyecto:** 1.2 – Medidas Cuantitativas en Autómatas Celulares  
> **Stack:** C++17 · CMake 3.16+ · Visual Studio 2022 · Windows x64  
> **Sin dependencias externas:** solo `<cmath>` para logaritmos  

---

## Índice

1. [Antecedentes: el modelo de autómata celular 1D](#1-antecedentes-el-modelo-de-autómata-celular-1d)
2. [Notación y convenciones](#2-notación-y-convenciones)
3. [Medida 1 — Densidad (ρ)](#3-medida-1--densidad-ρ)
4. [Medida 2 — Distancia de Hamming (H)](#4-medida-2--distancia-de-hamming-h)
5. [Medida 3 — Parámetro de Langton (λ)](#5-medida-3--parámetro-de-langton-λ)
6. [Medida 4 — Entropía Espacial de Shannon (Hₑₛₚ)](#6-medida-4--entropía-espacial-de-shannon-hₑₛₚ)
7. [Medida 5 — Entropía Temporal de Shannon (Hₜₑₘₚ)](#7-medida-5--entropía-temporal-de-shannon-hₜₑₘₚ)
8. [Medida 6 — Exponente de Lyapunov (λ_L) — Aproximación por Hamming](#8-medida-6--exponente-de-lyapunov-λ_l--aproximación-por-hamming)
9. [Clasificación de Wolfram](#9-clasificación-de-wolfram)
10. [Casos de prueba y valores esperados](#10-casos-de-prueba-y-valores-esperados)
11. [Referencias bibliográficas](#11-referencias-bibliográficas)

---

## 1. Antecedentes: el modelo de autómata celular 1D

Un **autómata celular (AC)** es un modelo de sistema discreto compuesto por celdas distribuidas en una cuadrícula. Cada celda adopta un estado finito en cada instante de tiempo (generación). El estado de todas las celdas en la generación *t + 1* se calcula exclusivamente aplicando una **función de transición determinista** al estado de la generación *t*.

### 1.1 Autómata celular elemental (1D, vecindad radio 1)

Este proyecto se limita al caso más estudiado de la literatura:

| Parámetro | Valor adoptado |
|-----------|---------------|
| Dimensión | 1 (sola fila de celdas) |
| Número de celdas | *N* (ancho del autómata) |
| Estados por celda | Binarios: **{0, 1}** (0 = muerta, 1 = viva) |
| Vecindad | Radio **r = 1** → 3 celdas por celda: `{izquierda, centro, derecha}` |
| Borde | **Circular (toroidal)**: `estado[-1] ≡ estado[N-1]`, `estado[N] ≡ estado[0]` |
| Sincronía | Paralela: todas las celdas se actualizan a la vez |
| Número de reglas | 2³ = **256 reglas de Wolfram** (numeradas 0–255) |

### 1.2 Codificación de la regla de Wolfram

Para cada célula, la vecindad forma un patrón de 3 bits. Hay 2³ = 8 patrones posibles:

| Patrón (izq, centro, der) | Índice binario | Índice decimal |
|---------------------------|----------------|----------------|
| (0, 0, 0) | 000 | 0 |
| (0, 0, 1) | 001 | 1 |
| (0, 1, 0) | 010 | 2 |
| (0, 1, 1) | 011 | 3 |
| (1, 0, 0) | 100 | 4 |
| (1, 0, 1) | 101 | 5 |
| (1, 1, 0) | 110 | 6 |
| (1, 1, 1) | 111 | 7 |

La regla es el número entero de 8 bits donde cada bit `k` (bit menos significativo = patrón 0, bit más significativo = patrón 7) especifica el resultado para ese patrón:

```
bit_k(regla) = salida cuando vecindad = patron k
```

La regla se consulta mediante desplazamiento y máscara:

```
salida = (regla >> patron) & 1
```

Por ejemplo, la **Regla 30** (binario `00011110`):

| patrón | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|--------|---|---|---|---|---|---|---|---|
| salida | 0 | 0 | 0 | 1 | 1 | 1 | 1 | 0 |

---

## 2. Notación y convenciones

| Símbolo | Significado |
|---------|-------------|
| *N* | Ancho del autómata (número de celdas) |
| *T* | Número de generaciones (pasos de tiempo) |
| sᵢ(t) | Estado de la celda i en la generación t, ∈ {0, 1} |
| **e**(t) | Vector de estado completo en la generación t: [s₀(t), s₁(t), …, sₙ₋₁(t)] |
| **E** | Matriz de historial: *T* × *N*, fila *t* = **e**(t) |

---

## 3. Medida 1 — Densidad (ρ)

### 3.1 Definición

La **densidad** mide la fracción de celdas en estado 1 ("vivas") en una generación dada. Es la medida más elemental del "nivel de actividad" del autómata.

### 3.2 Fórmula matemática

```
           N-1
ρ(t) = 1/N · Σ  s_i(t)
          i=0
```

Donde:
- *N* = ancho del autómata
- sᵢ(t) ∈ {0, 1}
- 0 ≤ ρ(t) ≤ 1

**Rango:** [0, 1]

| Caso | ρ(t) |
|------|------|
| Todas las celdas muertas | 0.0 |
| 50 % de celdas vivas | 0.5 |
| Todas las celdas vivas | 1.0 |

### 3.3 Interpretación física

- ρ(t) ≈ 0: el autómata está en un estado de muerte homogénea (Clase I).
- ρ(t) ≈ 1: todas las celdas están vivas (estado saturado, raro en AC elementales).
- ρ(t) ≈ 0.5: comportamiento intermedio; puede indicar caos (Clase III) o complejidad (Clase IV).

### 3.4 Algoritmo de implementación

```
ENTRADA: estado = vector<int> de tamaño N
SALIDA:  densidad ∈ [0, 1]

1. SI estado.empty(): RETORNAR 0.0
2. suma ← Σ estado[i]  para todo i
3. RETORNAR suma / N   (cast explícito a double)
```

---

## 4. Medida 2 — Distancia de Hamming (H)

### 4.1 Definición

La **distancia de Hamming** entre dos vectores binarios de igual longitud cuenta el número de posiciones en las que difieren. Mide cuánto se alejó el autómata de su estado inicial (o de cualquier otro estado de referencia).

### 4.2 Fórmula matemática (versión cruda)

```
           N-1
H(A,B) = Σ  |a_i - b_i|
          i=0
```

### 4.3 Versión normalizada

```
H_norm(A, B) = H(A, B) / N
```

**Rango normalizado:** [0, 1]

| Caso | H_norm |
|------|--------|
| A = B (idénticos) | 0.0 |
| Difieren en todos los bits | 1.0 |

### 4.4 Observación sobre la fórmula de la distancia binaria

Para estados binarios, |aᵢ - bᵢ| es equivalente a XOR bit a bit:

```
|a_i - b_i|  =  a_i XOR b_i   ∈ {0, 1}
```

Por tanto, H(A,B) = Σ(aᵢ XOR bᵢ).

### 4.5 Algoritmo de implementación

```
hammingCrudo(A, B):
    1. SI |A| ≠ |B|: RETORNAR -1  (error)
    2. diff ← 0
    3. PARA i DESDE 0 HASTA N-1:
           SI A[i] ≠ B[i]: diff ← diff + 1
    4. RETORNAR diff

hammingNormalizado(A, B):
    1. crudo ← hammingCrudo(A, B)
    2. SI crudo < 0: RETORNAR -1.0  (error)
    3. RETORNAR crudo / N
```

---

## 5. Medida 3 — Parámetro de Langton (λ)

### 5.1 Definición

El **parámetro de Langton** es una medida *estática* introducida por Christopher Langton en 1990 para estimar la "vida" o complejidad de una regla de AC binaria. Se define como la fracción de las 8 transiciones posibles que generan una celda viva (salida = 1).

### 5.2 Fórmula matemática

```
        número de transiciones que producen 1
λ = ──────────────────────────────────────────
                     8
```

λ ∈ [0, 1]

### 5.3 Tabla de comportamiento referencial

| λ | Región | Comportamiento típico | Clase Wolfram |
|---|--------|-----------------------|---------------|
| 0.0 – 0.2 | Orden | Muerte homogénea, estado fijo | I |
| 0.2 – 0.4 | Periodicidad | Estructuras estables o periódicas | II |
| ~0.5 | Complejidad | Emergencia de estructuras complejas | **IV** |
| 0.6 – 1.0 | Caos | Ruido pseudoaleatorio expansivo | III |

### 5.4 Ejemplos concretos

| Regla | λ (bits en 1 / 8) | λ numérico |
|-------|-------------------|------------|
| Regla 0 (binario `00000000`) | 0/8 | **0.000** → Clase I |
| Regla 30 (`00011110`) | 4/8 | **0.500** → Clase III |
| Regla 90 (`01011010`) | 4/8 | **0.500** → Clase IV |
| Regla 110 (`01101110`) | 5/8 | **0.625** → Clase IV |
| Regla 184 (`10111000`) | 4/8 | **0.500** → Clase II |

> ⚠️ **Limitación:** λ no predice perfectamente la clase de Wolfram. Reglas 30 y 110 tienen valores de λ cercanos pero comportamientos diametralmente opuestos (caos puro vs. computación universal).

### 5.5 Algoritmo de implementación

```
parametroLangton(regla):
    1. unos ← 0
    2. PARA patron DESDE 0 HASTA 7:
           SI ((regla >> patron) & 1) ≠ 0:  // bit 'patron' de la regla es 1
               unos ← unos + 1
    3. RETORNAR unos / 8.0
```

---

## 6. Medida 4 — Entropía Espacial de Shannon (Hₑₛₚ)

### 6.1 Definición

La **entropía espacial** mide el grado de desorden en una generación instantánea del autómata. Se calcula aplicando la fórmula de entropía de Shannon (1948) sobre la distribución de estados en una sola fila.

### 6.2 Fórmula matemática

```
          1
H_esp(t) = - Σ  p_k · log₂(p_k)
          k=0
```

Para el caso binario, solo hay dos categorías: k ∈ {0, 1}.

Con las probabilidades definidas como:

```
p₁ = ρ(t)        // proporción de celdas en estado 1
p₀ = 1 - ρ(t)   // proporción de celdas en estado 0
```

La fórmula se reduce a:

```
H_esp(t) = -(p₀ · log₂(p₀) + p₁ · log₂(p₁))
```

### 6.3 Propiedades

| Propiedad | Valor |
|-----------|-------|
| Rango | [0, 1] bits |
| Caso límite p = 0 | H = 0 (por definición: lim p·log(p) = 0 cuando p → 0) |
| Caso límite p = 1 | H = 0 |
| Máximo | H = 1 bit, cuando p₀ = p₁ = 0.5 |

### 6.4 Tabla de valores referenciales

| Situación | ρ | Hₑₛₚ |
|-----------|---|------|
| Todas las celdas muertas | 0.0 | 0.000 bits |
| 75 % muertas, 25 % vivas | 0.25 | 0.811 bits |
| Equiprobable (máximo desorden) | 0.50 | 1.000 bits |
| 25 % muertas, 75 % vivas | 0.75 | 0.811 bits |
| Todas las celdas vivas | 1.0 | 0.000 bits |

### 6.5 Algoritmo de implementación

```
entropiaShannon(estado):
    1. p1 ← densidad(estado)
    2. SI p1 ≤ 0.0 O p1 ≥ 1.0: RETORNAR 0.0
    3. p0 ← 1.0 - p1
    4. RETORNAR -(p0 * log2(p0) + p1 * log2(p1))   // std::log2 de <cmath>
```

---

## 7. Medida 5 — Entropía Temporal de Shannon (Hₜₑₘₚ)

### 7.1 Definición

La **entropía temporal** fija una celda individual *i* y examina su secuencia de estados a lo largo de las *T* generaciones. Cuantifica qué tan predecible es el comportamiento de una celda a través del tiempo. Luego se aplica Shannon exactamente igual que en el caso espacial.

### 7.2 Fórmula matemática

**Paso 1 — calcular la probabilidad de "1" en la historia de la celda *i*:**

```
           T-1
p₁^(i) = 1/T · Σ  s_i^(t)
          t=0

```

**Paso 2 — aplicar Shannon:**

```
           1
H̄_temp(i) = - Σ  p_k^(i) · log₂(p_k^(i))
           k=0

```

Para binario:

```
p₀ = 1 - p₁^(i)
H̄_temp(i) = -(p₀ · log₂(p₀) + p₁ · log₂(p₁))
```

### 7.3 Entropía temporal promedio

Para obtener una medida global del autómata, se promedian las entropías de todas sus celdas:

```
      1     N-1
H̄ = ───── · Σ  H̄_temp(i)
     N    i=0

```

### 7.4 Tabla de interpretación

| H̄ₜₑₘₚ | Significado |
|--------|-------------|
| ≈ 0 | Celdas extremadamente predecibles (siempre 0 o siempre 1) |
| ≈ 1 | Celdas extremadamente impredecibles (alternan sin patrón) |
| Intermedio | Mezcla: algunas celdas estables, otras caóticas |

### 7.5 Algoritmo de implementación

```
entropiaTemporal(historial, columna):
    // historial: vector<vector<int>>,  T filas × N columnas
    1. T ← historial.size()
    2. SI T = 0: RETORNAR 0.0
    3. unos ← 0
    4. PARA CADA fila EN historial:
           SI columna es válida en fila:
               unos ← unos + fila[columna]
    5. p1 ← unos / T   (cast a double)
    6. SI p1 ≤ 0.0 O p1 ≥ 1.0: RETORNAR 0.0
    7. p0 ← 1.0 - p1
    8. RETORNAR -(p0 * log2(p0) + p1 * log2(p1))

entropiaTemporalPromedio(historial):
    1. SI historial.empty(): RETORNAR 0.0
    2. N ← historial[0].size()
    3. suma ← 0.0
    4. PARA i DESDE 0 HASTA N-1:
           suma ← suma + entropiaTemporal(historial, i)
    5. RETORNAR suma / N
```

---

## 8. Medida 6 — Exponente de Lyapunov (λ_L) — Aproximación por Hamming

### 8.1 Definición conceptual

En sistemas dinámicos continuos, el **exponente de Lyapunov** mide la tasa media de divergencia de trayectorias vecinas (efecto mariposa). Para autómatas celulares discretos no existe una definición formal que sea universalmente aceptada; la aproximación más extendida se basa en medir cómo crece la **distancia de Hamming** entre dos copias del autómata que parten de condiciones iniciales casi idénticas.

### 8.2 Método experimental

**Paso 1 — configurar los dos autómatas:**

- Autómata **A**: regla *R*, ancho *N*, estado inicial = un "1" en el centro, resto 0.
- Autómata **B**: misma regla *R*, mismo ancho *N*, mismo estado inicial que **A**, pero se **invierte una celda** distinta del centro (posición *i₀* recomendada: *N/2 + 1*).

- Esto garantiza una distancia de Hamming inicial h₀ = 1/*N* > 0.

**Paso 2 — evolucionar ambos en paralelo (misma regla, mismo paso sincrónico):**

```
A.evolucionar()   // t = t + 1
B.evolucionar()
```

**Paso 3 — medir la distancia de Hamming en cada paso:**

```
h(t) = H_norm(A(t), B(t))
```

**Paso 4 — aproximación final (forma simple):**

```
λ_L ≈ (ln(h(T)) - ln(h₀)) / T
```

Donde ln es el logaritmo natural (base *e*).

### 8.3 Fórmula matemática detallada

**Forma más precisa (sumatoria de cocientes):**

```
      T
λ_L ≈ 1/T · Σ  ln( h(t) / h(t-1) )
     t=1
```

**Forma práctica adoptada en el proyecto:**

```
λ_L ≈ (ln(h(T)) - ln(h(0))) / T
```

### 8.4 Prevención de ln(0)

Si en algún paso la perturbación se aniquila (h(t) = 0), `ln(0)` es `-∞`. Se usa un **épsilon**:

```
EPSILON = 10⁻¹⁰
si h(t) < EPSILON:  h(t) ← EPSILON
```

### 8.5 Tabla de interpretación

| λ_L | Comportamiento |
|-----|----------------|
| λ_L > 0 | Sensibilidad a condiciones iniciales: perturbaciones crecen → caos (Clase III) o complejidad (Clase IV) |
| λ_L ≈ 0 | Perturbaciones se propagan linealmente o se estabilizan → Clase II (periódica) |
| λ_L < 0 | Perturbaciones decaen → convergencia a atractor fijo → Clase I (orden) |

### 8.6 Algoritmo de implementación

```
lyapunovAproximado(regla, ancho, T, celdaPerturbada):
    1. A ← Automata1D(ancho, regla, false);  A.reiniciarCentro()
    2. B ← Automata1D(ancho, regla, false);  B.reiniciarCentro()
    3. B.flipCelda(celdaPerturbada)   // inyecta la perturbación
    4. h0 ← hamming(A.getEstado(), B.getEstado())
    5. EPSILON ← 1e-10
    6. SI h0 < EPSILON: h0 ← EPSILON
    7. PARA t DESDE 0 HASTA T-1:
           A.evolucionar();  B.evolucionar()
    8. hT ← hamming(A.getEstado(), B.getEstado())
    9. SI hT < EPSILON: hT ← EPSILON
   10. RETORNAR (ln(hT) - ln(h0)) / T   // std::log = ln en C++
```

---

## 9. Clasificación de Wolfram

### 9.1 Origen

Stephen Wolfram, en su artículo fundacional *"Universality and Complexity in Cellular Automata"* (Physica D, 1984) y posteriormente en *A New Kind of Science* (2002), clasificó los 256 autómatas elementales en **cuatro clases de comportamiento observable**:

| Clase | Descripción | Toma de muestra |
|-------|-------------|----------------|
| **I** | Decaimiento → estado homogéneo fijo | Reglas 0, 32, 128 |
| **II** | Estructuras estables o periódicas | Reglas 4, 56, 90, 184 |
| **III** | Caos aparente — apariencia de aleatoriedad | Reglas 30, 60, 126, 150 |
| **IV** | Complejidad — estructuras complejas y persistentes | **Regla 110** |

### 9.2 Cuadro completo de reglas por clase

**Clase I — Orden, decaimiento homogéneo**

| Reglas |
|--------|
| 0, 32, 128, 136, 160, 162, 168, 192, 200, 204, 232 |

**Clase II — Estructuras periódicas/estables locales**

| Reglas |
|--------|
| 4, 12, 40, 56, 60, 72, 90, 104, 108, 152, 164, 180, 184, 200, 232 |

**Clase III — Caos aparente**

| Reglas |
|--------|
| 15, 18, 22, 26, 30, 42, 46, 50, 54, 58, 62, 94, 102, 110, 122, 126, 146, 150, 154, 158, 182, 186, 210, 214 |

> ⚠️ Las reglas 110, 122, 126 aparecen en Clase III en la clasificación original de Wolfram (1984) y en el conjunto de datos de Wuensche (1999). El proyecto las ubica aquí también.

**Clase IV — Complejidad y universalidad**

| Reglas |
|--------|
| 18, 26, 54, 58, 62, 90, 94, 102, **110**, 122, 126, 146, 150, 154, 158, 166, 182, 186, 210, 222 |

> 🔑 **Regla 110** (tratamiento especial): demostrada Turing-completa por Matthew Cook (2004). Retorna el mensaje `"IV (Complejo - Universal)"`.

### 9.3 Reglas en múltiples clases

Las reglas 90, 110, 122, 126, 200 y 232 aparecen en más de un conjunto. El criterio de prioridad del proyecto:

```
verificar en orden: IV → III → II → I
retornar la primera coincidencia
```

- Regla 110: siempre `"IV (Complejo - Universal)"` (prioridad máxima).
- Reglas restantes en múltiples conjuntos: se devuelve la etiqueta más informativa (IV > III > II > I).

### 9.4 Algoritmo de clasificación

```
obtenerClaseWolfram(regla):
    IV  = {18, 26, 54, 58, 62, 90, 94, 102, 110, 122, 126, 146, 150, 154, 158, 166, 182, 186, 210, 222}
    III = {15, 18, 22, 26, 30, 42, 46, 50, 54, 58, 62, 94, 102, 110, 122, 126, 146, 150, 154, 158, 182, 186, 210, 214}
    II  = {4, 12, 40, 56, 60, 72, 90, 104, 108, 152, 164, 180, 184, 200, 232}
    I   = {0, 32, 128, 136, 160, 162, 168, 192, 200, 204, 232}

    SI regla = 110:         RETORNAR "IV (Complejo - Universal)"
    SI regla ∈ IV:          RETORNAR "IV (Complejidad)"
    SI regla ∈ III:         RETORNAR "III (Caos)"
    SI regla ∈ II:          RETORNAR "II (Periodica)"
    SI regla ∈ I:           RETORNAR "I (Orden)"
    RETORNAR "Desconocida"
```

---

## 10. Casos de prueba y valores esperados

### 10.1 Parámetros de ejecución batch

| Parámetro | Valor |
|-----------|-------|
| Ancho del autómata *N* | 200 celdas |
| Generaciones *T* | 500 |
| Reglas analizadas | 0, 30, 90, 110, 184 |
| Estado inicial | Un "1" en el centro *N/2*, resto ceros |

### 10.2 Matriz de criterios de aceptación

| Criterio | Regla 0 | Regla 30 | Regla 90 | Regla 110 | Regla 184 |
|----------|---------|----------|----------|-----------|-----------|
| λ_Langton = `unos / 8` | 0÷8 = **0.000** | 4÷8 = **0.500** | 4÷8 = **0.500** | 5÷8 = **0.625** | 4÷8 = **0.500** |
| Clase Wolfram | **I (Orden)** | **III (Caos)** | **IV (Complejidad)** | **IV (Complejo - Universal)** | **II (Periodica)** |
| Densidad final ≈ 0 | ✅ | ❌ | ❌ | ❌ | ✅ |
| Entropía espacial final = 0 | ✅ | ❌ | ❌ | ❌ | ✅ |
| λ_Lyapunov ≤ 0 | ✅ | ❌ | — | — | — |
| λ_Lyapunov > 0 | — | ✅ | ✅ | ✅ | — |
| Salida CSV generada | ✅ | ✅ | ✅ | ✅ | ✅ |
| Filas por CSV | 501 (cab + 500) | 501 | 501 | 501 | 501 |

### 10.3 Salida en consola esperada (ejemplo: Regla 30)

```
========================================
  REGLA 30 - III (Caos)
========================================
-> Parametro Langton (lambda): 0.5000
-> Evolucionando 500 generaciones...
-> Densidad final:     ~0.50  (± 0.10)
-> Entropia espacial:  ~1.00  (± 0.01)
-> Entropia temporal:  ~0.99  (± 0.05)
-> Exponente Lyapunov: > 0   (sensible a CI)
[OK] CSV exportado: resultados/regla_30_metricas.csv (500 filas)
```

### 10.4 Formato del CSV de salida

```
generacion,densidad,entropia_espacial,hamming_vs_inicial
0,0.005000,0.045415,0.000000
1,0.000000,0.000000,0.000000
...
499,0.560000,0.989588,0.490000
```

- Precisión: 6 decimales (`std::fixed << std::setprecision(6)`)
- Cabecera fija sin columna de generación en salida de tabla
- Una fila por generación de 0 a *T*-1

---

## 11. Referencias bibliográficas

1. **Wolfram, S. (2002).** *A New Kind of Science.* Wolfram Media.  
   → Fundamento teórico de las reglas elementales y la clasificación en 4 clases.

2. **Langton, C. G. (1990).** "Computation at the edge of chaos." *Physica D: Nonlinear Phenomena*, 42(1–3), 12–37.  
   → Introducción del parámetro λ como medida de complejidad en AC.

3. **Cook, M. (2004).** "Universality in Elementary Cellular Automata." *Complex Systems*, 15(1), 1–40.  
   → Demostración formal de que la Regla 110 es universalmente Turing-completa.

4. **Shannon, C. E. (1948).** "A Mathematical Theory of Communication." *Bell System Technical Journal*, 27(3), 379–423.  
   → Fundamento de la entropía de Shannon usada en las medidas espacial y temporal.

5. **Wolfram, S. (1984).** "Universality and complexity in cellular automata." *Physica D*, 10(1–2), 1–35.  
   → Artículo original que establece la clasificación en 4 clases de comportamiento.

6. **Wuensche, A. (1999).** "Classifying Cellular Automata Automatically." *Complexity*, 4(3), 47–66.  
   → Métodos automáticos para clasificar AC, incluyendo medidas de entropía y Lyapunov.

7. **Shalizi, C. R. (2001).** *Causal Architecture, Complexity and Self-Organization in Time Series and Cellular Automata.* PhD Thesis, University of Wisconsin-Madison.  
   → Tratamiento riguroso de entropías espacial y temporal en AC; discusión del exponente de Lyapunov en sistemas discretos.

8. **SFML Documentation.** Simple and Fast Multimedia Library. https://www.sfml-dev.org/documentation/3.0.0/  
   → Referencia técnica para la parte gráfica (si se extiende el proyecto con visualización).

---

*Documento de referencia formal — Proyecto 1.2 Medidas Cuantitativas AC — v1.1 — 2026-05-17*
