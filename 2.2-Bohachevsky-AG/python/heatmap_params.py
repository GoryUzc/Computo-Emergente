import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
import sys


def cargar_datos_grid(filepath):
    df = pd.read_csv(filepath)
    # Cambiar para incluir 'simple' en lugar de 'linear' y 'window'
    return df[df['ag_type'].isin(['simple'])]


def calcular_fitness_final(df):
    max_gen = df['generation'].max()
    df_final = df[df['generation'] == max_gen][['pc', 'pm', 'simulation', 'best_fitness']].copy()
    df_final.columns = ['pc', 'pm', 'simulation', 'best_fitness_final']
    return df_final


def promediar_fitness_final(df):
    return df.groupby(['pc', 'pm']).agg({
        'best_fitness_final': 'mean'
    }).reset_index()


def graficar_heatmap(df_prom, output_path):
    plt.style.use('seaborn-v0_8-whitegrid')
    pc_values = sorted(df_prom['pc'].unique())
    pm_values = sorted(df_prom['pm'].unique())
    
    matrix = np.zeros((len(pc_values), len(pm_values)))
    for _, row in df_prom.iterrows():
        i = pc_values.index(row['pc'])
        j = pm_values.index(row['pm'])
        matrix[i, j] = row['best_fitness_final']
    
    fig, ax = plt.subplots(figsize=(10, 6))
    
    # Usar un colormap más sensible y normalización
    im = ax.imshow(matrix, cmap='RdYlGn', aspect='auto', 
                   vmin=matrix.min(), vmax=matrix.max())
    
    ax.set_xticks(np.arange(len(pm_values)))
    ax.set_yticks(np.arange(len(pc_values)))
    ax.set_xticklabels([f'{pm:.3f}' for pm in pm_values])
    ax.set_yticklabels([f'{pc:.1f}' for pc in pc_values])
    
    # Ajustar el color del texto según el valor
    for i in range(len(pc_values)):
        for j in range(len(pm_values)):
            # Usar texto negro para valores claros, blanco para oscuros
            color = 'black' if matrix[i, j] > (matrix.min() + matrix.max()) / 2 else 'blue'
            text = ax.text(j, i, f'{matrix[i, j]:.4f}', 
                          ha='center', va='center', color=color, fontsize=9)
    
    ax.set_xlabel('Probabilidad de Mutación (Pm)', fontsize=12)
    ax.set_ylabel('Probabilidad de Cruce (Pc)', fontsize=12)
    ax.set_title('Fitness Final Promedio (30 simulaciones) - PC vs PM\n(Menor valor = Mejor optimización)', 
                fontsize=14, fontweight='bold')
    
    # Colorbar con más precisión
    cbar = plt.colorbar(im, ax=ax)
    cbar.set_label('Fitness Promedio Final', fontsize=11)
    cbar.ax.tick_params(labelsize=10)
    
    plt.tight_layout()
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close()
    
    print(f"[OK] Heatmap guardado en: {output_path}")
    print(f"[INFO] Rango de fitness: {matrix.min():.4f} - {matrix.max():.4f}")
    print(f"[INFO] Diferencia máxima: {matrix.max() - matrix.min():.4f}")


def generar_reporte_estadistico(df_prom):
    """Genera un resumen estadístico de los resultados"""
    print("\n" + "="*60)
    print("REPORTE ESTADÍSTICO DE OPTIMIZACIÓN")
    print("="*60)
    
    # Mejor combinación (menor fitness = mejor)
    mejor = df_prom.loc[df_prom['best_fitness_final'].idxmin()]
    print(f"\n🏆 MEJOR COMBINACIÓN:")
    print(f"   PC = {mejor['pc']:.1f}, PM = {mejor['pm']:.3f}")
    print(f"   Fitness = {mejor['best_fitness_final']:.4f}")
    
    # Peor combinación
    peor = df_prom.loc[df_prom['best_fitness_final'].idxmax()]
    print(f"\n⚠️  PEOR COMBINACIÓN:")
    print(f"   PC = {peor['pc']:.1f}, PM = {peor['pm']:.3f}")
    print(f"   Fitness = {peor['best_fitness_final']:.4f}")
    
    # Estadísticas generales
    print(f"\n📊 ESTADÍSTICAS:")
    print(f"   Media: {df_prom['best_fitness_final'].mean():.4f}")
    print(f"   Desv. Est.: {df_prom['best_fitness_final'].std():.4f}")
    print(f"   Rango: {df_prom['best_fitness_final'].min():.4f} - {df_prom['best_fitness_final'].max():.4f}")
    print("="*60 + "\n")


if __name__ == '__main__':
    csv_path = Path(__file__).parent.parent / 'resultados' / 'results_comparativa.csv'
    graficos_path = Path(__file__).parent.parent / 'resultados' / 'graficos'
    graficos_path.mkdir(parents=True, exist_ok=True)
    
    if not csv_path.exists():
        print(f"[ERROR] No se encontró el archivo: {csv_path}")
        print(f"[INFO] Ejecuta primero el programa C++ para generar results.csv")
        sys.exit(1)
    
    print(f"[INFO] Cargando datos desde: {csv_path}")
    df = cargar_datos_grid(csv_path)
    
    if df.empty:
        print("[ERROR] No se encontraron datos con los filtros aplicados")
        sys.exit(1)
    
    print(f"[INFO] Total de registros: {len(df)}")
    print(f"[INFO] Tipos de AG encontrados: {df['ag_type'].unique()}")
    
    df_final = calcular_fitness_final(df)
    df_prom = promediar_fitness_final(df_final)
    
    generar_reporte_estadistico(df_prom)
    
    graficar_heatmap(df_prom, graficos_path / 'heatmap_pc_pm.png')