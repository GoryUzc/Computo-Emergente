#include "fuzzy_core/aggregator.hpp"
#include <algorithm>
#include <stdexcept>

namespace fuzzy_core {

Aggregator::Aggregator(int n_points) : n_points_(n_points) {
    if (n_points <= 0)
        throw std::invalid_argument("n_points debe ser > 0");
}

Eigen::VectorXd Aggregator::aggregate(
    const std::vector<double>& rule_degrees,
    const std::vector<FuzzyRule>& rules,
    const LinguisticVariable& output_var) const {
    
    Eigen::VectorXd universe = getDiscretizedUniverse(output_var);
    Eigen::VectorXd mu_agg = Eigen::VectorXd::Zero(n_points_);
    
    for (size_t i = 0; i < rules.size(); ++i) {
        double alpha = rule_degrees[i];
        if (alpha <= 0.0) continue;
        
        const auto& [var_name, term_name] = rules[i].getConsequent();
        const auto& mf = output_var.getTermByName(term_name);
        
        for (int k = 0; k < n_points_; ++k) {
            double mu_consequent = mf.evaluate(universe(k));
            double mu_clipped = std::min(alpha, mu_consequent);
            mu_agg(k) = std::max(mu_agg(k), mu_clipped);
        }
    }
    return mu_agg;
}

Eigen::VectorXd Aggregator::getDiscretizedUniverse(const LinguisticVariable& output_var) const {
    auto [min_val, max_val] = output_var.getUniverse();
    Eigen::VectorXd universe(n_points_);
    double step = (max_val - min_val) / (n_points_ - 1);
    for (int i = 0; i < n_points_; ++i)
        universe(i) = min_val + i * step;
    return universe;
}

int Aggregator::getNPoints() const { return n_points_; }

} // namespace fuzzy_core