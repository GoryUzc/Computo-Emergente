# Algoritmo Genético Simple (AGS)

Implementación en C++ (C++17) de un algoritmo genético para maximizar la función $F(x)=x^2$, con visualización de resultados en Python.

## Estructura del proyecto
```
2.1-SIMPLE-AG/
├─ CMakeLists.txt          # Configuración de construcción
├─ README.md               # Este archivo
├─ docs/                   # Documentación LaTeX
├─ SDD/                    # Especificaciones y diseño
│   ├─ ARCH-DESINGH.md     # Arquitectura y diseño
│   ├─ PLANS/PLAN_AGS.md   # Plan de implementación
│   └─ SPECS/
│       ├─ SPEC-001.md    # Especificación C++
│       └─ SPEC-002.md    # Especificación Python
├─ src/                    # Código fuente C++
│   ├─ main.cpp
│   ├─ ag_core.hpp/.cpp
│   ├─ operadores.hpp/.cpp
│   ├─ utils.hpp/.cpp
│   └─ exportador.hpp/.cpp
├─ python/                 # Script de visualización
│   ├─ visualizar_ag.py
│   └─ requirements.txt
└─ resultados/             # CSV y gráficos generados
```

## Compilación y ejecución
```bash
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
./ags_maximizador        # genera los CSV en ../resultados/
cd ..
python python/visualizar_ag.py   # genera los PNG en resultados/graficos/
```

## Experimentos disponibles
1. `experimento_base` – n=20, L=6, Pc=0.8, Pm=0.002
2. `experimento_pc06` – Pc=0.6
3. `experimento_pc04` – Pc=0.4
4. `experimento_pm001` – Pm=0.01
5. `experimento_pm01` – Pm=0.1
6. `experimento_real` – n=50, L=13, precisión 2 decimales

## Referencias
- Goldberg, D. E. *Genetic Algorithms in Search, Optimization, and Machine Learning* (1989)
- Michalewicz, Z. *Genetic Algorithms + Data Structures = Evolution Programs* (1996)