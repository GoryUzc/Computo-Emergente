# python/plot_auto.py
import sys
import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider
from pathlib import Path

# ============ CONFIGURACIÓN GLOBAL ============
plt.style.use('seaborn-v0_8-whitegrid')
plt.rcParams.update({
    'figure.dpi': 150,
    'savefig.dpi': 300,
    'font.family': 'sans-serif',
    'font.sans-serif': ['Segoe UI', 'DejaVu Sans', 'Arial'],
    'font.size': 10,
    'axes.titlesize': 12,
    'axes.labelsize': 11,
    'legend.fontsize': 9,
    'lines.linewidth': 1.2,
    'lines.markersize': 3,
    'grid.alpha': 0.25,
    'axes.facecolor': 'white',
    'legend.frameon': True,
    'legend.framealpha': 0.95,
    'legend.facecolor': 'white',
    'legend.edgecolor': '#cccccc'
})

# Paleta académica (colorblind-friendly)
C_BLUE   = '#1f77b4'
C_ORANGE = '#ff7f0e'
C_GREEN  = '#2ca02c'
C_RED    = '#d62728'
C_PURPLE = '#9467bd'
C_GRAY   = '#7f7f7f'


# ============ MODELOS EXISTENTES (sin cambios) ============

def plot_perceptron(df):
    """Perceptrón Simple y McCulloch-Pitts"""
    fig, ax = plt.subplots(figsize=(9, 5), constrained_layout=True)
    ax.plot(df['Epoca'], df['Error'], color=C_BLUE, marker='o', markersize=2, label='Error (MSE)')
    ax.set_ylabel('Error Cuadrático Medio', color=C_BLUE)
    ax.set_title('Perceptrón Simple: Convergencia y Exactitud')
    if 'Aciertos' in df.columns:
        ax2 = ax.twinx()
        ax2.plot(df['Epoca'], df['Aciertos'], color=C_GREEN, marker='s', markersize=2, label='Aciertos')
        ax2.set_ylabel('Aciertos (de 4)', color=C_GREEN)
        ax2.set_ylim(0, 4.5)
        l1, v1 = ax.get_legend_handles_labels()
        l2, v2 = ax2.get_legend_handles_labels()
        ax.legend(l1+l2, v1+v2, loc='upper left', bbox_to_anchor=(0.02, 0.98))
    ax.set_xlabel('Época / Iteración')
    ax.grid(True, alpha=0.25)
    plt.show()


def plot_adaline(df):
    """ADALINE con Regla Delta"""
    fig, ax = plt.subplots(figsize=(9, 5), constrained_layout=True)
    ax.semilogy(df['Epoca'], df['MSE'], color=C_BLUE, marker='.', markersize=2, label='MSE')
    ax.set_ylabel('MSE (escala log)', color=C_BLUE)
    ax.set_title('ADALINE: Evolución del Error')
    if 'MAE' in df.columns:
        ax2 = ax.twinx()
        ax2.plot(df['Epoca'], df['MAE'], color=C_ORANGE, marker='.', markersize=2, label='MAE')
        ax2.set_ylabel('MAE', color=C_ORANGE)
        l1, v1 = ax.get_legend_handles_labels()
        l2, v2 = ax2.get_legend_handles_labels()
        ax.legend(l1+l2, v1+v2, loc='upper right', bbox_to_anchor=(0.98, 0.98))
    ax.set_xlabel('Época / Iteración')
    ax.grid(True, alpha=0.25)
    plt.show()


def plot_mlp(df):
    """Perceptrón Multicapa (Backpropagation clásico)"""
    fig, ax = plt.subplots(figsize=(9, 5), constrained_layout=True)
    ax.semilogy(df['Epoca'], df['Error'], color=C_RED, marker='.', markersize=2, label='Error de Entrenamiento')
    ax.set_ylabel('Error (escala log)')
    ax.set_title('Perceptrón Multicapa: Convergencia')
    ax.legend(loc='upper right', bbox_to_anchor=(0.98, 0.98))
    ax.set_xlabel('Época / Iteración')
    ax.grid(True, alpha=0.25)
    plt.show()


# ============ KOHONEN SOM — EXPANDIDO ============

def plot_kohonen(df, csv_path):
    """
    Kohonen SOM con:
    - Gráfica 1: QE vs Época + decaimiento η/σ (eje dual)
    - Gráfica 2: Slider con snapshots de pesos cada 100 épocas
    """
    # --- Gráfica 1: Convergencia (igual que antes) ---
    fig1, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5), constrained_layout=True)
    
    ax1.plot(df['Epoca'], df['QuantizationError'], color=C_PURPLE, marker='.', markersize=2, label='QE')
    ax1.set_ylabel('Error de Cuantización (QE)')
    ax1.set_title('Convergencia Topológica')
    ax1.legend(loc='upper right', bbox_to_anchor=(1.0, 1.0))
    ax1.grid(True, alpha=0.3)
    ax1.set_xlabel('Época')
    
    if 'Sigma' in df.columns and 'LearningRate' in df.columns:
        ax2.plot(df['Epoca'], df['Sigma'], color=C_ORANGE, linestyle='--', linewidth=1.0, label='σ(t)')
        ax2.plot(df['Epoca'], df['LearningRate'], color=C_BLUE, linestyle='--', linewidth=1.0, label='η(t)')
        ax2.set_ylabel('Valor del Parámetro')
        ax2.set_title('Decaimiento Exponencial de Hiperparámetros')
        ax2.legend(loc='upper right', bbox_to_anchor=(1.0, 1.0))
        ax2.grid(True, alpha=0.3)
        ax2.set_xlabel('Época')
    
    plt.suptitle('Mapas Autoorganizados de Kohonen — Convergencia', fontweight='bold', fontsize=14, y=1.02)
    plt.show()
    
    # --- Gráfica 2: Slider de snapshots (pesos cada 100 épocas) ---
    snapshots_path = Path(csv_path).parent / 'kohonen_snapshots.csv'
    if not snapshots_path.exists():
        print(f"[INFO] No se encontró {snapshots_path.name}. Saltando visualización de clusters.")
        return
    
    df_snap = pd.read_csv(snapshots_path)
    epochs_available = sorted(df_snap['SnapshotEpoch'].unique())
    
    if len(epochs_available) < 2:
        print("[INFO] Solo hay 1 snapshot. No se genera slider.")
        return
    
    # Datos sintéticos de los 4 clusters (para superponer)
    rng = np.random.RandomState(42)
    centers = [[0.2, 0.2], [0.8, 0.2], [0.2, 0.8], [0.8, 0.8]]
    data_clusters = []
    for c in centers:
        cluster = rng.normal(c, 0.06, (50, 2))
        cluster = np.clip(cluster, 0, 1)
        data_clusters.append(cluster)
    data_all = np.vstack(data_clusters)
    
    fig2, ax = plt.subplots(figsize=(8, 8), constrained_layout=True)
    ax.scatter(data_all[:, 0], data_all[:, 1], c=C_GRAY, alpha=0.3, s=8, label='Datos (clusters)')
    
    # Pesos iniciales (época 0)
    snap_0 = df_snap[df_snap['SnapshotEpoch'] == epochs_available[0]]
    scatter_w = ax.scatter(snap_0['WeightX'], snap_0['WeightY'], c=C_RED, s=25, marker='o',
                           edgecolors='black', linewidth=0.5, label='Pesos (t=0)')
    
    ax.set_xlim(-0.05, 1.05)
    ax.set_ylim(-0.05, 1.05)
    ax.set_aspect('equal')
    ax.set_title(f'Kohonen SOM — Evolución de Pesos (época {epochs_available[0]})',
                 fontsize=13, fontweight='bold')
    ax.legend(loc='upper right', fontsize=9)
    ax.grid(True, alpha=0.3)
    
    # Slider
    ax_slider = plt.axes([0.2, 0.02, 0.6, 0.03])
    slider = Slider(ax_slider, 'Época', 0, len(epochs_available)-1, valinit=0, valstep=1)
    
    def update(val):
        idx = int(slider.val)
        epoch = epochs_available[idx]
        snap = df_snap[df_snap['SnapshotEpoch'] == epoch]
        scatter_w.set_offsets(np.c_[snap['WeightX'], snap['WeightY']])
        ax.set_title(f'Kohonen SOM — Evolución de Pesos (época {epoch})',
                     fontsize=13, fontweight='bold')
        fig2.canvas.draw_idle()
    
    slider.on_changed(update)
    plt.show()
    print("✅ Ventana de Kohonen cerrada. Retornando a C++...")


# ============ HOPFIELD — EXPANDIDO ============

def _parse_pixels(s):
    """Convierte string '1-1-1...' de 42 chars a matriz 7x6."""
    s = str(s).strip()
    arr = np.array([1 if c == '1' else 0 for c in s if c in '1-'])
    if len(arr) != 42:
        return np.zeros((7, 6))
    return arr.reshape(7, 6)


def plot_hopfield(df):
    """
    Hopfield con:
    - Gráfica 1: Barras de % recuperación vs ruido + Energía
    - Gráfica 2: Slider navegando muestras (entrada ruidosa vs salida recuperada)
    """
    # --- Gráfica 1: Métricas (igual que antes) ---
    fig1, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5), constrained_layout=True)
    
    recovery_stats = df.groupby('RuidoPct')['RecuperacionPct'].mean().reset_index()
    colors_bar = [C_GREEN, C_ORANGE, C_RED]
    bars = ax1.bar(
        recovery_stats['RuidoPct'].astype(str) + '%',
        recovery_stats['RecuperacionPct'],
        color=colors_bar, edgecolor='black', width=0.5
    )
    for bar in bars:
        height = bar.get_height()
        ax1.text(bar.get_x() + bar.get_width()/2., height + 1, f'{height:.1f}%',
                 ha='center', va='bottom', fontweight='bold', color='black')
    ax1.set_title('Robustez ante Ruido Binomial')
    ax1.set_xlabel('Nivel de Ruido Inyectado')
    ax1.set_ylabel('% Recuperación Promedio')
    ax1.set_ylim(0, 110)
    ax1.grid(axis='y', alpha=0.5, linestyle='--')
    
    df_head = df.head(50)
    ax2.plot(df_head['Epoca'], df_head['Energia'], color=C_BLUE, marker='o',
             markersize=3, linewidth=1.5, label='Energía (E)')
    ax2.axhline(y=df_head['Energia'].iloc[-1], color='gray', linestyle='--',
                alpha=0.5, label='Estado Estable')
    ax2.set_title('Descenso de Energía (Primeras 50 épocas)')
    ax2.set_xlabel('Iteración / Época')
    ax2.set_ylabel('Energía de Lyapunov')
    ax2.legend(loc='lower right')
    ax2.grid(True, alpha=0.5)
    
    plt.suptitle('Red de Hopfield: Memoria Asociativa y Estabilidad',
                 fontsize=14, fontweight='bold', y=1.02)
    plt.show()
    
    # --- Gráfica 2: Slider de muestras (entrada vs salida) ---
    if 'NoisyPixels' not in df.columns or 'RecoveredPixels' not in df.columns:
        print("[INFO] CSV sin columnas de píxeles. Saltando visualización de patrones.")
        return
    
    fig2, axes = plt.subplots(1, 3, figsize=(12, 5), constrained_layout=True)
    ax_noisy, ax_recov, ax_info = axes
    
    # Muestra inicial
    row0 = df.iloc[0]
    noisy_img = _parse_pixels(row0['NoisyPixels'])
    recov_img = _parse_pixels(row0['RecoveredPixels'])
    pattern_names = ['A', 'B', 'C', 'D']
    pat_name = pattern_names[int(row0['PatternIdx'])]
    
    im_noisy = ax_noisy.imshow(noisy_img, cmap='gray_r', vmin=0, vmax=1)
    ax_noisy.set_title(f'Entrada (ruido {row0["RuidoPct"]}%)', fontweight='bold')
    ax_noisy.set_xticks([]); ax_noisy.set_yticks([])
    
    im_recov = ax_recov.imshow(recov_img, cmap='gray_r', vmin=0, vmax=1)
    ax_recov.set_title(f'Salida recuperada → Patrón {pat_name}', fontweight='bold')
    ax_recov.set_xticks([]); ax_recov.set_yticks([])
    
    ax_info.axis('off')
    info_text = ax_info.text(0.1, 0.5,
        f"Muestra: 0 / {len(df)-1}\n"
        f"Patrón objetivo: {pat_name}\n"
        f"Ruido inyectado: {row0['RuidoPct']}%\n"
        f"Recuperación: {row0['RecuperacionPct']:.1f}%\n"
        f"Energía final: {row0['Energia']:.2f}",
        transform=ax_info.transAxes, fontsize=11, verticalalignment='center',
        bbox=dict(boxstyle='round,pad=0.5', facecolor='wheat', alpha=0.9, edgecolor='black'))
    
    # Slider
    ax_slider = plt.axes([0.2, 0.02, 0.6, 0.03])
    slider = Slider(ax_slider, 'Muestra', 0, len(df)-1, valinit=0, valstep=1)
    
    def update(val):
        idx = int(slider.val)
        row = df.iloc[idx]
        pat_name = pattern_names[int(row['PatternIdx'])]
        
        im_noisy.set_data(_parse_pixels(row['NoisyPixels']))
        im_recov.set_data(_parse_pixels(row['RecoveredPixels']))
        ax_noisy.set_title(f'Entrada (ruido {row["RuidoPct"]}%)', fontweight='bold')
        ax_recov.set_title(f'Salida recuperada → Patrón {pat_name}', fontweight='bold')
        
        info_text.set_text(
            f"Muestra: {idx} / {len(df)-1}\n"
            f"Patrón objetivo: {pat_name}\n"
            f"Ruido inyectado: {row['RuidoPct']}%\n"
            f"Recuperación: {row['RecuperacionPct']:.1f}%\n"
            f"Energía final: {row['Energia']:.2f}")
        
        fig2.canvas.draw_idle()
    
    slider.on_changed(update)
    plt.show()
    print("✅ Ventana de Hopfield cerrada. Retornando a C++...")


# ============ DISPATCHER PRINCIPAL ============

def main():
    if len(sys.argv) < 2:
        print("Uso: python plot_auto.py <ruta_csv> [tipo_modelo]")
        print("  tipos: perceptron, adaline, mlp, kohonen, hopfield")
        sys.exit(1)
    
    csv_path = sys.argv[1]
    if not os.path.exists(csv_path):
        print(f"Error: Archivo no encontrado -> {csv_path}")
        sys.exit(1)
    
    df = pd.read_csv(csv_path)
    model = str(df['Modelo'].iloc[0]).strip()
    
    print(f"[DEBUG] Modelo detectado: '{model}'")
    print(f"[DEBUG] Columnas disponibles: {list(df.columns)}")
    
    if any(k in model for k in ['Perceptron', 'McCulloch']):
        plot_perceptron(df)
    elif 'ADALINE' in model:
        plot_adaline(df)
    elif any(k in model for k in ['MLP', 'Backprop']):
        plot_mlp(df)
    elif any(k in model for k in ['Kohonen', 'SOM']):
        plot_kohonen(df, csv_path)
    elif 'Hopfield' in model:
        plot_hopfield(df)
    else:
        print(f"Modelo '{model}' no soportado.")
        sys.exit(1)


if __name__ == "__main__":
    main()