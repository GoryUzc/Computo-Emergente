#ifndef AG_CORE_HPP
#define AG_CORE_HPP

#include "tsp_core.hpp"
#include "operadores.hpp"
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>

struct Individuo {
    std::vector<int> ruta;
    double distancia;
    double fitness;
};

using Poblacion = std::vector<Individuo>;

struct MetricasGeneracion {
    int generacion;
    double mejor_distancia;
    double peor_distancia;
    double promedio_distancia;
    double desv_est_distancia;
    double mejor_fitness;
    double promedio_fitness;
};

class AlgoritmoGenetico {
public:
    AlgoritmoGenetico(const TSP& tsp,
                        int tam_poblacion = 100,
                        double prob_cruce = 0.9,
                        double prob_mutacion = 0.05,
                        int max_generaciones = 200,
                        unsigned int seed = 42);

    void ejecutar();

    Individuo get_mejor_individuo() const;
    double get_mejor_distancia() const;
    const std::vector<MetricasGeneracion>& get_historial() const;
    const Poblacion& get_poblacion_final() const;

private:
    void inicializar_poblacion();
    void evaluar_poblacion(Poblacion& pop);
    Poblacion seleccionar_padres_y_cruzar();
    void aplicar_mutacion(Poblacion& pop);
    void aplicar_elitismo(Poblacion& nueva_poblacion, 
                           const Poblacion& poblacion_anterior);
    MetricasGeneracion calcular_metricas(const Poblacion& pop, int gen);

    // Helper to create random individual
    Individuo crear_individuo_aleatorio();

    // References to problem TSP
    const TSP& tsp_;

    // Configuration parameters
    int tam_poblacion_;
    double prob_cruce_;
    double prob_mutacion_;
    int max_generaciones_;

    // State
    Poblacion poblacion_;
    std::vector<MetricasGeneracion> historial_;
    Individuo mejor_individuo_global_;

    // Random number generator
    std::mt19937 gen_;
};

#endif