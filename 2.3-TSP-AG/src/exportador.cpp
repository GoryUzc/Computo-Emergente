#include "exportador.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

bool Exportador::exportar_ciudades(const std::string& archivo,
                                    const std::vector<Ciudad>& ciudades) {
    std::ofstream file(archivo);
    if (!file.is_open()) {
        return false;
    }
    
    file << "id,x,y\n";
    for (const auto& ciudad : ciudades) {
        file << ciudad.id << "," 
             << std::fixed << std::setprecision(2) << ciudad.x << ","
             << std::fixed << std::setprecision(2) << ciudad.y << "\n";
    }
    file.close();
    return true;
}

bool Exportador::exportar_mejor_ruta(const std::string& archivo,
                                     const std::vector<Ciudad>& ciudades,
                                     const Individuo& mejor) {
    std::ofstream file(archivo);
    if (!file.is_open()) {
        return false;
    }
    
    file << "orden_visita,id_ciudad,x,y\n";
    
    // Add the first city at the end to close the route
    std::vector<int> ruta_completa = mejor.ruta;
    ruta_completa.push_back(mejor.ruta[0]);
    
    for (size_t i = 0; i < ruta_completa.size(); ++i) {
        int ciudad_id = ruta_completa[i];
        const Ciudad& c = ciudades[ciudad_id];
        file << i << "," << c.id << ","
             << std::fixed << std::setprecision(2) << c.x << ","
             << std::fixed << std::setprecision(2) << c.y << "\n";
    }
    file.close();
    return true;
}

bool Exportador::exportar_historial(const std::string& archivo,
                                     const std::vector<MetricasGeneracion>& historial) {
    std::ofstream file(archivo);
    if (!file.is_open()) {
        return false;
    }
    
    file << "generacion,mejor_distancia,peor_distancia,promedio_distancia,desv_est_distancia,mejor_fitness,promedio_fitness\n";
    
    for (const auto& m : historial) {
        file << m.generacion << ","
             << std::fixed << std::setprecision(2) << m.mejor_distancia << ","
             << std::fixed << std::setprecision(2) << m.peor_distancia << ","
             << std::fixed << std::setprecision(2) << m.promedio_distancia << ","
             << std::fixed << std::setprecision(2) << m.desv_est_distancia << ","
             << std::fixed << std::setprecision(6) << m.mejor_fitness << ","
             << std::fixed << std::setprecision(6) << m.promedio_fitness << "\n";
    }
    file.close();
    return true;
}

bool Exportador::exportar_simulaciones(const std::string& archivo,
                                        const std::vector<std::vector<MetricasGeneracion>>& todas_simulaciones) {
    std::ofstream file(archivo);
    if (!file.is_open()) {
        return false;
    }
    
    file << "simulacion,generacion,mejor_distancia,promedio_distancia\n";
    
    for (size_t sim = 0; sim < todas_simulaciones.size(); ++sim) {
        for (const auto& m : todas_simulaciones[sim]) {
            file << sim << ","
                 << m.generacion << ","
                 << std::fixed << std::setprecision(2) << m.mejor_distancia << ","
                 << std::fixed << std::setprecision(2) << m.promedio_distancia << "\n";
        }
    }
    file.close();
    return true;
}