# Especificación implementada: Kohonen SOM

**Archivos:** `src/kohonen_som.hpp`, `src/kohonen_som.cpp`  
**Integración visual:** `python/plot_auto.py`  
**Estado:** Contrato documentado a partir de la implementación actual.

## Propósito y contrato

El módulo implementa un mapa autoorganizado no supervisado para datos sintéticos bidimensionales. El tamaño del mapa y los datos no son parámetros del CLI: se usan 10 × 10 neuronas y 200 muestras distribuidas en cuatro grupos gaussianos. Las entradas y los pesos están en `[0, 1]²`; los datos se generan con semilla 42 y se recortan a ese intervalo.

La clase `KohonenSOM` extiende `RNA_Base` e implementa `initialize`, `train_epoch`, `compute_metrics` y `export_csv`. Cada época procesa las 200 muestras en el orden generado:

1. Encuentra la BMU minimizando la distancia euclidiana al peso de cada neurona.
2. Calcula la vecindad gaussiana con distancias cuadradas precalculadas sobre la malla.
3. Actualiza todos los pesos hacia la muestra, ponderados por la vecindad.
4. Cuenta la activación de la BMU y acumula el error de cuantización.

La malla se aplana en índices de `i * map_size + j`. Para una muestra, el BMU se elige antes de actualizar los pesos; el QE de esa muestra se mide después de esa actualización usando el BMU ya elegido.

## Parámetros y entrenamiento

- `map_size = 10`; `weights_` contiene 100 vectores bidimensionales.
- Pesos iniciales: uniforme en `[0, 1]`, semilla 42.
- Centros de datos: `(0.2, 0.2)`, `(0.8, 0.2)`, `(0.2, 0.8)` y `(0.8, 0.8)`, 50 muestras por centro, desviación estándar `0.06`, semilla 42.
- `sigma_` parte de `5.0`; la tasa inicial efectiva es `params_.learning_rate` (la solicita el CLI), no un `0.5` fijo.
- `tau = params_.max_epochs / ln(5)`. Al inicio de la época `t` (índice interno desde cero): `eta(t) = learning_rate * exp(-t/tau)` y `sigma(t) = 5 * exp(-t/tau)`.
- El umbral de error común del motor puede terminar el entrenamiento si el QE devuelto está entre cero y `error_threshold`. De lo contrario se ejecuta hasta `max_epochs`.
- La métrica devuelta y almacenada por época es el QE medio, no MSE. `compute_metrics` informa el QE final y el número de neuronas que fueron BMU al menos una vez.

La etiqueta `ALTA` de preservación topológica que se imprime corresponde únicamente a más del 60 % de neuronas activadas. No es un cálculo independiente de preservación topológica ni garantiza que los cuatro grupos formen regiones visuales correctas.

## Archivos de salida

`output/rna_model_4.csv`:

```csv
Modelo,Epoca,QuantizationError,Sigma,LearningRate
Kohonen_SOM,1,0.045342,5.000000,0.001000
```

Los valores de ejemplo son ilustrativos; la primera fila de datos registra los parámetros utilizados en la primera época. El número de filas depende de la parada del motor.

`output/kohonen_snapshots.csv` contiene los pesos para visualizar su evolución:

```csv
Modelo,SnapshotEpoch,NeuronIdx,WeightX,WeightY
Kohonen_SOM,0,0,0.123456,0.654321
```

`initialize()` vuelve a crear este archivo con el estado inicial (época 0). Se añaden snapshots al finalizar los índices internos 100, 200, etc. El contador de snapshot es el índice interno de época; por ello no es idéntico al número de época presentado en el CSV, que empieza en 1.

## Visualización

El despachador Python reconoce `Kohonen_SOM` por su columna `Modelo` y presenta:

1. QE por época y decaimiento de `Sigma` y `LearningRate`.
2. Si `kohonen_snapshots.csv` existe y contiene al menos dos épocas distintas, una segunda ventana con slider para explorar los pesos y los cuatro grupos sintéticos.

Si no hay suficientes snapshots, el script informa que omite el slider. En ejecuciones de 100 épocas o menos, normalmente solo existe el snapshot inicial.

## Diferencias relevantes frente al diseño anterior

- La tasa inicial se toma del valor de aprendizaje ingresado en el CLI; el `0.5` del constructor es un valor inicial del campo y se reemplaza al entrenar.
- Los snapshots y el slider de evolución son parte de la implementación actual.
- El CSV de métricas tiene cinco columnas; los snapshots están en un segundo CSV separado.
- Las épocas y el umbral se heredan del motor común, no se fijan en 1000.
- La condición de activación de más del 60 % es la heurística mostrada en consola, no una validación cuantitativa de topología.

## Verificación del contrato

- Compilar con el target CMake `Lab_RNA`.
- Ejecutar la opción 4 desde la raíz del proyecto y comprobar ambas cabeceras CSV.
- Para comprobar el slider, ejecutar más de 100 épocas y confirmar que se guardan al menos los snapshots 0 y 100.
- No asumir que el QE desciende monótonamente ni usar los snapshots como prueba de una topología específica.
