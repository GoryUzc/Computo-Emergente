#ifndef TSP_CORE_HPP
#define TSP_CORE_HPP

#include <vector>
#include <string>
#include <random>

struct Ciudad {
    int id;
    double x, y;

    double distancia_a(const Ciudad& otra) const;
};

class TSP {
public:
    TSP(const std::vector<Ciudad>& ciudades);

    double calcular_distancia_total(const std::vector<int>& ruta) const;
    double calcular_fitness(const std::vector<int>& ruta) const;
    int get_n_ciudades() const;
    const std::vector<Ciudad>& get_ciudades() const;
    double get_distancia(int i, int j) const;

private:
    std::vector<Ciudad> ciudades_;
    std::vector<std::vector<double>> matriz_distancias_;

    void precomputar_matriz_distancias();
};

std::vector<Ciudad> generar_ciudades_aleatorias(int n,
                                                double ancho = 100.0,
                                                double alto = 100.0,
                                                unsigned int seed = 42);

#endif