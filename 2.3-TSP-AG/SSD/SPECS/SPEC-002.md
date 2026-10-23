# SPEC-002: Implementacion Python de Visualizaciones para TSP

> **Documento de Especificacion Tecnica**  

---

## 1. Proposito

Especificar la implementacion en Python de las visualizaciones requeridas. El script debe leer los archivos CSV generados por el solver C++ y producir graficas que muestren:

1. **Ubicacion de las ciudades** en el plano 2D
2. **Mejor ruta encontrada** con flechas direccionales
3. **Convergencia del algoritmo** (mejor distancia vs promedio)
4. **Analisis estadistico de las 30 simulaciones**

---

## 2. Datos de Entrada (CSV desde C++)

### 2.1 `resultados/ciudades.csv`

Coordenadas de las 10 ciudades generadas aleatoriamente.

```csv
id,x,y
0,45.23,67.89
1,12.45,89.12
2,78.90,23.45
...
9,34.56,56.78
```

### 2.2 `resultados/mejor_ruta.csv`

La mejor ruta encontrada por el AG (orden de visita de las ciudades).

```csv
orden_visita,id_ciudad,x,y
0,3,45.23,67.89
1,7,12.45,89.12
2,1,78.90,23.45
...
9,5,34.56,56.78
```

### 2.3 `resultados/historial_sim0.csv`

Evolucion de las metricas por generacion para la simulacion 0.

```csv
generacion,mejor_distancia,peor_distancia,promedio_distancia,desv_est_distancia,mejor_fitness,promedio_fitness
0,850.23,1250.45,1023.67,125.34,0.001176,0.000977
1,820.12,1230.89,1005.23,120.45,0.001219,0.000995
...
199,245.67,389.23,289.34,45.12,0.004070,0.003456
```

### 2.4 `resultados/simulaciones.csv`

Resultados consolidados de las 30 simulaciones independientes.

```csv
simulacion,generacion,mejor_distancia,promedio_distancia
0,0,850.23,1023.67
0,1,820.12,1005.23
...
0,199,245.67,289.34
1,0,870.45,1045.23
...
29,199,250.12,295.67
```

---

## 3. Modulos a Implementar

### 3.1 Script Principal: `visualizar_tsp.py`

```python
# python/visualizar_tsp.py
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import FancyArrowPatch
from pathlib import Path
import sys

class VisualizadorTSP:
    # Clase para generar las visualizaciones del Algoritmo Genetico para TSP.

    def __init__(self, resultados_dir):
        self.resultados_dir = Path(resultados_dir)
        self.graficos_dir = self.resultados_dir / 'graficos'
        self.graficos_dir.mkdir(parents=True, exist_ok=True)

        # Cargar datos
        self.df_ciudades = self._cargar_csv('ciudades.csv')
        self.df_mejor_ruta = self._cargar_csv('mejor_ruta.csv')
        self.df_historial = self._cargar_csv('historial_sim0.csv')
        self.df_simulaciones = self._cargar_csv('simulaciones.csv')

    def _cargar_csv(self, filename):
        # Carga un archivo CSV desde el directorio de resultados
        filepath = self.resultados_dir / filename
        if not filepath.exists():
            print(f"[ERROR] No se encontro: {filepath}")
            sys.exit(1)
        return pd.read_csv(filepath)

    def graficar_ciudades(self):
        # GRAFICA 1: Ubicacion de las ciudades en el plano 2D
        fig, ax = plt.subplots(figsize=(10, 8))

        # Dibujar ciudades
        scatter = ax.scatter(self.df_ciudades['x'], 
                            self.df_ciudades['y'],
                            c='red', s=200, zorder=3, 
                            edgecolors='black', linewidth=2,
                            label='Ciudades')

        # Etiquetar ciudades
        for _, row in self.df_ciudades.iterrows():
            ax.annotate(f"C{row['id']}", 
                       (row['x'], row['y']),
                       textcoords="offset points",
                       xytext=(10, 10),
                       ha='center',
                       fontsize=10,
                       fontweight='bold',
                       bbox=dict(boxstyle='round,pad=0.3', 
                                facecolor='yellow', 
                                alpha=0.7))

        ax.set_xlabel('Coordenada X', fontsize=13)
        ax.set_ylabel('Coordenada Y', fontsize=13)
        ax.set_title('Ubicacion de las Ciudades en el Plano 2D\n'
                    'Problema del Viajero de Comercio (TSP)',
                    fontsize=14, fontweight='bold')
        ax.legend(fontsize=12)
        ax.grid(True, alpha=0.3)
        ax.set_aspect('equal')

        plt.tight_layout()
        plt.savefig(self.graficos_dir / '01_ciudades.png', dpi=300, bbox_inches='tight')
        plt.close()
        print(f"[OK] Grafica 1 guardada: 01_ciudades.png")

    def graficar_mejor_ruta(self):
        # GRAFICA 2: Mejor ruta encontrada con flechas direccionales
        fig, ax = plt.subplots(figsize=(12, 10))

        # Ordenar por orden de visita
        df_ordenado = self.df_mejor_ruta.sort_values('orden_visita')

        # Dibujar ciudades
        ax.scatter(df_ordenado['x'], df_ordenado['y'],
                  c='red', s=200, zorder=3,
                  edgecolors='black', linewidth=2)

        # Dibujar ruta con flechas
        for i in range(len(df_ordenado)):
            idx_actual = i
            idx_siguiente = (i + 1) % len(df_ordenado)

            x1, y1 = df_ordenado.iloc[idx_actual]['x'], df_ordenado.iloc[idx_actual]['y']
            x2, y2 = df_ordenado.iloc[idx_siguiente]['x'], df_ordenado.iloc[idx_siguiente]['y']

            # Flecha
            arrow = FancyArrowPatch((x1, y1), (x2, y2),
                                   arrowstyle='->', 
                                   mutation_scale=20,
                                   linewidth=2,
                                   color='blue',
                                   alpha=0.7)
            ax.add_patch(arrow)

            # Numero de orden
            ax.annotate(f"{i+1}", 
                       (x1, y1),
                       textcoords="offset points",
                       xytext=(0, 15),
                       ha='center',
                       fontsize=9,
                       fontweight='bold',
                       color='blue')

        # Calcular distancia total
        distancia_total = 0
        for i in range(len(df_ordenado)):
            idx_actual = i
            idx_siguiente = (i + 1) % len(df_ordenado)
            x1, y1 = df_ordenado.iloc[idx_actual]['x'], df_ordenado.iloc[idx_actual]['y']
            x2, y2 = df_ordenado.iloc[idx_siguiente]['x'], df_ordenado.iloc[idx_siguiente]['y']
            distancia_total += np.sqrt((x2-x1)**2 + (y2-y1)**2)

        ax.set_xlabel('Coordenada X', fontsize=13)
        ax.set_ylabel('Coordenada Y', fontsize=13)
        ax.set_title(f'Mejor Ruta Encontrada por el Algoritmo Genetico\n'
                    f'Distancia Total: {distancia_total:.2f} unidades',
                    fontsize=14, fontweight='bold')
        ax.grid(True, alpha=0.3)
        ax.set_aspect('equal')

        # Texto con estadisticas
        stats_text = (f'Ciudades: {len(df_ordenado)}\n'
                     f'Distancia: {distancia_total:.2f}\n'
                     f'Ruta: {" -> ".join([str(int(row["id_ciudad"])) 
                                         for _, row in df_ordenado.iterrows()])}')
        ax.text(0.02, 0.98, stats_text,
               transform=ax.transAxes,
               fontsize=10,
               verticalalignment='top',
               bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))

        plt.tight_layout()
        plt.savefig(self.graficos_dir / '02_mejor_ruta.png', dpi=300, bbox_inches='tight')
        plt.close()
        print(f"[OK] Grafica 2 guardada: 02_mejor_ruta.png")

    def graficar_convergencia(self):
        # GRAFICA 3: Convergencia del algoritmo (mejor vs promedio)
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6))

        # Panel izquierdo: Evolucion de la mejor distancia
        ax1.plot(self.df_historial['generacion'], 
                self.df_historial['mejor_distancia'],
                'b-', linewidth=2.5, label='Mejor distancia')
        ax1.plot(self.df_historial['generacion'],
                self.df_historial['promedio_distancia'],
                'r--', linewidth=2, alpha=0.7, label='Distancia promedio')

        # Relleno entre curvas
        ax1.fill_between(self.df_historial['generacion'],
                        self.df_historial['mejor_distancia'],
                        self.df_historial['promedio_distancia'],
                        alpha=0.2, color='purple')

        ax1.set_xlabel('Generacion', fontsize=12)
        ax1.set_ylabel('Distancia', fontsize=12)
        ax1.set_title('Convergencia del Algoritmo Genetico\n'
                     'Evolucion de la Distancia',
                     fontsize=13, fontweight='bold')
        ax1.legend(fontsize=11)
        ax1.grid(True, alpha=0.3)

        # Panel derecho: Evolucion del fitness
        ax2.plot(self.df_historial['generacion'],
                self.df_historial['mejor_fitness'],
                'g-', linewidth=2.5, label='Mejor fitness')
        ax2.plot(self.df_historial['generacion'],
                self.df_historial['promedio_fitness'],
                'm--', linewidth=2, alpha=0.7, label='Fitness promedio')

        ax2.set_xlabel('Generacion', fontsize=12)
        ax2.set_ylabel('Fitness (1/distancia)', fontsize=12)
        ax2.set_title('Convergencia del Algoritmo Genetico\n'
                     'Evolucion del Fitness',
                     fontsize=13, fontweight='bold')
        ax2.legend(fontsize=11)
        ax2.grid(True, alpha=0.3)

        plt.tight_layout()
        plt.savefig(self.graficos_dir / '03_convergencia.png', dpi=300, bbox_inches='tight')
        plt.close()
        print(f"[OK] Grafica 3 guardada: 03_convergencia.png")

    def graficar_30_simulaciones(self):
        # GRAFICA 4: Analisis estadistico de las 30 simulaciones
        fig, axes = plt.subplots(2, 2, figsize=(16, 12))

        # Panel 1: Todas las simulaciones (mejor distancia)
        ax = axes[0, 0]
        for sim in self.df_simulaciones['simulacion'].unique():
            df_sim = self.df_simulaciones[self.df_simulaciones['simulacion'] == sim]
            ax.plot(df_sim['generacion'], df_sim['mejor_distancia'],
                   alpha=0.3, linewidth=1, color='blue')

        # Promedio de todas las simulaciones
        promedio_general = self.df_simulaciones.groupby('generacion')['mejor_distancia'].mean()
        ax.plot(promedio_general.index, promedio_general.values,
               'r-', linewidth=3, label='Promedio general')

        ax.set_xlabel('Generacion', fontsize=12)
        ax.set_ylabel('Mejor Distancia', fontsize=12)
        ax.set_title('30 Simulaciones Independientes\n'
                    'Evolucion de la Mejor Distancia',
                    fontsize=13, fontweight='bold')
        ax.legend(fontsize=11)
        ax.grid(True, alpha=0.3)

        # Panel 2: Boxplot del mejor fitness final
        ax = axes[0, 1]
        datos_finales = []
        labels = []
        for sim in sorted(self.df_simulaciones['simulacion'].unique()):
            df_sim = self.df_simulaciones[self.df_simulaciones['simulacion'] == sim]
            max_gen = df_sim['generacion'].max()
            final = df_sim[df_sim['generacion'] == max_gen]['mejor_distancia'].values[0]
            datos_finales.append(final)
            labels.append(f'Sim {sim}')

        bp = ax.boxplot(datos_finales, labels=labels, patch_artist=True)
        for patch in bp['boxes']:
            patch.set_facecolor('lightblue')
            patch.set_alpha(0.7)

        ax.set_ylabel('Mejor Distancia Final', fontsize=12)
        ax.set_title('Distribucion del Mejor Fitness Final\n'
                    '30 Simulaciones Independientes',
                    fontsize=13, fontweight='bold')
        ax.grid(True, alpha=0.3, axis='y')
        plt.setp(ax.xaxis.get_majorticklabels(), rotation=90)

        # Panel 3: Estadisticas comparativas
        ax = axes[1, 0]
        estadisticas = self.df_simulaciones.groupby('simulacion').agg({
            'mejor_distancia': ['mean', 'std', 'min', 'max']
        }).reset_index()

        # Aplanar columnas
        estadisticas.columns = ['simulacion', 'media', 'desv_est', 'minimo', 'maximo']

        ax.errorbar(estadisticas['simulacion'],
                   estadisticas['media'],
                   yerr=estadisticas['desv_est'],
                   fmt='o', capsize=5, capthick=2,
                   color='green', markersize=8)

        ax.set_xlabel('Simulacion', fontsize=12)
        ax.set_ylabel('Mejor Distancia (media +- desv. est.)', fontsize=12)
        ax.set_title('Estadisticas por Simulacion\n'
                    'Media y Desviacion Estandar',
                    fontsize=13, fontweight='bold')
        ax.grid(True, alpha=0.3)

        # Panel 4: Histograma de mejores distancias finales
        ax = axes[1, 1]
        max_gen = self.df_simulaciones['generacion'].max()
        finales = self.df_simulaciones[self.df_simulaciones['generacion'] == max_gen]['mejor_distancia']

        ax.hist(finales, bins=15, color='purple', alpha=0.7, edgecolor='black')
        ax.axvline(finales.mean(), color='red', linestyle='--', 
                  linewidth=2, label=f'Media: {finales.mean():.2f}')
        ax.axvline(finales.median(), color='green', linestyle='--',
                  linewidth=2, label=f'Mediana: {finales.median():.2f}')

        ax.set_xlabel('Mejor Distancia Final', fontsize=12)
        ax.set_ylabel('Frecuencia', fontsize=12)
        ax.set_title('Distribucion de Mejores Distancias Finales\n'
                    'Histograma',
                    fontsize=13, fontweight='bold')
        ax.legend(fontsize=11)
        ax.grid(True, alpha=0.3, axis='y')

        plt.tight_layout()
        plt.savefig(self.graficos_dir / '04_30_simulaciones.png', dpi=300, bbox_inches='tight')
        plt.close()
        print(f"[OK] Grafica 4 guardada: 04_30_simulaciones.png")

    def generar_reporte_estadistico(self):
        # Genera un reporte estadistico completo
        print("\n" + "="*80)
        print("REPORTE ESTADISTICO DEL ALGORITMO GENETICO PARA TSP")
        print("="*80)

        # Estadisticas de la simulacion 0
        print("\n> SIMULACION 0:")
        print(f"  Generaciones: {len(self.df_historial)}")
        print(f"  Mejor distancia inicial: {self.df_historial.iloc[0]['mejor_distancia']:.2f}")
        print(f"  Mejor distancia final: {self.df_historial.iloc[-1]['mejor_distancia']:.2f}")
        print(f"  Mejora: {((self.df_historial.iloc[0]['mejor_distancia'] - self.df_historial.iloc[-1]['mejor_distancia']) / self.df_historial.iloc[0]['mejor_distancia'] * 100):.2f}%")

        # Estadisticas de las 30 simulaciones
        print("\n> 30 SIMULACIONES INDEPENDIENTES:")
        max_gen = self.df_simulaciones['generacion'].max()
        finales = self.df_simulaciones[self.df_simulaciones['generacion'] == max_gen]['mejor_distancia']

        print(f"  Media: {finales.mean():.2f}")
        print(f"  Mediana: {finales.median():.2f}")
        print(f"  Desv. Est.: {finales.std():.2f}")
        print(f"  Minimo: {finales.min():.2f}")
        print(f"  Maximo: {finales.max():.2f}")
        print(f"  Rango: {finales.max() - finales.min():.2f}")

        # Mejor ruta
        print("\n> MEJOR RUTA ENCONTRADA:")
        df_ordenado = self.df_mejor_ruta.sort_values('orden_visita')
        ruta_str = " -> ".join([str(int(row['id_ciudad'])) for _, row in df_ordenado.iterrows()])
        print(f"  Ruta: {ruta_str}")

        distancia_total = 0
        for i in range(len(df_ordenado)):
            idx_actual = i
            idx_siguiente = (i + 1) % len(df_ordenado)
            x1, y1 = df_ordenado.iloc[idx_actual]['x'], df_ordenado.iloc[idx_actual]['y']
            x2, y2 = df_ordenado.iloc[idx_siguiente]['x'], df_ordenado.iloc[idx_siguiente]['y']
            distancia_total += np.sqrt((x2-x1)**2 + (y2-y1)**2)

        print(f"  Distancia total: {distancia_total:.2f}")

        print("\n" + "="*80)

    def generar_todas(self):
        # Ejecuta todas las visualizaciones
        print("="*80)
        print("VISUALIZACIONES - ALGORITMO GENETICO PARA TSP")
        print("="*80)

        self.graficar_ciudades()
        self.graficar_mejor_ruta()
        self.graficar_convergencia()
        self.graficar_30_simulaciones()
        self.generar_reporte_estadistico()

        print("\n" + "="*80)
        print("TODAS LAS VISUALIZACIONES GENERADAS CORRECTAMENTE")
        print(f"Ubicacion: {self.graficos_dir}")
        print("="*80)


if __name__ == '__main__':
    resultados_dir = Path(__file__).parent.parent / 'resultados'

    if not resultados_dir.exists():
        print(f"[ERROR] Directorio no encontrado: {resultados_dir}")
        print("[INFO] Ejecuta primero el programa C++ para generar los CSV")
        sys.exit(1)

    vis = VisualizadorTSP(str(resultados_dir))
    vis.generar_todas()
```

---

## 4. Especificaciones Tecnicas

### 4.1 Librerias Requeridas

| Libreria | Version | Proposito |
|----------|---------|-----------|
| `numpy` | 1.24+ | Calculos numericos |
| `pandas` | 2.0+ | Carga y procesamiento de CSV |
| `matplotlib` | 3.7+ | Visualizacion y graficas |

### 4.2 `requirements.txt`

```txt
numpy>=1.24.0
pandas>=2.0.0
matplotlib>=3.7.0
```

### 4.3 Configuracion de Graficas

| Parametro | Valor | Razon |
|-----------|-------|-------|
| `dpi` | 300 | Calidad para informe LaTeX |
| `figsize` | Variable | Ajustado por tipo de grafica |
| `bbox_inches` | `'tight'` | Evita recorte de etiquetas |
| Colores ciudades | Rojo | Destacar puntos de interes |
| Colores ruta | Azul | Diferenciar de ciudades |
| Flechas | `FancyArrowPatch` | Direccion clara del recorrido |

---

## 5. Estructura de Salida

```
resultados/
├── graficos/
│   ├── 01_ciudades.png              # Ubicacion de las 10 ciudades
│   ├── 02_mejor_ruta.png            # Recorrido optimo con flechas
│   ├── 03_convergencia.png          # Evolucion mejor vs promedio
│   └── 04_30_simulaciones.png       # Analisis estadistico completo
├── ciudades.csv
├── mejor_ruta.csv
├── historial_sim0.csv
└── simulaciones.csv
```

---

## 6. Criterios de Aceptacion

| Criterio | Verificacion | Estado |
|----------|--------------|--------|
| **4 graficas generadas** | Existen los 4 archivos PNG en `graficos/` | Pendiente |
| **Ciudades etiquetadas** | Cada ciudad tiene su ID visible | Pendiente |
| **Ruta con flechas** | Las flechas muestran direccion del recorrido | Pendiente |
| **Distancia calculada** | El titulo muestra la distancia total | Pendiente |
| **Convergencia visible** | La curva de mejor distancia desciende | Pendiente |
| **30 simulaciones** | Se muestran las 30 lineas individuales + promedio | Pendiente |
| **Estadisticas completas** | Boxplot, histograma y error bars presentes | Pendiente |
| **Resolucion adecuada** | Imagenes nitidas a 300 DPI | Pendiente |

---

## 7. Prompt Maestro para Kilo IA

```markdown
# Tarea: Implementar Visualizaciones Python para TSP

## Contexto
Estoy desarrollando la Tarea 03 de Computo Emergente. Ya tengo el solver C++ 
que genera 4 archivos CSV en la carpeta resultados/. Necesito un script Python 
que genere las 4 visualizaciones requeridas.

## Especificacion
Lee el archivo SDD/SPECS/spec-002.md para los detalles completos.

## Entrada (CSV desde C++)
- resultados/ciudades.csv: Coordenadas de las 10 ciudades
- resultados/mejor_ruta.csv: Mejor ruta encontrada (orden de visita)
- resultados/historial_sim0.csv: Evolucion de metricas por generacion
- resultados/simulaciones.csv: Resultados de 30 simulaciones independientes

## Salida (4 PNG en resultados/graficos/)
1. 01_ciudades.png: Scatter plot con ciudades etiquetadas
2. 02_mejor_ruta.png: Recorrido optimo con flechas direccionales
3. 03_convergencia.png: Evolucion mejor vs promedio (distancia y fitness)
4. 04_30_simulaciones.png: Analisis estadistico (4 subplots)

## Requisitos
1. Usar clase VisualizadorTSP con metodos separados
2. Cargar CSV con pandas
3. Usar matplotlib con estilo profesional
4. DPI = 300 para calidad de informe
5. Calcular distancia total correctamente
6. Generar reporte estadistico en consola

## Primero
Muestrame el codigo completo de visualizar_tsp.py
```

---

## 8. Checklist de Implementacion

- [x] Crear archivo `python/visualizar_tsp.py`
- [x] Crear archivo `python/requirements.txt`
- [ ] Instalar dependencias: `pip install -r python/requirements.txt` (pendiente - requiere Python)
- [ ] Ejecutar solver C++ primero para generar CSV (pendiente - después de compilación)
- [ ] Ejecutar script Python: `python python/visualizar_tsp.py` (pendiente - después de generar CSV)
- [ ] Verificar que se generen los 4 PNG (pendiente - después de ejecución)
- [ ] Validar visualmente cada grafica (pendiente - después de generar PNG)
- [ ] Revisar el reporte estadistico en consola (pendiente - después de ejecución)
- [ ] Incluir graficas en el informe LaTeX (pendiente - después de validación)

**Notas de implementación del script:**
1. `visualizar_tsp.py` creado con la clase `VisualizadorTSP` completa
2. Panel 1: Evolución de mejor distancia para todas las simulaciones + promedio
3. Panel 2: Boxplot único con jitter para los 30 valores finales
4. Panel 3: Boxplot por etapas (inicial 0-20, media 50-100, final 150-200)
5. Panel 4: Histograma con media, mediana y mínimo marcados
6. Reporte estadístico en consola con mejora porcentual

---

**Fin del documento SPEC-002.md**
