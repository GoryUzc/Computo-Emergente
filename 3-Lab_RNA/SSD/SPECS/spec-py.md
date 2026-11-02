# Especificación implementada: visualización Python

**Archivo:** `python/plot_auto.py`  
**Entrada:** Ruta a un CSV generado por un módulo C++.  
**Dependencias declaradas:** `python/requeriment.txt` (`pandas>=1.3.0`, `matplotlib>=3.6.0`). NumPy es usado por el script y normalmente se instala como dependencia de pandas/Matplotlib.

## Flujo de ejecución

1. Recibe la ruta del CSV como primer argumento (`python plot_auto.py <ruta_csv>`). El texto de ayuda menciona un argumento opcional de tipo, pero el código no lo usa.
2. Comprueba que se haya pasado una ruta y que el archivo exista.
3. Lee el CSV con pandas, obtiene el primer valor de la columna `Modelo` y muestra `[DEBUG] Modelo detectado` y `[DEBUG] Columnas disponibles`.
4. Selecciona una gráfica por coincidencia de texto con el nombre del modelo.
5. Las ventanas se muestran de manera interactiva mediante `plt.show()`. En Kohonen y Hopfield hay más de una ventana y la ejecución continúa tras cerrar cada una.

El despachador no valida previamente todas las columnas específicas ni el caso de CSV vacío; las rutinas de gráfica acceden directamente a las columnas que necesitan. Un formato de entrada inválido puede producir una excepción de pandas o Matplotlib.

## Formatos CSV y gráficas

| Modelo detectado | Columnas esperadas por la gráfica | Visualización implementada |
|---|---|---|
| `McCulloch_Perceptron` | `Modelo, Epoca, Error`; opcional `Aciertos` | Error por época. Si existe `Aciertos`, eje secundario con aciertos de cuatro. |
| `ADALINE` | `Modelo, Epoca, MSE`; opcional `MAE` | MSE en escala logarítmica. Si existe `MAE`, se dibuja en eje Y secundario. |
| `MLP_Backprop` | `Modelo, Epoca, Error` | Error de entrenamiento en escala logarítmica. |
| `Kohonen_SOM` | `Modelo, Epoca, QuantizationError`; opcionales `Sigma`, `LearningRate` | QE por época y, si existen ambas columnas de parámetros, una gráfica de su decaimiento. Luego intenta abrir el visor de pesos. |
| `Hopfield_Net` | `Modelo, Epoca, Energia, RuidoPct, RecuperacionPct`; opcionales `PatternIdx`, `NoisyPixels`, `RecoveredPixels` | Promedio de recuperación por nivel de ruido y energía de las primeras 50 filas. Si están las tres columnas opcionales, slider de patrones ruidosos/recuperados. |

Nombres y columnas corresponden a los CSV actuales de los módulos. El modelo se detecta por subcadena: `Perceptron`/`McCulloch`, `ADALINE`, `MLP`/`Backprop`, `Kohonen`/`SOM` o `Hopfield`.

## Visores interactivos de Kohonen y Hopfield

### Kohonen

El CSV de métricas no incluye coordenadas de pesos. El script busca `kohonen_snapshots.csv` en el mismo directorio que el CSV de métricas. El formato esperado es:

```csv
Modelo,SnapshotEpoch,NeuronIdx,WeightX,WeightY
```

Si el archivo no existe, se omite el visor; si solo hay un valor de época de snapshot, también. Con al menos dos épocas, muestra los datos sintéticos y un slider para cambiar los pesos visibles entre snapshots.

### Hopfield

El script requiere `PatternIdx`, `NoisyPixels` y `RecoveredPixels` para abrir el visor de muestras. Cada cadena codifica 42 celdas: `1` se muestra como píxel activo y `-` como inactivo. Si faltan las columnas, se informa y se omite esta ventana.

## Presentación y operación

- Estilo: `seaborn-v0_8-whitegrid`; paleta colorblind-friendly azul, naranja, verde, rojo, morado y gris.
- Algunas rutinas incluyen un `plt.show()` por figura. Kohonen y Hopfield muestran sus gráficas de resumen y luego, cuando corresponde, el visor de slider.
- No se guarda una imagen automáticamente.
- El proceso Python es invocado por `RNA_Base::trigger_plot()` al finalizar el CSV. En Windows, el comando usa `python .\python\plot_auto.py "<ruta>"`; el proceso C++ espera la ejecución y reporta el código de retorno.
- El programa C++ comprueba al inicio que se ejecute desde la raíz del repositorio, donde están `CMakeLists.txt` y `python/plot_auto.py`.

## Diferencias frente a especificaciones previas

- El script actual no imprime un resumen completo del CSV ni usa una validación común de esquema; solo comprueba la ruta y luego consulta el contenido requerido por cada gráfica.
- Perceptrón y ADALINE admiten columnas opcionales, y Kohonen/Hopfield tienen visualizadores de snapshots/patrones que no aparecían en la documentación anterior.
- La gráfica de ADALINE usa dos ejes sobre una figura, no dos subplots.
- El error de MLP se grafica como una serie simple semilogarítmica.
- `numpy` no aparece explícitamente en `requeriment.txt`, aunque se importa directamente en el script.

## Criterios de comprobación

- Probar cada CSV generado por las opciones 1–5; comprobar selección del modelo y columnas disponibles impresas.
- Verificar las ventanas de resumen y los visores opcionales al cerrar cada ventana.
- Para Kohonen, generar al menos dos snapshots; para Hopfield, comprobar que el CSV contiene las tres columnas de píxeles/índice.
- Ejecutar desde la raíz del proyecto, que es el directorio requerido por el CLI y por las rutas relativas.
