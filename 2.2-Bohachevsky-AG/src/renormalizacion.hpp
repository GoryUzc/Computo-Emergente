#ifndef RENORMALIZACION_HPP
#define RENORMALIZACION_HPP

#include <vector>

std::vector<double> renormalizar_lineal(const std::vector<double>& fitness);
std::vector<double> renormalizar_ventana(const std::vector<double>& fitness);

#endif