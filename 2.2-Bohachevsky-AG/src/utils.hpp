#ifndef UTILS_HPP
#define UTILS_HPP

#include <bitset>
#include <random>
#include <cmath>

constexpr int TAM_POBLACION = 100;
constexpr int MAX_GENERACIONES = 100;
constexpr int NUM_SIMULACIONES = 30;
constexpr int BITS_X = 16;
constexpr int BITS_Y = 16;
constexpr int BITS_TOTAL = 32;
constexpr double PC_DEFAULT = 0.7;
constexpr double PM_DEFAULT = 0.005;
constexpr int SEED_DEFAULT = 42;
constexpr double C_VENTANA = 0.01;

double decodificar_variable(const std::bitset<32>& crom, int inicio, int fin);
double decodificar_x(const std::bitset<32>& crom);
double decodificar_y(const std::bitset<32>& crom);
double bohachevsky(double x, double y);
std::mt19937 crear_generador(int seed);

#endif