# Especificación implementada: perceptrón multicapa

**Archivos:** `src/mlp_backprop.hpp`, `src/mlp_backprop.cpp`  
**Salida:** `output/rna_model_3.csv`

## Tareas y selección

El módulo soporta dos tareas, seleccionadas en el constructor por `RNA_Params::extra_1`:

- `extra_1 < 1.5` (incluido el valor predeterminado): XOR, arquitectura `2-2-1`, sigmoide en capa oculta y salida.
- `extra_1 >= 1.5`: aproximación de sin(x), arquitectura `1-10-1`, con 50 entradas equiespaciadas en `[0,2π]`, targets escalados de `[-1,1]` a `[0.1,0.9]`, sigmoide oculta y salida lineal.

El CLI actual no presenta selector y construye `RNA_Params` con `extra_1=0`, por lo que siempre selecciona XOR desde el menú.

## Entrenamiento

Los pesos se inicializan usando valores generados con semilla 42 y distribución normal de media 0 y desviación estándar 0.5; los biases y deltas previos comienzan en cero. El entrenamiento recorre las muestras en orden fijo y actualiza online mediante backpropagation. La tasa de aprendizaje se toma del CLI.

La actualización con momento usa `beta = 0.1` en `train_epoch()`. El resumen teórico del código todavía menciona `beta=0.9`; ese texto no refleja la constante efectiva de entrenamiento. La salida de la tarea sin(x) es lineal y sus targets se mantienen escalados durante la optimización.

El valor retornado por época es el MSE medio de las salidas calculadas durante el recorrido. El motor común usa ese valor y el umbral ingresado para decidir la parada. No hay un decaimiento de tasa implementado.

## Métricas y CSV

El CSV de ambas tareas tiene la cabecera `Modelo,Epoca,Error`.

- XOR: se informa MSE, tabla final con decisión `y > 0.5` y exactitud sobre cuatro casos.
- sin(x): se informa MSE en dominio escalado y una estimación de RMSE en dominio original. El CSV contiene el error escalado por época, no predicciones ni errores individuales.

El graficador representa `Error` por época con eje Y logarítmico.

## Diferencias con el contrato anterior

- El selector se encuentra en `extra_1`, pero no se configura desde el menú.
- El factor de momento implementado es `0.1`, aunque el resumen teórico de `get_theory_summary()` anuncia `0.9`.
- `learning_rate` viene del CLI; no hay decay configurado.
- El CSV solo exporta una columna de error adicional a modelo/época; no exporta resultados o predicciones de sin(x).
- Los criterios de exactitud o MSE descritos previamente no son umbrales garantizados por el código.

## Comprobación

Ejecutar la opción 3 para validar el flujo XOR y el CSV de tres columnas. Para validar sin(x), construir el módulo con `extra_1 >= 1.5`; no existe una opción CLI actual para seleccionarlo.
