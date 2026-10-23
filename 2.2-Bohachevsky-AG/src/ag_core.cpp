#include "ag_core.hpp"
#include "utils.hpp"
#include <numeric>
#include <cmath>

Poblacion inicializar_poblacion(std::mt19937& gen) {
    std::uniform_int_distribution<int> bit_dist(0, 1);
    Poblacion pop(TAM_POBLACION);
    for (auto& ind : pop) {
        for (int i = 0; i < BITS_TOTAL; ++i) {
            ind.cromosoma[i] = bit_dist(gen);
        }
        double x = decodificar_x(ind.cromosoma);
        double y = decodificar_y(ind.cromosoma);
        ind.fitness = bohachevsky(x, y);
    }
    return pop;
}

void evaluar_poblacion(Poblacion& pop) {
    for (auto& ind : pop) {
        double x = decodificar_x(ind.cromosoma);
        double y = decodificar_y(ind.cromosoma);
        ind.fitness = bohachevsky(x, y);
    }
}

int encontrar_mejor(const Poblacion& pop) {
    return static_cast<int>(std::distance(pop.begin(), std::min_element(pop.begin(), pop.end(),
        [](const Individuo& a, const Individuo& b){ return a.fitness < b.fitness; })));
}

int encontrar_peor(const Poblacion& pop) {
    return static_cast<int>(std::distance(pop.begin(), std::max_element(pop.begin(), pop.end(),
        [](const Individuo& a, const Individuo& b){ return a.fitness < b.fitness; })));
}

MetricasGen obtener_metricas(const Poblacion& pop, int sim, int gen,
                              const std::string& type, double pc, double pm) {
    MetricasGen m{};
    m.simulacion = sim;
    m.generacion = gen;
    m.ag_type = type;
    m.pc = pc;
    m.pm = pm;
    int best_idx = encontrar_mejor(pop);
    const Individuo& best = pop[best_idx];
    m.best_fitness = best.fitness;
    m.best_x = decodificar_x(best.cromosoma);
    m.best_y = decodificar_y(best.cromosoma);
    double sum = 0.0, sum_sq = 0.0;
    for (const auto& ind : pop) {
        sum += ind.fitness;
        sum_sq += ind.fitness * ind.fitness;
    }
    m.avg_fitness = sum / static_cast<double>(pop.size());
    double var = sum_sq / static_cast<double>(pop.size()) - m.avg_fitness * m.avg_fitness;
    m.std_fitness = std::sqrt(var);
    return m;
}