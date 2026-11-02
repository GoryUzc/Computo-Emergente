#include "fuzzy_core/defuzzifier.hpp"
#include <cmath>

namespace fuzzy_core {

double Defuzzifier::centroid(const Eigen::VectorXd& mu, const Eigen::VectorXd& universe) {
    double numerator   = (universe.array() * mu.array()).sum();
    double denominator = mu.array().sum();
    
    if (denominator < 1e-10)
        return (universe(0) + universe(universe.size() - 1)) / 2.0;
    
    return numerator / denominator;
}

double Defuzzifier::mom(const Eigen::VectorXd& mu, const Eigen::VectorXd& universe) {
    double max_mu = mu.maxCoeff();
    double sum = 0.0;
    int count = 0;
    for (int i = 0; i < mu.size(); ++i) {
        if (std::abs(mu(i) - max_mu) < 1e-10) {
            sum += universe(i);
            count++;
        }
    }
    return (count > 0) ? (sum / count) : ((universe(0) + universe(universe.size() - 1)) / 2.0);
}

} // namespace fuzzy_core