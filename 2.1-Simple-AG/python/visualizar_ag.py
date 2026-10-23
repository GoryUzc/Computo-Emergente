#!/usr/bin/env python3
import argparse
import os
import pandas as pd
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np

def leer_metricas(ruta):
    return pd.read_csv(ruta)

def graficar_convergencia(df, titulo, ruta_salida, y_max=None):
    fig, ax = plt.subplots(figsize=(10, 6), dpi=150)
    
    ax.plot(df['generacion'], df['mejor_fitness'], 'b-', linewidth=2, label='Mejor individuo')
    ax.plot(df['generacion'], df['promedio_fitness'], '--', color='red', label='Promedio población')
    
    ax.axhline(y=3969, color='gray', linestyle='-.', alpha=0.5, label='Óptimo teórico ($63^2$ = 3969)')
    
    ax.set_xlabel('Generación')
    ax.set_ylabel('Fitness')
    ax.set_title(titulo)
    ax.legend(loc='best')
    ax.grid(True, linestyle='--', alpha=0.3)
    
    if y_max:
        ax.set_ylim(top=y_max)
    
    plt.savefig(ruta_salida, bbox_inches='tight', facecolor='white')
    plt.close()

def graficar_comparativa(dfs, etiquetas, titulo, ruta_salida, modo='mejor', colores=None):
    fig, ax = plt.subplots(figsize=(10, 6), dpi=150)
    
    if colores is None:
        colores = plt.cm.tab10(np.linspace(0, 1, len(dfs)))
    
    for df, etiqueta, color in zip(dfs, etiquetas, colores):
        columna = 'mejor_fitness' if modo == 'mejor' else 'promedio_fitness'
        ax.plot(df['generacion'], df[columna], color=color, linewidth=2, label=etiqueta)
    
    ax.axhline(y=3969, color='gray', linestyle='-.', alpha=0.5, label='Óptimo teórico ($63^2$ = 3969)')
    
    ax.set_xlabel('Generación')
    ax.set_ylabel('Fitness')
    ax.set_title(titulo)
    ax.legend(loc='best')
    ax.grid(True, linestyle='--', alpha=0.3)
    
    plt.savefig(ruta_salida, bbox_inches='tight', facecolor='white')
    plt.close()

def main():
    parser = argparse.ArgumentParser(description='Visualización del AGS')
    parser.add_argument('--input-dir', default='../resultados/', help='Directorio con CSV de métricas')
    parser.add_argument('--output-dir', default='../resultados/graficos/', help='Directorio de salida')
    parser.add_argument('--formato', default='png', help='Formato de salida')
    args = parser.parse_args()
    
    os.makedirs(args.output_dir, exist_ok=True)
    
    df_base = leer_metricas(os.path.join(args.input_dir, 'experimento_base.csv'))
    df_pc06 = leer_metricas(os.path.join(args.input_dir, 'experimento_pc06.csv'))
    df_pc04 = leer_metricas(os.path.join(args.input_dir, 'experimento_pc04.csv'))
    df_pm01 = leer_metricas(os.path.join(args.input_dir, 'experimento_pm001.csv'))
    df_pm10 = leer_metricas(os.path.join(args.input_dir, 'experimento_pm01.csv'))
    df_real = leer_metricas(os.path.join(args.input_dir, 'experimento_real.csv'))
    
    graficar_convergencia(df_base, 'Convergencia AGS - Base (Pc=0.8, Pm=0.002)', 
                          os.path.join(args.output_dir, 'convergencia_base.png'))
    
    graficar_comparativa([df_base, df_pc06, df_pc04], 
                         ['Experimento Base (Pc=0.8)', 'Experimento Pc=0.6', 'Experimento Pc=0.4'],
                         'Comparativa variación Pc (Pm fijo = 0.002)',
                         os.path.join(args.output_dir, 'comparativa_pc.png'),
                         modo='mejor')
    
    graficar_comparativa([df_base, df_pm01, df_pm10],
                         ['Experimento Base (Pm=0.002)', 'Experimento Pm=0.01', 'Experimento Pm=0.1'],
                         'Comparativa variación Pm (Pc fijo = 0.8)',
                         os.path.join(args.output_dir, 'comparativa_pm.png'),
                         modo='mejor')
    
    graficar_convergencia(df_real, 'Convergencia AGS - Modo Real (13 bits, precisión 2 decimales)',
                          os.path.join(args.output_dir, 'convergencia_real.png'))
    
    print(f'Se generaron 4 gráficos en {args.output_dir}')

if __name__ == '__main__':
    main()