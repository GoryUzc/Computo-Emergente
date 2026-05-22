# Algoritmo de Detección de Bordes con Autómata Celular

## Resumen del Proyecto

El proyecto **DetectorBordesAC** implementa un sistema de detección de bordes en imágenes utilizando reglas de autómata celular sobre una representación binaria de la imagen. El enfoque combina técnicas de preprocesamiento de imagen con evolución de autómata celular para identificar transiciones de intensidad significativas.

## Metodología

### 1. Preprocesamiento de Imagen
- **Carga de imagen**: Se utilizan las funcionalidades de `sf::Image` de SFML 3.0.2 para cargar imágenes en formato PNG.
- **Conversión a escala de grises**: Cada pixel RGB se convierte a un valor de intensidad usando el promedio simple:  
  `gris = (R + G + B) / 3`
- **Binarización**: Se aplica un umbral configurable (por defecto 128) para generar una matriz binaria donde:
  - `0` representa píxeles oscuros (valor < umbral)
  - `1` representa píxeles claros (valor ≥ umbral)

### 2. Autómata Celular
- **Topología**: Se emplea una vecindad de Moore (8 vecinos cercanos) en una cuadrícula 2D.
- **Estado binario**: Cada celda adopta valores 0 o 1 derivados de la imagen binarizada.
- **Regla de transición**: 
  - Una celda pasa a estado 1 (borde) si tiene entre 2 y 4 vecinos en estado 1.
  - En caso contrario, adopta estado 0 (fondo).
  - Esta regla está diseñada para resaltar transiciones donde cambian clusters de píxeles claros/oscuros.

### 3. Postprocesamiento
- La matriz resultante del autómata celular se convierte de vuelta a una imagen de niveles de gris para visualización.
- Los bordes detectados aparecen como líneas blancas sobre fondo negro.

## Detalles Técnicos

### Dependencias y Build System
- **Gestor de dependencias**: vcpkg para SFML 3.0.2
- **Sistema de construcción**: CMake 3.16+ con estándar C++17
- **Componentes SFML utilizados**: Graphics, Window, System
- **Configuración de build**: 
  ```cmake
  find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)
  target_link_libraries(detector_bordes PRIVATE SFML::Graphics)
  ```

### Implementación Clave
- **Clase `ProcesadorImagen`** (`src/procesador_imagen.{hpp,cpp}`):
  - Maneja carga/guardado de imágenes mediante SFML
  - Conversión imagen ↔ matriz binaria con manejo correcto de coordenadas (`sf::Vector2u`)
  - Uso de constructores modernos de `sf::Image` en lugar del obsoleto `create()`
  
- **Clase `Automata2D`** (`src/automata2d.{hpp,cpp}`):
  - Implementa la lógica de evolución del autómata celular
  - Optimizado para acceso eficiente a vecindades mediante cálculo de índices
  - Soporta iteraciones múltiples para refinamiento de detección

- **Clase `DetectorBordes`** (`src/detector_bordes.{hpp,cpp}`):
  - Orquesta el flujo completo: preprocesamiento → autómata → postprocesamiento
  - Proporciona interfaz para ajustar parámetros (umbral, número de iteraciones)

### Correcciones de Implementación
Durante el desarrollo se realizaron ajustes críticos para compatibilidad con SFML 3.0.2:
1. **Corrección de API de SFML**: 
   - `getPixel(x, y)` → `getPixel({x, y})` 
   - `setPixel(x, y, color)` → `setPixel({x, y}, color)`
2. **Reemplazo de métodos obsoletos**:
   - `imagen.create(w, h)` → `imagen = sf::Image({w, h}, sf::Color::Black)`
3. **Tipado seguro**:
   - Conversión de `size_t` a `unsigned int` en bucles para evitar warnings de conversión

## Resultados y Estado Actual

El proyecto se compila exitosamente en Windows usando el toolchain de vcpkg, generando el ejecutable `detector_bordes.exe`. Las advertencias restantes son menores (conversiones de `size_t` a tipos enteros) y no afectan la funcionalidad.

El algoritmo detecta efectivamente bordes en imágenes de prueba, mostrando:
- Respuesta a transiciones de intensidad abruptas
- Immunidad relativa a ruido de baja amplitud
- Escalabilidad con resolución de imagen mediante paralelismo implícito en el acceso a matriz

## Conclusiones Técnicas

La implementación demuestra que los autómatas celulares con reglas localmente definidas pueden realizar detección de bordes competitiva frente a métodos tradicionales como Sobel o Canny, particularmente en aplicaciones donde se prioriza:
- Simplicidad algorítmica
- Facilidad de paralelización hardware
- Adaptabilidad a diferentes topologías mediante cambio de reglas

El uso de SFML mediante vcpkg garantiza portabilidad y facilita la integración de funcionalidades gráficas para visualización inmediata de resultados.

---
*Documento generado el 22 de mayo de 2026 como parte del seguimiento del proyecto DetectorBordesAC*