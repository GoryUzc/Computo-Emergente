# Arquitectura de diseño — Laboratorio de RNA

**Estado:** Descripción de la implementación existente.  
**Lenguaje:** C++17; visualización con Python.  
**Plataforma observada:** Windows/MSVC, con Eigen 3 y CMake.

## 1. Propósito

Aplicación de consola modular para ejecutar cinco modelos de redes neuronales y memoria asociativa. Cada módulo imprime su resumen teórico, inicializa datos/pesos, ejecuta hasta el criterio común de parada, muestra métricas, exporta un CSV y llama al visualizador Python.

## 2. Estructura real

```text
3.1-Lab_RNA/
├── CMakeLists.txt
├── include/
│   └── RNA_Engine.hpp
├── src/
│   ├── main.cpp
│   ├── mcculloch_perceptron.{hpp,cpp}
│   ├── adaline.{hpp,cpp}
│   ├── mlp_backprop.{hpp,cpp}
│   ├── kohonen_som.{hpp,cpp}
│   └── hopfield.{hpp,cpp}
├── python/
│   ├── plot_auto.py
│   └── requeriment.txt
├── output/                  # CSV y snapshots generados
└── SSD/
    ├── ARCH_DESING.md
    └── SPECS/
        ├── spec-core.md
        ├── spec-01-mcculloch-perceptron.md
        ├── spec-02-adaline.md
        ├── spec-03-mlp-backprop.md
        ├── spec-04-kohonen-som.md
        ├── spec-05-hopfield.md
        └── spec-py.md
```

## 3. Stack y compilación

- C++17 y CMake 3.16 o posterior.
- Eigen3 encontrado mediante `find_package(Eigen3 CONFIG REQUIRED)`.
- Ejecutable CMake: `Lab_RNA`, compuesto por `main.cpp` y los cinco `.cpp` de modelos.
- MSVC utiliza `/W4 /O2 /fp:fast /permissive-`; otros compiladores usan `-Wall -Wextra -O3 -ffast-math -march=native`.
- El CMake crea `output` y `python` en el directorio de compilación. Sin embargo, el CLI valida rutas relativas (`./CMakeLists.txt`, `./python/plot_auto.py`) y espera que se ejecute desde la raíz del proyecto.
- Python se ejecuta como `python`; las dependencias declaradas en `python/requeriment.txt` son pandas y Matplotlib.

## 4. Motor común y ciclo de vida

`include/RNA_Engine.hpp` define `RNA_Params` (`learning_rate`, `max_epochs`, `error_threshold`, `csv_path`, `extra_1`, `extra_2`) y la clase abstracta `RNA_Base`. El método `run()`:

1. Imprime nombre y resumen teórico.
2. Llama a `initialize()`.
3. Ejecuta `train_epoch()` hasta `max_epochs` o hasta que el valor retornado sea no negativo y menor que `error_threshold`.
4. Guarda cada retorno en `error_history_`.
5. Llama a `compute_metrics()` y `export_csv()`.
6. Invoca `python .\python\plot_auto.py "<csv>"` mediante `std::system` y notifica un retorno distinto de cero.

El CSV genérico del motor tiene `Modelo,Epoca,Error`; cada modelo actual sobrescribe `export_csv()` con su propio esquema. Los parámetros son compartidos en la estructura, pero cada módulo usa una parte distinta. En el menú actual `extra_1` y `extra_2` quedan en sus valores por defecto.

## 5. Módulos y salidas

| Opción | Modelo | Implementación observada | CSV principal |
|---|---|---|---|
| 1 | McCulloch-Pitts / perceptrón simple | Clasifica AND por defecto con sesgo explícito; el código contempla OR mediante `extra_1`, que el menú no configura. | `Modelo,Epoca,Error,Aciertos` |
| 2 | ADALINE | Descifrador supervisado de 3 bits a decimal; salida lineal y regla delta. | `Modelo,Epoca,MSE,MAE` |
| 3 | MLP / backpropagation | XOR por defecto (`2-2-1`); el código incluye una tarea de aproximación de sin(x) (`1-10-1`) seleccionada por `extra_1`, sin selector en el menú actual. | `Modelo,Epoca,Error` |
| 4 | Kohonen SOM | Mapa 10×10, 200 muestras en cuatro clusters sintéticos, BMU y vecindad gaussiana. | `Modelo,Epoca,QuantizationError,Sigma,LearningRate`; además `kohonen_snapshots.csv`. |
| 5 | Hopfield | Cuatro patrones bipolares 7×6; un test cíclico por llamada a `train_epoch()`. | Métricas, nivel de ruido, índice de patrón y patrones ruidoso/recuperado. |

Los contratos, formatos CSV y límites de cada modelo se detallan en sus especificaciones `spec-01` a `spec-05`. El motor y CLI están descritos en [spec-core.md](SPECS/spec-core.md), y el contrato del graficador en [spec-py.md](SPECS/spec-py.md).

## 6. CLI y rutas

`src/main.cpp` presenta un menú cíclico 0–5. Para cada modelo solicita tasa de aprendizaje `0.001–1.0`, máximo de épocas/pruebas `10–100000` y umbral `1e-8–1.0`. El CSV se asigna como `./output/rna_model_<opción>.csv`. El programa configura la consola Windows en UTF-8 y requiere ejecutarse desde la raíz para localizar CMake y el script de gráficas.

Los valores solicitados no tienen el mismo significado en todos los módulos: por ejemplo, Hopfield utiliza el máximo de iteraciones como número de pruebas, mientras que Kohonen lo usa para calcular el decaimiento temporal. El MLP no selecciona la tarea de sin(x) desde el menú actual.

## 7. Flujo de visualización

`python/plot_auto.py` detecta el modelo por la primera celda de `Modelo`, muestra el nombre y las columnas detectadas y grafica las columnas del CSV correspondiente. Las figuras son interactivas y no se guardan automáticamente. Kohonen puede abrir un slider con snapshots de pesos; Hopfield puede abrir un slider para comparar cada patrón ruidoso y recuperado. Cerrar las ventanas permite que el proceso Python termine y el CLI continúe.

## 8. Reproducibilidad y límites

- Varios generadores usan semillas fijas 42 o derivadas de 42 para producir resultados repetibles bajo la misma implementación.
- Los valores de parámetros ingresados por consola y la configuración del compilador influyen en el resultado.
- La semilla fija no garantiza idénticos resultados entre todas las plataformas y bibliotecas estándar.
- Las métricas impresas son las implementadas por cada modelo; no representan automáticamente los umbrales teóricos o criterios de evaluación descritos en documentos previos.
- No se declara aquí un objetivo garantizado de convergencia, calidad topológica ni porcentaje de recuperación.

## 9. Documentos del proyecto

- [spec-core.md](SPECS/spec-core.md): ciclo base, CLI y configuración de compilación.
- [spec-01-mcculloch-perceptron.md](SPECS/spec-01-mcculloch-perceptron.md): perceptrón AND/OR y formato de salida.
- [spec-02-adaline.md](SPECS/spec-02-adaline.md): decodificador binario, errores y pesos objetivo.
- [spec-03-mlp-backprop.md](SPECS/spec-03-mlp-backprop.md): tareas XOR/sin(x), selector y entrenamiento real.
- [spec-04-kohonen-som.md](SPECS/spec-04-kohonen-som.md): comportamiento, métricas y snapshots del SOM.
- [spec-05-hopfield.md](SPECS/spec-05-hopfield.md): almacenamiento, recuperación, ensayos y esquema CSV.
- [spec-py.md](SPECS/spec-py.md): entradas, dispatcher, gráficos y visores interactivos.
- [README.md](../README.md): instalación, compilación y uso para el usuario.
