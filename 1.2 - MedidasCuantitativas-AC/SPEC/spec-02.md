## SPEC-002: Script de Visualización para Medidas Cuantitativas AC


## 1. Stack:
Lenguaje: Python 3.10+
Dependencias: pandas, matplotlib (YA INSTALADAS)
Sistema operativo: Windows 10/11 (x64)

**Objetivo**: Leer los CSV generados por el programa C++ y producir gráficas comparativas profesionales para la evidencia de la actividad.

**Estado actual:**
- ✅ Programa C++ compilado y ejecutado
- ✅ Archivos CSV en `resultados/regla_*_metricas.csv`
- ✅ Dependencias Python instaladas


## 2. Estructura de Carpetas

```
1.2-Medidas-Cuantitativas-AC/
├── src/                          # Código C++ (ya implementado)
├── resultados/                   # CSV generados por C++
│   ├── regla_0_metricas.csv
│   ├── regla_30_metricas.csv
│   ├── regla_90_metricas.csv
│   ├── regla_110_metricas.csv
│   └── regla_184_metricas.csv
├── python/                       # ← ← CARPETA OBJETIVO
│   └── graficar.py               # ← ← SCRIPT A IMPLEMENTAR
└── docs/
```


## 3. Dependencias (YA INSTALADAS)

| Librería | Estado | Verificación |
|----------|--------|--------------|
| `pandas` | ✅ Instalada | `python -c "import pandas; print(pandas.__version__)"` |
| `matplotlib` | ✅ Instalada | `python -c "import matplotlib; print(matplotlib.__version__)"` |

> No es necesario volver a instalar.


## 4. Formato de Entrada (CSV)

Los archivos CSV tienen exactamente este formato:

```csv
generacion,densidad,entropia_espacial,hamming_vs_inicial
0,0.005000,0.045420,0.000000
1,0.010000,0.089000,0.005000
...
499,0.495000,0.999912,0.490000
```

| Columna | Tipo | Descripción |
|---------|------|-------------|
| `generacion` | int | Índice temporal t (0 a 499) |
| `densidad` | float | ρ(t) ∈ [0, 1] |
| `entropia_espacial` | float | H_esp(t) ∈ [0, 1] bits |
| `hamming_vs_inicial` | float | h(S(t), S(0)) ∈ [0, 1] |

**Convención:** `regla_{NUMERO}_metricas.csv`


## 5. Gráficas a Implementar


### a. Dashboard Combinado (3 subplots)

**Descripción:** Una sola figura con 3 gráficos apilados verticalmente, compartiendo eje X.

**Subplot 1 - Densidad:**
- Título: "Evolución de la Densidad"
- Eje Y: "Densidad ρ(t)" (rango: 0 a 1)
- 5 líneas (una por regla), colores distintos, leyenda

**Subplot 2 - Entropía Espacial:**
- Título: "Entropía Espacial de Shannon"
- Eje Y: "Entropía (bits)" (rango: 0 a 1.1)
- Línea horizontal punteada en y=1.0 (máximo teórico)

**Subplot 3 - Hamming:**
- Título: "Divergencia del Estado Inicial"
- Eje Y: "Hamming normalizado" (rango: 0 a 1)
- Eje X compartido: "Generación"

**Especificaciones comunes:**
- Tamaño: 12 × 14 pulgadas
- Título general: "Medidas Cuantitativas - Autómatas Celulares 1D"
- Grid ligero (alpha=0.3)
- Líneas: grosor 1.5, estilo sólido
- Guardar como: `resultados/comparativa_dashboard.png`, 150 DPI

**Colores por regla:**

| Regla | Color | Clase | Razón |
|-------|-------|-------|-------|
| 0 | `#888888` (Gris) | I | Muerte |
| 30 | `#E74C3C` (Rojo) | III | Caos |
| 90 | `#3498DB` (Azul) | III | Caos |
| 110 | `#2ECC71` (Verde) | IV | Complejidad |
| 184 | `#F39C12` (Naranja) | II | Periódico |


### b. Barras - Parámetro de Langton

**Descripción:** Gráfico de barras comparando λ para cada regla.

**Especificaciones:**
- Tipo: barras verticales
- Título: "Parámetro de Langton (λ) por Regla"
- Eje X: "Regla" (0, 30, 90, 110, 184)
- Eje Y: "λ" (rango: 0 a 1)
- Línea horizontal punteada roja en λ=0.5 (zona de complejidad)
- Colores de barras según clase de Wolfram (misma paleta arriba)
- Valores anotados sobre cada barra
- Guardar como: `resultados/langton_por_regla.png`, 150 DPI

**Datos a hardcodear:**

| Regla | λ | Clase | Color barra |
|-------|---|-------|-------------|
| 0 | 0.000 | I | `#888888` |
| 30 | 0.500 | III | `#E74C3C` |
| 90 | 0.500 | III | `#3498DB` |
| 110 | 0.625 | IV | `#2ECC71` |
| 184 | 0.500 | II | `#F39C12` |


## 6. Algoritmo del Script

```python
ENTRADA:  Carpeta resultados/ con archivos regla_*_metricas.csv
SALIDA:   Archivos PNG en resultados/

1. Importar pandas, matplotlib.pyplot, glob, os

2. CONFIGURACIÓN (diccionarios globales):
   COLORES = {
       0: '#888888', 30: '#E74C3C', 90: '#3498DB',
       110: '#2ECC71', 184: '#F39C12'
   }
   CLASES_WOLFRAM = {
       0: 'I', 30: 'III', 90: 'III', 110: 'IV', 184: 'II'
   }
   DATOS_LANGTON = {
       0: 0.0, 30: 0.5, 90: 0.5, 110: 0.625, 184: 0.5
   }

3. FUNCION cargar_datos():
       archivos = glob.glob('resultados/regla_*_metricas.csv')
       SI archivos ESTÁ VACÍO:
           Imprimir: "[ERROR] No se encontraron CSV en resultados/"
           Imprimir: "Ejecuta primero: .\\build\\Release\\medidas.exe"
           Salir con código 1

       datos = diccionario vacío
       PARA CADA archivo EN sorted(archivos):
           nombre = os.path.basename(archivo)       # "regla_30_metricas.csv"
           partes = nombre.split('_')                # ["regla", "30", "metricas.csv"]
           regla = int(partes[1])                    # 30
           df = pandas.read_csv(archivo)
           datos[regla] = df

       RETORNAR datos

4. FUNCION graficar_dashboard(datos):
       fig, axes = plt.subplots(3, 1, figsize=(12, 14), sharex=True)
       fig.suptitle('Medidas Cuantitativas - Autómatas Celulares 1D',
                    fontsize=16, fontweight='bold')

       # Subplot 0: Densidad
       PARA CADA (regla, df) EN datos.items():
           axes[0].plot(df['generacion'], df['densidad'],
                        color=COLORES[regla], 
                        label=f'Regla {regla} ({CLASES_WOLFRAM[regla]})',
                        linewidth=1.5)
       axes[0].set_ylabel('Densidad ρ(t)')
       axes[0].set_ylim(0, 1)
       axes[0].grid(True, alpha=0.3)
       axes[0].legend(loc='upper right', fontsize=9)
       axes[0].set_title('Evolución de la Densidad')

       # Subplot 1: Entropía Espacial
       PARA CADA (regla, df) EN datos.items():
           axes[1].plot(df['generacion'], df['entropia_espacial'],
                        color=COLORES[regla], linewidth=1.5)
       axes[1].axhline(y=1.0, color='black', linestyle='--', alpha=0.4,
                       label='Máximo teórico (1 bit)')
       axes[1].set_ylabel('Entropía (bits)')
       axes[1].set_ylim(0, 1.1)
       axes[1].grid(True, alpha=0.3)
       axes[1].legend(loc='lower right', fontsize=9)
       axes[1].set_title('Entropía Espacial de Shannon')

       # Subplot 2: Hamming
       PARA CADA (regla, df) EN datos.items():
           axes[2].plot(df['generacion'], df['hamming_vs_inicial'],
                        color=COLORES[regla], linewidth=1.5)
       axes[2].set_ylabel('Hamming normalizado')
       axes[2].set_xlabel('Generación')
       axes[2].set_ylim(0, 1)
       axes[2].grid(True, alpha=0.3)
       axes[2].set_title('Divergencia del Estado Inicial')

       plt.tight_layout()
       plt.savefig('resultados/comparativa_dashboard.png', dpi=150, bbox_inches='tight')
       plt.close(fig)
       Imprimir: "[OK] Dashboard guardado: resultados/comparativa_dashboard.png"

5. FUNCION graficar_langton():
       fig, ax = plt.subplots(figsize=(10, 6))

       reglas = [0, 30, 90, 110, 184]
       lambdas = [DATOS_LANGTON[r] for r in reglas]
       colores_barras = [COLORES[r] for r in reglas]
       etiquetas = [f'Regla {r}\\n(Clase {CLASES_WOLFRAM[r]})' for r in reglas]

       ax.bar(etiquetas, lambdas, color=colores_barras, edgecolor='black', alpha=0.85)
       ax.axhline(y=0.5, color='red', linestyle='--', linewidth=2,
                  label='Zona de complejidad (λ = 0.5)')

       ax.set_title('Parámetro de Langton (λ) por Regla', 
                    fontsize=14, fontweight='bold')
       ax.set_ylabel('λ', fontsize=12)
       ax.set_ylim(0, 1)
       ax.grid(True, axis='y', alpha=0.3)
       ax.legend(fontsize=10)

       # Anotaciones de valores sobre barras
       PARA i, val EN enumerate(lambdas):
           ax.text(i, val + 0.03, f'{val:.3f}', 
                   ha='center', fontsize=11, fontweight='bold')

       plt.tight_layout()
       plt.savefig('resultados/langton_por_regla.png', dpi=150, bbox_inches='tight')
       plt.close(fig)
       Imprimir: "[OK] Gráfico Langton guardado: resultados/langton_por_regla.png"

6. FUNCION main():
       Imprimir: "=== Generador de Gráficas - Medidas Cuantitativas AC ==="

       datos = cargar_datos()
       Imprimir: f"Cargados {len(datos)} archivos CSV"

       graficar_dashboard(datos)
       graficar_langton()

       Imprimir: "\\n=== Todas las gráficas generadas correctamente ==="
       Imprimir: "Revisa la carpeta resultados/"

7. SI __name__ == '__main__':
       main()
```


## 7. Ejemplo de Ejecución

```powershell
# 1. Navegar al proyecto
PS C:\> cd "C:\Users\User\OneDrive\Desktop\Ejercicios dev\Computo Emergente\1.2-Medidas-Cuantitativas-AC"

# 2. Verificar CSV existen
PS ...> ls resultados\
Mode                 LastWriteTime         Length Name
----                 -------------         ------ ----
-a----        17/05/2026     14:30          28543 regla_0_metricas.csv
-a----        17/05/2026     14:30          28543 regla_30_metricas.csv
-a----        17/05/2026     14:30          28543 regla_90_metricas.csv
-a----        17/05/2026     14:30          28543 regla_110_metricas.csv
-a----        17/05/2026     14:30          28543 regla_184_metricas.csv

# 3. Ejecutar script Python
PS ...> python python\graficar.py
=== Generador de Gráficas - Medidas Cuantitativas AC ===
Cargados 5 archivos CSV
[OK] Dashboard guardado: resultados/comparativa_dashboard.png
[OK] Gráfico Langton guardado: resultados/langton_por_regla.png

=== Todas las gráficas generadas correctamente ===
Revisa la carpeta resultados/

# 4. Verificar imágenes
PS ...> ls resultados\*.png
Mode                 LastWriteTime         Length Name
----                 -------------         ------ ----
-a----        17/05/2026     14:35         245832 comparativa_dashboard.png
-a----        17/05/2026     14:35          89234 langton_por_regla.png

# 5. Abrir
PS ...> start resultados\comparativa_dashboard.png
PS ...> start resultados\langton_por_regla.png
```


## 8. Entregables

| Archivo | Ruta | Estado |
|---------|------|--------|
| `graficar.py` | `python/graficar.py` | Implementado |
| `comparativa_dashboard.png` | `resultados/comparativa_dashboard.png` | Generado (497 KB, 150 DPI) |
| `langton_por_regla.png` | `resultados/langton_por_regla.png` | Generado (42 KB, 150 DPI) |


## Historial de Cambios

| Fecha | Versión | Descripción |
|-------|---------|-------------|
| 2026-05-17 | 1.0 | Implementación inicial del script de visualización según SPEC-002. |


## 9. Criterios de Aceptación

- [x] Script lee correctamente los 5 CSV en `resultados/`.
- [x] Genera `comparativa_dashboard.png` con 3 subplots apilados y leyenda.
- [x] Genera `langton_por_regla.png` con barras, valores anotados, y línea en λ=0.5.
- [x] Colores coinciden con la especificación por clase de Wolfram.
- [x] Gráficas tienen títulos, ejes etiquetados, grid ligero, resolución 150 DPI.
- [x] Si no hay CSV, muestra mensaje útil indicando ejecutar C++ primero.
- [x] Las imágenes son aptas para incluir en reporte académico.


## REFERENCIAS

1. Matplotlib Documentation. https://matplotlib.org/stable/
2. Pandas Documentation. https://pandas.pydata.org/docs/
3. Wolfram, S. (2002). A New Kind of Science. Wolfram Media.
4. Langton, C. G. (1990). "Computation at the edge of chaos." Physica D, 42(1-3), 12-37.

