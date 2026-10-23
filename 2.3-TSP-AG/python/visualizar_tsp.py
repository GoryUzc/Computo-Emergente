import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import FancyArrowPatch
from pathlib import Path
import sys

class VisualizadorTSP:

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
        """Carga un archivo CSV desde el directorio de resultados"""
        filepath = self.resultados_dir / filename
        if not filepath.exists():
            print(f"[ERROR] No se encontró: {filepath}")
            sys.exit(1)
        return pd.read_csv(filepath)

    def graficar_ciudades(self):
        """GRÁFICA 1: Ubicación de las ciudades en el plano 2D"""
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
        ax.set_title('Ubicación de las Ciudades en el Plano 2D\n'
                     'Problema del Viajero de Comercio (TSP)',
                     fontsize=14, fontweight='bold')
        ax.legend(fontsize=12)
        ax.grid(True, alpha=0.3)
        ax.set_aspect('equal')

        plt.tight_layout()
        plt.savefig(self.graficos_dir / '01_ciudades.png', dpi=300, bbox_inches='tight')
        plt.close()
        print(f"[OK] Gráfica 1 guardada: 01_ciudades.png")

    def graficar_mejor_ruta(self):
        """GRÁFICA 2: Mejor ruta encontrada con flechas direccionales"""
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

            # Número de orden
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
        ax.set_title(f'Mejor Ruta Encontrada por el Algoritmo Genético\n'
                     f'Distancia Total: {distancia_total:.2f} unidades',
                     fontsize=14, fontweight='bold')
        ax.grid(True, alpha=0.3)
        ax.set_aspect('equal')

        # Texto con estadísticas
        stats_text = (f'Ciudades: {len(df_ordenado)}\n'
                     f'Distancia: {distancia_total:.2f}\n'
                     f'Ruta: {" → ".join([str(int(row["id_ciudad"])) 
                                             for _, row in df_ordenado.iterrows()])}')
        ax.text(0.02, 0.98, stats_text,
               transform=ax.transAxes,
               fontsize=10,
               verticalalignment='top',
               bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))

        plt.tight_layout()
        plt.savefig(self.graficos_dir / '02_mejor_ruta.png', dpi=300, bbox_inches='tight')
        plt.close()
        print(f"[OK] Gráfica 2 guardada: 02_mejor_ruta.png")

    def graficar_convergencia(self):
        """GRÁFICA 3: Convergencia del algoritmo (mejor vs promedio)"""
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6))

        # Panel izquierdo: Evolución de la mejor distancia
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

        ax1.set_xlabel('Generación', fontsize=12)
        ax1.set_ylabel('Distancia', fontsize=12)
        ax1.set_title('Convergencia del Algoritmo Genético\n'
                     'Evolución de la Distancia',
                     fontsize=13, fontweight='bold')
        ax1.legend(fontsize=11)
        ax1.grid(True, alpha=0.3)

        # Panel derecho: Evolución del fitness
        ax2.plot(self.df_historial['generacion'],
                self.df_historial['mejor_fitness'],
                'g-', linewidth=2.5, label='Mejor fitness')
        ax2.plot(self.df_historial['generacion'],
                self.df_historial['promedio_fitness'],
                'm--', linewidth=2, alpha=0.7, label='Fitness promedio')

        ax2.set_xlabel('Generación', fontsize=12)
        ax2.set_ylabel('Fitness (1/distancia)', fontsize=12)
        ax2.set_title('Convergencia del Algoritmo Genético\n'
                     'Evolución del Fitness',
                     fontsize=13, fontweight='bold')
        ax2.legend(fontsize=11)
        ax2.grid(True, alpha=0.3)

        plt.tight_layout()
        plt.savefig(self.graficos_dir / '03_convergencia.png', dpi=300, bbox_inches='tight')
        plt.close()
        print(f"[OK] Gráfica 3 guardada: 03_convergencia.png")

    def graficar_30_simulaciones(self):
        """GRÁFICA 4: Análisis estadístico de las 30 simulaciones"""
        fig, axes = plt.subplots(2, 2, figsize=(16, 12))
        

        ax = axes[0, 0]
        for sim in self.df_simulaciones['simulacion'].unique():
            df_sim = self.df_simulaciones[self.df_simulaciones['simulacion'] == sim]
            ax.plot(df_sim['generacion'], df_sim['mejor_distancia'],
                   alpha=0.3, linewidth=1, color='blue')
        
        # Promedio de todas las simulaciones
        promedio_general = self.df_simulaciones.groupby('generacion')['mejor_distancia'].mean()
        ax.plot(promedio_general.index, promedio_general.values,
               'r-', linewidth=3, label='Promedio general')
        
        ax.set_xlabel('Generación', fontsize=12)
        ax.set_ylabel('Mejor Distancia', fontsize=12)
        ax.set_title('30 Simulaciones Independientes\n'
                    'Evolución de la Mejor Distancia',
                    fontsize=13, fontweight='bold')
        ax.legend(fontsize=11)
        ax.grid(True, alpha=0.3)
        

        ax = axes[0, 1]
        
        # Obtener los valores finales de las 30 simulaciones
        max_gen = self.df_simulaciones['generacion'].max()
        finales = self.df_simulaciones[self.df_simulaciones['generacion'] == max_gen]['mejor_distancia']
        
        # Un solo boxplot con los 30 valores finales
        bp = ax.boxplot(finales.values, 
                       patch_artist=True,
                       vert=True,
                       widths=0.5)
        
        for patch in bp['boxes']:
            patch.set_facecolor('lightblue')
            patch.set_alpha(0.7)
        
        ax.set_ylabel('Mejor Distancia Final', fontsize=12)
        ax.set_title('Distribución del Mejor Fitness Final\n'
                    f'30 simulaciones (Gen {max_gen})',
                    fontsize=13, fontweight='bold')
        ax.set_xticks([1])
        ax.set_xticklabels(['Todas las\nsimulaciones'])
        ax.grid(True, alpha=0.3, axis='y')
        
        # Agregar puntos individuales (jitter)
        np.random.seed(42)  # Para reproducibilidad
        x_jitter = np.random.normal(1, 0.04, size=len(finales))
        ax.scatter(x_jitter, finales.values, alpha=0.6, color='darkblue', s=50, zorder=3)
        ax = axes[1, 0]
        
        # Dividir las generaciones en etapas
        etapas = {
            'Inicial\n(Gen 0-20)': (0, 20),
            'Media\n(Gen 50-100)': (50, 100),
            'Final\n(Gen 150-200)': (150, 200)
        }
        
        datos_etapas = []
        labels_etapas = []
        
        for nombre, (gen_inicio, gen_fin) in etapas.items():
            df_rango = self.df_simulaciones[
                (self.df_simulaciones['generacion'] >= gen_inicio) & 
                (self.df_simulaciones['generacion'] <= gen_fin)
            ]
            datos_etapas.append(df_rango['mejor_distancia'].values)
            labels_etapas.append(nombre)
        
        bp = ax.boxplot(datos_etapas, 
                       tick_labels=labels_etapas,
                       patch_artist=True)
        
        colores = ['#ff9999', '#66b3ff', '#99ff99']
        for patch, color in zip(bp['boxes'], colores):
            patch.set_facecolor(color)
            patch.set_alpha(0.7)
        
        ax.set_ylabel('Mejor Distancia', fontsize=12)
        ax.set_title('Evolución por Etapas\n'
                    'Distribución en diferentes fases',
                    fontsize=13, fontweight='bold')
        ax.grid(True, alpha=0.3, axis='y')
        
        ax = axes[1, 1]
        
        ax.hist(finales.values, bins=15, color='purple', alpha=0.7, edgecolor='black')
        ax.axvline(finales.mean(), color='red', linestyle='--', 
                  linewidth=2, label=f'Media: {finales.mean():.2f}')
        ax.axvline(finales.median(), color='green', linestyle='--',
                  linewidth=2, label=f'Mediana: {finales.median():.2f}')
        ax.axvline(finales.min(), color='blue', linestyle=':',
                  linewidth=2, label=f'Mínimo: {finales.min():.2f}')
        
        ax.set_xlabel('Mejor Distancia Final', fontsize=12)
        ax.set_ylabel('Frecuencia', fontsize=12)
        ax.set_title('Distribución de Mejores Distancias Finales\n'
                    'Histograma',
                    fontsize=13, fontweight='bold')
        ax.legend(fontsize=11)
        ax.grid(True, alpha=0.3, axis='y')
        
        plt.tight_layout()
        plt.savefig(self.graficos_dir / '04_30_simulaciones.png', dpi=300, bbox_inches='tight')
        plt.close()
        print(f"[OK] Gráfica 4 guardada: 04_30_simulaciones.png")

    def generar_reporte_estadistico(self):
        """Genera un reporte estadístico completo"""
        print("\n" + "="*80)
        print("REPORTE ESTADÍSTICO DEL ALGORITMO GENÉTICO PARA TSP")
        print("="*80)

        # Estadísticas de la simulación 0
        print("\n▶ SIMULACIÓN 0:")
        print(f"  Generaciones: {len(self.df_historial)}")
        print(f"  Mejor distancia inicial: {self.df_historial.iloc[0]['mejor_distancia']:.2f}")
        print(f"  Mejor distancia final: {self.df_historial.iloc[-1]['mejor_distancia']:.2f}")
        mejora = ((self.df_historial.iloc[0]['mejor_distancia'] - self.df_historial.iloc[-1]['mejor_distancia']) 
                  / self.df_historial.iloc[0]['mejor_distancia'] * 100)
        print(f"  Mejora: {mejora:.2f}%")

        # Estadísticas de las 30 simulaciones
        print("\n▶ 30 SIMULACIONES INDEPENDIENTES:")
        max_gen = self.df_simulaciones['generacion'].max()
        finales = self.df_simulaciones[self.df_simulaciones['generacion'] == max_gen]['mejor_distancia']

        print(f"  Media: {finales.mean():.2f}")
        print(f"  Mediana: {finales.median():.2f}")
        print(f"  Desv. Est.: {finales.std():.2f}")
        print(f"  Mínimo: {finales.min():.2f}")
        print(f"  Máximo: {finales.max():.2f}")
        print(f"  Rango: {finales.max() - finales.min():.2f}")

        # Mejor ruta
        print("\n▶ MEJOR RUTA ENCONTRADA:")
        df_ordenado = self.df_mejor_ruta.sort_values('orden_visita')
        ruta_str = " → ".join([str(int(row['id_ciudad'])) for _, row in df_ordenado.iterrows()])
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
        """Ejecuta todas las visualizaciones"""
        print("="*80)
        print("VISUALIZACIONES - ALGORITMO GENÉTICO PARA TSP")
        print("="*80)

        self.graficar_ciudades()
        self.graficar_mejor_ruta()
        self.graficar_convergencia()
        self.graficar_30_simulaciones()
        self.generar_reporte_estadistico()

        print("\n" + "="*80)
        print("TODAS LAS VISUALIZACIONES GENERADAS CORRECTAMENTE")
        print(f"Ubicación: {self.graficos_dir}")
        print("="*80)


if __name__ == '__main__':
    resultados_dir = Path(__file__).parent.parent / 'resultados'

    if not resultados_dir.exists():
        print(f"[ERROR] Directorio no encontrado: {resultados_dir}")
        print("[INFO] Ejecuta primero el programa C++ para generar los CSV")
        sys.exit(1)

    vis = VisualizadorTSP(str(resultados_dir))
    vis.generar_todas()