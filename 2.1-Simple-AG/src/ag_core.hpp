#pragma once

#include <vector>
#include <string>
#include "utils.hpp"

struct Individuo {
    std::vector<int> cromosoma;
    double fitness;
    double x_fenotipo;
};

struct Poblacion {
    std::vector<Individuo> individuos;
    int n;
    int L;
    bool es_real;
    int precision;
};

struct MetricasGeneracion {
    int generacion;
    double mejor_fitness;
    double fitness_promedio;
    double mejor_x;
};

Poblacion inicializarPoblacion(int n, int L, bool es_real, int precision);

double decodificarCromosoma(const std::vector<int>& cromosoma, bool es_real, int precision);

double evaluarFitness(double x);
