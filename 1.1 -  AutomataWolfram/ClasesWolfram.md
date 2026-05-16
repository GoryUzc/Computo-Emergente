# Clases de Wolfram: Teoría y Comportamiento de los Autómatas Celulares 1D

## Introducción

Stephen Wolfram clasificó los autómatas celulares unidimensionales en **cuatro clases** basándose en el comportamiento observable de sus patrones espaciales-temporales. Esta clasificación, presentada en "A New Kind of Science" (2002), proporciona un marco teórico para entender la complejidad emergente en sistemas discretos simples.

---

## Clase I: Sistemas Homogéneos (Fijos)

### Características
- **Comportamiento**: Evolucionan rápidamente hacia un estado homogéneo o periódico estable en el tiempo.
- **Patrones**: Estructuras simples, sencillas dominadas por bloques uniformes.
- **Ejemplos notables**: Regla 0, Regla 255, Regla 32, Regla 128

### Propiedades
- **Entropía**: Disminuye con el tiempo (orden crece)
- **Estabilidad**: Alcanza equilibrio rápidamente
- **Predicción**: Completamente determinista, evoluciona a un estado fijo.

### Ejemplo Visual (Regla 0)
```
        ●●●●●●●●●●●●●●●●
        0000000000000000000000000000000000000000
        0000000000000000000000000000000000000000
        0000000000000000000000000000000000000000
        0000000000000000000000000000000000000000
```
*Todos los estados convergen a cero*

---

## Clase II: Sistemas Periódicos (Estructuras Estables)

### Características
- **Comportamiento**: Evolucionan hacia estructuras periódicas locales en la region.
- **Patrones**: Composición repetitiva de dominós, bloques, triominós, figuras.
- **Ejemplos notables**: Regla 18, Regla 42, Regla 60, Regla 90, Regla 102

### Propiedades
- **Entropía**: Mantiene estructuras de baja complejidad.
- **Estabilidad**: Configuraciones que se replican o quedan estáticas
- **Simetría**: Estructuras geométricas regulares (triángulos, líneas, cuadrados)

### Ejemplo Visual (Regla 90)
```
        ●    ●    ●    ●
          ●    ●    ●
        ●    ●    ●
          ●    ●    ●
        ●    ●    ●    ●
```
*Patrón de Sierpinski (triángulo de fractales)*

---

## Clase III: Sistemas Caóticos (Pseudoaleatorios)

### Características
- **Comportamiento**: Caos aparente, aparentan aleatoriedad en el tiempo.
- **Patrones**: Distribución aparentemente aleatoria de celdas vivas/muertas.
- **Ejemplos notables**: Regla 15, Regla 22, Regla 30, Regla 45, Regla 102

### Propiedades
- **Entropía**: Alta, similar a generador de números aleatorios.
- **Difusión**: Perturbaciones se propagan en todas direcciones.
- **Mixtura**: Combina orden local con apariencia global caótica

### Ejemplo Visual (Regla 30)
```
        ●●●●●●●●●●●●●●●
          ●●  ●●  ●●  ●●
        ●●      ●●      ●●
          ●●  ●●  ●●  ●●
        ●●●●  ●●●●  ●●●●
```
*Patrón caótico con dominio activo central*

---

## Clase IV: Sistemas Complejos (Emergentes)

### Características
- **Comportamiento**: Estructuras locales estables con zonas activas
- **Patrones**: Combinan orden y caos, comportamiento emergente
- **Ejemplos notables**: Regla 26, Regla 58, Regla 94, **Regla 110**

### Propiedades
- **Entropía**: Media-alta, equilibrio entre orden y desorden
- **Universalidad**: Algunos son universales Turing (ej. Regla 110)
- **Complejidad**: Permiten comportamientos complejos a partir de reglas simples

### Ejemplo Visual (Regla 110)
```
        ●●●●●●●●●●●●●●●●
          ●●●    ●●●    ●●
        ●●●  ●●●  ●●●  ●●
          ●●●    ●●●    ●●
        ●●●●●●●●●●●●●●●●
```
*Estructuras que interactúan y se preservan en colisión*

---

## Clasificación Comparativa

| Clase | Entropía | Orden | Predictibilidad | Ejemplo Clave |
|-------|----------|-------|-----------------|---------------|
| I | Baja | Alto | Alta | Regla 0, 255 |
| II | Baja-Media | Alto | Alta | Regla 90 (Sierpinski) |
| III | Alta | Bajo | Baja | Regla 30 (caos) |
| IV | Media-Alta | Variable | Baja-Media | Regla 110 (universal) |

---

## Distribución de Reglas

De las 256 reglas posibles:

- **Clase I**: ~10 reglas (3.9%)
- **Clase II**: ~80 reglas (31.2%)
- **Clase III**: ~120 reglas (46.9%)
- **Clase IV**: ~46 reglas (18.0%)

---

## Implications Teóricas

1. **Complejidad emergente**: Sistemas simples pueden generar comportamientos complejos
2. **Computabilidad**: Clase IV abarca máquinas universales Turing
3. **Física computacional**: Modelo para entender leyes físicas desde principios discretos
4. **Biología**: Paralelos con crecimiento de tejidos y patrones biológicos

---

## Referencias

1. Wolfram, S. (2002). *A New Kind of Science*. Wolfram Media.
2. Cook, M. (2004). Universality in Elementary Cellular Automata. *Complex Systems*, 15(1).
3. Martinez, G. et al. (1984). Behavior of Rule 110. *Physica D*, 14(1-2).