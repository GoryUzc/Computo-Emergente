#include "fuzzy_core/fuzzifier.hpp"

namespace fuzzy_core {

std::map<std::string, std::vector<double>> Fuzzifier::fuzzify(
    const std::map<std::string, double>& crisp_inputs,
    const std::map<std::string, LinguisticVariable>& variables) {
    
    std::map<std::string, std::vector<double>> result;
    for (const auto& [var_name, var] : variables) {
        auto it = crisp_inputs.find(var_name);
        if (it != crisp_inputs.end())
            result[var_name] = var.fuzzify(it->second);
    }
    return result;
}

} // namespace fuzzy_core