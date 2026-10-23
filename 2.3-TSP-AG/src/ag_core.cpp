#include "ag_core.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>
#include <iostream>
#include <iomanip>  

AlgoritmoGenetico::AlgoritmoGenetico(const TSP& tsp,
                                      int tam_poblacion,
                                      double prob_cruce,
                                      double prob_mutacion,
                                      int max_generaciones,
                                      unsigned int seed)
    : tsp_(tsp),
      tam_poblacion_(tam_poblacion),
      prob_cruce_(prob_cruce),
      prob_mutacion_(prob_mutacion),
      max_generaciones_(max_generaciones),
      gen_(seed),
      mejor_individuo_global_({{}, std::numeric_limits<double>::max(), 0.0}) {}

void AlgoritmoGenetico::inicializar_poblacion() {
    poblacion_.clear();
    poblacion_.reserve(tam_poblacion_);
    
    for (int i = 0; i < tam_poblacion_; ++i) {
        poblacion_.push_back(crear_individuo_aleatorio());
    }
    
    evaluar_poblacion(poblacion_);
}

Individuo AlgoritmoGenetico::crear_individuo_aleatorio() {
    int n = tsp_.get_n_ciudades();
    std::vector<int> ruta(n);
    std::iota(ruta.begin(), ruta.end(), 0);
    std::shuffle(ruta.begin(), ruta.end(), gen_);
    
    Individuo ind;
    ind.ruta = ruta;
    return ind;
}

void AlgoritmoGenetico::evaluar_poblacion(Poblacion& pop) {
    for (auto& ind : pop) {
        ind.distancia = tsp_.calcular_distancia_total(ind.ruta);
        ind.fitness = 1.0 / ind.distancia;
    }
}

Poblacion AlgoritmoGenetico::seleccionar_padres_y_cruzar() {
    Poblacion nueva_poblacion;
    nueva_poblacion.reserve(tam_poblacion_);
    
    // Calculate fitness vector for selection
    std::vector<double> fitness(poblacion_.size());
    for (size_t i = 0; i < poblacion_.size(); ++i) {
        fitness[i] = poblacion_[i].fitness;
    }
    
    // Generate offspring until we have enough
    while ((int)nueva_poblacion.size() < tam_poblacion_) {
        // Selection
        int idx1 = seleccion_ruleta(fitness, gen_);
        int idx2 = seleccion_ruleta(fitness, gen_);
        
        const Individuo& padre1 = poblacion_[idx1];
        const Individuo& padre2 = poblacion_[idx2];
        
        std::vector<int> hijo_ruta;
        
        // Crossover
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        if (dist(gen_) < prob_cruce_) {
            hijo_ruta = cruce_OX(padre1.ruta, padre2.ruta, gen_);
        } else {
            hijo_ruta = padre1.ruta;
        }
        
        // Create offspring individual
        Individuo hijo;
        hijo.ruta = hijo_ruta;
        nueva_poblacion.push_back(hijo);
    }
    
    return nueva_poblacion;
}

void AlgoritmoGenetico::aplicar_mutacion(Poblacion& pop) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    
    for (auto& ind : pop) {
        if (dist(gen_) < prob_mutacion_) {
            ind.ruta = mutacion_swap(ind.ruta, gen_);
        }
    }
}

void AlgoritmoGenetico::aplicar_elitismo(Poblacion& nueva_poblacion, 
                                         const Poblacion& poblacion_anterior) {
    std::vector<Individuo> ordenada = poblacion_anterior;
    std::sort(ordenada.begin(), ordenada.end(),
              [](const Individuo& a, const Individuo& b) {
                  return a.fitness > b.fitness;
              });
    
    // Replace the first two individuals in new population with the best two
    if (ordenada.size() >= 2 && nueva_poblacion.size() >= 2) {
        nueva_poblacion[0] = ordenada[0];
        nueva_poblacion[1] = ordenada[1];
    }
}

MetricasGeneracion AlgoritmoGenetico::calcular_metricas(const Poblacion& pop, int gen) {
    std::vector<double> distancias;
    std::vector<double> fitness_vals;
    distancias.reserve(pop.size());
    fitness_vals.reserve(pop.size());
    
    for (const auto& ind : pop) {
        distancias.push_back(ind.distancia);
        fitness_vals.push_back(ind.fitness);
    }
    
    double mejor_distancia = *std::min_element(distancias.begin(), distancias.end());
    double peor_distancia = *std::max_element(distancias.begin(), distancias.end());
    double promedio_distancia = std::accumulate(distancias.begin(), distancias.end(), 0.0) / distancias.size();
    double mejor_fitness = *std::max_element(fitness_vals.begin(), fitness_vals.end());
    double promedio_fitness = std::accumulate(fitness_vals.begin(), fitness_vals.end(), 0.0) / fitness_vals.size();
    
    // Standard deviation
    double sum_sq = 0.0;
    for (double d : distancias) {
        double diff = d - promedio_distancia;
        sum_sq += diff * diff;
    }
    double desv_est_distancia = std::sqrt(sum_sq / distancias.size());
    
    MetricasGeneracion metricas;
    metricas.generacion = gen;
    metricas.mejor_distancia = mejor_distancia;
    metricas.peor_distancia = peor_distancia;
    metricas.promedio_distancia = promedio_distancia;
    metricas.desv_est_distancia = desv_est_distancia;
    metricas.mejor_fitness = mejor_fitness;
    metricas.promedio_fitness = promedio_fitness;
    
    return metricas;
}

void AlgoritmoGenetico::ejecutar() {
    std::cout << "  Iniciando AG..." << std::endl;
    
    inicializar_poblacion();
    
    // Métricas iniciales
    MetricasGeneracion metricas_iniciales = calcular_metricas(poblacion_, 0);
    historial_.push_back(metricas_iniciales);
    
    // Actualizar mejor global inicial
    for (const auto& ind : poblacion_) {
        if (ind.distancia < mejor_individuo_global_.distancia) {
            mejor_individuo_global_ = ind;
        }
    }
    
    std::cout << "  Gen    0 | Mejor: " << std::fixed << std::setprecision(2)
              << mejor_individuo_global_.distancia 
              << " | Promedio: " << metricas_iniciales.promedio_distancia 
              << std::endl << std::flush;  // ← FORZAR IMPRESIÓN
    
    for (int gen = 1; gen <= max_generaciones_; ++gen) {
        // Paso 1: Seleccionar y cruzar
        Poblacion nueva_poblacion = seleccionar_padres_y_cruzar();
        
        // Paso 2: Aplicar elitismo (Top 2)
        aplicar_elitismo(nueva_poblacion, poblacion_);
        
        // Paso 3: Aplicar mutación
        aplicar_mutacion(nueva_poblacion);
        
        // Paso 4: Evaluar nueva población
        evaluar_poblacion(nueva_poblacion);
        
        // Paso 5: Actualizar mejor global
        for (const auto& ind : nueva_poblacion) {
            if (ind.distancia < mejor_individuo_global_.distancia) {
                mejor_individuo_global_ = ind;
            }
        }
        
        // Paso 6: Calcular métricas
        MetricasGeneracion metricas = calcular_metricas(nueva_poblacion, gen);
        historial_.push_back(metricas);
        
        // Paso 7: Reemplazar población
        poblacion_ = std::move(nueva_poblacion);
        
        // ✅ MENSAJES CADA 10 GENERACIONES (más frecuente)
        if (gen % 10 == 0 || gen == max_generaciones_) {
            std::cout << "  Gen " << std::setw(4) << gen 
                      << " | Mejor: " << std::fixed << std::setprecision(2)
                      << mejor_individuo_global_.distancia 
                      << " | Promedio: " << metricas.promedio_distancia 
                      << std::endl << std::flush;  // ← FORZAR IMPRESIÓN
        }
    }
    
    std::cout << "  AG completado. Mejor distancia: " 
              << std::fixed << std::setprecision(2)
              << mejor_individuo_global_.distancia << std::endl << std::flush;
}

Individuo AlgoritmoGenetico::get_mejor_individuo() const {
    return mejor_individuo_global_;
}

double AlgoritmoGenetico::get_mejor_distancia() const {
    return mejor_individuo_global_.distancia;
}

const std::vector<MetricasGeneracion>& AlgoritmoGenetico::get_historial() const {
    return historial_;
}

const Poblacion& AlgoritmoGenetico::get_poblacion_final() const {
    return poblacion_;
}