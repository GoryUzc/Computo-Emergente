#ifndef INTERFAZ_HPP
#define INTERFAZ_HPP

#include <SFML/Graphics.hpp>
#include "automata.hpp"
#include <vector>
#include <string>
#include <optional>
#include <filesystem>

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
    static int contadorManual;
    std::filesystem::path directorioCapturas;

public:
    InterfazAutomata(unsigned int ancho, unsigned int alto, unsigned int tamanoCelda, const std::filesystem::path& dirCapturas);
    void dibujarGeneracion(const std::vector<int>& generacion, unsigned int fila);
    void actualizarPantalla();
    void agregarGeneracion(const std::vector<int>& gen);
    void mostrarInfo(int regla, int generacion, const std::string& clase);
    bool estaAbierta() const { return ventana.isOpen(); }
    void procesarEventos();
    void limpiarHistorial();
    void cerrar() { ventana.close(); }
    void guardarCaptura(const std::string& nombreArchivo);
};

#endif