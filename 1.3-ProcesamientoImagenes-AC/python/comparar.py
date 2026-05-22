#!/usr/bin/env python3
"""Script para comparar imágenes original y procesada con visualización y métricas."""

import argparse
import sys
from PIL import Image
import numpy as np

try:
    from scipy import ndimage
    HAS_SCIPY = True
except ImportError:
    HAS_SCIPY = False

def calcular_metricas(procesada_bin):
    metricas = {}
    metricas['total_bordes'] = int(np.sum(procesada_bin))
    metricas['porcentaje_bordes'] = float(metricas['total_bordes'] / procesada_bin.size * 100)
    
    if HAS_SCIPY:
        labeled, num = ndimage.label(procesada_bin, structure=np.ones((3, 3)))
        metricas['componentes_conectados'] = int(num)
        sizes = ndimage.sum(procesada_bin, labeled, range(1, num + 1))
        metricas['posible_ruido'] = int(np.sum(sizes <= 2))
    else:
        metricas['componentes_conectados'] = -1
        metricas['posible_ruido'] = -1
    
    return metricas

def generar_diferencia(orig, proc):
    orig_arr = np.array(orig)
    proc_arr = np.array(proc)
    
    orig_bin = (orig_arr >= 128).astype(np.uint8)
    proc_bin = (proc_arr >= 128).astype(np.uint8)
    
    h, w = orig_bin.shape
    diff = np.zeros((h, w, 3), dtype=np.uint8)
    
    diff[proc_bin == 1] = [255, 0, 0]
    diff[orig_bin == 1] = [0, 255, 0]
    diff[(orig_bin == 1) & (proc_bin == 1)] = [255, 255, 0]
    
    return diff, proc_bin

def crear_comparativa(orig, proc, diff):
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt
    
    fig, axes = plt.subplots(1, 3, figsize=(12, 4))
    
    axes[0].imshow(orig, cmap='gray')
    axes[0].set_title('Original')
    axes[0].axis('off')
    
    axes[1].imshow(proc, cmap='gray')
    axes[1].set_title('Procesada')
    axes[1].axis('off')
    
    axes[2].imshow(diff)
    axes[2].set_title('Diferencia')
    axes[2].axis('off')
    
    return fig

def main():
    parser = argparse.ArgumentParser(description='Comparar imágenes original y procesada')
    parser.add_argument('-o', '--original', required=True, help='Imagen original')
    parser.add_argument('-p', '--procesada', required=True, help='Imagen procesada')
    parser.add_argument('-s', '--salida', help='Guardar figura en archivo')
    args = parser.parse_args()
    
    try:
        orig = Image.open(args.original).convert('L')
        proc = Image.open(args.procesada).convert('L')
    except Exception as e:
        print(f"Error al cargar imágenes: {e}")
        sys.exit(1)
    
    print(f"📥 Original: {args.original}")
    print(f"📥 Procesada: {args.procesada}")
    
    min_w = min(orig.width, proc.width)
    min_h = min(orig.height, proc.height)
    orig = orig.resize((min_w, min_h)) if orig.size != (min_w, min_h) else orig
    proc = proc.resize((min_w, min_h)) if proc.size != (min_w, min_h) else proc
    
    print("🔍 Calculando métricas...")
    diff, proc_bin = generar_diferencia(orig, proc)
    metricas = calcular_metricas(proc_bin)
    
    print("🎨 Generando comparativa...")
    fig = crear_comparativa(orig, proc, diff)
    
    if args.salida:
        fig.savefig(args.salida, dpi=300, bbox_inches='tight')
        print(f"✅ Guardado: {args.salida}")
    else:
        import matplotlib.pyplot as plt
        plt.show()
    
    print("\n📊 Métricas:")
    for k, v in metricas.items():
        print(f"  • {k}: {v}")
    
    import matplotlib.pyplot as plt
    plt.close(fig)
    
    return 0

if __name__ == "__main__":
    sys.exit(main())