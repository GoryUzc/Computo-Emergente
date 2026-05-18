#include "automata1d.hpp"

Automata1D::Automata1D(int ancho, int regla, bool guardarHistorial)
    : ancho(ancho), regla(regla), guardarHistorial(guardarHistorial)
{
    estadoActual.resize(ancho, 0);

    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }
}

void Automata1D::reiniciarCentro() {
    std::fill(estadoActual.begin(), estadoActual.end(), 0);
    estadoActual[ancho / 2] = 1;

    if (guardarHistorial) {
        limpiarHistorial();
        historial.push_back(estadoActual);
    }
}

void Automata1D::reiniciarAleatorio() {
    for (int i = 0; i < ancho; ++i) {
        estadoActual[i] = std::rand() % 2;
    }

    if (guardarHistorial) {
        limpiarHistorial();
        historial.push_back(estadoActual);
    }
}

int Automata1D::obtenerPatron(int izq, int centro, int der) const {
    return izq * 4 + centro * 2 + der * 1;
}

int Automata1D::aplicarRegla(int patron) const {
    return (regla >> patron) & 1;
}

void Automata1D::evolucionar() {
    std::vector<int> nuevoEstado(ancho);

    for (int i = 0; i < ancho; ++i) {
        int izq    = estadoActual[(i - 1 + ancho) % ancho];
        int centro = estadoActual[i];
        int der    = estadoActual[(i + 1) % ancho];

        nuevoEstado[i] = aplicarRegla(obtenerPatron(izq, centro, der));
    }

    estadoActual = nuevoEstado;

    if (guardarHistorial) {
        historial.push_back(estadoActual);
    }
}

const std::vector<int>& Automata1D::getEstado() const {
    return estadoActual;
}

const std::vector<std::vector<int>>& Automata1D::getHistorial() const {
    return historial;
}

int Automata1D::getRegla() const {
    return regla;
}

int Automata1D::getAncho() const {
    return ancho;
}

void Automata1D::setEstado(const std::vector<int>& nuevo) {
    if (static_cast<int>(nuevo.size()) != ancho) return;
    estadoActual = nuevo;
}

void Automata1D::flipCelda(int indice) {
    if (indice >= 0 && indice < ancho) {
        estadoActual[indice] ^= 1;
    }
}

void Automata1D::limpiarHistorial() {
    historial.clear();
}

double Automata1D::calcularDensidad() const {
    int suma = 0;
    for (int celda : estadoActual) suma += celda;
    if (ancho == 0) return 0.0;
    return static_cast<double>(suma) / ancho;
}
