# SPEC-002 — Script Python: Visualización del AGS

> Especificación del script Python encargado de generar las visualizaciones del Algoritmo Genético Simple.
>
> **Estado:** Especificación para implementación.

---

## 1. Propósito

Este documento especifica el script Python `visualizar_ag.py` encargado de:

1. Leer los CSV de métricas exportados por el programa C++.
2. Generar gráficos de convergencia: mejor fitness y fitness promedio vs. generación.
3. Comparar el impacto de $P_c$ y $P_m$ en la convergencia.
4. Exportar las figuras como PNG de alta calidad para el analisis de resultados.

---

## 2. requirements.txt

```
numpy>=1.24
matplotlib>=3.7
pandas>=2.0
```

> YA INSTALADOS 

## 3. Ubicación y estructura

- **Archivo:** `python/visualizar_ag.py`
- **Tipo:** Script Python 3.x
- **Líneas estimadas:** ~150
- **Dependencias:** `matplotlib`, `numpy`, `pandas`

---

## 4. Flujo de trabajo

```
1. Lectura de los 6 CSV desde resultados/
   - experimento_base.csv
   - experimento_pc06.csv
   - experimento_pc04.csv
   - experimento_pm001.csv
   - experimento_pm01.csv
   - experimento_real.csv

2. Generación de figura 1: Convergencia base (mejor + promedio)
3. Generación de figura 2: Comparativa variación Pc (0.8 vs 0.6 vs 0.4)
4. Generación de figura 3: Comparativa variación Pm (0.002 vs 0.01 vs 0.1)
5. Generación de figura 4: Convergencia modo real (13 bits, precisión 2 decimales)
6. Guardado de figuras en resultados/graficos/
7. Mensaje de confirmación
```

---

## 5. Argumentos de línea de comandos

| Argumento | Descripción | Valor por defecto |
|-----------|-------------|-------------------|
| `--input-dir` | Directorio con los CSV de métricas | `resultados/` |
| `--output-dir` | Directorio de salida de los PNG | `resultados/graficos/` |
| `--formato` | Formato de salida (png, pdf, svg) | `png` |

Uso:
```bash
# Desde la raíz del proyecto
python python/visualizar_ag.py

# Con rutas personalizadas
python python/visualizar_ag.py --input-dir ../resultados/ --output-dir ../resultados/graficos/
```

---

## 6. Funciones principales

### 6.1 `leer_metricas(ruta) -> pd.DataFrame`

- **Entrada:** ruta al archivo CSV de métricas.
- **Salida:** DataFrame con columnas `generacion, mejor_fitness, promedio_fitness, mejor_x`.
- **Error:** Lanza `FileNotFoundError` si el archivo no existe.

### 6.2 `graficar_convergencia(df, titulo, ruta_salida, y_max=None)`

**Características implementadas:**
- Backend `Agg` para entornos sin display.
- Tamaño: 10 × 6 pulgadas, DPI 150.
- Eje X: generación (0–100).
- Eje Y: fitness (mejor y promedio).
- Líneas:
  - Azul sólido, línea gruesa (`linewidth=2`): mejor fitness.
  - Rojo punteado (`linestyle='--'`): fitness promedio.
- Grid: sí, linestyle `--`, alpha 0.3.
- Leyenda: ubicación `best`, con etiquetas "Mejor individuo" y "Promedio población".
- Línea horizontal: fitness óptimo teórico $3969$ (gris, `linestyle='-.'`, alpha 0.5).
- Etiqueta del óptimo: "Óptimo teórico ($63^2$ = 3969)".

### 6.3 `graficar_comparativa(dfs, etiquetas, titulo, ruta_salida, modo='mejor', colores=None)`

**Características implementadas:**
- `modo='mejor'`: compara solo la curva del mejor individuo.
- `modo='promedio'`: compara solo la curva del fitness promedio.
- Colores: `tab10` por defecto, o lista personalizada.
- Grid: sí, linestyle `--`, alpha 0.3.
- Leyenda: título descriptivo con formato "Experimento X (Pc=0.8, Pm=0.002)".
- Línea horizontal: fitness óptimo teórico $3969$.

---

## 7. Figuras a generar

| Figura | Archivo | Contenido | Datos usados |
|--------|---------|-----------|--------------|
| 1 | `convergencia_base.png` | Experimento base: mejor + promedio vs generación | `experimento_base.csv` |
| 2 | `comparativa_pc.png` | Comparativa Pc = 0.8, 0.6, 0.4 (Pm fijo = 0.002) | `experimento_base.csv`, `experimento_pc06.csv`, `experimento_pc04.csv` |
| 3 | `comparativa_pm.png` | Comparativa Pm = 0.002, 0.01, 0.1 (Pc fijo = 0.8) | `experimento_base.csv`, `experimento_pm001.csv`, `experimento_pm01.csv` |
| 4 | `convergencia_real.png` | Modo real (13 bits, n=50): mejor + promedio | `experimento_real.csv` |

---

## 8. Formato de salida

- **Archivos:** `resultados/graficos/*.png`
- **Backend:** `Agg`
- **Formato:** PNG con fondo blanco
- **Bbox:** `tight`
- **DPI:** 150

---

## 9. Integración con el sistema C++

El script puede invocarse desde C++ con:

```cpp
// En main.cpp (opcional)
// system("python python/visualizar_ag.py");
```

O ejecutarse manualmente después de que el C++ genere los CSV:

```bash
# Desde la raíz del proyecto
python python/visualizar_ag.py
```

---

## 10. Ejemplo de uso

```bash
# Instalar dependencias
pip install -r python/requirements.txt

# Ejecutar visualización
python python/visualizar_ag.py

# Con parámetros personalizados
python python/visualizar_ag.py --input-dir ../resultados/ --output-dir ../resultados/graficos/ --formato pdf
```

---

## 11. Notas de implementación

- Se usa matplotlib backend `"Agg"` para entornos sin display (servidores, CI/CD).
- Los colores usan el colormap `tab10` para visualización científica estándar.
- Las curvas del fitness promedio pueden ser más ruidosas que las del mejor individuo; se recomienda suavizado opcional con `scipy.ndimage.gaussian_filter1d` si se instala scipy.
- Creación automática del directorio de salida si no existe (`os.makedirs`).
- Manejo de errores básico para archivos no encontrados.

---

## 12. Criterios de aceptación del gráfico

| Criterio | Cómo se verifica |
|----------|-----------------|
| **Archivos generados** | 4 PNG existen y tienen tamaño > 0 bytes |
| **Dimensiones** | 10 × 6 pulgadas, 150 DPI |
| **Curvas visibles** | Mejor fitness siempre ≥ promedio en cada generación |
| **Convergencia** | Curva del mejor tiende a 3969 en ≤100 generaciones |
| **Comparativas** | Diferencias visibles entre curvas de Pc/Pm distintos |
| **Leyendas** | Identifican correctamente cada curva y experimento |
| **Grid activo** | Líneas de referencia visibles, no intrusivas |
| **Óptimo teórico** | Línea horizontal en 3969 presente en todas las figuras |

---

## 13. Referencias

1. **Goldberg, D. E.** (1989). *Genetic Algorithms in Search, Optimization, and Machine Learning*. Addison-Wesley.
2. **Matplotlib Documentation.** *Pyplot tutorial.* https://matplotlib.org/stable/tutorials/pyplot.html
3. **Pandas Documentation.** *IO tools (text, CSV, HDF5, …).* https://pandas.pydata.org/pandas-docs/stable/user_guide/io.html
