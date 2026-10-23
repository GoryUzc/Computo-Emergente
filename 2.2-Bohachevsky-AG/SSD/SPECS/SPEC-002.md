# SPEC-002: Script Python de Visualización y Análisis

---

## 1. Alcance

Este documento especifica los scripts Python para leer los resultados del AG (CSV generado por C++). 

**Dependencia:** ARCH_DESIGN.md 

---

## 2. Dependencias

| Paquete | Versión | Uso |
|---------|---------|-----|
| `pandas` | 1.5+ | Lectura y manipulación de CSV |
| `numpy` | 1.24+ | Cálculos estadísticos, promedios |
| `matplotlib` | 3.7+ | Gráficas de líneas, barras, heatmaps |
| `seaborn` | 0.12+ | Heatmaps estilizados (opcional, puede usarse matplotlib puro) |

**Nota:** Ya esta todo todo instalado. No se incluye `requirements.txt` ni instrucciones de instalación.

---

## 3. Módulos

### 3.1 visualizar_ag.py

**Propósito:** Generar gráficas de convergencia para partes (a) y (b).

#### Funciones

| Función | Firma | Descripción |
|---------|-------|-------------|
| `cargar_datos` | `df = cargar_datos(filepath)` | Lee CSV con pandas, retorna DataFrame |
| `promediar_simulaciones` | `df_prom = promediar_simulaciones(df)` | Agrupa por (generation, ag_type, pc, pm), calcula mean y std |
| `graficar_convergencia_simple` | `graficar_convergencia_simple(df, output_path)` | Parte (a): 2 líneas (mejor + promedio) con bandas de confianza |
| `graficar_comparativa_ag` | `graficar_comparativa_ag(df, output_path)` | Parte (b): 3 líneas (simple vs linear vs window) |
| `main` | `main()` | Orquesta lectura, procesamiento y generación de gráficas |

#### Algoritmo cargar_datos
```
1. df = pd.read_csv(filepath)
2. Verificar columnas: simulation, generation, ag_type, pc, pm, 
   best_fitness, avg_fitness, std_fitness, best_x, best_y
3. Retornar df
```

#### Algoritmo promediar_simulaciones
```
1. Agrupar por ['generation', 'ag_type', 'pc', 'pm']
2. Calcular:
   - best_fitness_mean = mean(best_fitness)
   - best_fitness_std = std(best_fitness)
   - avg_fitness_mean = mean(avg_fitness)
   - avg_fitness_std = std(avg_fitness)
3. Retornar DataFrame agrupado
```

#### Algoritmo graficar_convergencia_simple (Parte a)
```
1. Filtrar df donde ag_type == 'simple', pc == 0.7, pm == 0.005
2. df_prom = promediar_simulaciones(df_filtrado)
3. Crear figura (10, 6)
4. Línea 1: generation vs best_fitness_mean
   - Color: 'blue', label: 'Mejor individuo (promedio 30 sims)'
   - Banda: ±best_fitness_std, alpha=0.2
5. Línea 2: generation vs avg_fitness_mean
   - Color: 'orange', label: 'Promedio población (promedio 30 sims)'
   - Banda: ±avg_fitness_std, alpha=0.2
6. Título: 'AG Simple - Convergencia Bohachevsky (30 simulaciones)'
7. Eje X: 'Generación', Eje Y: 'Fitness (menor = mejor)'
8. Leyenda, grid, ylim ajustado
9. Guardar en output_path como PNG, 300 DPI
```

#### Algoritmo graficar_comparativa_ag (Parte b)
```
1. Para cada ag_type en ['simple', 'linear', 'window']:
   a. Filtrar df donde ag_type == tipo, pc == 0.7, pm == 0.005
   b. df_prom = promediar_simulaciones(df_filtrado)
   c. Graficar generation vs best_fitness_mean
2. Colores: simple='blue', linear='green', window='red'
3. Título: 'Comparativa AG: Simple vs Linear vs Window'
4. Eje X: 'Generación', Eje Y: 'Mejor Fitness (promedio 30 sims)'
5. Leyenda, grid
6. Guardar en output_path como PNG, 300 DPI
```

### 3.2 heatmap_params.py

**Propósito:** Generar heatmap de variación de parámetros PC × PM (parte c).

#### Funciones

| Función | Firma | Descripción |
|---------|-------|-------------|
| `cargar_datos_grid` | `df = cargar_datos_grid(filepath)` | Lee CSV, filtra datos de grid search |
| `calcular_fitness_final` | `df_final = calcular_fitness_final(df)` | Para cada (pc, pm, sim), toma fitness de última generación |
| `promediar_fitness_final` | `df_prom = promediar_fitness_final(df_final)` | Promedia fitness final sobre 30 sims por (pc, pm) |
| `graficar_heatmap` | `graficar_heatmap(df_prom, output_path)` | Heatmap 4×3 de PC × PM |
| `main` | `main()` | Orquesta lectura, procesamiento y generación |

#### Algoritmo calcular_fitness_final
```
1. Para cada combinación (pc, pm, simulation):
   a. Filtrar filas con esa combinación
   b. Tomar la fila con generation == max(generation)
   c. Extraer best_fitness
2. Retornar DataFrame con columnas: pc, pm, simulation, best_fitness_final
```

#### Algoritmo graficar_heatmap
```
1. Crear matriz 4×3 (PC × PM)
2. Rellenar con best_fitness_final_mean para cada celda
3. Usar plt.imshow() o seaborn.heatmap()
4. Colormap: 'viridis_r' (invertido, menor fitness = mejor = más brillante)
5. Anotar valores en cada celda con 4 decimales
6. Título: 'Fitness Final Promedio (30 sims) - PC × PM'
7. Eje X: 'PM', Eje Y: 'PC'
8. Colorbar label: 'Fitness (menor = mejor)'
9. Guardar en output_path como PNG, 300 DPI
```

---

## 4. Formatos de Salida

### 4.1 Archivos PNG Generados

| Archivo | Script | Contenido |
|---------|--------|-----------|
| `convergencia_simple.png` | visualizar_ag.py | Parte (a): 2 líneas con bandas de confianza |
| `comparativa_ag.png` | visualizar_ag.py | Parte (b): 3 líneas comparativas |
| `heatmap_pc_pm.png` | heatmap_params.py | Parte (c): Matriz de calor PC × PM |

### 4.2 Especificaciones Visuales

- **Resolución:** 300 DPI (calidad de publicación)
- **Tamaño figura:** 10×6 pulgadas (por defecto)
- **Formato:** PNG
- **Estilo:** `plt.style.use('seaborn-v0_8-whitegrid')` o similar
- **Colores:**
  - Simple: `#1f77b4` (azul)
  - Linear: `#2ca02c` (verde)
  - Window: `#d62728` (rojo)
- **Fuentes:** Tamaño 12 para títulos, 10 para ejes

---

## 5. Criterios de Aceptación

| Criterio | Verificación | Estado |
|----------|-------------|--------|
| Lectura CSV correcta | Sin errores de parsing, columnas correctas | [ ] |
| Promedio 30 sims | Datos agrupados correctamente por generación | [ ] |
| Banda de confianza | ±std visible en gráfica de convergencia | [ ] |
| 3 líneas comparativa | Simple, Linear, Window distinguibles | [ ] |
| Heatmap 4×3 | Dimensiones correctas, valores anotados | [ ] |
| Colorbar invertido | Menor fitness = color más brillante | [ ] |
| 300 DPI | Verificar resolución del PNG generado | [ ] |
| Sin warnings matplotlib | Ejecución limpia | [ ] |

---

## 6. Ejemplo de Uso

```bash
# Después de ejecutar C++ y generar resultados/resultados.csv
cd resultados
python ../python/visualizar_ag.py
python ../python/heatmap_params.py
```

---

## 7. Notas de Implementación

1. **Manejo de rutas:** Usar `pathlib.Path` para compatibilidad multiplataforma.
2. **Semillas:** No se requiere fijar semilla en Python (solo visualización determinística).
3. **Memoria:** DataFrames de ~30,000 filas son manejables sin optimización especial.
4. **Error handling:** Si el CSV no existe, mostrar mensaje claro y salir con código 1.
