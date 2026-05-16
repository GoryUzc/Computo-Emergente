#include "interfaz.hpp"
#include <iostream>
#include <ctime>

InterfazAutomata::InterfazAutomata(unsigned int ancho, unsigned int alto, unsigned int tamanoCelda, const std::filesystem::path& dirCapturas) 
    : anchoCelda(tamanoCelda), altoCelda(tamanoCelda), directorioCapturas(dirCapturas) {
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
            if (keyPressed->code == sf::Keyboard::Key::S) {
                auto t = std::time(nullptr);
                std::string nombre = "automata_manual_" + std::to_string(t) + "_" + 
                                    std::to_string(contadorManual++) + ".png";
                guardarCaptura(nombre);
            }
        }
    }
}

void InterfazAutomata::limpiarHistorial() {
    historial.clear();
}

int InterfazAutomata::contadorManual = 0;

void InterfazAutomata::guardarCaptura(const std::string& nombreArchivo) {
    try {
        std::filesystem::path rutaCompleta = directorioCapturas / nombreArchivo;
        
        std::filesystem::create_directories(directorioCapturas);
        
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