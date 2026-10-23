# Algoritmo Genético para TSP (Problema del Viajero de Comercio)

Implementación en C++17 de un Algoritmo Genético para resolver el TSP con 10 ciudades.

## Características

- **10 ciudades** con ubicaciones aleatorias en un plano [0, 100] × [0, 100]
- **Codificación por permutación** - cada individuo representa una ruta válida
- **Cruce OX (Order Crossover)** - operador especializado para permutaciones
- **Mutación Swap** - intercambio de dos ciudades
- **Selección por Ruleta** - proporcional al fitness
- **Elitismo Top 2** - preserva los 2 mejores individuos
- **30 simulaciones independientes** con semillas diferentes

## Estructura del Proyecto

```
2.3-TSP-AG/
├── CMakeList.txt        # Build system (CMake 3.16+)
├── README.md            # Este archivo
├── Docs/              # Informe de resultados
├── SDD/               # Specification-Driven Development
│   └── SPECS/
│       ├── spec-001.md # Implementación C++
│       └── spec-002.md # Visualización Python
├── src/
│   ├── main.cpp
│   ├── tsp_core.hpp/cpp
│   ├── ag_core.hpp/cpp
│   ├── operadores.hpp/cpp
│   └── exportador.hpp/cpp
├── resultados/
│   ├── ciudades.csv
│   ├── mejor_ruta.csv
│   ├── historial_sim0.csv
│   ├── simulaciones.csv
│   └── graficos/
│       ├── 01_ciudades.png
│       ├── 02_mejor_ruta.png
│       ├── 03_convergencia.png
│       └── 04_30_simulaciones.png
└── python/
    ├── visualizar_tsp.py
    └── requirements.txt
```

## Requisitos

- **CMake** 3.16+
- **Compilador C++** con soporte C++17 (MSVC 2022, GCC 11+, Clang 14+)
- **Python** 3.10+ con numpy, pandas, matplotlib

## Compilación y Ejecución

### Windows (MSVC)
```powershell
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
.\Release\tsp_ag.exe
```

### Linux/macOS (GCC/Clang)
```bash
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
./tsp_ag
```

## Generar Visualizaciones

```bash
pip install -r python/requirements.txt
cd python
python visualizar_tsp.py
```

## Parámetros Configurables

| Parámetro | Valor | Descripción |
|-----------|-------|-------------|
| N_CIUDADES | 10 | Número de ciudades |
| TAM_POBLACION | 100 | Individuos por generación |
| MAX_GENERACIONES | 200 | Iteraciones del AG |
| PROB_CRUCE | 0.9 | Probabilidad de cruce |
| PROB_MUTACION | 0.02 | Probabilidad de mutación |
| NUM_SIMULACIONES | 30 | Ejecuciones independientes |

## Salida Esperada

El programa genera archivos CSV en `resultados/`:
- `ciudades.csv`: Coordenadas de las 10 ciudades
- `mejor_ruta.csv`: La mejor ruta encontrada
- `historial_sim0.csv`: Evolución de la simulación 0
- `simulaciones.csv`: Todos los datos de las 30 simulaciones

El script Python genera las gráficas en `resultados/graficos/`.

## Algoritmo

El flujo del algoritmo genético:

1. INICIALIZACIÓN: Generar ciudades y población aleatoria
2. EVALUACIÓN: Calcular distancia y fitness
3. CICLO PRINCIPAL:
   - Selección por ruleta
   - Cruce OX (si random < Pc)
   - Mutación Swap (si random < Pm)
   - Elitismo Top 2
4. EXPORTACIÓN: Guardar resultados a CSV
5. VISUALIZACIÓN: Generar gráficas con Python

## Referencias

- Goldberg, D. E. (1989). Genetic Algorithms in Search, Optimization, and Machine Learning
- Holland, J. H. (1975). Adaptation in Natural and Artificial Systems
- Mitchell, M. (1996). An Introduction to Genetic Algorithms