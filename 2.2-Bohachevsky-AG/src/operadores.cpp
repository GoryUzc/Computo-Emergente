#include "operadores.hpp"
#include "utils.hpp"
#include "renormalizacion.hpp"
#include <algorithm>
#include <numeric>
#include <random>

Poblacion seleccionar_padres(const Poblacion& pop, std::mt19937& gen) {
    const size_t n = pop.size();
    std::vector<double> fitness_vals;
    fitness_vals.reserve(n);
    for (const auto& ind : pop) fitness_vals.push_back(ind.fitness);
    return seleccionar_padres(pop, fitness_vals, gen);
}

Poblacion seleccionar_padres(const Poblacion& pop, const std::vector<double>& fitness_vals, std::mt19937& gen) {
    const size_t n = pop.size();
    std::vector<size_t> indices(n);
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
        return fitness_vals[a] < fitness_vals[b];
    });

    std::vector<double> probs(n);
    double sum = static_cast<double>(n * (n + 1)) / 2.0;
    for (size_t i = 0; i < n; ++i) {
        probs[i] = static_cast<double>(i + 1) / sum;
    }

    std::vector<double> cumulative(n);
    cumulative[0] = probs[0];
    for (size_t i = 1; i < n; ++i) {
        cumulative[i] = cumulative[i - 1] + probs[i];
    }

    std::uniform_real_distribution<double> dist(0.0, 1.0);
    Poblacion parents;
    parents.reserve(n);
    for (size_t k = 0; k < n; ++k) {
        double r = dist(gen);
        auto it = std::lower_bound(cumulative.begin(), cumulative.end(), r);
        size_t idx = static_cast<size_t>(std::distance(cumulative.begin(), it));
        if (idx >= n) idx = n - 1;
        parents.push_back(pop[indices[idx]]);
    }
    return parents;
}

std::pair<Individuo, Individuo> cruce_uniforme(const Individuo& p1, const Individuo& p2, double pc, std::mt19937& gen) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    if (dist(gen) >= pc) {
        return {p1, p2};
    }

    Individuo h1, h2;
    std::uniform_real_distribution<double> bit_dist(0.0, 1.0);
    for (int i = 0; i < BITS_TOTAL; ++i) {
        if (bit_dist(gen) < 0.5) {
            h1.cromosoma[i] = p1.cromosoma[i];
            h2.cromosoma[i] = p2.cromosoma[i];
        } else {
            h1.cromosoma[i] = p2.cromosoma[i];
            h2.cromosoma[i] = p1.cromosoma[i];
        }
    }

    double x1 = decodificar_x(h1.cromosoma);
    double y1 = decodificar_y(h1.cromosoma);
    h1.fitness = bohachevsky(x1, y1);

    double x2 = decodificar_x(h2.cromosoma);
    double y2 = decodificar_y(h2.cromosoma);
    h2.fitness = bohachevsky(x2, y2);

    return {h1, h2};
}

void mutacion(Individuo& ind, double pm, std::mt19937& gen) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (int i = 0; i < BITS_TOTAL; ++i) {
        if (dist(gen) < pm) {
            ind.cromosoma[i] = !ind.cromosoma[i];
        }
    }
    double x = decodificar_x(ind.cromosoma);
    double y = decodificar_y(ind.cromosoma);
    ind.fitness = bohachevsky(x, y);
}

void aplicar_elitismo(Poblacion& nueva, const Individuo& mejor) {
    if (nueva.empty()) {
        return;
    }
    auto worst_it = std::max_element(nueva.begin(), nueva.end(),
        [](const Individuo& a, const Individuo& b) {
            return a.fitness < b.fitness;
        });
    *worst_it = mejor;
}

Poblacion evolucionar_generacion(const Poblacion& actual, double pc, double pm, const std::string& ag_type, std::mt19937& gen) {
    int best_idx = encontrar_mejor(actual);
    Individuo mejor = actual[best_idx];

    std::vector<double> fitness_vals;
    fitness_vals.reserve(actual.size());
    for (const auto& ind : actual) fitness_vals.push_back(ind.fitness);

    if (ag_type == "linear") {
        fitness_vals = renormalizar_lineal(fitness_vals);
    } else if (ag_type == "window") {
        fitness_vals = renormalizar_ventana(fitness_vals);
    }

    Poblacion padres = seleccionar_padres(actual, fitness_vals, gen);
    Poblacion nueva;
    nueva.reserve(actual.size());
    for (size_t i = 0; i < padres.size(); i += 2) {
        auto hijos = cruce_uniforme(padres[i], padres[i+1], pc, gen);
        mutacion(hijos.first, pm, gen);
        mutacion(hijos.second, pm, gen);
        nueva.push_back(hijos.first);
        nueva.push_back(hijos.second);
    }

    // aplicar_elitismo(nueva, mejor);
    return nueva;
}