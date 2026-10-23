#include "operadores.hpp"
#include <random>
#include <algorithm>
#include <numeric>

std::vector<Individuo> seleccionRuleta(const Poblacion& poblacion, std::mt19937& rng) {
    std::vector<Individuo> seleccionados;
    seleccionados.reserve(poblacion.n);

    double fitnessTotal = 0.0;
    for (const auto& ind : poblacion.individuos) {
        fitnessTotal += ind.fitness;
    }

    std::vector<double> probAcumulada;
    double acumulada = 0.0;
    for (const auto& ind : poblacion.individuos) {
        acumulada += ind.fitness / fitnessTotal;
        probAcumulada.push_back(acumulada);
    }

    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (int i = 0; i < poblacion.n; ++i) {
        double r = dist(rng);
        auto it = std::upper_bound(probAcumulada.begin(), probAcumulada.end(), r);
        int idx = std::distance(probAcumulada.begin(), it);
        seleccionados.push_back(poblacion.individuos[idx]);
    }

    return seleccionados;
}

std::vector<Individuo> cruceUnPunto(const std::vector<Individuo>& padres, double Pc, std::mt19937& rng) {
    std::vector<Individuo> hijos;
    hijos.reserve(padres.size());

    std::uniform_real_distribution<double> distPc(0.0, 1.0);
    std::uniform_int_distribution<int> distCorte(1, padres.empty() ? 1 : padres[0].cromosoma.size() - 1);

    int L = padres.empty() ? 0 : padres[0].cromosoma.size();

    for (size_t i = 0; i < padres.size(); i += 2) {
        const Individuo& p1 = padres[i];
        const Individuo& p2 = padres[i + 1];

        Individuo h1 = p1;
        Individuo h2 = p2;

        if (distPc(rng) < Pc && L > 1) {
            int k = distCorte(rng);
            h1.cromosoma.resize(L);
            h2.cromosoma.resize(L);

            for (int j = 0; j < k; ++j) {
                h1.cromosoma[j] = p1.cromosoma[j];
                h2.cromosoma[j] = p2.cromosoma[j];
            }
            for (int j = k; j < L; ++j) {
                h1.cromosoma[j] = p2.cromosoma[j];
                h2.cromosoma[j] = p1.cromosoma[j];
            }
        }

        hijos.push_back(h1);
        hijos.push_back(h2);
    }

    return hijos;
}

void mutarPoblacion(std::vector<Individuo>& poblacion, double Pm, std::mt19937& rng) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (auto& ind : poblacion) {
        for (int& bit : ind.cromosoma) {
            if (dist(rng) < Pm) {
                bit = 1 - bit;
            }
        }
    }
}