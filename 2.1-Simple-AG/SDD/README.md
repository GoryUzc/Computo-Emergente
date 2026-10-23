# Algoritmo Genético Simple (AGS)

Proyecto para maximizar $F(x) = x^2$ mediante algoritmos genéticos en C++ con visualización Python.

## Stack
| Capa | Tecnología | Versión |
|------|-----------|---------|
| Cálculo | C++17 | ISO/IEC 14882:2017 |
| Visualización | Python 3.10+ | matplotlib, pandas |
| Build | CMake 3.16+, g++ 11+ | - |

## Estructura
```
SDD/
├── ARCH-DESINGH.md   # Arquitectura y diseño teórico
├── SPECS/
│   ├── SPEC-001.md    # Especificación C++
│   └── SPEC-002.md    # Especificación Python
├── PLANS/
│   └── PLAN_AGS.md    # Plan de implementación
src/
├── main.cpp           # 6 experimentos definidos
├── ag_core.hpp/cpp    # Población, fitness, inicialización
├── operadores.hpp/cpp # Ruleta, cruce, mutación
├── utils.hpp/cpp      # Conversión binario-fenotipo
└── exportador.hpp/cpp # CSV de métricas
python/
├── visualizar_ag.py   # Gráficas de convergencia
└── requirements.txt
resultados/            # CSV y gráficos generados
```

## Build y ejecución
```bash
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
./ags_maximizador
python ../python/visualizar_ag.py
```

## Experimentos
1. `experimento_base` — n=20, L=6, Pc=0.8, Pm=0.002
2. `experimento_pc06` — Pc=0.6
3. `experimento_pc04` — Pc=0.4  
4. `experimento_pm001` — Pm=0.01
5. `experimento_pm01` — Pm=0.1
6. `experimento_real` — n=50, L=13, Pc=0.8, Pm=0.002, precisión 2 decimales

## Referencias
- Goldberg, D. E. (1989). *Genetic Algorithms in Search, Optimization, and Machine Learning*
- Michalewicz, Z. (1996). *Genetic Algorithms + Data Structures = Evolution Programs*