## Actividad 4. Revisión de Trabajos de Investigación

---

### 1. Referencia Bibliográfica

**Shukla, A. P.** (2016). *Training Cellular Automata for Image Edge Detection*. Romanian Journal of Information Science and Technology, 19(4), 338–359.

Disponible en: http://romjist.ro/content/pdf/shukla.pdf

**¿Por qué este artículo?**

- **Pertinencia directa:** Aplica AC bidimensionales exactamente al problema de detección de bordes.
- **Métricas experimentales:** Evalúa cuantitativamente contra ground truth y detectores clásicos.
- **Revisión por pares:** Publicado en revista indexada (ROMJIST).

---

### 2. Resumen del Trabajo Seleccionado

**¿Qué quería lograr el autor?**
Entrenar un autómata celular 2D para detectar bordes en imágenes, pero sin definir la regla a mano. En lugar de eso, dejar que el algoritmo encuentre la mejor regla automáticamente.

**¿Cómo lo hizo? En dos fases:**

**Fase 1 – Reducir el espacio de reglas:**
- Con vecindad Moore (3×3) hay millones de reglas posibles.
- El autor usó simetría rotacional (0°, 90°, 180°, 270°).
- Redujo todo a solo 51 reglas candidatas. Eso es manejable.

**Fase 2 – Encontrar la mejor combinación:**
- Usó SFFS, un algoritmo que prueba reglas una por una.
- Va añadiendo la que más mejora el resultado y quitando las que ya no aportan (como cuando armamos un equipo deportivo).
- La función que mide la calidad es el Misclassification Error: compara la salida del AC contra una imagen de referencia.

**¿Con qué experimentó?**
- 20 imágenes binarias con formas geométricas (círculos, líneas). El ground truth lo generó con el detector Canny.
- 12 imágenes en escala de grises del dataset USF. Primero las binarizó automáticamente con el método de Otsu.

**¿Qué descubrió?**
La regla 51 sola fue la ganadora. Su lógica es: "si un píxel blanco tiene los 8 vecinos blancos, es interior y se apaga; si no, es borde y se queda encendido". El resultado es un contorno de 1 píxel de grosor.

---

### 3. Métricas Utilizadas

#### a. SFFS — ¿Cómo eligió las reglas?

El autor partió de millones de reglas posibles con vecindad Moore. En lugar de probarlas todas (imposible), aplicó simetría rotacional (0°, 90°, 180°, 270°) y redujo el espacio a solo 51 reglas.

Luego usó SFFS (Sequential Floating Forward Search): un algoritmo que va probando reglas una por una. Añade la mejor, revisa si alguna ya no aporta y la quita. Así "flota" entre añadir y quitar hasta encontrar el subconjunto óptimo.

**Resultado:** la regla 51 sola fue suficiente.

#### b. Misclassification Error — ¿Qué tan mal etiquetó?

Es un porcentaje de error: de todos los píxeles, ¿cuántos el AC clasificó mal? Es decir, ¿marcó como borde algo que era fondo, o como fondo algo que era borde?

**Fórmula simplificada:**


- error = 0 → el AC acertó todo
- error = 1 → el AC falló en todo

Es mejor que RMS porque no le importa "cuánto gris" tiene un píxel, solo le importa si está en la categoría correcta: borde o fondo.

#### c. RMS y Ground Truth

**RMS (Root Mean Square Error):** mide la diferencia promedio de intensidad entre la imagen filtrada y la original. Shukla la usó solo para la etapa de "quitar ruido sal y pimienta", no para evaluar bordes.

**Ground Truth:** es la "respuesta correcta". Shukla usó el detector Canny para generar la imagen de referencia. El AC aprende a imitar a Canny. El problema: si Canny se equivoca, el AC aprende el error.

---

### 4. Conclusiones Personales

#### Lo que me pareció genial

1. Reducir millones de reglas a solo 51 usando simetría rotacional es una simplificación brillante. No es "hacer trampa", es usar matemáticas para recortar lo innecesario.

2. El algoritmo SFFS me pareció muy práctico: en lugar de fuerza bruta, va construyendo la solución paso a paso, añadiendo y quitando reglas. Es como armar un equipo de fútbol: no pruebas millones de alineaciones, vas probando jugador por jugador y ves quién realmente aporta.

3. Que la regla 51 sola haya sido la óptima me sorprendió. Pensaba que necesitaría una combinación compleja, pero a veces la solución simple es la mejor. Eso me hace reflexionar sobre no sobrecomplicar los algoritmos.

#### Lo que me hizo dudar

1. El ground truth fue generado con Canny, no por un humano experto. Entonces el AC no aprendió "qué es un borde real", aprendió a imitar a Canny. Si Canny se equivoca en alguna esquina, el AC reproduce ese error como si fuera correcto.

2. Las imágenes de entrenamiento fueron formas geométricas simples (círculos, líneas, conos). ¿Funcionaría igual con fotos reales que tengan sombras, texturas o ruido natural? El artículo no lo prueba a fondo.

3. No hay tiempos de ejecución comparativos. El AC es paralelizable por naturaleza, pero no sabemos si en la práctica es más rápido que Sobel o Canny en una computadora común.

---

### Referencias

1. Shukla, A. P. (2016). *Training Cellular Automata for Image Edge Detection*. ROMJIST, 19(4), 338–359. http://romjist.ro/content/pdf/shukla.pdf

2. Wolfram, S. (2002). *A New Kind of Science*. Wolfram Media.

3. Gonzalez, R. C., & Woods, R. E. (2018). *Digital Image Processing* (4th ed.). Pearson. Cap. 10.

4. Rosin, P. L., & Ioannidis, E. (2003). Image and Video Processing with Cellular Automata. *Handbook of Nature-Inspired Computing*, 433–480.

5. Popovici, A., & Popovici, D. (2002). Cellular Automata in Image Processing. *Proc. MTNS*.

---