#pragma once
#include "fuzzy_core/fuzzy_rule.hpp"
#include <map>
#include <vector>

namespace fuzzy_core {

class InferenceEngine {
public:
    static std::vector<double> fire(
        const std::vector<FuzzyRule>& rules,
        const std::map<std::string, double>& crisp_inputs,
        const std::map<std::string, LinguisticVariable>& variables);
};

} // namespace fuzzy_core