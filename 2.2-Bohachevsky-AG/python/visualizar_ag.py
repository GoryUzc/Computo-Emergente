import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
import sys

def cargar_datos(filepath):
    """Carga el CSV y valida los datos"""
    df = pd.read_csv(filepath)
    print(f"[INFO] CSV cargado: {len(df)} registros")
    print(f"[INFO] Tipos de AG: {df['ag_type'].unique()}")
    print(f"[INFO] Simulaciones: {df['simulation'].nunique()}")
    print(f"[INFO] Generaciones: {df['generation'].min()} a {df['generation'].max()}")
    return df

def graficar_comparativa_best_fitness(df, output_dir):
    """Parte a y b: Comparativa del mejor fitness promedio entre los 3 tipos de AG"""
    fig, ax = plt.subplots(figsize=(12, 7))
    
    tipos = df['ag_type'].unique()
    colores = {'simple': '#1f77b4', 'lineal': '#ff7f0e', 'ventana': '#2ca02c'}
    markers = {'simple': 'o', 'lineal': 's', 'ventana': '^'}
    
    for tipo in tipos:
        df_tipo = df[df['ag_type'] == tipo]
        
        # Promedio del mejor fitness por generación (sobre las 30 simulaciones)
        promedio_gen = df_tipo.groupby('generation')['best_fitness'].mean().reset_index()
        
        # Desviación estándar para bandas de error
        std_gen = df_tipo.groupby('generation')['best_fitness'].std().reset_index()
        
        ax.plot(promedio_gen['generation'], promedio_gen['best_fitness'],
                label=f'AG {tipo.capitalize()}',
                color=colores.get(tipo, 'black'),
                marker=markers.get(tipo, 'o'),
                markevery=5,
                linewidth=2.5,
                markersize=6)
        
        # Banda de error (mean ± std)
        ax.fill_between(promedio_gen['generation'],
                       promedio_gen['best_fitness'] - std_gen['best_fitness'],
                       promedio_gen['best_fitness'] + std_gen['best_fitness'],
                       color=colores.get(tipo, 'black'),
                       alpha=0.15)
    
    # Línea del óptimo global (Bohachevsky mínimo = 0)
    ax.axhline(y=0, color='red', linestyle='--', linewidth=1.5, alpha=0.7, 
               label='Óptimo Global (f=0)')
    
    ax.set_xlabel('Generación', fontsize=13)
    ax.set_ylabel('Best Fitness (promedio de 30 sims)', fontsize=13)
    ax.set_title('Evolución del Mejor Fitness Promedio\n'
                 'Función de Bohachevsky - Comparativa AG Simple vs Modificado',
                 fontsize=14, fontweight='bold')
    ax.legend(fontsize=11, loc='upper right')
    ax.grid(True, alpha=0.3)
    ax.set_xlim(0, df['generation'].max())
    
    plt.tight_layout()
    plt.savefig(output_dir / '01_comparativa_best_fitness_1.png', dpi=300, bbox_inches='tight')
    plt.close()
    print(f"[OK] Gráfica 1 guardada: 01_comparativa_best_fitness_1.png")

def graficar_comparativa_avg_fitness(df, output_dir):
    """Parte a y b: Comparativa del fitness promedio de la población"""
    fig, ax = plt.subplots(figsize=(12, 7))
    
    tipos = df['ag_type'].unique()
    colores = {'simple': '#1f77b4', 'lineal': '#ff7f0e', 'ventana': '#2ca02c'}
    
    for tipo in tipos:
        df_tipo = df[df['ag_type'] == tipo]
        promedio_gen = df_tipo.groupby('generation')['avg_fitness'].mean().reset_index()
        std_gen = df_tipo.groupby('generation')['avg_fitness'].std().reset_index()
        
        ax.plot(promedio_gen['generation'], promedio_gen['avg_fitness'],
                label=f'AG {tipo.capitalize()}',
                color=colores.get(tipo, 'black'),
                linewidth=2.5)
        
        ax.fill_between(promedio_gen['generation'],
                       promedio_gen['avg_fitness'] - std_gen['avg_fitness'],
                       promedio_gen['avg_fitness'] + std_gen['avg_fitness'],
                       color=colores.get(tipo, 'black'),
                       alpha=0.15)
    
    ax.axhline(y=0, color='red', linestyle='--', linewidth=1.5, alpha=0.7, 
               label='Óptimo Global (f=0)')
    
    ax.set_xlabel('Generación', fontsize=13)
    ax.set_ylabel('Avg Fitness (promedio de 30 sims)', fontsize=13)
    ax.set_title('Evolución del Fitness Promedio de la Población\n'
                 'Función de Bohachevsky - Comparativa AG Simple vs Modificado',
                 fontsize=14, fontweight='bold')
    ax.legend(fontsize=11, loc='upper right')
    ax.grid(True, alpha=0.3)
    ax.set_xlim(0, df['generation'].max())
    
    plt.tight_layout()
    plt.savefig(output_dir / '02_comparativa_avg_fitness_1.png', dpi=300, bbox_inches='tight')
    plt.close()
    print(f"[OK] Gráfica 2 guardada: 02_comparativa_avg_fitness_1.png")

def graficar_boxplot_final(df, output_dir):
    """Distribución del mejor fitness final en las 30 simulaciones"""
    fig, ax = plt.subplots(figsize=(10, 6))
    
    tipos = df['ag_type'].unique()
    colores = {'simple': '#1f77b4', 'lineal': '#ff7f0e', 'ventana': '#2ca02c'}
    
    datos_boxplot = []
    labels = []
    for tipo in tipos:
        df_tipo = df[df['ag_type'] == tipo]
        max_gen = df_tipo['generation'].max()
        finales = df_tipo[df_tipo['generation'] == max_gen]['best_fitness']
        datos_boxplot.append(finales.values)
        labels.append(f'AG {tipo.capitalize()}')
    
    bp = ax.boxplot(datos_boxplot, labels=labels, patch_artist=True)
    
    for patch, tipo in zip(bp['boxes'], tipos):
        patch.set_facecolor(colores.get(tipo, 'gray'))
        patch.set_alpha(0.7)
    
    ax.set_ylabel('Best Fitness Final (gen 100)', fontsize=13)
    ax.set_title('Distribución del Mejor Fitness Final\n'
                 '30 simulaciones independientes por cada tipo de AG',
                 fontsize=14, fontweight='bold')
    ax.grid(True, alpha=0.3, axis='y')
    
    plt.tight_layout()
    plt.savefig(output_dir / '03_boxplot_fitness_final_1.png', dpi=300, bbox_inches='tight')
    plt.close()
    print(f"[OK] Gráfica 3 guardada: 03_boxplot_fitness_final_1.png")

def graficar_convergencia_detallada(df, output_dir):
    """Gráfica detallada con las 30 simulaciones individuales"""
    fig, axes = plt.subplots(1, 3, figsize=(18, 6))
    
    tipos = df['ag_type'].unique()
    colores = {'simple': '#1f77b4', 'lineal': '#ff7f0e', 'ventana': '#2ca02c'}
    
    for idx, tipo in enumerate(tipos):
        ax = axes[idx]
        df_tipo = df[df['ag_type'] == tipo]
        
        # Graficar cada simulación individual (líneas tenues)
        for sim in df_tipo['simulation'].unique():
            df_sim = df_tipo[df_tipo['simulation'] == sim]
            ax.plot(df_sim['generation'], df_sim['best_fitness'],
                   color=colores.get(tipo, 'gray'), alpha=0.2, linewidth=0.8)
        
        # Promedio (línea gruesa)
        promedio_gen = df_tipo.groupby('generation')['best_fitness'].mean().reset_index()
        ax.plot(promedio_gen['generation'], promedio_gen['best_fitness'],
               color=colores.get(tipo, 'black'), linewidth=3, 
               label='Promedio')
        
        ax.axhline(y=0, color='red', linestyle='--', linewidth=1, alpha=0.7)
        ax.set_xlabel('Generación')
        ax.set_ylabel('Best Fitness')
        ax.set_title(f'AG {tipo.capitalize()}', fontsize=12, fontweight='bold')
        ax.grid(True, alpha=0.3)
        ax.legend()
    
    fig.suptitle('Evolución Individual de las 30 Simulaciones + Promedio',
                fontsize=14, fontweight='bold', y=1.02)
    
    plt.tight_layout()
    plt.savefig(output_dir / '04_convergencia_detallada_1.png', dpi=300, bbox_inches='tight')
    plt.close()
    print(f"[OK] Gráfica 4 guardada: 04_convergencia_detallada_1.png")

def generar_reporte_estadistico(df):
    """Genera un reporte estadístico completo"""
    print("\n" + "="*70)
    print("REPORTE ESTADÍSTICO - MEJOR FITNESS FINAL (Generación 100)")
    print("="*70)
    
    tipos = df['ag_type'].unique()
    
    for tipo in tipos:
        df_tipo = df[df['ag_type'] == tipo]
        max_gen = df_tipo['generation'].max()
        finales = df_tipo[df_tipo['generation'] == max_gen]['best_fitness']
        
        print(f"\n▶ AG {tipo.upper()}:")
        print(f"  Media:       {finales.mean():.6f}")
        print(f"  Mediana:     {finales.median():.6f}")
        print(f"  Desv.Est.:   {finales.std():.6f}")
        print(f"  Mínimo:      {finales.min():.6f}")
        print(f"  Máximo:      {finales.max():.6f}")
        print(f"  Éxitos (fit < 0.01): {(finales < 0.01).sum()}/30")
        print(f"  Éxitos (fit < 0.1):  {(finales < 0.1).sum()}/30")
    
    print("\n" + "="*70)

def graficar_heatmap_grid(df, output_dir):
    """Parte c: Heatmap de Pc vs Pm si hay datos de grid"""
    # Verificar si hay múltiples valores de Pc y Pm
    if df['pc'].nunique() <= 1 or df['pm'].nunique() <= 1:
        print("[INFO] No hay datos de grid para heatmap (parte c)")
        return
    
    tipos = df['ag_type'].unique()
    
    for tipo in tipos:
        df_tipo = df[df['ag_type'] == tipo]
        
        # Promedio del mejor fitness final por combinación (pc, pm)
        max_gen = df_tipo['generation'].max()
        finales = df_tipo[df_tipo['generation'] == max_gen]
        
        resumen = finales.groupby(['pc', 'pm'])['best_fitness'].mean().reset_index()
        
        pcs = sorted(resumen['pc'].unique())
        pms = sorted(resumen['pm'].unique())
        
        if len(pcs) < 2 or len(pms) < 2:
            continue
        
        matriz = np.zeros((len(pcs), len(pms)))
        for _, fila in resumen.iterrows():
            i = pcs.index(fila['pc'])
            j = pms.index(fila['pm'])
            matriz[i, j] = fila['best_fitness']
        
        fig, ax = plt.subplots(figsize=(10, 7))
        im = ax.imshow(matriz, cmap='viridis', aspect='auto')
        
        ax.set_xticks(range(len(pms)))
        ax.set_xticklabels([f'{pm:.3f}' for pm in pms])
        ax.set_yticks(range(len(pcs)))
        ax.set_yticklabels([f'{pc:.1f}' for pc in pcs])
        
        ax.set_xlabel('Probabilidad de Mutación (Pm)', fontsize=12)
        ax.set_ylabel('Probabilidad de Cruce (Pc)', fontsize=12)
        ax.set_title(f'AG {tipo.capitalize()}: Best Fitness Promedio Final\n'
                     f'Variación de Pc y Pm (30 sims por combinación)',
                     fontsize=14, fontweight='bold')
        
        # Anotar valores en cada celda
        for i in range(len(pcs)):
            for j in range(len(pms)):
                text = ax.text(j, i, f'{matriz[i,j]:.4f}',
                              ha='center', va='center',
                              color='white' if matriz[i,j] > matriz.mean() else 'black',
                              fontsize=10, fontweight='bold')
        
        plt.colorbar(im, ax=ax, label='Best Fitness (menor = mejor)')
        plt.tight_layout()
        plt.savefig(output_dir / f'05_heatmap_{tipo}_1.png', dpi=300, bbox_inches='tight')
        plt.close()
        print(f"[OK] Heatmap guardado: 05_heatmap_{tipo}_1.png")

if __name__ == '__main__':
    # Rutas
    csv_path = Path(__file__).parent.parent / 'resultados' / 'results_comparativa.csv'
    output_dir = Path(__file__).parent.parent / 'resultados' / 'graficos'
    output_dir.mkdir(parents=True, exist_ok=True)
    
    if not csv_path.exists():
        print(f"[ERROR] No se encontró: {csv_path}")
        sys.exit(1)
    
    # Cargar datos
    df = cargar_datos(csv_path)
    
    # Generar gráficas
    print("\n" + "="*70)
    print("GENERANDO GRÁFICAS PARA EL TALLER")
    print("="*70)
    
    graficar_comparativa_best_fitness(df, output_dir)
    graficar_comparativa_avg_fitness(df, output_dir)
    graficar_boxplot_final(df, output_dir)
    graficar_convergencia_detallada(df, output_dir)
    graficar_heatmap_grid(df, output_dir)
    
    # Reporte estadístico
    generar_reporte_estadistico(df)
    
    print("\n" + "="*70)
    print("TODAS LAS GRÁFICAS GENERADAS CORRECTAMENTE")
    print(f"Ubicación: {output_dir}")
    print("="*70)