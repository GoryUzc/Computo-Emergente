# AG-Bohachevsky: Algoritmo Genético para Optimización

> Implementación de un algoritmo genético (AG) para encontrar el mínimo global de la función de fitness **F(x, y)**.

---

## 📌 Resumen

- **Objetivo:** Minimizar la función de aptitud F(x, y) mediante un algoritmo genético simple.
- **Resultados:** 30 simulaciones, 101 generaciones máximo. Solo el **3.33 %** de las simulaciones alcanzó el óptimo global (≈ 1.11 × 10⁻⁸).
- **Análisis:** El informe *Analisis_AGB.md* contiene estadísticas detalladas, gráficos de convergencia y diagnóstico de los problemas de estancamiento y falta de elitismo.

---

## 🚀 Inicio rápido

### Requisitos

- **CMake** ≥ 3.16
- **Compilador C++** con soporte C++17
- **Vcpkg** (opcional, para dependencias externas)

### Compilación

```bash
git clone <repo-url>
cd 2.2-Bohachevsky-AG
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Ejecución

El ejecutable `ag_bohachevsky` acepta los siguientes argumentos:

| Argumento | Descripción | Valor por defecto |
|-----------|-------------|-------------------|
| `--type` | Tipo de AG (ej. `simple`) | `simple` |
| `--pc` | Probabilidad de cruce (Pc) | `0.7` |
| `--pm` | Probabilidad de mutación (Pm) | `0.005` |
| `--sims` | Número de simulaciones | `NUM_SIMULACIONES` (30) |
| `--seed` | Semilla aleatoria | `SEED_DEFAULT` |
| `--grid` | Ejecutar en modo grid search (pc/pm varying) | `false` |

Ejemplos:

```bash
# Ejecución estándar
./ag_bohachevsky

# Con parámetros customizados
./ag_bohachevsky --pc 0.8 --pm 0.01 --sims 10 --seed 1234
```

Los resultados se guardan en `../resultados/results.csv` y se imprimen mensajes de progreso en consola.

---

## 📊 Análisis de resultados

El archivo **resultados/analisis/Analisis_AGB.md** contiene:

- Estadísticas por simulación (`best_fitness`, `avg_fitness`, `std_fitness`).
- Gráficos de evolución y distribución de fitness.
- Diagnóstico de problemas (convergencia prematura, falta de elitismo, etc.).
- Recomendaciones de mejora (elitismo, mutación adaptativa, mayor población, etc.).

---

## 🛠️ Mejores prácticas y mejoras propuestas

1. **Implementar elitismo** para evitar pérdida de la mejor solución.
2. **Incrementar la tasa de mutación** (p. ej., de 0.005 a 0.05) para mayor diversidad.
3. **Mutación adaptativa** que reduzca la probabilidad de mutación con el número de generación.
4. **Aumentar el número de generaciones** (hasta 200) y el tamaño de población (hasta 100 individuos).
5. **Persistir la población mejor** entre simulaciones para estadísticas más estables.

---

## 📂 Estructura del proyecto

```
.
├── build/                 # Carpeta de salida de CMake
├── src/                   # Código fuente C++
│   ├── main.cpp
│   ├── utils.hpp / utils.cpp
│   ├── ag_core.hpp / ag_core.cpp
│   ├── operadores.hpp / operadores.cpp
│   └── exportador.hpp / exportador.cpp
├── resultados/            # Resultados de ejecución
│   ├── results.csv
│   └── analisis/Analisis_AGB.md
├── CMakeLists.txt
└── README.md
```

---

## 🤝 Contribuciones

1. Fork el repositorio.
2. Crea una rama para tu característica o corrección.
3. Ejecuta `cmake` y `make` para asegurarte de que todo compila.
4. Abre un Pull Request describiendo los cambios.

---

## 📄 Licencia

Este proyecto está bajo la licencia MIT. Consulta el archivo `LICENSE` para más detalles.

---

## 📞 Contacto

- **Autor:** *Gregory UZcategui* – [GitHub Profile](https://github.com/GoryUzc)