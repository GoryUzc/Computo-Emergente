#include "fuzzy_core/fuzzy_system.hpp"
#include <stdexcept>

namespace fuzzy_core {

FuzzySystem::FuzzySystem(LinguisticVariable output_var, int n_points)
    : output_(std::move(output_var)), aggregator_(n_points) {}

void FuzzySystem::addInput(LinguisticVariable var) {
    std::string name = var.getName();
    if (inputs_.count(name))
        throw std::invalid_argument("Variable de entrada duplicada: " + name);
    inputs_.emplace(name, std::move(var));
}

void FuzzySystem::addRule(FuzzyRule rule) {
    rules_.push_back(std::move(rule));
}

FuzzyResult FuzzySystem::compute(const std::map<std::string, double>& inputs) {
    // Validación
    for (const auto& [name, var] : inputs_) {
        auto it = inputs.find(name);
        if (it == inputs.end())
            throw std::invalid_argument("Falta entrada: " + name);
        auto [lo, hi] = var.getUniverse();
        if (it->second < lo || it->second > hi)
            throw std::invalid_argument("Entrada fuera de universo: " + name);
    }
    
    FuzzyResult result;
    
    // Etapa 1: Fusificación
    result.fuzzified_inputs = Fuzzifier::fuzzify(inputs, inputs_);
    
    // Etapa 2: Inferencia
    result.rule_degrees = InferenceEngine::fire(rules_, inputs, inputs_);
    
    // Etapa 3: Agregación
    result.aggregated_mu = aggregator_.aggregate(result.rule_degrees, rules_, output_);
    result.discretized_output = aggregator_.getDiscretizedUniverse(output_);
    
    // Etapa 4: Desfusificación
    result.crisp_output = Defuzzifier::centroid(result.aggregated_mu, result.discretized_output);
    
    return result;
}

std::vector<std::string> FuzzySystem::getInputNames() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : inputs_) names.push_back(name);
    return names;
}

const LinguisticVariable& FuzzySystem::getOutput() const { return output_; }
const std::vector<FuzzyRule>& FuzzySystem::getRules() const { return rules_; }

} // namespace fuzzy_core