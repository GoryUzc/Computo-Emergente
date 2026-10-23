#include "renormalizacion.hpp"
#include <algorithm>
#include <limits>

std::vector<double> renormalizar_lineal(const std::vector<double>& fitness) {
    if (fitness.empty()) return {};

    double f_min = *std::min_element(fitness.begin(), fitness.end());
    double f_max = *std::max_element(fitness.begin(), fitness.end());

    if (std::abs(f_max - f_min) < std::numeric_limits<double>::epsilon()) {
        return std::vector<double>(fitness.size(), 1.0);
    }

    double a = -1.0;
    double b = f_max + f_min;

    std::vector<double> result;
    result.reserve(fitness.size());
    for (double f : fitness) {
        double f_prime = a * f + b;
        if (f_prime <= 0) f_prime = 1e-10;
        result.push_back(f_prime);
    }
    return result;
}

std::vector<double> renormalizar_ventana(const std::vector<double>& fitness) {
    if (fitness.empty()) return {};

    double f_max = *std::max_element(fitness.begin(), fitness.end());
    const double c = 0.01;

    std::vector<double> result;
    result.reserve(fitness.size());
    for (double f : fitness) {
        result.push_back(f_max - f + c);
    }
    return result;
}