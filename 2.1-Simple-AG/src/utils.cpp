#include "utils.hpp"
#include <cmath>

int binarioAEntero(const std::vector<int>& bits) {
    int resultado = 0;
    int L = bits.size();
    for (int i = 0; i < L; ++i) {
        resultado += bits[i] * static_cast<int>(std::pow(2, L - 1 - i));
    }
    return resultado;
}

double mapeoReal(int entero, int L, int precision) {
    double maxEntero = std::pow(2, L) - 1;
    double x = entero * 63.0 / maxEntero;
    double factor = std::pow(10.0, precision);
    return std::round(x * factor) / factor;
}