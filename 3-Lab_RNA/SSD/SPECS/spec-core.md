# Especificación implementada: motor base y CLI

**Archivos:** `CMakeLists.txt`, `include/RNA_Engine.hpp`, `src/main.cpp`

## Build

- CMake mínimo 3.16, proyecto `Lab_RNA`, C++17 requerido y extensiones C++ desactivadas.
- Busca `Eigen3` como paquete de configuración.
- El target `Lab_RNA` lista explícitamente `main.cpp` y los cinco módulos; incluye `src` e `include`, y enlaza `Eigen3::Eigen`.
- MSVC: `/W4 /O2 /fp:fast /permissive-`. Otros compiladores: `-Wall -Wextra -O3 -ffast-math -march=native`.
- El CMake crea directorios `output` y `python` dentro del directorio de build. El ejecutable, sin embargo, utiliza rutas relativas al directorio de trabajo actual y exige que se ejecute desde la raíz del proyecto.

## Parámetros y clase base

`RNA_Params` contiene `learning_rate`, `max_epochs`, `error_threshold`, `csv_path`, `extra_1` y `extra_2`. Sus valores predeterminados son cero, salvo `csv_path` (`./output/result.csv`). `RNA_Base` conserva los parámetros y el historial común, y declara las operaciones de modelo como métodos virtuales puros. Su `export_csv()` genérico está definido `inline` y exporta `Modelo,Epoca,Error`; cada módulo actual proporciona su propia sobrecarga.

## Ciclo de `RNA_Base::run()`

1. Imprime nombre del modelo y resumen teórico.
2. Ejecuta `initialize()`.
3. Llama a `train_epoch()` hasta `max_epochs`. Después de cada llamada, añade el resultado a `error_history_`. Solo termina por umbral si `error >= 0 && error < error_threshold`.
4. Ejecuta `compute_metrics()` y `export_csv()`.
5. Lanza `python .\python\plot_auto.py "<csv>"` con `std::system`; imprime el código de retorno y avisa si no es cero.

La llamada a Python utiliza el ejecutable `python`; el código de error sugiere probar `py`, pero no reintenta automáticamente con ese comando.

## CLI

`src/main.cpp` configura las páginas de código Windows a UTF-8 y comprueba que existan `./python/plot_auto.py` y `./CMakeLists.txt`. Solo si pasan las comprobaciones, crea `output` y `python`. Presenta un menú cíclico para opciones 0–5. Para las opciones de modelo solicita:

- Tasa de aprendizaje entre `0.001` y `1.0`.
- Máximo de épocas entre `10` y `100000`.
- Umbral entre `1e-8` y `1.0`.

`get_input<T>` repite la lectura mientras la conversión o el rango sean inválidos y descarta el resto de la línea. Crea `RNA_Params` con la ruta `./output/rna_model_<opción>.csv`; los campos adicionales quedan por defecto. La instancia se administra con `std::unique_ptr`, `run()` está envuelto en `try/catch` y la aplicación espera ENTER antes de volver al menú. Si las comprobaciones iniciales de rutas fallan, informa que se ejecute desde la raíz y termina con código 1.

## Diferencias con el diseño anterior

- La condición real de parada incluye `error >= 0`, por lo que un módulo que devuelva valores negativos no terminará por umbral.
- `trigger_plot()` es privado de la base y se ejecuta después de exportar; usa únicamente `python` y no sustituye automáticamente por `py`.
- El comando de CMake no hace que las rutas runtime se resuelvan con relación al directorio del ejecutable: el usuario debe establecer el directorio de trabajo en la raíz.
- El CLI no llena `extra_1` ni `extra_2`, por lo que tareas/variantes que dependen de esos valores no son accesibles desde el menú.
- El límite mínimo de épocas ingresado es 10, aunque el motor genérico puede ejecutarse con otros parámetros si se instancia desde código.

## Verificación

Desde la raíz, configurar y compilar con CMake y ejecutar `build/Release/Lab_RNA.exe`. Comprobar el menú, validación de rangos, creación del CSV, llamada al script y retorno al menú tras cerrar las gráficas.
