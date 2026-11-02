#include "fuzzy_core/inference_engine.hpp"

namespace fuzzy_core {

std::vector<double> InferenceEngine::fire(
    const std::vector<FuzzyRule>& rules,
    const std::map<std::string, double>& crisp_inputs,
    const std::map<std::string, LinguisticVariable>& variables) {
    
    std::vector<double> degrees;
    degrees.reserve(rules.size());
    for (const auto& rule : rules)
        degrees.push_back(rule.evaluateDegree(crisp_inputs, variables));
    return degrees;
}

} // namespace fuzzy_core