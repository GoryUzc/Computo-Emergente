# Comparador de Imágenes - Detector de Bordes

Script para comparar visual y métricamente la imagen original con la imagen procesada por el detector de bordes.

## Instalación

```bash
pip install -r requirements.txt
```

## Uso

```bash
python comparar.py -o assets/test_formas.png -p resultados/bordes_detectados.png -s resultados/analisis.png
```

### Argumentos

- `-o, --original`: Ruta a la imagen original (requerido)
- `-p, --procesada`: Ruta a la imagen procesada (requerido)
- `-s, --salida`: Ruta para guardar la figura comparativa (opcional)

## Métricas

- **total_bordes**: Cantidad de píxeles blancos en la imagen procesada
- **porcentaje_bordes**: Porcentaje de píxeles que conforman el borde
- **componentes_conectados**: Número de regiones independientes
- **posible_ruido**: Componentes de 2 píxeles o menos

## Salida Visual

El script genera 3 paneles:
1. Original (gris)
2. Procesada (gris)
3. Diferencia (RGB codificado)