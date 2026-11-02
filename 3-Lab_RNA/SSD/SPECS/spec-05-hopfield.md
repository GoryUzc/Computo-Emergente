# Especificación implementada: Red de Hopfield

**Archivos:** `src/hopfield.hpp`, `src/hopfield.cpp`  
**Integración visual:** `python/plot_auto.py`  
**Estado:** Contrato documentado a partir de la implementación actual.

## Propósito y contrato

El módulo almacena cuatro patrones binarios A–D de 7 × 6 píxeles (42 unidades) y evalúa su recuperación después de añadir ruido. Cada píxel se codifica como `+1` si su valor binario es 1 y `-1` si es 0.

`HopfieldNet` extiende `RNA_Base`. `initialize()` define los cuatro patrones, construye la matriz de pesos y limpia los historiales. No hay aprendizaje por época: el almacenamiento Hebbiano se realiza de una vez. Cada llamada a `train_epoch()` lleva a cabo una prueba de recuperación y devuelve la energía final de ese estado.

## Almacenamiento y recuperación

- Los pesos se construyen como `W = Σ(k=1..4) xk * xkᵀ`; la diagonal se pone a cero.
- La matriz no se divide por `N` ni por el número de patrones. La escala de la energía es, por ello, la de esta suma sin normalizar.
- Cada prueba selecciona cíclicamente un patrón y un nivel de ruido del 10 %, 20 % o 30 %. Se invierte cada píxel independientemente con esa probabilidad.
- La semilla del ruido es `42 + pattern_index * 10 + noise_percent`, lo que hace determinista la entrada ruidosa para cada combinación.
- La actualización de neuronas es asíncrona. En cada barrido se hacen 42 selecciones aleatorias con reemplazo; no necesariamente se actualiza cada neurona exactamente una vez.
- Se usa `sgn(h) = +1` cuando `h >= 0` y `-1` en otro caso. La dinámica termina al completar un barrido sin cambios o tras un máximo de `42 * 10` barridos.
- La rutina de recuperación reinicia su generador de orden con semilla 42 en cada prueba.
- La energía exportada es la energía final `-0.5 * recoveredᵀ * W * recovered`. El CSV no guarda la trayectoria energética de cada actualización.
- El porcentaje de recuperación es `100 * (42 - distancia_Hamming) / 42`.

La afirmación de descenso de energía aplica a la dinámica asíncrona ideal con pesos simétricos y diagonal nula; la salida CSV registra solo una energía final por prueba y no permite comprobar el descenso paso a paso.

## Ciclo de pruebas y parada

Las pruebas siguen el orden: patrón 0 con ruido 10/20/30 %, patrón 1 con 10/20/30 %, y así sucesivamente entre A–D. El ciclo se repite si `max_epochs` excede 12; la configuración del CLI controla cuántas pruebas se ejecutan, y el motor común limita el máximo al valor ingresado (10–100000).

El motor solo aplica parada temprana cuando el valor retornado es no negativo y menor que el umbral. La energía Hopfield habitual es negativa; en ese caso el entrenamiento llega a `max_epochs`, no se detiene después de una ronda de 12 pruebas.

La capacidad aproximada `0.14N` es una referencia teórica, no una garantía de recuperación. Para `N=42`, se almacenan cuatro patrones, pero el porcentaje obtenido depende de patrones y ruido.

## Archivos de salida

`output/rna_model_5.csv` usa la siguiente cabecera:

```csv
Modelo,Epoca,Energia,RuidoPct,RecuperacionPct,PatternIdx,NoisyPixels,RecoveredPixels
```

- `Epoca` es el número de prueba, no una época de entrenamiento de pesos.
- `PatternIdx` es cero-based: `0=A`, `1=B`, `2=C`, `3=D`.
- `NoisyPixels` y `RecoveredPixels` son cadenas de 42 caracteres. `1` representa `+1`; `-` representa `-1`.
- `Energia` se escribe con cuatro decimales; `RecuperacionPct`, con dos.
- La cantidad de filas es la cantidad de pruebas ejecutadas.

## Métricas y visualización

La consola informa el número de pruebas y recuperación promedio para ruido del 10 % y 30 %, calculadas con las observaciones disponibles en cada nivel. El promedio del 20 % no se imprime, aunque sí se exporta.

El script Python agrupa todas las filas por ruido para dibujar el promedio de recuperación y muestra la energía de las primeras 50 pruebas. Si están presentes las columnas de píxeles e índice del patrón, abre además un slider para recorrer entrada ruidosa y salida recuperada.

## Diferencias relevantes frente al diseño anterior

- El CSV actual tiene ocho columnas; las tres últimas (`PatternIdx`, `NoisyPixels`, `RecoveredPixels`) alimentan el visor de patrones.
- No existe una configuración fija de 12 pruebas: `max_epochs` del CLI establece la cantidad de pruebas y el ciclo de cuatro patrones × tres ruidos se repite.
- La regla actual no normaliza la matriz por `N`.
- `recover_pattern` retorna `void`; no registra ni exporta el número de pasos de convergencia.
- La energía almacenada por fila es el resultado final de una prueba, no una secuencia de energías por actualización.
- La implementación informa promedios empíricos, no fuerza objetivos como 90 % o 75 % de recuperación.

## Verificación del contrato

- Compilar con el target CMake `Lab_RNA`.
- Ejecutar la opción 5 desde la raíz del proyecto y verificar la cabecera de ocho columnas y que las cadenas de píxeles tengan 42 caracteres.
- Usar un número de pruebas superior a 12 para confirmar el ciclo A–D/10–30 % repetido.
- Interpretar los valores de recuperación como resultados experimentales, no como umbrales garantizados.
