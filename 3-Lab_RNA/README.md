# Laboratorio de Redes Neuronales Artificiales

Aplicación de consola en C++17 para explorar cinco modelos neuronales. Cada ejecución muestra un resumen teórico y métricas, exporta los resultados a CSV y abre gráficas interactivas con Python.

## Requisitos

- CMake 3.16 o posterior.
- Compilador C++17.
- Eigen3 disponible para CMake (`Eigen3 CONFIG`).
- Python 3 y `pip`.
- Paquetes listados en `python/requeriment.txt`:

```powershell
python -m pip install -r .\python\requeriment.txt
```

## Compilar en Windows

Desde la carpeta del proyecto:

```powershell
cmake -B build -S .
cmake --build build --config Release
```

## Ejecutar

Ejecuta el programa desde la **raíz del proyecto**; el CLI valida desde allí las rutas relativas a `CMakeLists.txt` y `python/plot_auto.py`.

```powershell
.\build\Release\Lab_RNA.exe
```

En cada modelo se solicitan tasa de aprendizaje (`0.001–1.0`), máximo de épocas/pruebas (`10–100000`) y umbral (`1e-8–1.0`). Su efecto depende del módulo. El menú ofrece:

| Opción | Modelo | Comportamiento |
|---|---|---|
| 1 | McCulloch-Pitts / perceptrón | Clasificación AND con los valores predeterminados actuales. |
| 2 | ADALINE | Descifrador de 3 bits a decimal (0–7). |
| 3 | MLP / backpropagation | Clasificación XOR. |
| 4 | Kohonen SOM | Mapa 10×10 sobre cuatro clusters sintéticos bidimensionales. |
| 5 | Hopfield | Recuperación de cuatro patrones (A–D) de 7×6 píxeles con ruido del 10 %, 20 % y 30 %. |
| 0 | Salir | Termina la aplicación. |

El código de perceptrón contempla OR y el de MLP contempla aproximación de sin(x), pero el menú actual no ofrece controles para seleccionar esas tareas: `extra_1` permanece en su valor por defecto.

## Resultados y visualización

Los resultados se escriben en `output/rna_model_<opción>.csv`. Kohonen genera además `output/kohonen_snapshots.csv`, con pesos iniciales y snapshots periódicos para su visor interactivo. Hopfield incluye el índice de patrón y las cadenas de píxeles ruidosas/recuperadas para navegar por sus pruebas.

El motor invoca automáticamente el script Python con la ruta del CSV. Las gráficas son interactivas y no se guardan como imágenes; cierra las ventanas para regresar al CLI. Para ejecutar el graficador manualmente:

```powershell
python .\python\plot_auto.py .\output\rna_model_4.csv
```

Si el comando `python` no está disponible, instala Python o ajusta el comando utilizado por `RNA_Base::trigger_plot()`. El script también puede ejecutarse manualmente con `py` en Windows.

## Documentación técnica

- [Arquitectura de diseño](SSD/ARCH_DESING.md)
- [Especificación del motor y CLI](SSD/SPECS/spec-core.md)
- [Especificación del perceptrón](SSD/SPECS/spec-01-mcculloch-perceptron.md)
- [Especificación de ADALINE](SSD/SPECS/spec-02-adaline.md)
- [Especificación del MLP](SSD/SPECS/spec-03-mlp-backprop.md)
- [Especificación de Kohonen SOM](SSD/SPECS/spec-04-kohonen-som.md)
- [Especificación de Hopfield](SSD/SPECS/spec-05-hopfield.md)
- [Especificación del graficador Python](SSD/SPECS/spec-py.md)

Las especificaciones documentan el comportamiento implementado, incluidos los límites y diferencias respecto a requisitos de diseño anteriores.
