# Teoría de Detección de Bordes con Autómatas Celulares

## Vecindad en 2D

A diferencia del AC 1D (3 vecinos en línea), en 2D tenemos dos opciones clásicas:

### Vecindad de Von Neumann (4 vecinos):
```
    N
  W C E
    S
```

### Vecindad de Moore (8 vecinos):
```
    NW N NE
    W  C  E
    SW S SE
```

**Para detección de bordes se recomienda Moore** (más información del entorno).

---

## Algoritmo de Detección de Bordes

### Regla de Silueta/Contorno

Un píxel pertenece al borde si:
1. Es píxel blanco (valor 1 en la matriz celular)
2. Tiene menos de 8 vecinos blancos

**Fórmula matemática:**

```
Para cada celda C en posición (i,j):
    V = Σ Moore(i,j)  // Suma de valores de los 8 vecinos

    Si C == 1:
        Si V < 8:
            C' = 1  // Es borde
        Si V == 8:
            C' = 0  // Es interior, se apaga
    Si C == 0:
        C' = 0  // El fondo permanece negro
```

### Regla de Borde Completo

Esta variante detecta ambos lados del borde (interior y exterior):

```
Para cada celda C:
    V = Σ Moore(i,j)

    Si C == 1 y V < 8:
        C' = 1  // Borde interior
    Si C == 0 y V > 0:
        C' = 1  // Borde exterior
    Si no:
        C' = 0
```

**Esto produce un borde más grueso (2 píxeles de ancho).**

### Regla de Erosión

Operación morfológica que contrae la región blanca:

```
Para cada celda C:
    Si C == 1:
        Si TODOS los vecinos Moore == 1:
            C' = 1  // Mantiene el píxel
        Si no:
            C' = 0  // Se borra (erosión)
    Si C == 0:
        C' = 0
```

### Regla de Dilatación

Operación morfológica que expande la región blanca:

```
Para cada celda C:
    Si C == 1:
        C' = 1  // Mantener píxeles blancos
    Si C == 0:
        Si ALGÚN vecino Moore == 1:
            C' = 1  // Convertir a blanco (expansión)
        Si no:
            C' = 0
```

---

## Interpretación de Resultados

| Caso | Condición | Resultado |
|------|-----------|-----------|
| **Interior** | C=1 y 8 vecinos vivos | Píxel se apaga (no es borde) |
| **Borde interior** | C=1 y < 8 vecinos vivos | Píxel permanece blanco |
| **Fondo alejado** | C=0 y 0 vecinos vivos | Píxel permanece negro |
| **Borde exterior** | C=0 y ≥ 1 vecino vivo | Píxel se vuelve blanco |

El resultado es una imagen donde solo se ven los contornos de los objetos originales, creando una "silhouette" o "outline" efectivo.

---

## Ejemplo Visual

**Imagen original (5x5):**
```
0 0 0 0 0
0 1 1 1 0
0 1 1 1 0
0 1 1 1 0
0 0 0 0 0
```

**Después de regla de silueta (1 iteración):**
```
0 0 0 0 0
0 1 0 1 0  ← bordes laterales
0 1 0 1 0  ← interior apagado
0 1 0 1 0  ← bordes laterales
0 0 0 0 0
```

**Después de regla de borde completo:**
```
0 0 0 0 0
0 1 1 1 0  ← borde superior
0 1 0 1 0  ← laterales + hueco interior
0 1 1 1 0  ← borde inferior
0 0 0 0 0
```