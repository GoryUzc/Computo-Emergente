#pragma once
#include <map>
#include <vector>
#include <string>
#include <Eigen/Core>

namespace fuzzy_core {

struct FuzzyResult {
    double crisp_output;
    std::map<std::string, std::vector<double>> fuzzified_inputs;
    std::vector<double> rule_degrees;
    Eigen::VectorXd aggregated_mu;
    Eigen::VectorXd discretized_output;
};

} // namespace fuzzy_core