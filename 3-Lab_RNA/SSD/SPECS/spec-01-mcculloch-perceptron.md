# Especificación implementada: McCulloch-Pitts / perceptrón simple

**Archivos:** `src/mcculloch_perceptron.hpp`, `src/mcculloch_perceptron.cpp`  
**Salida:** `output/rna_model_1.csv`

## Contrato

El módulo implementa un perceptrón binario con sesgo explícito (`x0 = 1`) y cuatro patrones de dos entradas. La activación es `H(z) = 1` si `z >= 0`, y `0` en caso contrario. Los pesos se inicializan con distribución uniforme `[0,1]` y semilla 42. El código implementa dos tablas lógicas:

- AND, por defecto.
- OR si `RNA_Params::extra_1 > 0.5`. El CLI actual no ofrece selector, así que ejecuta AND.

McCulloch-Pitts aparece en el nombre y en el resumen teórico como referencia de umbral fijo; los pesos entrenables del programa siguen la regla del perceptrón.

## Entrenamiento y métricas

Para cada patrón, en orden fijo, calcula la salida usando los pesos actuales, evalúa el error binario y actualiza online con `w += learning_rate * (d - y) * x`. La métrica de época es `MSE = sum(error²)/4`; también guarda el conteo de aciertos por época. El motor común termina por límite o cuando el retorno está entre cero y el umbral solicitado.

Las métricas muestran pesos finales, frontera lineal y predicciones finales. El texto de consola añade literalmente `(100%)` junto al conteo de aciertos, incluso si el conteo no fuera cuatro; considerar el conteo `/4` como el valor informativo. No se valida separabilidad mediante un algoritmo aparte.

## CSV y gráficas

Cabecera: `Modelo,Epoca,Error,Aciertos`. El graficador usa el error por época y el conteo de aciertos en un eje secundario cuando existe la columna `Aciertos`.

## Diferencias con el contrato anterior

- Los archivos de cabecera viven en `src/`, no en `include/`.
- El selector AND/OR existe en `extra_1`, pero no está conectado a entradas del CLI.
- El umbral y la tasa son parámetros compartidos ingresados por el usuario; no hay garantía de converger en diez épocas ni configuración obligatoria `eta=0.1`.
- La capacidad reportada no debe confundirse con una implementación separada de McCulloch-Pitts con pesos fijos.

## Comprobación

Ejecutar la opción 1 desde la raíz del proyecto y comprobar las cuatro columnas y que `Aciertos` esté entre 0 y 4. Para ejercitar OR hace falta crear `RNA_Params` con `extra_1 > 0.5`; el menú actual no lo permite.
