#include "automata.hpp"
#include <cstdlib>
#include <ctime>

Automata1D::Automata1D(int ancho, int regla) : ancho(ancho), regla(regla) {
    estadoActual.resize(ancho, 0);
    // Seed random number generator once
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