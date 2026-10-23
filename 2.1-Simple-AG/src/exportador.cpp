#include <fstream>
#include <iomanip>
#include "exportador.hpp"

void exportarMetricas(const std::string& ruta, const std::vector<MetricasGeneracion>& metricas) {
    std::ofstream file(ruta);
    if (!file.is_open()) {
        return; // No se fatal en caso de error (según especificación)
    }

    file << "generacion,mejor_fitness,promedio_fitness,mejor_x\n";
    for (const auto& m : metricas) {
        file << m.generacion << ","
             << std::fixed << std::setprecision(6) << m.mejor_fitness << ","
             << std::fixed << std::setprecision(6) << m.fitness_promedio << ","
             << std::fixed << std::setprecision(6) << m.mejor_x << "\n";
    }
}