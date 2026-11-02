# ARCH_DESIGN.md — Arquitectura de Diseño
## Sistema Difuso Interactivo para Evaluación de Severidad Sísmica
---

## 1. Propósito y Alcance

Este documento define la **arquitectura**, los **contratos de interfaz** y las **reglas del pipeline** del sistema difuso Mamdani para estimar la Intensidad Sísmica Percibida a partir de Magnitud (M), Profundidad (h) y Distancia epicentral (d).

### Alcance
- Motor de cálculo en C++ con Eigen.
- Front-end interactivo en Python (REPL + matplotlib + pandas).
- Comunicación nativa mediante pybind11.
- Análisis de sensibilidad por comparación de configuraciones.

### Fuera de alcance
- Sistemas neurodifusos (ANFIS).
- Control en lazo cerrado.
- Aprendizaje automático de reglas.

---

## 2. Arquitectura General

### 2.1. Estilo arquitectónico
**Capas + Pipeline**, con separación estricta entre:
1. **Motor de cálculo** (C++): puro, sin I/O, testeable.
2. **Capa de enlace** (pybind11): expone el motor a Python.
3. **Front-end** (Python): REPL, visualización, exportación.

### 2.2. Diagrama de capas

```
┌─────────────────────────────────────────────────────────┐
│  PYTHON (Front-end)                                     │
│  ┌────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐  │
│  │  cli   │──│  bridge  │──│ plotter  │──│ exporter │  │
│  └────────┘  └────┬─────┘  └──────────┘  └──────────┘  │
└───────────────────┼─────────────────────────────────────┘
                    │ pybind11 (FFI en memoria)
                    ▼
┌─────────────────────────────────────────────────────────┐
│  C++ (Motor) — fuzzy_core                               │
│  FuzzySystem (Facade)                                   │
│    ├── Fuzzifier                                        │
│    ├── InferenceEngine                                  │
│    ├── Aggregator                                       │
│    └── Defuzzifier                                      │
└─────────────────────────────────────────────────────────┘
```

---

## 3. Módulos y Responsabilidades

### 3.1. Motor C++ (`fuzzy_core`)

| Módulo | Responsabilidad | Principio SOLID |
|---|---|---|
| `MembershipFunction` (abstracta) | Define contrato de función de pertenencia | ISP, OCP |
| `TriangularMF`, `TrapezoidalMF` | Implementaciones concretas de MF | SRP |
| `LinguisticVariable` | Agrupa universo + términos lingüísticos | SRP |
| `FuzzyRule` | Representa y evalúa una regla difusa | SRP |
| `Fuzzifier` | Etapa 1: crisp → grados | SRP |
| `InferenceEngine` | Etapa 2: evalúa reglas con T-norma | SRP |
| `Aggregator` | Etapa 3: combina consecuentes con S-norma | SRP |
| `Defuzzifier` | Etapa 4: centroide con Eigen | SRP |
| `FuzzySystem` | Fachada que orquesta el pipeline | Facade pattern |
| `FuzzyResult` | DTO con todos los resultados intermedios | DTO pattern |

### 3.2. Front-end Python (`fuzzy_ui`)

| Módulo | Responsabilidad |
|---|---|
| `main.py` | Punto de entrada; orquesta todo |
| `cli.py` | REPL, validación de entradas, menú |
| `bridge.py` | Wrapper de pybind11 |
| `plotter.py` | Visualización con matplotlib |
| `exporter.py` | Exportación a CSV con pandas |
| `config.py` | Configuración por defecto (MF, reglas, universos) |

---

## 4. Contratos de Interfaz

### 4.1. Contrato de `MembershipFunction` (C++)

```cpp
class MembershipFunction {
public:
    virtual ~MembershipFunction() = default;
    virtual double evaluate(double x) const = 0;
    virtual std::pair<double,double> getSupport() const = 0;
    virtual std::string getName() const = 0;
};
```

**Invariantes:**
- `evaluate(x)` siempre retorna un valor en [0, 1].
- `getSupport()` retorna el intervalo donde μ(x) > 0.

### 4.2. Contrato de `LinguisticVariable` (C++)

```cpp
class LinguisticVariable {
public:
    void addTerm(std::shared_ptr<MembershipFunction> mf);
    std::vector<double> fuzzify(double x) const;
    const MembershipFunction& getTerm(size_t index) const;
    const MembershipFunction& getTermByName(const std::string& name) const;
    size_t size() const;
    std::string getName() const;
    std::pair<double,double> getUniverse() const;
};
```

**Precondición:** `x` debe estar dentro del universo (validado por el llamador).  
**Postcondición:** `fuzzify(x)` retorna un vector de tamaño igual al número de términos.

### 4.3. Contrato de `FuzzyRule` (C++)

```cpp
class FuzzyRule {
public:
    double evaluateDegree(
        const std::map<std::string, double>& crispInputs,
        const std::map<std::string, LinguisticVariable>& variables
    ) const;
    
    std::pair<std::string, std::string> getConsequent() const;
    const std::vector<std::pair<std::string, std::string>>& getAntecedents() const;
    double getWeight() const;
    int getId() const;
};
```

**Semántica:** el grado de activación es la T-norma (mínimo) de los grados de los antecedentes.

### 4.4. Contrato de `FuzzySystem` (Fachada, C++)

```cpp
class FuzzySystem {
public:
    FuzzyResult compute(const std::map<std::string, double>& inputs);
    void addRule(FuzzyRule rule);
    std::vector<std::string> getInputNames() const;
    const LinguisticVariable& getOutput() const;
    const std::vector<FuzzyRule>& getRules() const;
};
```

`compute()` valida que cada entrada configurada esté presente y dentro de su universo. Lanza `std::invalid_argument` cuando una entrada falta o está fuera de rango.

### 4.5. Contrato de `FuzzyResult` (DTO, C++)

```cpp
struct FuzzyResult {
    double crisp_output;                                    // y*
    std::map<std::string, std::vector<double>> fuzzified_inputs;
    std::vector<double> rule_degrees;                        // α_i
    Eigen::VectorXd aggregated_mu;                           // μ_agregado
    Eigen::VectorXd discretized_output;                      // universo de salida
};
```

### 4.6. Contrato del Bridge Python

```python
class FuzzySystemBridge:
    def compute(self, inputs: dict[str, float]) -> FuzzyResultDTO: ...
    def get_membership_functions(self, var: str) -> list[dict]: ...
    def get_rules(self) -> list[dict]: ...
    def build_default_seismic_system(self) -> None: ...
```

---

## 5. Reglas del Pipeline

### 5.1. Flujo de datos estricto

```
crisp inputs
    │
    ▼
[1. FUZZIFIER]
    │  → map<var_name, vector<double>>   (grados por término)
    ▼
[2. INFERENCE ENGINE]
    │  → vector<double>                  (α_i por regla)
    ▼
[3. AGGREGATOR]
    │  → Eigen::VectorXd (N=1000 puntos) (μ_agregado)
    ▼
[4. DEFUZZIFIER]
    │  → double                          (y* centroide)
    ▼
FuzzyResult (DTO)
```

### 5.2. Reglas inmutables del pipeline

1. **Unidireccionalidad:** los datos fluyen en una sola dirección, sin retroalimentación.
2. **Inmutabilidad de entradas:** cada etapa recibe datos y produce datos nuevos; no modifica los de entrada.
3. **Completitud:** todas las etapas se ejecutan en cada llamada a `compute()`. No hay etapas opcionales.
4. **Determinismo:** misma entrada → misma salida. No hay aleatoriedad.
5. **Discretización fija:** el universo de salida se discretiza en N=1000 puntos (configurable, pero constante durante una ejecución).

### 5.3. Reglas de validación

- Las entradas deben estar dentro de sus universos de discurso.
- Toda variable de entrada debe tener al menos un término definido.
- Toda regla debe tener al menos un antecedente y exactamente un consecuente.
- El peso de una regla debe estar en [0, 1].

---

## 6. Decisiones de Diseño (ADR-style)

### ADR-001: Usar Mamdani en lugar de Sugeno
- **Contexto:** la rúbrica pide comparar ambos; el problema es de inferencia experta.
- **Decisión:** implementar Mamdani.
- **Consecuencia:** se obtiene un conjunto agregado graficable; mayor costo computacional (aceptable).
- **Referencia:** Ross (2010), Cap. 5.

### ADR-002: T-norma mínimo por defecto
- **Contexto:** existen múltiples T-normas.
- **Decisión:** mínimo (Zadeh) como default; producto y Lukasiewicz como alternativas configurables.
- **Consecuencia:** comportamiento estándar y defendible.
- **Referencia:** Klir & Yuan (1995), Cap. 3.

### ADR-003: Desfusificación por centroide
- **Contexto:** existen múltiples métodos (centroide, MOM, SOM, BIS).
- **Decisión:** centroide como default.
- **Consecuencia:** método más usado; calculable vectorialmente con Eigen.
- **Referencia:** Ross (2010), Cap. 6.

### ADR-004: Polimorfismo en MembershipFunction
- **Contexto:** se necesitan múltiples formas de MF.
- **Decisión:** clase abstracta + subclases concretas (patrón Strategy).
- **Consecuencia:** cumple OCP; permite análisis de sensibilidad real.
- **Referencia:** Gamma et al. (1994).

### ADR-005: pybind11 sobre subprocess/JSON
- **Contexto:** comunicación C++ ↔ Python.
- **Decisión:** pybind11 (FFI nativo en memoria).
- **Consecuencia:** sin overhead de serialización; tipado fuerte.

### ADR-006: Eigen para agregación y centroide
- **Contexto:** operaciones vectorizadas sobre universos discretizados.
- **Decisión:** `Eigen::VectorXd` para μ_agregado y cálculo del centroide.
- **Consecuencia:** rendimiento y elegancia matemática.

---

## 7. Convenciones de Código

### 7.1. C++
- Estándar: **C++17**.
- Estilo: **Google C++ Style Guide** (adaptado).
- Nombres de clases: `PascalCase`.
- Nombres de métodos: `camelCase`.
- Nombres de variables privadas: `camelCase_` (con guion bajo final).
- Punteros inteligentes: `std::shared_ptr` para MFs; `std::unique_ptr` donde aplique.
- Documentación: **Doxygen** en todas las clases públicas.

### 7.2. Python
- Versión: **Python 3.10+**.
- Estilo: **PEP 8**.
- Tipado: **type hints** en todas las funciones públicas.
- Documentación: **docstrings** estilo Google.
- Formateo: **black** + **isort**.
- Linting: **ruff**.

### 7.3. Build
- Sistema de build C++: **CMake 3.20+**, C++17.
- Eigen3 se localiza como paquete CMake mediante `find_package(Eigen3 CONFIG REQUIRED)`; en la configuración Windows validada se proporciona mediante vcpkg.
- pybind11 y GoogleTest se obtienen con **FetchContent**.
- El target `fuzzy_core_lib` enlaza `Eigen3::Eigen`; el módulo `fuzzy_core` y el ejecutable de pruebas enlazan esa biblioteca.
- El target pybind11 copia el módulo compilado a `workspace/python/fuzzy_ui`.
- No hay actualmente `requirements.txt` ni `pyproject.toml`; las dependencias del frontend deben instalarse en el entorno Python activo.

---

## 8. Estructura de Directorios

```
4-LogDfuzzy-Sismic/
├── README.md
├── CMakeLists.txt
├── SSD/
|    ├── ARCH_DESIGN.md
|    └── SPECS/
|           ├── spec-01.md
|           └──spec-02.md
|
├── cpp/
│   ├── include/
│   │   └── fuzzy_core/
│   │       ├── membership_function.hpp
│   │       ├── triangular_mf.hpp
│   │       ├── trapezoidal_mf.hpp
│   │       ├── linguistic_variable.hpp
│   │       ├── fuzzy_rule.hpp
│   │       ├── fuzzifier.hpp
│   │       ├── inference_engine.hpp
│   │       ├── aggregator.hpp
│   │       ├── defuzzifier.hpp
│   │       ├── fuzzy_system.hpp
│   │       └── fuzzy_result.hpp
│   ├── src/
│   │   └── CMakeLists.txt + implementaciones .cpp
│   ├── bindings/
│   │   ├── CMakeLists.txt
│   │   └── pybind_module.cpp
│   └── tests/
│       ├── CMakeLists.txt
│       └── test_fuzzy_system.cpp (GoogleTest)
│
└── workspace/
    └── python/
        └── fuzzy_ui/
            ├── __init__.py
            ├── main.py
            ├── cli.py
            ├── bridge.py
            ├── plotter.py
            ├── exporter.py
            ├── config.py
            └── fuzzy_core.<abi>.pyd (generado por el build en Windows)
```

---

## 9. Criterios de Aceptación

El sistema solo se considera **completo** cuando se cumplan todos estos criterios. Estado verificado en la implementación actual:

- [x] El bridge construye el sistema predeterminado y calcula intensidad para entradas M, h, d. La CLI recoge esos valores por consola.
- [ ] Las cuatro visualizaciones son funcionales. Las MF ahora se evalúan desde el bridge; la superficie de decisión aún oculta excepciones durante sus cálculos.
- [ ] Al cerrar la gráfica, vuelve al REPL sin perder estado.
- [ ] Permite comparar dos configuraciones de sensibilidad. El comando actual solo muestra instrucciones.
- [x] Exporta el historial a CSV mediante pandas.
- [ ] Hay tests unitarios para cada etapa del pipeline. Los cuatro tests de integración y tres casos para MF trapezoidales están registrados; los siete pasan en el entorno validado, pero falta cobertura aislada de cada etapa del pipeline.
- [ ] Doxygen y docstrings cubren el 100% de la API pública.
- [x] El build CMake configura y compila en el entorno Windows validado con Eigen3 disponible; CTest pasa los siete casos.

### Limitaciones conocidas

El comando de sensibilidad aún no compara configuraciones y la superficie de decisión del plotter captura excepciones de forma amplia. La cobertura de pruebas por etapas y la documentación exhaustiva de la API pública también siguen pendientes.

---

## 10. Referencias que Sostienen el Diseño

1. **Zadeh, L. A.** (1965). *Fuzzy Sets*. Information and Control, 8(3), 338–353.
2. **Mamdani, E. H., & Assilian, S.** (1975). *An experiment in linguistic synthesis with a fuzzy logic controller*. IJMSC, 7(1), 1–13.
3. **Klir, G. J., & Yuan, B.** (1995). *Fuzzy Sets and Fuzzy Logic*. Prentice Hall.
4. **Ross, T. J.** (2010). *Fuzzy Logic with Engineering Applications*, 3ª ed. Wiley.
5. **Jang, J.-S. R., Sun, C.-T., & Mizutani, E.** (1997). *Neuro-Fuzzy and Soft Computing*. Prentice Hall.
6. **Gamma, E., Helm, R., Johnson, R., & Vlissides, J.** (1994). *Design Patterns*. Addison-Wesley.
