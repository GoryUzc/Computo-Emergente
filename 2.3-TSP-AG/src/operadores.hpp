#ifndef OPERADORES_HPP
#define OPERADORES_HPP

#include <vector>
#include <random>

std::vector<int> cruce_OX(const std::vector<int>& padre1,
                          const std::vector<int>& padre2,
                          std::mt19937& gen);

std::vector<int> mutacion_swap(const std::vector<int>& ruta,
                               std::mt19937& gen);

int seleccion_ruleta(const std::vector<double>& fitness,
                     std::mt19937& gen);

#endif