#include "procesador_imagen.hpp"
#include <stdexcept>
#include <iostream>
#include <cstdint>  

ProcesadorImagen::ProcesadorImagen() = default;

bool ProcesadorImagen::cargarImagen(const std::string& ruta) {
    if (!imagen.loadFromFile(ruta)) {
        std::cerr << "[ProcesadorImagen] Error al cargar: " << ruta << '\n';
        return false;
    }
    return true;
}

bool ProcesadorImagen::guardarImagen(const std::string& ruta) const {
    if (!imagen.saveToFile(ruta)) {
        std::cerr << "[ProcesadorImagen] Error al guardar: " << ruta << '\n';
        return false;
    }
    return true;
}

std::vector<std::vector<int>> ProcesadorImagen::aMatrizBinaria(int umbral) const {
    sf::Vector2u tam = imagen.getSize();
    
    std::vector<std::vector<int>> matriz(
        static_cast<size_t>(tam.y), 
        std::vector<int>(static_cast<size_t>(tam.x), 0)
    );

    for (unsigned int y = 0; y < tam.y; ++y) {
        for (unsigned int x = 0; x < tam.x; ++x) {
            sf::Color pixel = imagen.getPixel(sf::Vector2u(x, y));
            int gris = (pixel.r + pixel.g + pixel.b) / 3;
            matriz[y][x] = (gris >= umbral) ? 1 : 0;
        }
    }
    return matriz;
}

void ProcesadorImagen::deMatrizBinaria(const std::vector<std::vector<int>>& matriz) {
    if (matriz.empty() || matriz[0].empty()) {
        throw std::invalid_argument("La matriz binaria está vacía.");
    }

    sf::Vector2u tam(
        static_cast<unsigned int>(matriz[0].size()), 
        static_cast<unsigned int>(matriz.size())
    );
    

    imagen = sf::Image(tam, sf::Color::Black);

    for (unsigned int y = 0; y < tam.y; ++y) {
        for (unsigned int x = 0; x < tam.x; ++x) {
            // ✅ SFML 3: usar std::uint8_t estándar
            std::uint8_t valor = static_cast<std::uint8_t>(matriz[y][x] * 255);
            imagen.setPixel(sf::Vector2u(x, y), sf::Color(valor, valor, valor));
        }
    }
}

sf::Vector2u ProcesadorImagen::getTamano() const {
    return imagen.getSize();
}