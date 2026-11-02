#pragma once
#include <Eigen/Core>
#include <vector>
#include "fuzzy_core/fuzzy_rule.hpp"
#include "fuzzy_core/linguistic_variable.hpp"

namespace fuzzy_core {

class Aggregator {
public:
    explicit Aggregator(int n_points = 1000);
    ~Aggregator() = default;

    Eigen::VectorXd aggregate(
        const std::vector<double>& rule_degrees,
        const std::vector<FuzzyRule>& rules,
        const LinguisticVariable& output_var) const;

    Eigen::VectorXd getDiscretizedUniverse(const LinguisticVariable& output_var) const;

    int getNPoints() const;

private:
    int n_points_;
};

} // namespace fuzzy_core