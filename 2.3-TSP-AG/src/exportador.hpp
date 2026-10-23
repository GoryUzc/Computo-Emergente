#ifndef EXPORTADOR_HPP
#define EXPORTADOR_HPP

#include "tsp_core.hpp"
#include "ag_core.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

class Exportador {
public:
    // Exportar coordenadas de las ciudades
    static bool exportar_ciudades(const std::string& archivo,
                                   const std::vector<Ciudad>& ciudades);

    // Exportar la mejor ruta encontrada
    static bool exportar_mejor_ruta(const std::string& archivo,
                                     const std::vector<Ciudad>& ciudades,
                                     const Individuo& mejor);

    // Exportar historial de metricas por generacion
    static bool exportar_historial(const std::string& archivo,
                                   const std::vector<MetricasGeneracion>& historial);

    // Exportar resultados de multiples simulaciones
    static bool exportar_simulaciones(const std::string& archivo,
                                        const std::vector<std::vector<MetricasGeneracion>>& todas_simulaciones);
};
#endif