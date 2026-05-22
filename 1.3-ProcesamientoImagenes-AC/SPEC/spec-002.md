
---

## SPEC-002: Script de Comparación Python — Detector de Bordes con Autómatas Celulares


## 1. Stack

Lenguaje: Python 3.8+
Dependencias: `pillow>=10.0.0`, `matplotlib>=3.7.0`, `numpy>=1.24.0`, `scipy>=1.10.0` (opcional para métricas avanzadas)
Sistema operativo: Windows 10/11 (x64), Linux, macOS
Gestor de paquetes: pip / requirements.txt

**Objetivo:** Proveer script de validación visual y métrica para comparar imagen original vs procesada por el detector de bordes.

**Restricción:** Script autocontenido, sin dependencias del código C++, opera sobre archivos PNG de entrada/salida.


## 2. Estructura de Archivos a Implementar

```
python/
├── comparar.py                 # Script principal de comparación visual y métrica
├── requirements.txt            # Dependencias Python para instalación
└── README.md                   # Instrucciones de uso y ejemplos
```


## 3. Conceptos Fundamentales


### a. Carga y Normalización de Imágenes

**Proceso:**

```
1. Abrir imagen con PIL/Pillow en modo 'L' (escala de grises, 0-255)
2. Convertir a array numpy para operaciones vectorizadas
3. Para métricas: binarizar con umbral 128 → 0 o 1
```

**Modo 'L' de PIL:** Garantiza array 2D (no 3D como RGB), simplifica operaciones.


### b. Métricas de Calidad de Bordes

| Métrica | Cálculo | Interpretación |
|---------|---------|----------------|
| **Total de bordes** | `np.sum(procesada_binaria)` | Píxeles blancos en salida |
| **Porcentaje de bordes** | `total_bordes / total_píxeles * 100` | Densidad relativa del contorno |
| **Componentes conectados** | `scipy.ndimage.label()` con estructura 3x3 | Número de regiones independientes |
| **Posible ruido** | Componentes de tamaño ≤ 2 píxeles | Artefactos aislados no deseados |


### c. Visualización Comparativa

**Layout de figura (matplotlib):**

```
[ Original ]  [ Procesada ]  [ Diferencia ]
   (gris)        (gris)      (RGB: R=borde, G=pérdida, Y=coincidencia)
```

**Código de colores para diferencia:**

| Color | RGB | Significado |
|-------|-----|-------------|
| 🔴 Rojo | `[255,0,0]` | Píxel blanco en procesada pero negro en original → borde detectado |
| 🟢 Verde | `[0,255,0]` | Píxel blanco en original pero negro en procesada → objeto perdido |
| 🟡 Amarillo | `[255,255,0]` | Píxel blanco en ambas → coincidencia correcta |
| ⚫ Negro | `[0,0,0]` | Fondo en ambas → correcto |


## 4. Algoritmo General del Script

```
ENTRADA:  Imagen original (entrada del AC) + Imagen procesada (salida del AC)
SALIDA:   Figura comparativa (matplotlib) o archivo PNG

1. Parsear argumentos de línea de comandos (-o, -p, -s opcional)
2. Validar existencia de archivos de entrada
3. Cargar ambas imágenes en escala de grises (modo 'L')
4. Si las dimensiones difieren: recortar al mínimo común
5. Binarizar ambas imágenes con umbral 128
6. Calcular métricas: bordes totales, porcentaje, componentes, ruido
7. Generar imagen de diferencia codificada por colores RGB
8. Crear figura con 3 paneles: Original, Procesada, Diferencia
9. Añadir texto con métricas en la parte inferior
10. Si se especifica -s: guardar como PNG (DPI 300)
    Si no: mostrar ventana interactiva matplotlib
11. Imprimir resumen de métricas en consola
```


## 5. Medidas de Calidad del Script

| Medida | Descripción | Cómo verificar |
|--------|-------------|----------------|
| **Robustez de carga** | Maneja PNG, JPG, con/sin alpha | Probar con múltiples formatos |
| **Manejo de dimensiones** | Recorta automáticamente si hay mismatch | Usar imágenes de tamaños distintos |
| **Métricas reproducibles** | Mismas entradas → mismos valores | Ejecutar 2 veces, comparar salida |
| **Salida visual clara** | Leyenda y colores intuitivos | Revisión visual por usuario |
| **Dependencias mínimas** | Funciona sin scipy (métricas básicas) | Desinstalar scipy, ejecutar script |


## 6. Variantes de Configuración

| Parámetro | Valores | Efecto |
|-----------|---------|--------|
| `--original`, `-o` | Ruta a PNG | Imagen de entrada del AC |
| `--procesada`, `-p` | Ruta a PNG | Imagen de salida del AC |
| `--salida`, `-s` | Ruta a PNG (opcional) | Guarda figura en lugar de mostrar ventana |
| `umbral` | 128 (hardcodeado) | Coherente con binarización C++ |


## 7. Ejemplo de Ejecución

```bash
# 1. Ejecutar detector C++ (desde build/)
./Release/detector_bordes.exe assets/test_formas.png resultados/salida.png silueta

# 2. Comparar resultados
python python/comparar.py -o assets/test_formas.png -p resultados/salida.png -s resultados/analisis.png

# 3. Ver resumen en consola
📥 Original: assets/test_formas.png
📥 Procesada: resultados/salida.png
🔍 Calculando métricas...
🎨 Generando comparativa...
✅ Guardado: resultados/analisis.png

📊 Métricas:
  • total_bordes: 342
  • porcentaje_bordes: 1.32%
  • componentes_conectados: 3
  • posible_ruido: 2
```


## 8. Aspectos a Evaluar

| Aspecto | Cómo se demuestra |
|---------|-------------------|
| **Instalación limpia** | `pip install -r requirements.txt` sin conflictos |
| **Argumentos validados** | Error claro si falta `-o` o `-p` |
| **Manejo de errores** | Mensaje legible si archivo no existe |
| **Visualización útil** | Panel de diferencia permite identificar falsos positivos/negativos |
| **Métricas accionables** | Valores numéricos permiten comparar configuraciones de reglas |


## 9. Evidencias Solicitadas

| Evidencia | Archivo/Formato |
|-----------|-----------------|
| Script principal | `python/comparar.py` |
| Dependencias | `python/requirements.txt` |
| Documentación | `python/README.md` |
| Ejemplo de salida | `resultados/comparativa.png` (generado por script) |
| Log de ejecución | Captura de consola con métricas impresas |


## 10. Referencias

1. **Pillow Documentation.** https://pillow.readthedocs.io/
   → `Image.open()`, `.convert('L')`, modos de color.

2. **Matplotlib Gallery.** https://matplotlib.org/stable/gallery/index.html
   → Subplots, `imshow()`, `fig.text()`.

3. **NumPy Indexing.** https://numpy.org/doc/stable/user/basics.indexing.html
   → Indexado booleano para codificación de colores.

4. **SciPy ndimage.** https://docs.scipy.org/doc/scipy/reference/ndimage.html
   → `label()` para componentes conectados (opcional).


## 11. Notas de Implementación

- **Modo 'L' de PIL:** Garantiza array 2D (no 3D como RGB), simplifica operaciones.
- **Umbral fijo 128:** Coherente con binarización en C++. Si se cambia en el detector, actualizar aquí.
- **scipy opcional:** El script funciona sin él; métricas avanzadas muestran `-1` si no está disponible.
- **DPI 300 para guardado:** Asegura calidad para informes o documentación.
- **Recorte automático:** Si las imágenes difieren en tamaño, se recorta al mínimo común en ambas dimensiones.


## 12. Criterios de Aceptación

- [ ] `comparar.py` se ejecuta con `python 3.8+` sin errores de sintaxis.
- [ ] Instalación de dependencias con `pip install -r requirements.txt` exitosa.
- [ ] Argumentos `-o` y `-p` requeridos; error claro si faltan.
- [ ] Maneja imágenes de dimensiones distintas recortando al mínimo común.
- [ ] Genera figura con 3 paneles: original, procesada, diferencia codificada por colores.
- [ ] Imprime métricas en consola: total_bordes, porcentaje, componentes, ruido.
- [ ] Opción `-s` guarda figura en PNG con DPI 300.
- [ ] Funciona sin `scipy` (métricas avanzadas muestran valor placeholder).

---
