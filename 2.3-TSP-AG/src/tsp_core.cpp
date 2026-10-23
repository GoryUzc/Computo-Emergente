#include "tsp_core.hpp"
#include <cmath>
#include <algorithm>
#include <random>

double Ciudad::distancia_a(const Ciudad& otra) const {
    double dx = x - otra.x;
    double dy = y - otra.y;
    return std::sqrt(dx * dx + dy * dy);
}

TSP::TSP(const std::vector<Ciudad>& ciudades) : ciudades_(ciudades) {
    precomputar_matriz_distancias();
}

void TSP::precomputar_matriz_distancias() {
    int n = ciudades_.size();
    matriz_distancias_.resize(n, std::vector<double>(n, 0.0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                matriz_distancias_[i][j] = ciudades_[i].distancia_a(ciudades_[j]);
            }
        }
    }
}

double TSP::calcular_distancia_total(const std::vector<int>& ruta) const {
    double distancia = 0.0;
    int n = ruta.size();

    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        distancia += matriz_distancias_[ruta[i]][ruta[j]];
    }

    return distancia;
}

double TSP::calcular_fitness(const std::vector<int>& ruta) const {
    double distancia = calcular_distancia_total(ruta);
    return 1.0 / distancia;
}

int TSP::get_n_ciudades() const {
    return ciudades_.size();
}

const std::vector<Ciudad>& TSP::get_ciudades() const {
    return ciudades_;
}

double TSP::get_distancia(int i, int j) const {
    return matriz_distancias_[i][j];
}

std::vector<Ciudad> generar_ciudades_aleatorias(int n,
                                                 double ancho,
                                                 double alto,
                                                 unsigned int seed) {
    std::vector<Ciudad> ciudades;
    ciudades.reserve(n);

    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dist_x(0.0, ancho);
    std::uniform_real_distribution<double> dist_y(0.0, alto);

    for (int i = 0; i < n; ++i) {
        Ciudad c;
        c.id = i;
        c.x = dist_x(gen);
        c.y = dist_y(gen);
        ciudades.push_back(c);
    }

    return ciudades;
}