#include "utils.hpp"
#include <random>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

double decodificar_variable(const std::bitset<32>& crom, int inicio, int fin) {
    unsigned long valor_entero = 0;
    for (int i = inicio; i <= fin; i++) {
        valor_entero = (valor_entero << 1) | crom[i];
    }
    int n_bits = fin - inicio + 1;
    double max_valor = static_cast<double>((1 << n_bits) - 1);
    return -1.0 + valor_entero * (2.0 / max_valor);
}

double decodificar_x(const std::bitset<32>& crom) {
    return decodificar_variable(crom, 0, 15);
}

double decodificar_y(const std::bitset<32>& crom) {
    return decodificar_variable(crom, 16, 31);
}

double bohachevsky(double x, double y) {
    return x * x + 2.0 * y * y - 0.3 * std::cos(3.0 * M_PI * x) 
           - 0.4 * std::cos(4.0 * M_PI * y) + 0.7;
}

std::mt19937 crear_generador(int seed) {
    return std::mt19937(seed);
}