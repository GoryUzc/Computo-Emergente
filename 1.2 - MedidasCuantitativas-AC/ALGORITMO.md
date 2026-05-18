# Documentación Técnica del Algoritmo

**Proyecto:** 1.2 – Medidas Cuantitativas en Autómatas Celulares 1D  
**Stack:** C++17 · CMake 3.16+ · Visual Studio 2022  
**Fecha:** 2026-05-17

---

## 1. Arquitectura del Sistema

### 1.1 Estructura de Módulos

```
src/
├── main.cpp          # Orquestación batch, generación de reportes
├── automata1d.hpp    # Interfaz de la clase Automata1D
├── automata1d.cpp    # Implementación del autómata celular
├── analizador.hpp    # Interfaz de la clase AnalizadorAC
└── analizador.cpp    # Implementación de las 6 medidas cuantitativas
```

### 1.2 Modelo de Autómata Celular Elemental

- **Dimensión:** 1D (fila única de celdas)
- **Estados:** Binarios {0, 1}
- **Vecindad:** Radio 1 (3 celdas: izquierda, centro, derecha)
- **Borde:** Toroidal (circular)
- **Reglas:** 256 reglas de Wolfram (0-255)

---

## 2. Implementación del Autómata Celular 1D

### 2.1 Codificación de la Regla de Wolfram

La regla se almacena como entero de 8 bits. Cada bit *k* (0-7) representa la salida para el patrón *k*.

```cpp
// automata1d.cpp:40-42
int Automata1D::aplicarRegla(int patron) const {
    return (regla >> patron) & 1;
}
```

**Patrón a índice:**
```cpp
// automata1d.cpp:36-38
int Automata1D::obtenerPatron(int izq, int centro, int der) const {
    return izq * 4 + centro * 2 + der * 1;
}
```

### 2.2 Evolución Sincrónica con Bordes Toroidales

```cpp
// automata1d.cpp:44-60
void Automata1D::evolucionar() {
    std::vector<int> nuevoEstado(ancho);

    for (int i = 0; i < ancho; ++i) {
        int izq    = estadoActual[(i - 1 + ancho) % ancho];
        int centro = estadoActual[i];
        int der    = estadoActual[(i + 1) % ancho];

        nuevoEstado[i] = aplicarRegla(obtenerPatron(izq, centro, der));
    }

    estadoActual = nuevoEstado;
}
```

**Complejidad:** O(N) por generación, donde N es el ancho del autómata.

---

## 3. Medidas Cuantitativas

### 3.1 Densidad (ρ)

**Fórmula:** ρ = (1/N) Σ sᵢ

```cpp
// automata1d.cpp:93-98
double Automata1D::calcularDensidad() const {
    int suma = 0;
    for (int celda : estadoActual) suma += celda;
    if (ancho == 0) return 0.0;
    return static_cast<double>(suma) / ancho;
}
```

**Propiedades:**
- Rango: [0, 1]
- ρ = 0: estado completamente muerto
- ρ = 1: estado completamente vivo
- ρ = 0.5: máxima mezcla binaria

---

### 3.2 Distancia de Hamming

**Fórmula cruda:** H(A,B) = Σ |aᵢ - bᵢ|  
**Versión normalizada:** H_norm = H/N

```cpp
// analizador.cpp:21-34
int AnalizadorAC::hammingCrudo(const std::vector<int>& a, const std::vector<int>& b) {
    if (a.size() != b.size()) return -1;
    int diff = 0;
    for (size_t i = 0; i < a.size(); ++i)
        if (a[i] != b[i]) ++diff;
    return diff;
}

double AnalizadorAC::hamming(const std::vector<int>& a, const std::vector<int>& b) {
    int crudo = hammingCrudo(a, b);
    if (crudo < 0) return -1.0;
    return static_cast<double>(crudo) / static_cast<double>(a.size());
}
```

---

### 3.3 Parámetro de Langton (λ)

**Fórmula:** λ = (bits en 1) / 8

```cpp
// analizador.cpp:4-10
double AnalizadorAC::parametroLangton(int regla) {
    int unos = 0;
    for (int patron = 0; patron < 8; ++patron) {
        if (((regla >> patron) & 1) != 0) ++unos;
    }
    return static_cast<double>(unos) / 8.0;
}
```

**Interpretación:**
- λ ∈ [0, 0.2]: Clase I (Orden)
- λ ∈ [0.2, 0.4]: Clase II (Periódica)
- λ ≈ 0.5: Clase IV (Complejidad)
- λ ∈ [0.6, 1.0]: Clase III (Caos)

---

### 3.4 Entropía Espacial de Shannon

**Fórmula:** H_esp = -(p₀ log₂ p₀ + p₁ log₂ p₁)

```cpp
// analizador.cpp:37-43
double AnalizadorAC::entropiaShannon(const std::vector<int>& estado) {
    double p1 = densidad(estado);
    if (p1 <= 0.0 || p1 >= 1.0) return 0.0;

    double p0 = 1.0 - p1;
    return -(p0 * std::log2(p0) + p1 * std::log2(p1));
}
```

**Casos límite:**
- p₁ = 0 o p₁ = 1: H = 0 (estado totalmente ordenado)
- p₁ = 0.5: H = 1 (máxima entropía para binario)

---

### 3.5 Entropía Temporal de Shannon

**Fórmula:** H_temp = -(p₀ log₂ p₀ + p₁ log₂ p₁), donde p₁ = (1/T) Σ sᵢ⁽ᵗ⁾

```cpp
// analizador.cpp:46-62
double AnalizadorAC::entropiaTemporal(const std::vector<std::vector<int>>& historial, int columna) {
    int T = static_cast<int>(historial.size());
    if (T == 0) return 0.0;

    int unos = 0;
    for (const auto& fila : historial) {
        if (columna >= 0 && columna < static_cast<int>(fila.size())) {
            unos += fila[columna];
        }
    }

    double p1 = static_cast<double>(unos) / static_cast<double>(T);
    if (p1 <= 0.0 || p1 >= 1.0) return 0.0;

    double p0 = 1.0 - p1;
    return -(p0 * std::log2(p0) + p1 * std::log2(p1));
}
```

---

### 3.6 Exponente de Lyapunov (Aproximación por Hamming)

**Método:**
1. Crear dos autómatas A y B con la misma regla
2. Estado inicial de A: 1 en el centro
3. Estado inicial de B: 1 en el centro + perturbación en celda diferente
4. Evolucionar T generaciones
5. Calcular h(t) = H_norm(A(t), B(t)) en cada paso
6. Aproximar: λ_L ≈ (ln(h_T) - ln(h₀)) / T

```cpp
// analizador.cpp:76-97
double AnalizadorAC::lyapunovAproximado(int regla, int ancho, int T, int celdaPerturbada) {
    Automata1D A(ancho, regla, false);
    Automata1D B(ancho, regla, false);

    A.reiniciarCentro();
    B.reiniciarCentro();
    B.flipCelda(celdaPerturbada);

    double h0 = hamming(A.getEstado(), B.getEstado());
    const double EPSILON = 1e-10;
    if (h0 < EPSILON) h0 = EPSILON;

    for (int t = 0; t < T; ++t) {
        A.evolucionar();
        B.evolucionar();
    }

    double hT = hamming(A.getEstado(), B.getEstado());
    if (hT < EPSILON) hT = EPSILON;

    return (std::log(hT) - std::log(h0)) / static_cast<double>(T);
}
```

**Manejo de casos degenerados:**
- Si h(t) = 0, se usa EPSILON = 10⁻¹⁰ para evitar ln(0)
- La celda perturbada debe ser distinta del centro para garantizar h₀ > 0

---

## 4. Clasificación de Wolfram

```cpp
// analizador.cpp:121-140
std::string AnalizadorAC::obtenerClaseWolfram(int regla) {
    const int claseIV[]  = {18, 26, 54, 58, 62, 90, 94, 102, 110, 122, 126, 146, 150, 154, 158, 166, 182, 186, 210, 222};
    const int claseIII[] = {15, 18, 22, 26, 30, 42, 46, 50, 54, 58, 62, 94, 102, 110, 122, 126, 146, 150, 154, 158, 182, 186, 210, 214};
    const int claseII[]  = {4, 12, 40, 56, 60, 72, 90, 104, 108, 152, 164, 180, 184, 200, 232};
    const int claseI[]   = {0, 32, 128, 136, 160, 162, 168, 192, 200, 204, 232};

    auto pertenece = [](int valor, const int* arr, size_t n) {
        for (size_t i = 0; i < n; ++i) if (arr[i] == valor) return true;
        return false;
    };

    if (regla == 110) return "IV (Complejo - Universal)";
    if (pertenece(regla, claseIV,  20)) return "IV (Complejidad)";
    if (pertenece(regla, claseIII, 24)) return "III (Caos)";
    if (pertenece(regla, claseII,  15)) return "II (Periodica)";
    if (pertenece(regla, claseI,   11)) return "I (Orden)";

    return "Desconocida";
}
```

---

## 5. Exportación de Resultados

```cpp
// analizador.cpp:100-119
void AnalizadorAC::exportarCSV(const std::string& nombreArchivo,
                               const std::vector<MetricasGeneracion>& datos) {
    std::ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir el archivo: " << nombreArchivo << std::endl;
        return;
    }

    archivo << "generacion,densidad,entropia_espacial,hamming_vs_inicial\n";

    for (const auto& m : datos) {
        archivo << m.generacion << ','
                << std::fixed << std::setprecision(6) << m.densidad << ','
                << std::fixed << std::setprecision(6) << m.entropiaEspacial << ','
                << std::fixed << std::setprecision(6) << m.hammingVsInicial << '\n';
    }

    archivo.close();
    std::cout << "[OK] CSV exportado: " << nombreArchivo << " (" << datos.size() << " filas)" << std::endl;
}
```

**Formato CSV:**
```
generacion,densidad,entropia_espacial,hamming_vs_inicial
0,0.005000,0.045415,0.000000
...
```

---

## 6. Consideraciones de Implementación

### 6.1 Complejidad Computacional

| Operación | Complejidad |
|-----------|-------------|
| Evolución (1 generación) | O(N) |
| Densidad | O(N) |
| Hamming | O(N) |
| Parámetro Langton | O(1) |
| Entropía espacial | O(N) |
| Entropía temporal | O(T) por celda |
| Exponente Lyapunov | O(T × N) |

### 6.2 Precisión Numérica

- Uso de `double` para todas las medidas continuas
- `std::log2()` para cálculo de entropía
- `std::log()` (natural) para cálculo de Lyapunov
- EPSILON = 10⁻¹⁰ para prevenir singularidades

### 6.3 Memoria

- El historial consume O(T × N) de memoria cuando `guardarHistorial = true`
- Se recomienda desactivar para análisis de Lyapunov o T muy grande
- Estado actual: O(N)

---

## 7. Referencias

1. Wolfram, S. (2002). *A New Kind of Science*. Wolfram Media.
2. Langton, C. G. (1990). "Computation at the edge of chaos." *Physica D*.
3. Shannon, C. E. (1948). "A Mathematical Theory of Communication."
4. Cook, M. (2004). "Universality in Elementary Cellular Automata."

---

*ALGORITMO.md — Documentación Técnica v1.0*