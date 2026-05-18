# SPEC-001: Implementación de src/ para 02-Medidas-Cuantitativas-AC

---

## Archivos a Implementar

### 3.1 automata1d.hpp

**Propósito:** Modelar un autómata celular 1D con vecindad de radio 1, estados binarios y evolución determinista según reglas de Wolfram.

**Requisitos:**

- Debe usar `std::vector<int>` para el estado (array dinámico).
- Debe soportar guardado de historial completo (matriz de generaciones) para entropía temporal.
- Bordes circulares: índice `-1` → `N-1`, índice `N` → `0`.

**Interfaz pública:**

```cpp
class Automata1D {
public:
    // Constructor: ancho = número de celdas, regla = 0-255,
    //              guardarHistorial = true si se necesita entropía temporal
    Automata1D(int ancho, int regla, bool guardarHistorial = false);

    // Estado inicial: un 1 en el centro, resto 0
    void reiniciarCentro();

    // Estado inicial: aleatorio binario
    void reiniciarAleatorio();

    // Evoluciona una generación completa (paralelismo sincrónico)
    void evolucionar();

    // Acceso
    const std::vector<int>& getEstado() const;
    const std::vector<std::vector<int>>& getHistorial() const;
    int getRegla() const;
    int getAncho() const;

    // Modificación
    void setEstado(const std::vector<int>& nuevo);
    void flipCelda(int indice);         // XOR con 1, para Lyapunov
    void limpiarHistorial();

    // Medida básica inline
    double calcularDensidad() const;
};
```

**Interfaz privada:**

```cpp
private:
    std::vector<int> estadoActual;
    std::vector<std::vector<int>> historial;   // Solo si guardarHistorial = true
    int regla;
    int ancho;
    bool guardarHistorial;

    int obtenerPatron(int izq, int centro, int der) const;   // P = 4*izq + 2*centro + der
    int aplicarRegla(int patron) const;                       // (regla >> patron) & 1
};
```

**Restricciones:**

- `evolucionar()` debe usar buffer temporal (`nuevoEstado`) para evitar usar el futuro como pasado.
- `reiniciarCentro()` y `reiniciarAleatorio()` deben limpiar el historial si `guardarHistorial = true`.
- La semilla aleatoria (`std::srand`) debe inicializarse una sola vez por ejecución (usar `static bool`).

---

### 3.2 automata1d.cpp

**Implementación detallada:**

**Constructor**

- Inicializar `ancho`, `regla`, `guardarHistorial`.
- `estadoActual.resize(ancho, 0)`.
- Inicializar semilla aleatoria con `std::time(nullptr)` si es la primera vez (usar `static bool`).

**`reiniciarCentro()`**

- Todo a `0`.
- `estadoActual[ancho / 2] = 1`.
- Si `guardarHistorial`: limpiar historial, `push_back` del estado inicial.

**`reiniciarAleatorio()`**

- Para cada celda: `std::rand() % 2`.
- Si `guardarHistorial`: limpiar historial, `push_back` del estado inicial.

**`obtenerPatron(izq, centro, der)`**

- Retornar `izq * 4 + centro * 2 + der * 1`.
- No hay validación de rango (asume que `izq`/`centro`/`der` son 0 o 1).

**`aplicarRegla(patron)`**

- Retornar `(regla >> patron) & 1`.
- `patron` está en `[0, 7]`.

**`evolucionar()`**

```cpp
void Automata1D::evolucionar() {
    std::vector<int> nuevoEstado(ancho);

    for (int i = 0; i < ancho; ++i) {
        int izq    = estadoActual[(i - 1 + ancho) % ancho];
        int centro = estadoActual[i];
        int der    = estadoActual[(i + 1) % ancho];

        nuevoEstado[i] = aplicarRegla(obtenerPatron(izq, centro, der));
    }

    estadoActual = nuevoEstado;

    if (guardarHistorial) {
        historial.push_back(estadoActual);
    }
}
```

**`setEstado(nuevo)`**

- Validar que `nuevo.size() == ancho`.
- Copiar a `estadoActual`.

**`flipCelda(indice)`**

- Validar `0 <= indice < ancho`.
- `estadoActual[indice] ^= 1` (XOR bit a bit).

**`calcularDensidad()`**

- Sumar todos los elementos de `estadoActual`.
- Dividir por `ancho` (cast a `double`).

---

### 3.3 analizador.hpp

**Propósito:** Clase estática que implementa las 6 medidas cuantitativas solicitadas.

| # | Medida | Tipo | Descripción |
|---|--------|------|-------------|
| 1 | Parámetro de Langton (λ) | Estática (propiedad de regla) | Fracción de transiciones que producen 1 |
| 2 | Densidad (ρ) | Dinámica (sobre estado) | Proporción de celdas vivas |
| 3 | Distancia de Hamming (H) | Comparativa | Celdas que difieren entre dos estados |
| 4 | Entropía Espacial (Hₑₛₚ) | Dinámica | Shannon sobre una generación |
| 5 | Entropía Temporal (Hₜₑₘₚ) | Dinámica | Shannon sobre la historia de una celda |
| 6 | Exponente de Lyapunov (λ_`) | Dinámica | Aproximación por divergencia de Hamming |

**Estructura de datos para exportación:**

```cpp
struct MetricasGeneracion {
    int generacion;
    double densidad;
    double entropiaEspacial;
    double hammingVsInicial;
};
```

**Interfaz de la clase:**

```cpp
class AnalizadorAC {
public:
    // 1. LANGTON
    static double parametroLangton(int regla);

    // 2. DENSIDAD
    static double densidad(const std::vector<int>& estado);

    // 3. HAMMING
    static double hamming(const std::vector<int>& a, const std::vector<int>& b);      // Normalizado [0, 1]
    static int    hammingCrudo(const std::vector<int>& a, const std::vector<int>& b);   // Entero [0, N]

    // 4. ENTROPÍA ESPACIAL
    static double entropiaShannon(const std::vector<int>& estado);

    // 5. ENTROPÍA TEMPORAL
    static double entropiaTemporal(const std::vector<std::vector<int>>& historial, int columna);
    static double entropiaTemporalPromedio(const std::vector<std::vector<int>>& historial);

    // 6. LYAPUNOV
    static double lyapunovAproximado(int regla, int ancho, int T, int celdaPerturbada);

    // UTILIDADES
    static void exportarCSV(const std::string& nombreArchivo,
                            const std::vector<MetricasGeneracion>& datos);
    static std::string obtenerClaseWolfram(int regla);
};
```

---

### 3.4 analizador.cpp — Algoritmos Detallados

#### 3.4.1 `parametroLangton(int regla)`

```
ENTRADA:  regla (int, 0-255)
SALIDA:   lambda (double, [0, 1])

1. unos ← 0
2. PARA patron DESDE 0 HASTA 7:
       SI (regla >> patron) & 1 == 1:
           unos ← unos + 1
3. RETORNAR unos / 8.0
```

**Explicación:** La regla es una tabla de 8 bits. Cada bit representa la salida para un patrón de vecindad. Contar los bits en 1 y dividir por 8 da la proporción de transiciones "activas".

#### 3.4.2 `densidad(const std::vector<int>& estado)`

```
ENTRADA:  estado (vector<int>)
SALIDA:   densidad (double, [0, 1])

1. SI estado.empty(): RETORNAR 0.0
2. suma ← Σ estado[i] para todo i
3. RETORNAR suma / estado.size() como double
```

#### 3.4.3 `hammingCrudo(a, b)` y `hamming(a, b)`

```
hammingCrudo — ENTRADA: a, b (vector<int>)
               SALIDA: diferencias (int, [0, N]) o -1 si tamaños difieren

1. SI a.size() != b.size(): RETORNAR -1
2. diff ← 0
3. PARA i DESDE 0 HASTA a.size() - 1:
       SI a[i] != b[i]: diff ← diff + 1
4. RETORNAR diff

hamming (normalizado):

1. crudo ← hammingCrudo(a, b)
2. SI crudo == -1: manejar error (no debería ocurrir en uso interno)
3. RETORNAR crudo / double(a.size())
```

#### 3.4.4 `entropiaShannon(const std::vector<int>& estado)`

```
ENTRADA:  estado (vector<int>)
SALIDA:   entropía (double, [0, 1] bits para binario)

1. p1 ← densidad(estado)
2. SI p1 <= 0.0 O p1 >= 1.0: RETORNAR 0.0
   (casos límite: 0 * log(0) se define como 0)
3. p0 ← 1.0 - p1
4. RETORNAR -(p0 * log2(p0) + p1 * log2(p1))
```

**Nota:** Usar `std::log2` de `<cmath>`. El máximo teórico para distribución binaria es 1.0 bit (cuando `p0 = p1 = 0.5`).

#### 3.4.5 `entropiaTemporal(historial, columna)`

```
ENTRADA:  historial (vector<vector<int>>), columna (int)
SALIDA:   entropía temporal (double, [0, 1])

1. T ← historial.size()
2. SI T == 0: RETORNAR 0.0
3. unos ← 0
4. PARA CADA fila EN historial:
       SI columna ES VÁLIDA EN fila: unos ← unos + fila[columna]
5. p1 ← unos / T como double
6. SI p1 <= 0.0 O p1 >= 1.0: RETORNAR 0.0
7. p0 ← 1.0 - p1
8. RETORNAR -(p0 * log2(p0) + p1 * log2(p1))
```

**`entropiaTemporalPromedio`:**

```
ENTRADA:  historial (vector<vector<int>>)
SALIDA:   promedio de entropías temporales (double)

1. SI historial.empty(): RETORNAR 0.0
2. N ← historial[0].size()
3. suma ← 0.0
4. PARA i DESDE 0 HASTA N - 1:
       suma ← suma + entropiaTemporal(historial, i)
5. RETORNAR suma / N
```

#### 3.4.6 `lyapunovAproximado(regla, ancho, T, celdaPerturbada)`

```
ENTRADA:  regla (int), ancho (int), T (int), celdaPerturbada (int)
SALIDA:   lambda_L (double)

1. Crear Automata1D A(ancho, regla, false)
2. Crear Automata1D B(ancho, regla, false)
3. A.reiniciarCentro()
4. B.reiniciarCentro()
5. B.flipCelda(celdaPerturbada)

6. h0 ← hamming(A.getEstado(), B.getEstado())
7. EPSILON ← 1e-10
8. SI h0 < EPSILON: h0 ← EPSILON

9. PARA t DESDE 0 HASTA T - 1:
       A.evolucionar()
       B.evolucionar()

10. hT ← hamming(A.getEstado(), B.getEstado())
11. SI hT < EPSILON: hT ← EPSILON

12. RETORNAR (ln(hT) - ln(h0)) / T
```

**Notas importantes:**

- Usar `std::log` (logaritmo natural, base e).
- El epsilon evita `ln(0)` que es `-inf`.
- `celdaPerturbada` debe ser diferente del centro para que `h0 > 0`. Recomendado: `ancho / 2 + 1`.
- Esta es una aproximación práctica, no el exponente de Lyapunov formal (que requiere espacio continuo).

#### 3.4.7 `exportarCSV(nombreArchivo, datos)`

```
ENTRADA:  nombreArchivo (string), datos (vector<MetricasGeneracion>)
SALIDA:   archivo CSV en disco, o mensaje de error

1. Abrir archivo de texto para escritura
2. SI no se pudo abrir: imprimir error en cerr, RETORNAR
3. Escribir cabecera: "generacion,densidad,entropia_espacial,hamming_vs_inicial\n"
4. PARA CADA m EN datos:
       Escribir línea con precisión de 6 decimales:
       m.generacion, m.densidad, m.entropiaEspacial, m.hammingVsInicial
5. Cerrar archivo
6. Imprimir confirmación en cout con ruta y número de filas
```

**Formato de números:** `std::fixed << std::setprecision(6)`

#### 3.4.8 `obtenerClaseWolfram(regla)`

**Algoritmo:** Tabla de búsqueda por conjuntos. Retornar `string` con la clase I, II, III o IV.

Implementar con cadenas de `if` que verifiquen membresía en lists predefinidas:

| Clase | Descripción | Reglas de ejemplo |
|-------|-------------|-------------------|
| **I** | Evoluciona hacia estado homogéneo (fijo) | 0, 32, 128, 160, 192, 232 |
| **II** | Estructuras periódicas o estables locales | 4, 56, 90, 108, 184 |
| **III** | Caos aparente, pseudoaleatorio | 30, 60, 126, 150 |
| **IV** | Complejidad (emergencia, universales) | **110** (universalmente Turing) |

- **Clase I:** `{0, 32, 128, 136, 160, 162, 168, 192, 200, 204, 232}`
- **Clase II:** `{4, 12, 40, 56, 60, 72, 90, 104, 108, 152, 164, 180, 184, 200, 232}`
- **Clase III:** `{15, 18, 22, 26, 30, 42, 46, 50, 54, 58, 62, 94, 102, 110, 122, 126, 146, 150, 154, 158, 182, 186, 210, 214}`
- **Clase IV:** `{18, 26, 54, 58, 62, 90, 94, 102, 110, 122, 126, 146, 150, 154, 158, 166, 182, 186, 210, 222}`

- Regla 110 tiene tratamiento especial: retornar `"IV (Complejo - Universal)"`.
- Reglas en múltiples clases: priorizar la clasificación más informativa (IV > III > II > I).

---

### 3.5 main.cpp

**Propósito:** Orquestar el análisis batch de múltiples reglas, medir todo y exportar resultados.

**Flujo principal:**

```
1. Imprimir encabezado del programa
2. Crear directorio "resultados" si no existe (usar std::filesystem)
3. Definir parámetros:
      ANCHO         = 200 (celdas)
      GENERACIONES  = 500
      REGLAS        = {0, 30, 90, 110, 184}
4. PARA CADA regla EN REGLAS:
      a. Imprimir separador con número de regla y clase Wolfram
      b. Calcular y mostrar parámetro Langton
      c. Crear Automata1D con guardarHistorial = true
      d. reiniciarCentro()
      e. Guardar estado inicial como referencia para Hamming
      f. PARA t DESDE 0 HASTA GENERACIONES - 1:
              Medir densidad, entropía espacial, Hamming vs inicial
              Guardar en vector<MetricasGeneracion>
              Evolucionar autómata
      g. Calcular entropía temporal promedio (sobre historial completo)
      h. Calcular Lyapunov aproximado (perturbación en ancho / 2 + 1)
      i. Mostrar resumen en consola (densidad final, entropías, Lyapunov)
      j. Exportar CSV a "resultados/regla_X_metricas.csv"
      k. Mostrar tabla resumen de primeras y últimas generaciones
5. Imprimir mensaje de finalización
```

**Salida en consola esperada (ejemplo Regla 30):**

```
========================================
  REGLA 30 - III (Caos)
========================================
-> Parametro Langton (λ): 0.5000
   (λ ~ 0: orden | λ ~ 0.5: complejidad | λ ~ 1: caos)
-> Evolucionando 500 generaciones...
-> Densidad final: 0.5000
-> Entropia espacial final: 1.0000 bits
-> Entropia temporal promedio: 0.9876 bits
-> Exponente Lyapunov (aprox): 0.0012
   (λ > 0: sensible a CI | λ ≈ 0: estable | λ < 0: converge)
[OK] CSV exportado: resultados/regla_30_metricas.csv (500 filas)

   Gen | Densidad | Entropia | Hamming(vs inicial)
   ----|----------|----------|---------------------
     0 | 0.005000 | 0.045420 | 0.000000
    50 | 0.480000 | 0.998721 | 0.495000
   ...
   450 | 0.505000 | 0.999832 | 0.500000
   499 | 0.495000 | 0.999912 | 0.490000
```

---

## Restricciones y Consideraciones Técnicas

### 4.1 C++17

- Usar `std::filesystem` para crear directorios (`create_directories`).
- Usar `std::vector` para todos los arreglos dinámicos.
- No usar arrays C-style (`int arr[N]`).
- Usar `static_cast<double>` para conversiones explícitas.

### 4.2 Precisión numérica

- Todos los cálculos de medidas en `double`.
- Exportar CSV con `std::fixed << std::setprecision(6)`.

### 4.3 Manejo de casos límite

| Caso | Comportamiento |
|------|----------------|
| Entropía Shannon con `p = 0` o `p = 1` | Retornar `0.0` (no calcular `log2(0)`) |
| Lyapunov con Hamming = 0 en algún paso | Usar `epsilon = 1e-10` para evitar `ln(0)` |
| Historial vacío | Entropía temporal retorna `0.0` |

### 4.4 Rendimiento

- El historial de 500 generaciones × 200 celdas = 100,000 enteros. Totalmente manejable en memoria.
- No optimizar prematuramente; priorizar claridad del código.

---

## Criterios de Aceptación

- [ ] `automata1d.hpp` / `.cpp` compilan sin warnings.
- [ ] `analizador.hpp` / `.cpp` compilan sin warnings.
- [ ] `main.cpp` compila y enlaza correctamente.
- [ ] Ejecutar genera 5 archivos CSV en `resultados/`.
- [ ] Cada CSV tiene 500 filas + cabecera.
- [ ] Consola muestra parámetro Langton correcto para cada regla:
  - Regla 0 → 0.0
  - Regla 30 → 0.5
  - Regla 110 → 0.625
- [ ] Densidad final de Regla 0 debe ser `0.0` (todo muere).
- [ ] Entropía espacial de Regla 0 debe ser `0.0` (estado homogéneo).
- [ ] Lyapunov de Regla 0 debe ser negativo o 0 (converge).
- [ ] Lyapunov de Regla 30 debe ser positivo (diverge, caos).

---

## Historial de Cambios

| Fecha | Versión | Descripción |
|-------|---------|-------------|
| 2026-05-17 | 1.1 | **Correcciones de formato:** reescritura completa del spec desde el archivo original sin formato (códigos rotos ````cpp Copy`, tablas mal formadas, subtítulos a nivel 1, líneas colapsadas). Jerarquía de headings ajustada a `#` / `##` / `###` / `####`. |
| 2026-05-17 | 1.1 | **Mejora W-01 — Precedencia de operadores en `parametroLangton`:** expresión `(regla >> patron) & 1 == 1` cambiada a `((regla >> patron) & 1) != 0` para eliminar ambigüedad de precedencia (`==` antes que `&`). |
| 2026-05-17 | 1.1 | **Mejora S-01 — Guarda de división por cero en `calcularDensidad()`:** agregado `if (ancho == 0) return 0.0;` antes de dividir por `ancho`. |

---

## Entregables

| Archivo | Ruta | Estado |
|---------|------|--------|
| automata1d.hpp | `src/automata1d.hpp` | Implementado |
| automata1d.cpp | `src/automata1d.cpp` | Implementado |
| analizador.hpp | `src/analizador.hpp` | Implementado |
| analizador.cpp | `src/analizador.cpp` | Implementado |
| main.cpp | `src/main.cpp` | Implementado |
