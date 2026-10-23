#ifndef OPERADORES_HPP
#define OPERADORES_HPP

#include "ag_core.hpp"
#include <vector>
#include <random>
#include <string>

Poblacion seleccionar_padres(const Poblacion& pop, std::mt19937& gen);
Poblacion seleccionar_padres(const Poblacion& pop, const std::vector<double>& fitness_vals, std::mt19937& gen);
std::pair<Individuo, Individuo> cruce_uniforme(const Individuo& p1, const Individuo& p2, double pc, std::mt19937& gen);
void mutacion(Individuo& ind, double pm, std::mt19937& gen);
void aplicar_elitismo(Poblacion& nueva, const Individuo& mejor);
Poblacion evolucionar_generacion(const Poblacion& actual, double pc, double pm, const std::string& ag_type, std::mt19937& gen);

#endif