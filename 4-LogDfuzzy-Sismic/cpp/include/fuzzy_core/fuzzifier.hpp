#pragma once
#include <map>
#include "fuzzy_core/linguistic_variable.hpp"
#include <map>

namespace fuzzy_core {

class Fuzzifier {
public:
    static std::map<std::string, std::vector<double>> fuzzify(
        const std::map<std::string, double>& crisp_inputs,
        const std::map<std::string, LinguisticVariable>& variables);
};

} // namespace fuzzy_core