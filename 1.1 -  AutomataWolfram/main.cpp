#include "automata.hpp"
#include "interfaz.hpp"
#include <iostream>
#include <string>
#include <map>
#include <filesystem>

// Clasificación de Wolfram para autómatas celulares 1D
std::string obtenerClaseWolfram(int regla) {
    // Clasificación aproximada basada en comportamiento típico
    // Clase 1: Evoluciona a estado homogéneo
    if (regla == 0 || regla == 32 || regla == 128 || regla == 136 || 
        regla == 160 || regla == 162 || regla == 168 || regla == 192 ||
        regla == 200 || regla == 204 || regla == 232) {
        return "I (Homogéneo)";
    }
    // Clase 2: Estructuras periódicas estables
    if (regla == 4 || regla == 12 || regla == 40 || regla == 56 ||
        regla == 60 || regla == 72 || regla == 90 || regla == 104 ||
        regla == 108 || regla == 152 || regla == 164 || regla == 180 ||
        regla == 184 || regla == 200 || regla == 232) {
        return "II (Periódico)";
    }
    // Clase 3: Caos aparente (pseudoaleatorio)
    if (regla == 15 || regla == 18 || regla == 22 || regla == 26 ||
        regla == 30 || regla == 42 || regla == 46 || regla == 50 ||
        regla == 54 || regla == 58 || regla == 62 || regla == 94 ||
        regla == 102 || regla == 110 || regla == 122 || regla == 126 ||
        regla == 146 || regla == 150 || regla == 154 || regla == 158 ||
        regla == 182 || regla == 186 || regla == 210 || regla == 214) {
        return "III (Caos)";
    }
    // Clase 4: Complejidad (incluyendo reglas notables como 110)
    if (regla == 18 || regla == 26 || regla == 54 || regla == 58 ||
        regla == 62 || regla == 90 || regla == 94 || regla == 102 ||
        regla == 110 || regla == 122 || regla == 126 || regla == 146 ||
        regla == 150 || regla == 154 || regla == 158 || regla == 166 ||
        regla == 182 || regla == 186 || regla == 210 || regla == 222) {
        // Regla 110 es universalmente conocida como Clase 4
        if (regla == 110) return "IV (Complejo - Universal)";
        return "IV (Complejo)";
    }
    return "II (Periódico)";  // Por defecto
}

int main(int argc, char* argv[]) {
    const int ANCHO = 80;      // Celdas por fila
    const int ALTO = 60;       // Generaciones visibles
    const int TAMANO_CELDA = 10; // Píxeles por celda

    std::cout << "=== Autómata Celular 1D - Reglas de Wolfram ===" << std::endl;
    std::cout << "Ejecutando reglas 128, 90, 210 y 110..." << std::endl;

    // Reglas a ejecutar
    std::vector<int> reglas = {128, 90, 210, 110};
    int delayMs = 50; // Milisegundos entre generaciones

    // argv[0] es la ruta del ejecutable: .../build/Debug/automata.exe
    // Subimos 2 niveles: Debug -> build -> raíz del proyecto
    std::filesystem::path exePath = std::filesystem::absolute(argv[0]).parent_path();
    std::filesystem::path projectRoot = exePath.parent_path().parent_path();
    
    // Forzar que el programa "trabaje" desde la carpeta principal
    std::filesystem::current_path(projectRoot);
    
    // Ruta de la carpeta capturas (en minúsculas)
    std::filesystem::path rutaCapturas = projectRoot / "capturas";
    std::cout << "Las capturas se guardarán en: " << std::filesystem::absolute(rutaCapturas) << std::endl;

    for (int regla : reglas) {
        std::cout << "\n--- Ejecutando Regla " << regla << " (" 
                << obtenerClaseWolfram(regla) << ") ---" << std::endl;

        Automata1D automata(ANCHO, regla);
        InterfazAutomata interfaz(ANCHO, ALTO, TAMANO_CELDA, rutaCapturas);

        automata.reiniciarCentro();
        interfaz.mostrarInfo(regla, 0, obtenerClaseWolfram(regla));

        int generacion = 0;
        sf::Clock reloj;

        while (interfaz.estaAbierta() && generacion < ALTO) {
            // Procesar eventos (cerrar ventana, etc.)
            interfaz.procesarEventos();

            interfaz.dibujarGeneracion(automata.getEstado(), generacion);

            if (reloj.getElapsedTime().asMilliseconds() > delayMs) {
                interfaz.actualizarPantalla();
                
                // Avanzar generación
                automata.evolucionar();
                generacion++;
                interfaz.agregarGeneracion(automata.getEstado());
                interfaz.mostrarInfo(regla, generacion, obtenerClaseWolfram(regla));
                
                reloj.restart();
            }
        }

        std::string nombreAuto = "automata_regla_" + std::to_string(regla) + 
                "_gen_" + std::to_string(generacion) + ".png";
        interfaz.guardarCaptura(nombreAuto);

        std::cout << "Regla " << regla << " completada: " 
                << generacion << " generaciones." << std::endl;

        // Esperar un momento antes de la siguiente regla
        sf::sleep(sf::seconds(1.0f));
    }

    std::cout << "\n=== Simulación completada ===" << std::endl;
    return 0;
}
