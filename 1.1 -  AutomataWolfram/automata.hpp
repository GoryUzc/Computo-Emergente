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
    const std::vector<int>& getEstado() const { return estadoActual; };
};
#endif