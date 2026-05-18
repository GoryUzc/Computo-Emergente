## SPEC-000: Contexto del Proyecto — Detector de Bordes con Autómatas Celulares


## 1. Stack

Lenguaje: C++17
Build: CMake 3.16+ con Visual Studio 2022
Dependencias: SFML 3.0.2 (para carga/guardado de imágenes y visualización)
Sistema operativo: Windows 10/11 (x64)
Gestor de paquetes: vcpkg

**Objetivo:** Implementar un detector de bordes o siluetas en imágenes utilizando autómatas celulares 2D. El espacio celular será la matriz de píxeles de la imagen.

**Restricción inicial:** Para la primera implementación, usar imágenes binarias (dos colores: negro y blanco).


## 2. Estructura de Carpetas a Crear

```
1.3-Detector-Bordes-AC/
├── CMakeLists.txt
├── ALGORITMO.md                    # Documentación técnica del algoritmo
├── src/
│   ├── main.cpp                    # Punto de entrada, carga imagen, aplica AC, guarda resultado
│   ├── automata2d.hpp              # Clase del autómata celular 2D
│   ├── automata2d.cpp              # Implementación de la lógica 2D
│   ├── procesador_imagen.hpp       # Carga/guarda imágenes, conversión a/from matriz
│   ├── procesador_imagen.cpp       # Implementación con SFML Image
│   └── detector_bordes.hpp         # Reglas específicas de detección de bordes
│   └── detector_bordes.cpp         # Implementación de reglas (Moore, Von Neumann)
├── assets/                         # Imágenes de prueba (entrada)
│   ├── test_binario.png
│   └── test_formas.png
├── resultados/                     # Imágenes procesadas de salida
│   └── bordes_detectados.png
├── docs/
│   └── teoria_bordes.md            # Explicación de la regla utilizada
└── python/
    └── comparar.py                 # Script opcional para comparar original vs procesada
```


## 3. Conceptos Fundamentales


### a. Conversión Imagen → Espacio Celular

Una imagen binaria es una matriz donde cada píxel es:
- **0 (negro)** → celda muerta
- **255 (blanco)** → celda viva

Para simplificar la lógica del AC, normalizamos a:
- **0** → celda muerta (fondo)
- **1** → celda viva (objeto/forma)

**Matemática:**

```
Para cada píxel (i, j):
    S(i,j) = 0  si  I(i,j) < 128   (negro o oscuro)
    S(i,j) = 1  si  I(i,j) >= 128  (blanco o claro)
```

Donde I(i,j) es el valor del canal R (o promedio RGB) del píxel.


### b. Vecindad en 2D

A diferencia del AC 1D (3 vecinos en línea), en 2D tenemos dos opciones clásicas:

**Vecindad de Von Neumann (4 vecinos):**
```
        N
      W C E
        S
```
Vecinos: Norte, Sur, Este, Oeste (cruz).

**Vecindad de Moore (8 vecinos):**
```
    NW N NE
    W  C  E
    SW S SE
```
Vecinos: Los 8 píxeles alrededor del centro.

**Para detección de bordes se recomienda Moore** (más información del entorno).


### c. La Regla de Detección de Bordes

**Concepto:** Un borde es un píxel que está en la frontera entre una región blanca (objeto) y una región negra (fondo).

**Regla propuesta (silueta/contorno):**

```
Para cada celda C con vecindad de Moore (8 vecinos):

    SI C == 1 (píxel blanco/objeto):
        Contar vecinos vivos (== 1)

        SI vecinos_vivos < 8:
            C' = 1   (es borde, queda blanco)
        SINO:
            C' = 0   (es interior, se vuelve negro)

    SINO (C == 0, fondo negro):
        C' = 0   (el fondo permanece negro)
```

**Interpretación:**
- Si un píxel blanco tiene 8 vecinos blancos → está en el **interior** → se apaga (0).
- Si un píxel blanco tiene menos de 8 vecinos blancos → está en el **borde** → permanece encendido (1).
- Los píxeles negros permanecen negros.

**Resultado:** Solo quedan los píxeles del borde/contorno en blanco sobre fondo negro.


### d. Variante: Detección de Bordes Completa (interior + exterior)

Para detectar ambos lados del borde (transición blanco→negro y negro→blanco):

```
Para cada celda C:
    Contar vecinos vivos (Moore)

    SI C == 1 Y vecinos_vivos < 8:
        C' = 1   (borde interior del objeto)
    SINO SI C == 0 Y vecinos_vivos > 0:
        C' = 1   (borde exterior, fondo tocando objeto)
    SINO:
        C' = 0
```

Esto produce un borde más grueso (2 píxeles de ancho).


### e. Conversión Espacio Celular → Imagen

```
Para cada celda S(i,j):
    I'(i,j) = 0   si  S(i,j) == 0   (negro)
    I'(i,j) = 255 si  S(i,j) == 1   (blanco)
```

La imagen resultante se guarda como PNG.


## 4. Algoritmo General del Programa

```
ENTRADA:  Imagen PNG (preferiblemente binaria o con alto contraste)
SALIDA:   Imagen PNG con bordes detectados

1. Cargar imagen de entrada desde assets/
2. Convertir a matriz de celdas (0 = negro, 1 = blanco)
3. Crear Automata2D con la matriz como estado inicial
4. Aplicar la regla de detección de bordes (1 iteración)
5. Convertir matriz resultante de vuelta a imagen
6. Guardar imagen procesada en resultados/
7. (Opcional) Mostrar comparación original vs procesada
```


## 5. Medidas de Calidad (para análisis)

| Medida | Descripción | Cómo calcular |
|--------|-------------|---------------|
| **Presición de bordes** | ¿Los bordes coinciden con la silueta real? | Comparación visual / pixel-wise con imagen de referencia |
| **Grosor del borde** | ¿El borde es de 1 píxel o más? | Contar píxeles blancos conectados en perpendicular al borde |
| **Continuidad** | ¿Hay huecos en el borde? | Análisis de componentes conectados (8-conectividad) |
| **Ruido** | ¿Aparecen píxeles aislados? | Contar componentes conectados de tamaño 1 o 2 |


## 6. Variantes de Reglas a Explorar

| Regla | Descripción | Resultado esperado |
|-------|-------------|-------------------|
| **Silueta/Contorno** | Píxeles blancos con < 8 vecinos blancos | Borde interior, 1 píxel de grosor |
| **Borde completo** | Transiciones blanco→negro y negro→blanco | Borde doble, 2 píxeles de grosor |
| **Esqueleto** | Iterar eliminando bordes sucesivamente | Línea central del objeto (más complejo) |
| **Dilatación** | Expandir regiones blancas | Objeto más grueso |
| **Erosión** | Contraer regiones blancas | Objeto más delgado |


## 7. Ejemplo Visual

**Imagen original (binaria):**
```
0 0 0 0 0 0 0 0
0 1 1 1 1 1 0 0
0 1 1 1 1 1 0 0
0 1 1 1 1 1 0 0
0 1 1 1 1 1 0 0
0 0 0 0 0 0 0 0
```

**Después de regla de silueta (1 iteración):**
```
0 0 0 0 0 0 0 0
0 1 1 1 1 1 0 0   ← borde superior
0 1 0 0 0 1 0 0   ← interiores apagados
0 1 0 0 0 1 0 0
0 1 1 1 1 1 0 0   ← borde inferior
0 0 0 0 0 0 0 0
```


## 8. Aspectos a Evaluar

| Aspecto | Cómo se demuestra |
|---------|-------------------|
| **Conversión correcta** | Código de carga PNG → matriz 0/1, y matriz → PNG. Imagen original y procesada se ven correctamente. |
| **Diseño de la regla** | Documentación en `docs/teoria_bordes.md` explicando la lógica de Moore y la regla de conteo de vecinos. |
| **Calidad de detección** | Imagen de entrada con formas conocidas (círculo, cuadrado) → borde limpio y continuo en la salida. |
| **Análisis visual** | Comparación lado a lado: original vs procesada. Métricas opcionales de grosor y continuidad. |


## 9. Evidencias Solicitadas

| Evidencia | Archivo/Formato |
|-----------|-----------------|
| Imagen original | `assets/test_binario.png` |
| Imagen procesada | `resultados/bordes_detectados.png` |
| Explicación de la regla | `docs/teoria_bordes.md` |
| Código fuente | `src/*.cpp`, `src/*.hpp` |


## 10. Referencias

1. **Wolfram, S. (2002).** *A New Kind of Science.* Wolfram Media.  
   → Capítulos sobre aplicaciones de AC en procesamiento de imágenes.

2. **Rosin, P. L., & Ioannidis, E. (2003).** "Image and Video Processing with Cellular Automata." *Handbook of Nature-Inspired and Innovative Computing*, 433-480.  
   → Revisión de técnicas de procesamiento de imágenes con AC.

3. **Popovici, A., & Popovici, D. (2002).** "Cellular Automata in Image Processing." *Proceedings of the 15th International Symposium on Mathematical Theory of Networks and Systems*.  
   → Métodos específicos de detección de bordes con AC.

4. **SFML Documentation.** *Simple and Fast Multimedia Library.* https://www.sfml-dev.org/documentation/3.0.0/  
   → Referencia para carga/guarda de imágenes con `sf::Image`.

5. **Gonzalez, R. C., & Woods, R. E. (2018).** *Digital Image Processing (4th ed.).* Pearson.  
   → Capítulo 10: Detección de bordes clásica (Sobel, Canny) para comparación conceptual.


## 11. Notas de Implementación

- **SFML Image:** Usar `sf::Image::loadFromFile()` para cargar, `getPixel()`/`setPixel()` para acceso pixel-wise, `saveToFile()` para guardar.
- **Bordes de la imagen:** Tratar como fondo negro (0) o replicar borde (padding). Recomendado: fondo negro para simplificar.
- **Imágenes de prueba:** Crear en Paint/GIMP: fondo negro, formas blancas (círculos, cuadrados, triángulos). Guardar como PNG.
- **Iteraciones:** La regla de silueta requiere **exactamente 1 iteración**. Más iteraciones erosionan el borde.
- **Color vs escala de grises:** Para la primera implementación, convertir a escala de grises: `gris = (R+G+B)/3`, luego umbralizar a 0/1.


## 12. Criterios de Aceptación Preliminares

- [ ] Carga una imagen PNG y la convierte correctamente a matriz 0/1.
- [ ] La regla de Moore cuenta correctamente los 8 vecinos (sin desbordarse en bordes).
- [ ] Aplica la regla de silueta en 1 iteración.
- [ ] Guarda la imagen resultante como PNG visible.
- [ ] El resultado muestra solo el borde/contorno de las formas originales.
- [ ] Imagen de prueba con círculo blanco → resultado con anillo blanco (círculo hueco).
- [ ] Documentación `docs/teoria_bordes.md` explica la regla matemáticamente.

