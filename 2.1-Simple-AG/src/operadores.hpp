#pragma once

#include <vector>
#include <random>
#include "ag_core.hpp"

std::vector<Individuo> seleccionRuleta(const Poblacion& poblacion, std::mt19937& rng);

std::vector<Individuo> cruceUnPunto(const std::vector<Individuo>& padres, double Pc, std::mt19937& rng);

void mutarPoblacion(std::vector<Individuo>& poblacion, double Pm, std::mt19937& rng);