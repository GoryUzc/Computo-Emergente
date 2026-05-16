# SPEC-002: Sistema de Capturas de Pantalla para AutomataWolfram

## 1. Contexto

Proyecto C++17 con SFML 3.0.2, CMake y vcpkg. El programa simula autómatas celulares 1D (Reglas de Wolfram) y las visualiza en una ventana gráfica. Se requiere agregar la capacidad de guardar capturas de pantalla de la simulación en la carpeta `capturas/` en la raíz del proyecto.

| Componente | Versión |
|------------|---------|
| Lenguaje | C++17 |
| Gráficos | SFML 3.0.2 (Graphics, Window, System) |
| Build | CMake 3.16+ con Visual Studio 2022 |
| Gestor de paquetes | vcpkg |
| SO objetivo | Windows 10/11 (x64) |

---

## 2. Requisitos Funcionales

### RF-01: Guardado automático al finalizar cada regla
Al completarse la simulación de cada regla (cuando `generacion >= ALTO`), el sistema debe guardar automáticamente una captura de la ventana actual como imagen PNG en la carpeta `capturas/`.

### RF-02: Nomenclatura de archivos
Patrón: `capturas/automata_regla_{NUMERO_REGLA}_gen_{NUMERO_GENERACIONES}.png`

Ejemplo: `capturas/automata_regla_30_gen_60.png`

### RF-03: Guardado manual por tecla
El usuario debe poder presionar la tecla **S** durante la simulación para guardar una captura instantánea con nombre secuencial:
`capturas/automata_manual_{TIMESTAMP}_{CONTADOR}.png`

Donde `{TIMESTAMP}` es tiempo Unix en segundos y `{CONTADOR}` es un entero autoincremental por sesión.

### RF-04: Creación automática de carpeta
Si la carpeta `capturas/` no existe, debe crearse automáticamente antes de escribir el archivo. No debe lanzar excepciones si la carpeta ya existe.

### RF-05: Confirmación en consola
Cada vez que se guarde una imagen, imprimir en `std::cout` la ruta absoluta del archivo guardado. Si falla, imprimir el error en `std::cerr`.

### RF-06: No bloquear la simulación
El guardado debe ser síncrono pero no debe congelar la ventana por más de 100ms. La simulación debe continuar inmediatamente después.

---

## 3. Requisitos No Funcionales

| ID | Requisito | Detalle |
|----|-----------|---------|
| RNF-01 | Formato de imagen | PNG (lossless) vía `sf::Image::saveToFile()` |
| RNF-02 | Resolución | Dimensiones exactas de la ventana actual |
| RNF-03 | Manejo de errores | No crash si falla el guardado; imprimir error y continuar |
| RNF-04 | Independencia de ruta | Funcionar desde VS Code, terminal o doble clic |

---

## 4. Cambios Requeridos por Archivo

### 4.1 interfaz.hpp

```cpp
// Includes agregados
#include <string>
#include <filesystem>  // C++17

// Miembros privados agregados
static int contadorManual;

// Métodos públicos agregados
static std::string obtenerRutaCapturas();
void guardarCaptura(const std::string& nombreArchivo);
```

### 4.2 interfaz.cpp

```cpp
#include <ctime>

// Inicialización del contador estático
int InterfazAutomata::contadorManual = 0;

// Implementación de obtenerRutaCapturas()
std::string InterfazAutomata::obtenerRutaCapturas() {
    std::filesystem::path rutaBase = std::filesystem::current_path() / "capturas";
    return rutaBase.string();
}

// Implementación de guardarCaptura()
void InterfazAutomata::guardarCaptura(const std::string& nombreArchivo) {
    try {
        std::filesystem::path rutaBase = std::filesystem::current_path() / "capturas";
        std::filesystem::path rutaCompleta = rutaBase / nombreArchivo;
        
        std::filesystem::create_directories(rutaBase);
        
        sf::Texture textura(ventana.getSize());
        textura.update(ventana);
        
        if (textura.copyToImage().saveToFile(rutaCompleta.string())) {
            std::filesystem::path rutaAbsoluta = std::filesystem::absolute(rutaCompleta);
            std::cout << "[OK] Guardado en: " << rutaAbsoluta << std::endl;
        } else {
            std::cerr << "[ERROR] No se pudo escribir: " << rutaCompleta << std::endl;
        }
    } catch (const std::exception& ex) {
        std::cerr << "[ERROR] Excepción al guardar: " << ex.what() << std::endl;
    }
}

// Modificación de procesarEventos() - agregar manejo de tecla S
if (keyPressed->code == sf::Keyboard::Key::S) {
    auto t = std::time(nullptr);
    std::string nombre = "automata_manual_" + std::to_string(t) + "_" + 
                         std::to_string(contadorManual++) + ".png";
    guardarCaptura(nombre);
}
```

### 4.3 main.cpp

```cpp
#include <filesystem>

// Llamada en el bucle principal, después de completar generaciones
std::string nombreAuto = "automata_regla_" + std::to_string(regla) + 
                         "_gen_" + std::to_string(generacion) + ".png";
interfaz.guardarCaptura(nombreAuto);
```

---

## 5. Algoritmos Detallados

### Algoritmo 5.1: Captura de pantalla (guardarCaptura)

```
ENTRADA: nombreArchivo (string, ej: "automata_regla_30.png")
SALIDA: archivo PNG escrito en disco, mensaje en consola

1. rutaBase ← filesystem::current_path() / "capturas"
2. rutaCompleta ← rutaBase / nombreArchivo
3. INTENTAR:
      a. filesystem::create_directories(rutaBase)
      b. textura ← nueva sf::Texture(ventana.getSize())
      c. textura.update(ventana)
      d. textura.copyToImage().saveToFile(rutaCompleta.string())
      e. SI éxito: imprimir "[OK] Guardado en: {ruta_absoluta}"
         SI error: imprimir "[ERROR] No se pudo escribir: {ruta}"
4. CAPTURAR excepciones: imprimir "[ERROR] Excepción: {mensaje}"
```

### Algoritmo 5.2: Evento de tecla S

```
EVENTO: KeyPressed detectado en pollEvent()

SI evento->code == sf::Keyboard::Key::S ENTONCES:
   timestamp ← time(nullptr)
   nombre ← "automata_manual_" + to_string(timestamp) + "_" + 
            to_string(contadorManual++) + ".png"
   guardarCaptura(nombre)
```

---

## 6. Criterios de Aceptación (Definition of Done)

- [ ] Al ejecutar el programa, se crea la carpeta `capturas/` automáticamente
- [ ] Al terminar la Regla 30, existe el archivo `capturas/automata_regla_30_gen_60.png`
- [ ] Al terminar la Regla 90, existe `capturas/automata_regla_90_gen_60.png`
- [ ] Al terminar la Regla 110, existe `capturas/automata_regla_110_gen_60.png`
- [ ] Presionar la tecla S genera archivo `automata_manual_{timestamp}_{n}.png`
- [ ] La consola muestra mensajes `[OK]` con rutas absolutas
- [ ] Si se borra la carpeta y se presiona S, se recrea sin error
- [ ] La simulación no se congela ni crashea si el guardado falla
- [ ] El código compila sin warnings en VS 2022 (C++17)

---

## 7. Notas de Implementación

- `std::filesystem` requiere C++17 (ya configurado en CMakeLists.txt)
- En Windows, `std::filesystem::current_path()` devuelve la ruta del ejecutable
- `sf::Texture(ventana.getSize())` usa constructor SFML 3
- El contador manual es `static int` a nivel de clase para persistencia entre instancias

---

## 8. Ejemplo de Salida Esperada

```
=== Autómata Celular 1D - Reglas de Wolfram ===
Ejecutando reglas 30, 90 y 110...

--- Ejecutando Regla 30 (III (Caos)) ---
[OK] Guardado en: C:\...\build\Debug\capturas\automata_regla_30_gen_60.png
Regla 30 completada: 60 generaciones.

--- Ejecutando Regla 90 (III (Caos)) ---
[OK] Guardado en: C:\...\build\Debug\capturas\automata_regla_90_gen_60.png
Regla 90 completada: 60 generaciones.

--- Ejecutando Regla 110 (IV (Complejo - Universal)) ---
[OK] Guardado en: C:\...\build\Debug\capturas\automata_regla_110_gen_60.png
Regla 110 completada: 60 generaciones.

=== Simulación completada ===
```