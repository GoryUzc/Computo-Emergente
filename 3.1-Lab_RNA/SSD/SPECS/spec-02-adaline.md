# Especificación implementada: ADALINE

**Archivos:** `src/adaline.hpp`, `src/adaline.cpp`  
**Salida:** `output/rna_model_2.csv`

## Contrato

ADALINE aproxima el descodificador binario de tres entradas a decimal: los ocho patrones de `000` a `111` corresponden a `0`–`7`, es decir, `d = 4x1 + 2x2 + x3`. Usa tres pesos sin bias y salida lineal `y = w·x`; no aplica umbral durante el entrenamiento. Los pesos iniciales son uniformes en `[0,1]` con semilla 42.

## Entrenamiento y métricas

Cada época recorre los ocho patrones en orden fijo. Para cada patrón calcula el error antes de actualizar los pesos mediante `w += learning_rate * (d-y) * x`. El retorno por época es `MSE = sum(error²)/8`, sin factor `1/2`; además se calcula `MAE = sum(abs(error))/8`.

El motor común aplica el criterio de parada con MSE y `error_threshold`, además de `max_epochs`. `compute_metrics()` compara los pesos finales con `[4,2,1]` e informa la desviación absoluta máxima; la salida imprime éxito solo si esta es menor que `0.05`. La convergencia depende de la tasa y del límite ingresados, no se garantiza para cualquier configuración.

## CSV y gráficas

Cabecera: `Modelo,Epoca,MSE,MAE`. El graficador representa MSE con escala logarítmica y MAE en un segundo eje vertical si la columna está disponible.

## Diferencias con el contrato anterior

- La fórmula implementada acumula error cuadrático completo y promedia por ocho; no multiplica cada término por `1/2`.
- La tasa (`gamma`) y el criterio de parada no están fijados a valores del documento anterior: el menú proporciona `learning_rate`, `max_epochs` y `error_threshold`.
- Las métricas leen el último elemento de los historiales; con el rango mínimo del CLI se entrena al menos una época.

## Comprobación

Ejecutar la opción 2 y revisar las cuatro columnas del CSV. La comparación con `[4,2,1]` es informativa; interpretar el estado de convergencia considerando los valores efectivos solicitados por consola.
