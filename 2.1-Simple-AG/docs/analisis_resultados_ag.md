# Tarea 01 - AGS: Analisis de Resultados

**Autor:** Gregory Uzcategui / CI: 27.376.191

---

## 1.1 Convergencia Base (Pc=0.8, Pm=0.002)

| Metrica | Valor |
|---------|-------|
| Mejor fitness final | 3969 (x=63) |
| Gen convergencia | 4 |
| Promedio final | 3969 |

El AGS converge rapidamente al optimo. Desde la gen 4 mantiene el mejor valor y el promedio de la poblacion tambien converge al optimo (gen 47-49).

**Grafica:** `resultados/graficos/convergencia_base.png`

---

## 1.2 Impacto de Pc (Pm=0.002)

| Pc | Mejor fitness | Gen convergencia | Promedio final | Observacion |
|----|--------------|------------------|----------------|-------------|
| 0.8 | 3969 | 4 | 3969 | Convergencia rapida y estable |
| 0.6 | 3969 | 7 | 3696.8 | Estancamiento en 3844 (gens 12-71) |
| 0.4 | 3969 | 1 | 3969 | Convergencia inmediata |

**Hallazgo:** Pc=0.4 converge mas rapido que Pc=0.8. Con poblacion pequena (n=20) y espacio limitado, menos cruces conservan mejor el optimo. Pc=0.6 sufre estancamiento por falta de elitismo de los mejores individuos.

**Grafica:** `resultados/graficos/comparativa_pc.png`

---

## 1.3 Impacto de Pm (Pc=0.8)

| Pm | Mejor fitness | Promedio final | Observacion |
|----|--------------|----------------|-------------|
| 0.002 | 3969 | 3913.35 | Convergencia estable |
| 0.01 | 3969 | 3913.35 | Similar al base, ligera perturbacion |
| 0.1 | 3969 | 2531.45 | Promedio muy bajo, alta perturbacion |

**Conclusion:** Pm=0.1 es demasiado alto. Destruye buenas soluciones y el promedio nunca converge. Pm=0.002 es mas optimo.

**Grafica:** `resultados/graficos/comparativa_pm.png`

---

## 1.4 Representacion Real (13 bits, n=50)

| Pregunta | Respuesta |
|----------|-----------|
| Cambio en el codigo | L=13, modo real, mapeo x = entero * 63/8191 |
| Partes del intervalo | 6301 (de 0.00 a 63.00, paso 0.01) |
| Bits del cromosoma | 13 (2^12=4096 < 6301 <= 8192=2^13) |

| Metrica | Entero (n=20) | Real (n=50) |
|---------|---------------|-------------|
| Mejor fitness | 3969 | 3969 |
| Mejor x | 63 | 63.00 |
| Promedio final | 3969 | 3787.55 |
| Gen convergencia | 4 | 38 |

El modo real converge mas lento (gen 38 vs 4) pero alcanza el mismo optimo. El mayor tamano de poblacion (50) mejora la exploracion pero ralentiza la convergencia.

**Grafica:** `resultados/graficos/convergencia_real.png`

---

## Conclusion General

- **Pc optimo:** 0.4-0.8 (depende de la poblacion)
- **Pm optimo:** 0.002 (baja, para no destruir soluciones)
- **Elitismo:** Recomendable para evitar estancamiento (observado en Pc=0.6)
- **Representacion:** 6 bits es suficiente para este problema; 13 bits aporta precision innecesaria con costo computacional mayor
