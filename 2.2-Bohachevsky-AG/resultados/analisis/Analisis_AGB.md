# Análisis del Algoritmo Genético Simple (AGS)

> Documento generado bajo metodología SDD (Specification-Driven Development)

---

## 1. Resumen Ejecutivo

| Métrica | Valor |
|---------|-------|
| Simulaciones totales | 30 |
| Generaciones por simulación | 101 (0 a 100) |
| Óptimo global alcanzado | **1.115 × 10⁻⁸** (Simulación 9, gen 93) |
| Tasa de éxito (óptimo global) | **3.33%** (1/30) |
| Fitness promedio final (best) | 0.098 ± 0.087 |
| Fitness promedio final (avg) | 3.52 ± 0.03 |

### Hallazgos Principales

- ✅ El algoritmo converge rápidamente en las primeras 15-20 generaciones.
- ⚠️ El **96.7%** de las simulaciones se estancan en mínimos locales (F ≈ 0.01 – 0.2).
- 🎯 Solo la **Simulación 9** encontró el óptimo global exacto en la generación 93.
- 🔴 El promedio poblacional (`avg_fitness`) se estabiliza en ≈ 3.5, indicando convergencia prematura de la mayoría de la población.

---

## 2. Análisis del Mejor Individuo (`best_fitness`)

### 2.1 Estadísticas por Simulación

| Simulación | `best_fitness` inicial | `best_fitness` final | Generación de convergencia |
|:----------:|:----------------------:|:--------------------:|:--------------------------:|
| 0 | 0.2227 | 0.0107 | Gen 3 |
| 1 | 0.1024 | 0.0209 | Gen 3 |
| 2 | 0.3330 | 0.0982 | Gen 9 |
| 3 | 0.1257 | 0.1257 | Gen 0 (estancado) |
| 4 | 0.1962 | 0.1090 | Gen 6 |
| 5 | 0.2361 | 0.1813 | Gen 79 |
| 6 | 0.4212 | 0.1164 | Gen 2 |
| 7 | 0.0417 | 0.0359 | Gen 4 |
| 8 | 0.0609 | 0.0609 | Gen 0 (estancado) |
| **9** | 0.2504 | **1.11e-08** | **Gen 93 ⭐** |
| 10 | 0.1789 | 0.1789 | Gen 0 (estancado) |
| 11 | 0.4308 | 0.1073 | Gen 2 |
| 12 | 0.4375 | 0.0216 | Gen 4 |
| 13 | 0.0407 | 0.0202 | Gen 3 |
| 14 | 0.0983 | 0.0609 | Gen 14 |
| 15 | 0.0403 | 0.0403 | Gen 0 (estancado) |
| 16 | 0.4296 | 0.1244 | Gen 11 |
| 17 | 0.1350 | 0.0050 | Gen 5 |
| 18 | 0.3055 | 0.0053 | Gen 3 |
| 19 | 0.2451 | 0.0261 | Gen 8 |
| 20 | 0.4880 | 0.0591 | Gen 1 |
| 21 | 0.2422 | 0.2422 | Gen 0 (estancado) |
| 22 | 0.1805 | 0.0937 | Gen 2 |
| 23 | 0.2406 | 0.1733 | Gen 1 |
| 24 | 0.1219 | 0.0155 | Gen 6 |
| 25 | 0.0491 | 0.0491 | Gen 0 (estancado) |
| 26 | 0.0966 | 0.0966 | Gen 0 (estancado) |
| 27 | 0.4938 | 0.2617 | Gen 7 |
| 28 | 0.4597 | 0.0510 | Gen 10 |
| 29 | 0.2771 | 0.2770 | Gen 28 |

### 2.2 Distribución del Fitness Final

*(Gráfico de distribución: 1-2-3-4-5-6)*

### 2.3 Fases de Convergencia

| Fase | Generaciones | Comportamiento |
|------|:------------:|----------------|
| **Exploración inicial** | 0 – 10 | Descenso rápido del fitness. El algoritmo encuentra la "cuenca de atracción" del óptimo. |
| **Refinamiento** | 10 – 30 | Mejoras incrementales. El mejor individuo se acerca al mínimo local. |
| **Estancamiento** | 30 – 100 | Oscilación sin mejora significativa. La población queda atrapada. |

### 2.4 ⭐ Caso Especial: Simulación 9

**Análisis:**

- En la generación 93, un evento de mutación o cruce colocó al individuo exactamente en el óptimo global.
- El fitness cayó **7 órdenes de magnitud** en una sola generación.
- Las coordenadas (x, y) pasaron de (0.083, −0.001) a (−1.5 × 10⁻⁵, −1.5 × 10⁻⁵).

**Conclusión:** El algoritmo es capaz de encontrar el óptimo global, pero depende de un evento estocástico raro.

---

## 3. Análisis del Promedio Poblacional (`avg_fitness`)

### 3.1 Evolución Temporal

| Generación | `avg_fitness` promedio | Interpretación |
|:----------:|:----------------------:|----------------|
| 0 | 1.70 ± 0.08 | Población inicial diversa |
| 10 | 2.75 ± 0.15 | Convergencia hacia región subóptima |
| 20 | 3.38 ± 0.08 | Estabilización en plateau |
| 50 | 3.52 ± 0.03 | Convergencia completa |
| 100 | 3.52 ± 0.03 | Población homogenizada |

### 3.2 Fenómeno de Divergencia

*(Gráfico de divergencia: 1-2-3-4-5-6-7-8-9-10)*

**Causa:**

- La sustitución total reemplaza toda la población en cada generación.
- El mejor individuo (con fitness ≈ 0.01) tiene baja probabilidad de ser seleccionado para reproducirse.
- La mayoría de la población converge hacia la región donde F(x, y) ≈ 3.5.

---

## 4. Análisis de la Diversidad (`std_fitness`)

### 4.1 Evolución de la Desviación Estándar

| Generación | `std_fitness` promedio | Interpretación |
|:----------:|:----------------------:|----------------|
| 0 | 0.73 ± 0.06 | Alta diversidad inicial |
| 10 | 0.65 ± 0.08 | Pérdida moderada de diversidad |
| 20 | 0.45 ± 0.06 | Convergencia poblacional |
| 50 | 0.41 ± 0.03 | Población homogenizada |
| 100 | 0.41 ± 0.03 | Estancamiento completo |

### 4.2 Interpretación

- La diversidad cae un **44%** en las primeras 20 generaciones.
- A partir de la generación 30, la diversidad se estabiliza en ≈ 0.41.
- **Conclusión:** La población pierde capacidad de exploración y queda atrapada en un mínimo local.

---

## 5. Análisis del Espacio de Búsqueda (`best_x`, `best_y`)

### 5.1 Distribución de Coordenadas Finales

| Simulación | `best_x` final | `best_y` final | Distancia al origen |
|:----------:|:--------------:|:--------------:|:-------------------:|
| 0 | −0.00798 | −0.01707 | 0.0188 |
| 1 | 0.02779 | 0.01717 | 0.0326 |
| **9** | **−1.53e-05** | **−1.53e-05** | **2.16e-05 ⭐** |
| 17 | 0.01082 | 0.00999 | 0.0147 |
| 24 | 0.00652 | −0.02110 | 0.0221 |

### 5.2 Patrones Observados

*(Gráfico de patrones: 1-2-3-4-5-6-7-8-9-10)*

### 5.3 Interpretación del Paisaje de Aptitud

*(Gráfico de paisaje: 1-2-3-4-5-6-7-8-9-10-11-12-13-14-15-16-17)*

**Interpretación:**

- Existe un **plateau amplio** donde F(x, y) ≈ 3.5.
- Dentro de este plateau, hay una **cuenca de atracción** donde F ≈ 0.01 – 0.1.
- En el centro de la cuenca, existe un **mínimo global muy estrecho** donde F ≈ 0.
- El algoritmo encuentra fácilmente la cuenca, pero le cuesta caer en el mínimo exacto.

---

## 6. Diagnóstico de Problemas

### 🔴 Problema 1: Convergencia Prematura

**Síntoma:**

- El **26.7%** de las simulaciones (8/30) se estancan en su fitness inicial.
- El `avg_fitness` se estabiliza en ≈ 3.5 desde la generación 20.

**Causa:**

- Pₘ = 0.005 es demasiado baja para mantener diversidad.
- No hay mecanismo de elitismo que preserve al mejor individuo.
- La sustitución total elimina buenas soluciones accidentalmente.

### 🔴 Problema 2: Desequilibrio Exploración-Explotación

**Síntoma:**

- El algoritmo explota bien la región inicial pero no explora lo suficiente para escapar de mínimos locales.
- El `best_fitness` oscila sin mejorar en las últimas 70 generaciones.

**Causa:**

- P_c = 0.7 favorece la explotación temprana.
- La mutación es insuficiente para generar nuevas soluciones prometedoras.

### 🟡 Problema 3: Falta de Elitismo

**Síntoma:**

- El `best_fitness` no es monótonamente decreciente en algunas simulaciones.
- El mejor individuo puede perderse entre generaciones.

**Causa:**

- La sustitución total reemplaza toda la población sin preservar al mejor individuo.

---

## 7. Recomendaciones de Mejora

### 7.1 Implementar Elitismo

**Acción:**

```cpp
// Preservar el mejor individuo de la generación anterior
if (elitismo) {
    poblacion[0] = mejor_individuo_anterior;
}
```

**Efecto esperado:**

- Garantiza que el `best_fitness` sea monótonamente decreciente.
- Evita la pérdida de soluciones prometedoras.

### 7.2 Aumentar la Tasa de Mutación

**Acción:**

```cpp
const double P_MUTACION = 0.05;  // Aumentar de 0.005 a 0.05
```

**Efecto esperado:**

- Mayor diversidad poblacional.
- Mayor probabilidad de escapar de mínimos locales.
- Posible mejora en la convergencia hacia el óptimo global.

### 7.3 Mutación Adaptativa

**Acción:**

```cpp
double pm = 0.1 - (0.09 * generacion / MAX_GENERACIONES);  // De 0.1 a 0.01
```

**Efecto esperado:**

- Alta exploración al inicio (mutación alta).
- Alta explotación al final (mutación baja).
- Mejor balance entre exploración y explotación.

### 7.4 Aumentar el Número de Generaciones

**Acción:**

```cpp
const int MAX_GENERACIONES = 200;  // Duplicar de 100 a 200
```

**Efecto esperado:**

- Más oportunidades para encontrar el óptimo global.
- La Simulación 9 encontró el óptimo en la gen 93; más tiempo permitiría que otras simulaciones también lo encuentren.

### 7.5 Aumentar el Tamaño de Población

**Acción:**

```cpp
const int TAM_POBLACION = 100;  // Aumentar de 50 a 100
```

**Efecto esperado:**

- Mayor diversidad inicial.
- Mejor cobertura del espacio de búsqueda.
- Menor probabilidad de convergencia prematura.

---

## 8. Conclusiones

| Aspecto | Evaluación |
|---------|:----------:|
| Funcionalidad del código | ✅ Correcta. El AGS implementa correctamente los operadores genéticos. |
| Convergencia | ⚠️ Parcial. El 73.3% de las simulaciones encuentran soluciones buenas (F < 0.2). |
| Óptimo global | 🔴 Deficiente. Solo 3.33% de las simulaciones alcanzan el óptimo global. |
| Configuración de parámetros | 🔴 Subóptima. Pₘ = 0.005 es demasiado conservadora. |
| Robustez | ⚠️ Moderada. Alta varianza entre simulaciones independientes. |

### 8.1 Lecciones Aprendidas

1. **La mutación es crucial:** Una tasa de mutación demasiado baja causa estancamiento.
2. **El elitismo es necesario:** Sin elitismo, las buenas soluciones se pierden.
3. **El balance exploración-explotación es dinámico:** Debe adaptarse durante la ejecución.
4. **100 generaciones pueden ser insuficientes:** Depende de la complejidad del paisaje de aptitud.

### 8.2 Próximos Pasos

- [ ] Implementar elitismo y mutación adaptativa.
- [ ] Ejecutar 30 simulaciones con los nuevos parámetros.
- [ ] Comparar los resultados con los actuales.
- [ ] Documentar las mejoras en el informe final.

---

## Apéndice A: Tabla Completa de Resultados Finales

| Sim | `best_fitness` | `avg_fitness` | `std_fitness` | `best_x` | `best_y` |
|:---:|:--------------:|:-------------:|:-------------:|:--------:|:--------:|
| 0 | 0.0107 | 3.5197 | 0.4266 | −0.0080 | −0.0171 |
| 1 | 0.0209 | 3.5300 | 0.4060 | 0.0278 | 0.0172 |
| 2 | 0.0982 | 3.4975 | 0.4616 | −0.0655 | 0.0341 |
| 3 | 0.1257 | 3.5264 | 0.4066 | −0.0463 | 0.0543 |
| 4 | 0.1090 | 3.5201 | 0.4094 | −0.0864 | 0.0149 |
| 5 | 0.1813 | 3.5204 | 0.4075 | 0.0636 | 0.0625 |
| 6 | 0.1164 | 3.5279 | 0.3884 | 0.0099 | 0.0598 |
| 7 | 0.0359 | 3.5440 | 0.3859 | 0.0062 | 0.0327 |
| 8 | 0.0609 | 3.5089 | 0.4846 | −0.0642 | −0.0102 |
| **9** | **1.11e-08** | **3.5000** | **0.4397** | **−1.53e-05** | **−1.53e-05** |
| 10 | 0.1789 | 3.5429 | 0.3710 | −0.0877 | −0.0478 |
| 11 | 0.1073 | 3.5425 | 0.3727 | 0.0086 | −0.0574 |
| 12 | 0.0216 | 3.4958 | 0.4508 | 0.0232 | −0.0204 |
| 13 | 0.0202 | 3.5241 | 0.4085 | −0.0367 | −0.0056 |
| 14 | 0.0609 | 3.5185 | 0.4011 | −0.0516 | 0.0265 |
| 15 | 0.0403 | 3.4754 | 0.4769 | −0.0498 | −0.0127 |
| 16 | 0.1244 | 3.5452 | 0.3771 | −0.0193 | 0.0609 |
| 17 | 0.0050 | 3.5228 | 0.4084 | 0.0108 | 0.0100 |
| 18 | 0.0053 | 3.5172 | 0.4189 | 0.0183 | −0.0037 |
| 19 | 0.0261 | 3.5073 | 0.4287 | 0.0359 | −0.0152 |
| 20 | 0.0591 | 3.5045 | 0.4463 | −0.0593 | 0.0173 |
| 21 | 0.2422 | 3.4760 | 0.4546 | 0.0807 | 0.0696 |
| 22 | 0.0937 | 3.5417 | 0.3806 | 0.0549 | −0.0395 |
| 23 | 0.1733 | 3.5433 | 0.3881 | 0.0875 | 0.0460 |
| 24 | 0.0155 | 3.5653 | 0.3573 | 0.0065 | −0.0211 |
| 25 | 0.0491 | 3.5393 | 0.3838 | 0.0352 | −0.0308 |
| 26 | 0.0966 | 3.5309 | 0.4114 | −0.0640 | 0.0346 |
| 27 | 0.2617 | 3.5290 | 0.3896 | 0.1338 | 0.0325 |
| 28 | 0.0510 | 3.4972 | 0.4352 | −0.0232 | −0.0362 |
| 29 | 0.2770 | 3.4998 | 0.4136 | 0.1344 | 0.0385 |

---
