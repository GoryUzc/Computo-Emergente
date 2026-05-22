#include "detector_bordes.hpp"

DetectorBordes::DetectorBordes() {}

std::function<int(const Automata2D&, size_t, size_t)> DetectorBordes::reglaSilueta() {
    return [](const Automata2D& ac, size_t row, size_t col) -> int {
        int celda = ac.getState(row, col);
        if (celda == 1) {
            int vecinos = ac.countMooreNeighbors(row, col);
            return (vecinos < 8) ? 1 : 0;
        }
        return 0;
    };
}

std::function<int(const Automata2D&, size_t, size_t)> DetectorBordes::reglaBordeCompleto() {
    return [](const Automata2D& ac, size_t row, size_t col) -> int {
        int celda = ac.getState(row, col);
        int vecinos = ac.countMooreNeighbors(row, col);
        
        if (celda == 1 && vecinos < 8) {
            return 1;
        } else if (celda == 0 && vecinos > 0) {
            return 1;
        }
        return 0;
    };
}

std::function<int(const Automata2D&, size_t, size_t)> DetectorBordes::reglaErosion() {
    return [](const Automata2D& ac, size_t row, size_t col) -> int {
        int celda = ac.getState(row, col);
        if (celda == 1) {
            int vecinos = ac.countMooreNeighbors(row, col);
            return (vecinos == 8) ? 1 : 0;
        }
        return 0;
    };
}

std::function<int(const Automata2D&, size_t, size_t)> DetectorBordes::reglaDilatacion() {
    return [](const Automata2D& ac, size_t row, size_t col) -> int {
        int celda = ac.getState(row, col);
        if (celda == 1) {
            return 1;
        }
        int vecinos = ac.countMooreNeighbors(row, col);
        return (vecinos > 0) ? 1 : 0;
    };
}

std::function<int(const Automata2D&, size_t, size_t)> DetectorBordes::obtenerRegla(const std::string& nombre) {
    if (nombre == "silueta") return reglaSilueta();
    if (nombre == "borde_completo") return reglaBordeCompleto();
    if (nombre == "erosion") return reglaErosion();
    if (nombre == "dilatacion") return reglaDilatacion();
    return nullptr;
}

std::vector<std::vector<int>> DetectorBordes::procesar(const std::vector<std::vector<int>>& matriz, const std::string& nombreRegla) {
    Automata2D automata(matriz);
    auto regla = obtenerRegla(nombreRegla);
    if (regla) {
        automata.applyRule(regla);
    }
    return automata.getMatrix();
}