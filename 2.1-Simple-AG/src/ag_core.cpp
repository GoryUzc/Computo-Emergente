#include "ag_core.hpp"
#include <random>
#include <algorithm>
#include <numeric>
#include <cmath>

Poblacion inicializarPoblacion(int n, int L, bool es_real, int precision) {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> bitDist(0, 1);

    Poblacion poblacion;
    poblacion.n = n;
    poblacion.L = L;
    poblacion.es_real = es_real;
    poblacion.precision = precision;
    poblacion.individuos.resize(n);

    for (int i = 0; i < n; ++i) {
        Individuo& ind = poblacion.individuos[i];
        ind.cromosoma.resize(L);
        for (int j = 0; j < L; ++j) {
            ind.cromosoma[j] = bitDist(rng);
        }
        ind.x_fenotipo = decodificarCromosoma(ind.cromosoma, es_real, precision);
        ind.fitness = evaluarFitness(ind.x_fenotipo);
    }

    return poblacion;
}

double decodificarCromosoma(const std::vector<int>& cromosoma, bool es_real, int precision) {
    int entero = binarioAEntero(cromosoma);
    if (es_real) {
        return mapeoReal(entero, cromosoma.size(), precision);
    } else {
        return static_cast<double>(entero);
    }
}

double evaluarFitness(double x) {
    return x * x;
}
