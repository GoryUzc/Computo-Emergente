#ifndef AG_CORE_HPP
#define AG_CORE_HPP

#include "utils.hpp"
#include <vector>
#include <string>

struct Individuo {
    std::bitset<32> cromosoma;
    double fitness;
};

struct MetricasGen {
    int simulacion;
    int generacion;
    std::string ag_type;
    double pc;
    double pm;
    double best_fitness;
    double avg_fitness;
    double std_fitness;
    double best_x;
    double best_y;
};

using Poblacion = std::vector<Individuo>;

Poblacion inicializar_poblacion(std::mt19937& gen);
void evaluar_poblacion(Poblacion& pop);
MetricasGen obtener_metricas(const Poblacion& pop, int sim, int gen, 
                              const std::string& type, double pc, double pm);
int encontrar_mejor(const Poblacion& pop);
int encontrar_peor(const Poblacion& pop);

#endif