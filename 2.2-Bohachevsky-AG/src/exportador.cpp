#include "exportador.hpp"
#include <fstream>
#include <sstream>

void acumular_metricas(std::vector<MetricasGen>& acumulador, const MetricasGen& m) {
    acumulador.push_back(m);
}

void exportar_csv(const std::vector<MetricasGen>& metricas, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return;

    file << "simulation,generation,ag_type,pc,pm,best_fitness,avg_fitness,std_fitness,best_x,best_y\n";
    for (const auto& m : metricas) {
        file << m.simulacion << ","
             << m.generacion << ","
             << m.ag_type << ","
             << m.pc << ","
             << m.pm << ","
             << m.best_fitness << ","
             << m.avg_fitness << ","
             << m.std_fitness << ","
             << m.best_x << ","
             << m.best_y << "\n";
    }
}