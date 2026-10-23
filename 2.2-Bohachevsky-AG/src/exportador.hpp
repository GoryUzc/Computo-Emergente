#ifndef EXPORTADOR_HPP
#define EXPORTADOR_HPP

#include "ag_core.hpp"
#include <vector>
#include <string>

void exportar_csv(const std::vector<MetricasGen>& metricas, const std::string& filename);
void acumular_metricas(std::vector<MetricasGen>& acumulador, const MetricasGen& m);

#endif