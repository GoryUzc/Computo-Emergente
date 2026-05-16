# Autómata Celular 1D — Documentación Técnica

## 1. Definición Formal

El sistema modela un **autómata celular unidimensional** con vecindad de radio 1 y espacio de estados binario, representado como una tupla:

$$
A = (S, s_0, N, \delta)
$$

donde:

- $S = \{0, 1\}$: Conjunto finito de estados (0 = apagado, 1 = encendido).
- $s_0 \in S^n$: Vector de estado inicial de longitud $n$ (células).
- $N(i) = \{i-1, i, i+1\}$: Función de vecindad que retorna el índice de los tres vecinos de la célula $i$ (borde circular).
- $\delta: S^3 \rightarrow S$: Función de transición local determinista.

La evolución discreta se define mediante:

$$
s_i^{(t+1)} = \delta(s_{i-1}^{(t)}, s_i^{(t)}, s_{i+1}^{(t)}) \quad \forall i \in [0, n-1]
$$

con condición de contorno **circular** (topología toroidal):

$$
s_{-1}^{(t)} = s_{n-1}^{(t)}, \quad s_n^{(t)} = s_0^{(t)}
$$

---

## 2. Codificación de Reglas de Wolfram

Cada regla es un número entero $R \in [0, 255]$ cuya representación binaria de 8 bits codifica la salida para cada uno de los 8 patrones posibles del vecindario:

$$
R = (b_7 b_6 b_5 b_4 b_3 b_2 b_1 b_0)_2
$$

El patrón local se codifica como entero mediante ponderación binaria:

$$
P = 4 \cdot s_{i-1} + 2 \cdot s_i + 1 \cdot s_{i+1} \in [0,7]
$$

La transición local se obtiene extrayendo el bit correspondiente:

$$
\delta(P) = \left\lfloor \frac{R}{2^P} \right\rfloor \bmod 2 = (R \gg P) \; \& \; 1
$$

**Tabla de mapeo patrón → índice:**

| Patrón $(s_{i-1}, s_i, s_{i+1})$ | Cálculo $P$ | Índice $P$ |
|:--------------------------------:|:-----------:|:----------:|
| (1, 1, 1)                       | 4+2+1       | 7          |
| (1, 1, 0)                       | 4+2+0       | 6          |
| (1, 0, 1)                       | 4+0+1       | 5          |
| (1, 0, 0)                       | 4+0+0       | 4          |
| (0, 1, 1)                       | 0+2+1       | 3          |
| (0, 1, 0)                       | 0+2+0       | 2          |
| (0, 0, 1)                       | 0+0+1       | 1          |
| (0, 0, 0)                       | 0+0+0       | 0          |

---

## 3. Clasificación de Comportamientos (Wolfram)

Los autómatas 1D exhiben cuatro clases de comportamiento asintótico. La implementación actual clasifica las reglas de la siguiente manera:

| Clase | Descripción | Reglas implementadas |
|-------|-------------|----------------------|
| **I** | Evoluciona hacia un estado homogéneo (fijo). | 0, 32, 128, 136, 160, 162, 168, 192, 200, 204, 232 |
| **II** | Estructuras periódicas o estables locales. | 4, 12, 40, 56, 60, 72, 90, 104, 108, 152, 164, 180, 184, 200, 232 |
| **III** | Caos aparente, pseudoaleatorio. | 15, 18, 22, 26, 30, 42, 46, 50, 54, 58, 62, 94, 102, 110, 122, 126, 146, 150, 154, 158, 182, 186, 210, 214 |
| **IV** | Complejidad (emergencia, universales). | 18, 26, 54, 58, 62, 90, 94, 102, **110**, 122, 126, 146, 150, 154, 158, 166, 182, 186, 210, 222 |

La regla **110** es **universalmente Turing** (Clase IV).

> **Nota:** Algunas reglas aparecen en múltiples clases según distintas fuentes de referencia. Esta clasificación sigue los valores canónicos más reconocidos en la literatura (Wolfram, Cook 2004).

---

## 4. Implementación Detallada

### 4.1. Estructura del Autómata (`Automata1D`)

**automata.hpp:**

```cpp
#ifndef AUTOMATA_HPP
#define AUTOMATA_HPP
#include <vector>

class Automata1D {
private:
    std::vector<int> estadoActual;
    int regla;
    int ancho;
    int obtenerPatron(int izq, int centro, int der) const;
    int aplicarRegla(int patron) const;
public:
    Automata1D(int ancho, int regla);
    void reiniciarCentro();
    void reiniciarAleatorio();
    void evolucionar();
    const std::vector<int>& getEstado() const { return estadoActual; }
};

#endif
```

**automata.cpp:**

```cpp
#include "automata.hpp"
#include <cstdlib>
#include <ctime>

Automata1D::Automata1D(int ancho, int regla) : ancho(ancho), regla(regla) {
    estadoActual.resize(ancho, 0);
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        seeded = true;
    }
}

void Automata1D::reiniciarCentro() {
    for (int i = 0; i < ancho; i++) estadoActual[i] = 0;
    estadoActual[ancho / 2] = 1;
}

void Automata1D::reiniciarAleatorio() {
    for (int i = 0; i < ancho; i++) estadoActual[i] = std::rand() % 2;
}

int Automata1D::obtenerPatron(int izq, int centro, int der) const {
    return (izq * 4) + (centro * 2) + (der * 1);
}

int Automata1D::aplicarRegla(int patron) const {
    return (regla >> patron) & 1;
}

void Automata1D::evolucionar() {
    std::vector<int> nuevoEstado(ancho);
    for (int i = 0; i < ancho; i++) {
        int izq = estadoActual[(i - 1 + ancho) % ancho];
        int centro = estadoActual[i];
        int der = estadoActual[(i + 1) % ancho];
        nuevoEstado[i] = aplicarRegla(obtenerPatron(izq, centro, der));
    }
    estadoActual = nuevoEstado;
}
```

#### Inicialización

El constructor reserva un vector de `ancho` celdas inicializadas en 0 y establece la semilla del generador de números aleatorios una sola vez mediante una variable estática.

#### Transición Local

- `obtenerPatron()` calcula el índice del vecindario con ponderación binaria: $P = 4s_{i-1} + 2s_i + s_{i+1}$.
- `aplicarRegla()` extrae el bit $P$-ésimo de la regla mediante desplazamiento y máscara: `(regla >> patron) & 1`.

#### Evolución Global (Paralelismo Sincrónico)

`evolucionar()` calcula el siguiente estado completo en un buffer temporal (`nuevoEstado`) y luego lo asigna al estado actual, garantizando que todas las celdas se actualicen simultáneamente (vecindario del paso anterior). El borde circular se implementa con aritmética modular: `(i - 1 + ancho) % ancho` e `(i + 1) % ancho`.

---

## 5. Algoritmo de Captura de Pantalla (SPEC-002)

### 5.1. Funcionamiento

La clase `InterfazAutomata` incluye funcionalidades para guardar capturas de la simulación:

```cpp
static std::string obtenerRutaCapturas();
void guardarCaptura(const std::string& nombreArchivo);
```

### 5.2. Flujo del algoritmo

```
1. rutaBase ← filesystem::current_path() / "capturas"
2. rutaCompleta ← rutaBase / nombreArchivo
3. INTENTAR:
     a. filesystem::create_directories(rutaBase)
     b. texture ← sf::Texture(ventana.getSize())
     c. texture.update(ventana)
     d. texture.copyToImage().saveToFile(rutaCompleta.string())
     e. Si éxito: imprimir "[OK] Guardado en: {ruta_absoluta}"
4. CAPTURAR excepciones: imprimir "[ERROR] Excepción: {mensaje}"
```

### 5.3. Casos de uso

| Tipo | Tecla | Nombre archivo |
|------|-------|----------------|
| Automático | — | `automata_regla_{N}_gen_{M}.png` |
| Manual | S | `automata_manual_{timestamp}_{contador}.png` |

---

## 6. Motor de Visualización (`InterfazAutomata`)

Renderiza la evolución temporal como matriz 2D (historial vertical):

- **Eje X**: Índice espacial $i \in [0, n-1]$.
- **Eje Y**: Tiempo discreto $t$ (filas descendentes).
- **Color**: Blanco = 1, Azul oscuro = 0.

**interfaz.hpp:**

```cpp
#ifndef INTERFAZ_HPP
#define INTERFAZ_HPP

#include <SFML/Graphics.hpp>
#include "automata.hpp"
#include <vector>
#include <string>
#include <optional>

class InterfazAutomata {
private:
    sf::RenderWindow ventana;
    unsigned int anchoCelda;
    unsigned int altoCelda;
    unsigned int anchoVentana;
    unsigned int altoVentana;
    std::vector<std::vector<int>> historial;
    unsigned int maxGeneracionesVisibles;
    sf::Font fuente;
    std::optional<sf::Text> textoInfo;
    sf::Color colorVivo;
    sf::Color colorMuerto;
public:
    InterfazAutomata(unsigned int ancho, unsigned int alto, unsigned int tamanoCelda);
    void dibujarGeneracion(const std::vector<int>& generacion, unsigned int fila);
    void actualizarPantalla();
    void agregarGeneracion(const std::vector<int>& gen);
    void mostrarInfo(int regla, int generacion, const std::string& clase);
    bool estaAbierta() const { return ventana.isOpen(); }
    void procesarEventos();
    void limpiarHistorial();
    void cerrar() { ventana.close(); }
};

#endif
```

**interfaz.cpp:**

```cpp
#include "interfaz.hpp"
#include <iostream>

InterfazAutomata::InterfazAutomata(unsigned int ancho, unsigned int alto, unsigned int tamanoCelda) 
    : anchoCelda(tamanoCelda), altoCelda(tamanoCelda) {
    anchoVentana = ancho * tamanoCelda;
    altoVentana = alto * tamanoCelda + 50;
    ventana.create(
        sf::VideoMode(sf::Vector2u(anchoVentana, altoVentana)), 
        "Autómata Celular 1D - Clasificacion de Wolfram"
    );
    ventana.setFramerateLimit(60);
    maxGeneracionesVisibles = alto;
    colorVivo = sf::Color::White;
    colorMuerto = sf::Color(20, 20, 40);
    if (!fuente.openFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
        std::cerr << "No se pudo cargar la fuente" << std::endl;
    }
    textoInfo.emplace(fuente, "", 14);
    textoInfo->setFillColor(sf::Color::White);
    textoInfo->setPosition(sf::Vector2f(10.f, static_cast<float>(altoVentana - 45)));
}

void InterfazAutomata::dibujarGeneracion(const std::vector<int>& generacion, unsigned int fila) {
    for (size_t i = 0; i < generacion.size(); i++) {
        sf::RectangleShape celda;
        celda.setSize(sf::Vector2f(
            static_cast<float>(anchoCelda - 1), 
            static_cast<float>(altoCelda - 1)
        ));
        celda.setPosition(sf::Vector2f(
            static_cast<float>(i * anchoCelda), 
            static_cast<float>(fila * altoCelda)
        ));
        if (generacion[i] == 1) {
            celda.setFillColor(colorVivo);
        } else {
            celda.setFillColor(colorMuerto);
        }
        ventana.draw(celda);
    }
}

void InterfazAutomata::actualizarPantalla() {
    ventana.clear(sf::Color::Black);
    for (size_t i = 0; i < historial.size() && i < maxGeneracionesVisibles; i++) {
        dibujarGeneracion(historial[i], static_cast<unsigned int>(i));
    }
    if (textoInfo.has_value()) {
        ventana.draw(textoInfo.value());
    }
    ventana.display();
}

void InterfazAutomata::agregarGeneracion(const std::vector<int>& gen) {
    historial.push_back(gen);
    if (historial.size() > maxGeneracionesVisibles) {
        historial.erase(historial.begin());
    }
}

void InterfazAutomata::mostrarInfo(int regla, int generacion, const std::string& clase) {
    std::string info = "Regla: " + std::to_string(regla) + 
                       " | Generacion: " + std::to_string(generacion) +
                       " | Clase " + clase;
    if (textoInfo.has_value()) {
        textoInfo->setString(info);
    }
}

void InterfazAutomata::procesarEventos() {
    while (const auto evento = ventana.pollEvent()) {
        if (evento->is<sf::Event::Closed>()) {
            ventana.close();
        }
        if (const auto* keyPressed = evento->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Space) {
                limpiarHistorial();
            }
        }
    }
}

void InterfazAutomata::limpiarHistorial() {
    historial.clear();
}
```

---

## 6. Flujo de Ejecución (`main.cpp`)

```cpp
#include "automata.hpp"
#include "interfaz.hpp"
#include <iostream>
#include <string>
#include <vector>

std::string obtenerClaseWolfram(int regla) {
    if (regla == 0 || regla == 32 || regla == 128 || regla == 136 || 
        regla == 160 || regla == 162 || regla == 168 || regla == 192 ||
        regla == 200 || regla == 204 || regla == 232)
        return "I (Homogéneo)";
    if (regla == 4 || regla == 12 || regla == 40 || regla == 56 ||
        regla == 60 || regla == 72 || regla == 90 || regla == 104 ||
        regla == 108 || regla == 152 || regla == 164 || regla == 180 ||
        regla == 184 || regla == 200 || regla == 232)
        return "II (Periódico)";
    if (regla == 15 || regla == 18 || regla == 22 || regla == 26 ||
        regla == 30 || regla == 42 || regla == 46 || regla == 50 ||
        regla == 54 || regla == 58 || regla == 62 || regla == 94 ||
        regla == 102 || regla == 110 || regla == 122 || regla == 126 ||
        regla == 146 || regla == 150 || regla == 154 || regla == 158 ||
        regla == 182 || regla == 186 || regla == 210 || regla == 214)
        return "III (Caos)";
    if (regla == 18 || regla == 26 || regla == 54 || regla == 58 ||
        regla == 62 || regla == 90 || regla == 94 || regla == 102 ||
        regla == 110 || regla == 122 || regla == 126 || regla == 146 ||
        regla == 150 || regla == 154 || regla == 158 || regla == 166 ||
        regla == 182 || regla == 186 || regla == 210 || regla == 222) {
        if (regla == 110) return "IV (Complejo - Universal)";
        return "IV (Complejo)";
    }
    return "II (Periódico)";
}

int main() {
    const int ANCHO = 80;         // Celdas por fila
    const int ALTO = 60;          // Generaciones visibles
    const int TAMANO_CELDA = 10;  // Píxeles por celda

    std::cout << "=== Autómata Celular 1D - Reglas de Wolfram ===" << std::endl;
    std::cout << "Ejecutando reglas 30, 90 y 110..." << std::endl;

    std::vector<int> reglas = {30, 90, 110};
    int delayMs = 50;

    for (int regla : reglas) {
        std::cout << "\n--- Ejecutando Regla " << regla << " (" 
                  << obtenerClaseWolfram(regla) << ") ---" << std::endl;

        Automata1D automata(ANCHO, regla);
        InterfazAutomata interfaz(ANCHO, ALTO, TAMANO_CELDA);

        automata.reiniciarCentro();
        interfaz.mostrarInfo(regla, 0, obtenerClaseWolfram(regla));

        int generacion = 0;
        sf::Clock reloj;

        while (interfaz.estaAbierta() && generacion < ALTO) {
            interfaz.procesarEventos();
            interfaz.dibujarGeneracion(automata.getEstado(), generacion);

            if (reloj.getElapsedTime().asMilliseconds() > delayMs) {
                interfaz.actualizarPantalla();
                automata.evolucionar();
                generacion++;
                interfaz.agregarGeneracion(automata.getEstado());
                interfaz.mostrarInfo(regla, generacion, obtenerClaseWolfram(regla));
                reloj.restart();
            }
        }

        std::cout << "Regla " << regla << " completada: " 
                  << generacion << " generaciones." << std::endl;

        sf::sleep(sf::seconds(1.0f));
    }

    std::cout << "\n=== Simulación completada ===" << std::endl;
    return 0;
}
```

### Secuencia principal

1. **Parámetros fijos**: `ANCHO = 80` celdas, `ALTO = 60` generaciones visibles, `TAMANO_CELDA = 10` px.
2. **Ejecución secuencial**: El programa ejecuta las reglas **30, 90 y 110** en secuencia. Para cada regla:
   - Se crea un `Automata1D` con estado inicial centrado (celda central = 1, resto = 0).
   - Se crea una ventana gráfica `InterfazAutomata`.
   - Bucle principal: mientras la ventana esté abierta y no se hayan alcanzado `ALTO` generaciones:
     - Procesar eventos (cierre de ventana, tecla Espacio para limpiar historial).
     - Dibujar el estado actual en la ventana.
     - Cada ~50 ms: actualizar la pantalla, evolucionar el autómata, agregar la nueva generación al historial y actualizar la barra de información.
   - Al completar `ALTO` generaciones, esperar 1 segundo antes de ejecutar la siguiente regla.
3. **Salida por consola**: Muestra el progreso de cada regla y el número de generaciones ejecutadas.

---

## 8. Instrucciones de Compilación y Ejecución

### 7.1. Requisitos del Sistema

- **Sistema Operativo**: Windows 10/11 (64-bit recomendado)
- **Compilador**: Compatible con C++17 (Visual Studio 2022, GCC ≥ 8, Clang ≥ 7)
- **CMake**: Versión 3.16 o superior
- **SFML**: Versión 3.0.2 exacta (obligatorio para la API de eventos utilizada)
- **Dependencias**:
  - SFML Graphics, Window, System modules
  - Fuente Arial (ruta predeterminada: `C:\\Windows\\Fonts\\arial.ttf`)

### 7.2. Proceso de Compilación

```bash
# Desde la raíz del proyecto:
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DSFML_DIR="<ruta-a-sfml-3.0.2>/lib/cmake/SFML"
cmake --build . --config Release
```

**Notas importantes**:
1. La variable `SFML_DIR` es necesaria si CMake no encuentra SFML 3.0.2 automáticamente.
2. En Visual Studio 2022 con la extensión CMake Tools, basta con seleccionar el kit adecuado y SFML 3.0.2 será detectado si está instalado vía vcpkg.
3. El ejecutable generado se nombrará `automata.exe` (Windows) o `automata` (Linux/macOS).

### 7.3. Ejecución del Programa

```bash
# Desde el directorio de build:
./automata        # En Linux/macOS
automata.exe      # En Windows (desde CMD/PowerShell)
```

**Secuencia de interacción**:
1. Se abre una ventana gráfica titulada "Autómata Celular 1D - Clasificacion de Wolfram".
2. La simulación comienza automáticamente con la regla 30, seguida de 90 y 110.
3. Controles disponibles:
   - **Clic en X de la ventana**: Termina el programa.
   - **Tecla Espacio**: Limpia el historial y reinicia la simulación de la regla actual desde el estado centrado.
4. Cada vez que se completan `ALTO` (60) generaciones, la simulación pasa automáticamente a la siguiente regla tras 1 segundo de espera.
5. La barra inferior muestra: `Regla: X | Generacion: Y | Clase Z`.

### 7.4. Salida Esperada

**Consola**:
```
=== Autómata Celular 1D - Reglas de Wolfram ===
Ejecutando reglas 30, 90 y 110...

--- Ejecutando Regla 30 (III (Caos)) ---
Regla 30 completada: 60 generaciones.

--- Ejecutando Regla 90 (III (Caos)) ---
Regla 90 completada: 60 generaciones.

--- Ejecutando Regla 110 (IV (Complejo - Universal)) ---
Regla 110 completada: 60 generaciones.

=== Simulación completada ===
```

**Ventana Gráfica**:
- Fila superior: Generación más reciente (estado actual).
- Histórico vertical: Últimas 60 generaciones.
- Células vivas: Blancas.
- Células muertas: Azul oscuro (#141428).
- Barra inferior: Texto blanco mostrando `Regla: X | Generacion: Y | Clase Z`.

---

## 9. Complejidad Asintótica

### 8.1. Tiempo

| Operación | Complejidad | Descripción |
|-----------|-------------|-------------|
| `obtenerPatron()` | $O(1)$ | Aritmética constante. |
| `aplicarRegla()` | $O(1)$ | Shift + máscara bitwise. |
| `evolucionar()` | $O(n)$ | Recorre $n$ celdas, $O(1)$ por célula. |
| `reiniciarCentro()` | $O(n)$ | Recorre $n$ celdas poniéndolas en 0, luego una asignación. |
| `reiniciarAleatorio()` | $O(n)$ | Recorre $n$ celdas generando valores aleatorios. |
| `dibujarGeneracion()` | $O(n)$ | Dibuja $n$ rectángulos por generación. |
| `agregarGeneracion()` | $O(n)$ | Copia un vector de $n$ elementos al historial. |
| **Total por $t$ pasos** | $O(t \cdot n)$ | $t$ evoluciones completas. |

### 8.2. Espacio

| Estructura | Complejidad | Descripción |
|------------|-------------|-------------|
| `estadoActual` | $O(n)$ | Vector de $n$ enteros. |
| `nuevoEstado` | $O(n)$ | Buffer temporal para evolución. |
| `historial` | $O(h \cdot n)$ | $h$ generaciones guardadas (máximo `ALTO` = 60). |
| **Total** | $O(n + h \cdot n) = O(h \cdot n)$ | Con $h$ fijo (60) $\Rightarrow$ $O(n)$. |

---

## 10. Compatibilidad con SFML 3.0.2

El código utiliza la API moderna de SFML 3.0.2 para el manejo de eventos. Los puntos clave de compatibilidad son:

### 9.1. `automata.hpp`
- `getEstado()` está definido inline dentro de la declaración de la clase, retornando una referencia constante al vector `estadoActual`. No requiere punto y coma adicional tras la llave de cierre del `#endif`.

### 9.2. `automata.cpp`
- Inicialización de semilla para `std::rand()` mediante variable estática `seeded`, garantizando variabilidad en el estado aleatorio entre ejecuciones.
- Uzo de `<cstdlib>` y `<ctime>` para `std::srand` y `std::time`.

### 9.3. `interfaz.hpp`
- Uso correcto de `std::optional<sf::Text>` para el miembro `textoInfo`, permitiendo verificación segura de estado con `has_value()`.

### 9.4. `interfaz.cpp` — Manejo de Eventos SFML 3.0.2

`procesarEventos()` utiliza el nuevo sistema de eventos de SFML 3.x basado en punteros opcionales:

```cpp
void InterfazAutomata::procesarEventos() {
    while (const auto evento = ventana.pollEvent()) {
        if (evento->is<sf::Event::Closed>()) {
            ventana.close();
        }
        if (const auto* keyPressed = evento->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Space) {
                limpiarHistorial();
            }
        }
    }
}
```

- `ventana.pollEvent()` retorna `std::optional<sf::Event>`, desenvelopado con `const auto evento = ...`.
- `evento->is<sf::Event::Closed>()` para detectar cierre de ventana.
- `evento->getIf<sf::Event::KeyPressed>()` retorna un puntero al evento de tecla si aplica, o `nullptr` en caso contrario.

---

## 11. Extensibilidad

- **Nuevas reglas**: Añadir entradas en `obtenerClaseWolfram()` en `main.cpp`.
- **Reglas dinámicas**: Modificar `main()` para aceptar reglas como argumento de línea de comandos o mediante entrada interactiva.
- **Dimensionalidad**: Generalizar a 2D adaptando vecindad (Moore/Von Neumann) y estructuras de datos.
- **Condición de borde**: Modificar índices en `evolucionar()` para bordes fijos/absorbentes.
- **Reglas estocásticas**: Reemplazar `aplicarRegla()` por función que devuelva probabilidades.
- **Personalización visual**: Añadir configuración de colores, tamaños y fuentes mediante archivo de configuración.

---

## 12. Referencias

1. Wolfram, S. (2002). *A New Kind of Science*. Wolfram Media.
2. Cook, M. (2004). Universality in Elementary Cellular Automata. *Complex Systems*, 15(1), 1-40.
3. SFML Documentation. *Simple and Fast Multimedia Library*. [https://www.sfml-dev.org/documentation/3.0.0/](https://www.sfml-dev.org/documentation/3.0.0/)
4. ISO/IEC 14882:2017. *C++ Standard (C++17)*.

---

*Documentación actualizada para reflejar el estado actual del código. Versión 3.0 - 13 de mayo de 2026.*
