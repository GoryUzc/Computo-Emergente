#pragma once
#include <map>
#include <string>
#include <vector>
#include "fuzzy_core/fuzzy_result.hpp"
#include "fuzzy_core/fuzzifier.hpp"
#include "fuzzy_core/inference_engine.hpp"
#include "fuzzy_core/aggregator.hpp"
#include "fuzzy_core/defuzzifier.hpp"
#include "fuzzy_core/linguistic_variable.hpp"
#include "fuzzy_core/fuzzy_rule.hpp"

namespace fuzzy_core {

class FuzzySystem {
public:
    FuzzySystem(LinguisticVariable output_var, int n_points = 1000);
    ~FuzzySystem() = default;

    void addInput(LinguisticVariable var);
    void addRule(FuzzyRule rule);

    FuzzyResult compute(const std::map<std::string, double>& inputs);

    std::vector<std::string> getInputNames() const;
    const LinguisticVariable& getOutput() const;
    const std::vector<FuzzyRule>& getRules() const;

private:
    std::map<std::string, LinguisticVariable> inputs_;
    LinguisticVariable output_;
    std::vector<FuzzyRule> rules_;
    Aggregator aggregator_;
};

} // namespace fuzzy_core
