## SPEC-001: Implementación de Módulos C++ — Detector de Bordes con Autómatas Celulares


## 1. Stack

Lenguaje: C++17
Build: CMake 3.16+ con Visual Studio 2022
Dependencias: SFML 3.0.2, vcpkg
Sistema operativo: Windows 10/11 (x64)
Gestor de paquetes: vcpkg

**Objetivo:** Implementar los módulos fuente en `src/` para cargar imágenes, ejecutar el autómata celular con regla de detección de bordes y guardar el resultado.

**Restricción:** Mantener coherencia con SPEC-000: imágenes binarias (0/1), vecindad de Moore por defecto, actualización síncrona.


## 2. Estructura de Archivos a Implementar

```
src/
├── main.cpp                    # Punto de entrada: orquesta flujo completo
├── automata2d.hpp              # Declaración de clase Automata2D
├── automata2d.cpp              # Implementación: estado, vecinos, evolución
├── procesador_imagen.hpp       # Declaración: conversión imagen ↔ matriz
├── procesador_imagen.cpp       # Implementación con SFML Image
├── detector_bordes.hpp         # Declaración de reglas de transición
└── detector_bordes.cpp         # Implementación: silueta, borde_completo, etc.
```


## 3. Conceptos Fundamentales de Implementación


### a. Diseño de la Clase `Automata2D`

**Responsabilidades:**
- Almacenar estado como `std::vector<std::vector<int>>` (0 = fondo, 1 = objeto)
- Proveer acceso seguro a celdas con validación de límites
- Calcular vecindades de Moore (8) y Von Neumann (4)
- Ejecutar evolución con regla inyectada vía `std::function`

**Matemática de vecindad Moore:**

```
Para celda en (i, j), vecinos = {
    (i-1,j-1), (i-1,j), (i-1,j+1),
    (i  ,j-1),           (i  ,j+1),
    (i+1,j-1), (i+1,j), (i+1,j+1)
} ∩ límites_de_imagen
```

**Actualización síncrona:**

```
nuevo_estado[i][j] = regla(estado_viejo[i][j], vecinos_viejos)
// Solo después de calcular toda la matriz: estado_ = nuevo_estado
```


### b. Conversión Imagen ↔ Matriz (ProcesadorImagen)

**Binarización (RGB → 0/1):**

```
gris = (R + G + B) / 3
matriz[y][x] = (gris >= 128) ? 1 : 0
```

**Reconstrucción (0/1 → PNG):**

```
color = (matriz[y][x] == 1) ? sf::Color::White : sf::Color::Black
img.setPixel(x, y, color)
```

**Nota de coordenadas:** SFML usa (x,y) = (columna, fila). La matriz interna usa [fila][columna]. Verificar consistencia en implementación.


### c. Reglas de Detección de Bordes

**Firma común:**

```cpp
int regla(int estado_actual, const std::vector<int>& vecinos);
// retorno: 0 o 1
```

**Regla de silueta (default):**

```
SI estado_actual == 0: retornar 0
SI contar(vecinos, 1) < 8: retornar 1  // borde interior
SINO: retornar 0                        // interior puro
```

**Regla de borde completo:**

```
SI (estado==1 Y vecinos_vivos<8) O (estado==0 Y vecinos_vivos>0): retornar 1
SINO: retornar 0
```

**Regla de erosión:**

```
SI estado==1 Y TODOS los vecinos == 1: retornar 1
SINO: retornar 0
```

**Regla de dilatación:**

```
SI estado==1: retornar 1
SI ALGÚN vecino == 1: retornar 1
SINO: retornar 0
```


## 4. Algoritmo General del Programa

```
ENTRADA:  Imagen PNG desde assets/
SALIDA:   Imagen PNG con bordes detectados en resultados/

1. Validar argumentos de línea de comandos (entrada, salida, regla)
2. Cargar imagen con SFML y convertir a matriz 0/1 (umbral 128)
3. Crear Automata2D con la matriz como estado inicial
4. Obtener regla de transición por nombre (silueta, borde_completo, etc.)
5. Ejecutar evolución (1 iteración por defecto para silueta)
6. Convertir matriz resultante a sf::Image (blanco=1, negro=0)
7. Guardar imagen procesada en resultados/
8. Reportar métricas básicas en consola
```


## 5. Medidas de Calidad de Implementación

| Medida | Descripción | Cómo verificar |
|--------|-------------|----------------|
| **Correctitud de vecinos** | Moore devuelve ≤8 vecinos válidos en bordes | Test unitario con celda en (0,0) |
| **Sincronía de evolución** | Estado nuevo no afecta cálculo de otras celdas | Debug: imprimir estado antes/después de evolucionar |
| **Binarización consistente** | Umbral 128 aplicado uniformemente | Imagen con gradiente → verificar matriz resultante |
| **Gestión de memoria** | Sin fugas en carga/evolución/guardado | Valgrind o VS Diagnostic Tools |
| **Manejo de errores** | Excepciones claras en archivos no encontrados | Probar con ruta inválida |


## 6. Variantes de Configuración

| Parámetro | Valores soportados | Efecto |
|-----------|-------------------|--------|
| `regla_nombre` | `"silueta"`, `"borde_completo"`, `"erosion"`, `"dilatacion"` | Cambia lógica de transición |
| `iteraciones` | Entero ≥ 1 | Número de pasos de evolución (silueta: usar 1) |
| `vecindad` | Moore (default), Von Neumann (extensible) | Define qué vecinos se consideran |


## 7. Ejemplo de Ejecución

```bash
# Compilar (desde raíz del proyecto)
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release

# Ejecutar con parámetros por defecto
./Release/detector_bordes.exe

# Ejecutar con argumentos personalizados
./Release/detector_bordes.exe assets/test_formas.png resultados/salida.png borde_completo
```

**Salida esperada en consola:**

```
[1/5] Cargando: assets/test_formas.png
[2/5] Autómata: 64x64
[3/5] Regla: borde_completo (1 iter)
[4/5] Guardando: resultados/salida.png
[5/5] ✅ Completado
```


## 8. Aspectos a Evaluar

| Aspecto | Cómo se demuestra |
|---------|-------------------|
| **Compilación limpia** | `cmake --build .` sin warnings con `/W4` o `-Wall -Wextra` |
| **Regla inyectable** | Cambiar regla vía argumento de línea de comandos sin recompilar |
| **Bordes de imagen** | Celda en esquina no accede a memoria inválida (valgrind clean) |
| **Salida visual** | `resultados/bordes_detectados.png` muestra contorno continuo |
| **Documentación inline** | Comentarios Doxygen en headers `.hpp` |


## 9. Evidencias Solicitadas

| Evidencia | Archivo/Formato |
|-----------|-----------------|
| Código fuente C++ | `src/*.cpp`, `src/*.hpp` |
| CMakeLists.txt actualizado | `CMakeLists.txt` con targets para nuevos archivos |
| Build exitoso | Log de compilación sin errores |
| Ejecución de prueba | Captura de consola mostrando `[1/5]...[5/5] ✅` |


## 10. Referencias

1. **C++ Core Guidelines.** https://isocpp.github.io/CppCoreGuidelines/
   → Secciones: Owners, Interfaces, Error handling.

2. **SFML Documentation.** https://www.sfml-dev.org/documentation/3.0.0/
   → `sf::Image`: loadFromFile, getPixel, setPixel, saveToFile.

3. **Gonzalez & Woods (2018).** *Digital Image Processing*, Cap. 10.
   → Operaciones morfológicas: erosión, dilatación, gradiente.

4. **Wolfram (2002).** *A New Kind of Science*, Cap. 5.
   → Actualización síncrona en autómatas celulares.


## 11. Notas de Implementación

- **SFML Image:** `getPixel()` y `setPixel()` usan coordenadas (x,y) = (columna, fila). La matriz interna usa [fila][columna]. Verificar consistencia.
- **Bordes de imagen:** Vecinos fuera de límites se ignoran (no se asumen 0). Esto equivale a padding implícito negro.
- **Rendimiento:** Para imágenes >1024x1024, considerar `std::vector<uint8_t>` plano en lugar de vector de vectores.
- **Extensibilidad:** Nuevas reglas solo requieren implementar función con firma estándar y registrar en `obtener_regla()`.
- **Argumentos CLI:** `main.cpp` acepta 3 argumentos opcionales: entrada, salida, nombre_regla.


## 12. Criterios de Aceptación

- [ ] `Automata2D::vecinos_moore()` retorna máximo 8 elementos, nunca accede fuera de límites.
- [ ] `evolucionar()` usa copia temporal para actualización síncrona.
- [ ] `ProcesadorImagen::cargar_y_binarizar()` aplica umbral 128 consistentemente.
- [ ] `main.cpp` acepta 3 argumentos opcionales: entrada, salida, nombre_regla.
- [ ] Compilación con CMake + vcpkg + SFML 3.0.2 en Windows 10/11 sin errores.
- [ ] Ejecución con imagen de prueba genera `resultados/bordes_detectados.png` visible.
- [ ] Imagen de círculo blanco → resultado es anillo blanco de 1 píxel de grosor.
- [ ] Headers `.hpp` incluyen documentación Doxygen mínima para funciones públicas.


---
