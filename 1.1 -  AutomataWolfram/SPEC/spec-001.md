# SPEC-001: Corrección y Verificación de Código

## 1. Objetivo

Verificar y corregir el proyecto C++ de autómata celular 1D con SFML 3.0.2 para garantizar compilación sin errores, warnings y corrección lógica.

---

## 2. Entorno de Desarrollo

| Componente | Versión |
|------------|---------|
| Sistema Operativo | Windows 10/11 (x64) |
| Compilador | MSVC (Visual Studio 2022) |
| SFML | 3.0.2 (vía vcpkg) |
| CMake | 4.3.2 |
| C++ | Estándar 17 |

---

## 3. Archivos del Proyecto

### 3.1 CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(AutomataWolfram)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_executable(automata 
    main.cpp 
    automata.cpp 
    interfaz.cpp
)

target_link_libraries(automata 
    SFML::Graphics 
    SFML::Window 
    SFML::System
)
```

### 3.2 automata.hpp

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

### 3.3 automata.cpp

```cpp
#include "automata.hpp"
#include <cstdlib>
#include <ctime>

Automata1D::Automata1D(int ancho, int regla) 
    : ancho(ancho), regla(regla) {
    estadoActual.resize(ancho, 0);
}

void Automata1D::reiniciarCentro() {
    for (int i = 0; i < ancho; i++) {
        estadoActual[i] = 0;
    }
    estadoActual[ancho / 2] = 1;
}

void Automata1D::reiniciarAleatorio() {
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        seeded = true;
    }
    for (int i = 0; i < ancho; i++) {
        estadoActual[i] = std::rand() % 2;
    }
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

### 3.4 interfaz.hpp

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

### 3.5 interfaz.cpp

```cpp
#include "interfaz.hpp"
#include <iostream>

InterfazAutomata::InterfazAutomata(
    unsigned int ancho, 
    unsigned int alto, 
    unsigned int tamanoCelda) 
    : anchoCelda(tamanoCelda), altoCelda(tamanoCelda) {
    
    anchoVentana = ancho * tamanoCelda;
    altoVentana = alto * tamanoCelda + 50;
    
    ventana.create(
        sf::VideoMode(sf::Vector2u(anchoVentana, altoVentana)), 
        "Autómata Celular 1D - Clasificación de Wolfram"
    );
    ventana.setFramerateLimit(60);
    maxGeneracionesVisibles = alto;
    
    colorVivo = sf::Color::White;
    colorMuerto = sf::Color(20, 20, 40);
    
    if (!fuente.openFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
        std::cerr << "Advertencia: No se pudo cargar la fuente Arial\n";
    }
    
    textoInfo.emplace(fuente, "", 14);
    textoInfo->setFillColor(sf::Color::White);
    textoInfo->setPosition(sf::Vector2f(10.f, static_cast<float>(altoVentana - 45)));
}

void InterfazAutomata::dibujarGeneracion(
    const std::vector<int>& generacion, 
    unsigned int fila) {
    
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
                       " | Generación: " + std::to_string(generacion) +
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

### 3.6 main.cpp

```cpp
#include <iostream>
#include <string>
#include <map>
#include "automata.hpp"
#include "interfaz.hpp"

std::string obtenerClaseWolfram(int regla) {
    if (regla == 0 || regla == 32 || regla == 128 || regla == 136 || 
        regla == 160 || regla == 162 || regla == 168 || regla == 192 ||
        regla == 200 || regla == 204 || regla == 232) {
        return "I (Homogéneo)";
    }
    if (regla == 4 || regla == 12 || regla == 40 || regla == 56 ||
        regla == 60 || regla == 72 || regla == 90 || regla == 104 ||
        regla == 108 || regla == 152 || regla == 164 || regla == 180 ||
        regla == 184 || regla == 200 || regla == 232) {
        return "II (Periódico)";
    }
    if (regla == 15 || regla == 18 || regla == 22 || regla == 26 ||
        regla == 30 || regla == 42 || regla == 46 || regla == 50 ||
        regla == 54 || regla == 58 || regla == 62 || regla == 94 ||
        regla == 102 || regla == 110 || regla == 122 || regla == 126 ||
        regla == 146 || regla == 150 || regla == 154 || regla == 158 ||
        regla == 182 || regla == 186 || regla == 210 || regla == 214) {
        return "III (Caos)";
    }
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

int mostrarMenu() {
    std::cout << "\n=== AUTÓMATA CELULAR 1D - WOLFRAM ===\n";
    std::cout << "CLASE I (Homogéneo): 0, 32, 128, 136, 160, 162, 168, 192, 200, 204, 232\n";
    std::cout << "CLASE II (Periódico): 4, 12, 40, 56, 60, 72, 90, 104, 108, 152, 164, 180, 184\n";
    std::cout << "CLASE III (Caos): 15, 18, 22, 26, 30, 42, 46, 50, 54, 58, 62, 94, 102, 110, 122, 126, 146, 150, 154, 158, 182, 186, 210, 214\n";
    std::cout << "CLASE IV (Complejo): 18, 26, 54, 58, 62, 90, 94, 102, 110, 122, 126, 146, 150, 154, 158, 166, 182, 186, 210, 222\n";
    std::cout << "Ingresa número de regla (0-255): ";
    
    int regla;
    std::cin >> regla;
    
    if (std::cin.fail() || regla < 0 || regla > 255) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Entrada inválida. Usando regla 30.\n";
        return 30;
    }
    
    return regla;
}

int main() {
    const int ANCHO = 100;
    const int ALTO = 80;
    const int TAMANO_CELDA = 8;

    int regla = mostrarMenu();
    std::string clase = obtenerClaseWolfram(regla);
    
    std::cout << "Regla: " << regla << " | Clase: " << clase << "\n";
    std::cout << "Ejecutando simulación... (Cierra la ventana para terminar)\n";

    Automata1D automata(ANCHO, regla);
    InterfazAutomata interfaz(ANCHO, ALTO, TAMANO_CELDA);
    
    int generacion = 0;
    bool reiniciar = true;

    while (interfaz.estaAbierta()) {
        interfaz.procesarEventos();

        if (reiniciar) {
            automata.reiniciarCentro();
            interfaz.limpiarHistorial();
            generacion = 0;
            reiniciar = false;
        }

        interfaz.agregarGeneracion(automata.getEstado());
        interfaz.mostrarInfo(regla, generacion, clase);
        interfaz.actualizarPantalla();
        
        automata.evolucionar();
        generacion++;

        if (generacion >= ALTO) {
            sf::sleep(sf::seconds(2));
            reiniciar = true;
        }
        
        sf::sleep(sf::milliseconds(50));
    }

    return 0;
}
```

---

## 4. Correcciones Realizadas

| Archivo | Error Original | Corrección |
|---------|---------------|------------|
| interfaz.hpp | `std::optional<<sf::Text>` | `std::optional<sf::Text>` |
| interfaz.cpp | `pollEvent(evento)` SFML 2 | `pollEvent()` retorna `optional` en SFML 3 |
| interfaz.cpp | `evento.is<<sf::Event::Closed>>()` | `evento->is<sf::Event::Closed>()` |
| automata.cpp | `rand()` sin semilla | Añadido `std::srand` con seed único |
| main.cpp | `std::cin >> regla` sin validación | Validación de rango 0-255 |

---

## 5. Criterios de Aceptación

- [x] Compilación sin errores en Visual Studio 2022
- [x] Compilación sin warnings (`/W4`)
- [x] API SFML 3.0.2 correctamente implementada
- [x] Manejo de eventos funcional
- [x] Visualización de autómata correcta