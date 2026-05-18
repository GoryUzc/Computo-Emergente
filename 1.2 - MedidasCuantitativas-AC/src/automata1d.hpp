#ifndef AUTOMATA1D_HPP
#define AUTOMATA1D_HPP

#include <vector>
#include <cstdlib>
#include <ctime>

class Automata1D {
public:
    // Constructor: ancho = número de celdas, regla = 0-255
    //              guardarHistorial = true si se necesita entropía temporal
    Automata1D(int ancho, int regla, bool guardarHistorial = false);

    // Estado inicial: un 1 en el centro, resto 0
    void reiniciarCentro();

    // Estado inicial: aleatorio binario
    void reiniciarAleatorio();

    // Evoluciona una generación completa (paralelismo sincrónico)
    void evolucionar();

    // Acceso
    const std::vector<int>& getEstado() const;
    const std::vector<std::vector<int>>& getHistorial() const;
    int getRegla() const;
    int getAncho() const;

    // Modificación
    void setEstado(const std::vector<int>& nuevo);
    void flipCelda(int indice);  // XOR con 1, para Lyapunov
    void limpiarHistorial();

    // Medida básica inline
    double calcularDensidad() const;

private:
    std::vector<int> estadoActual;
    std::vector<std::vector<int>> historial;  // Solo se llena si guardarHistorial = true
    int regla;
    int ancho;
    bool guardarHistorial;

    int obtenerPatron(int izq, int centro, int der) const;   // P = 4*izq + 2*centro + der
    int aplicarRegla(int patron) const;                       // (regla >> patron) & 1
};

#endif // AUTOMATA1D_HPP
