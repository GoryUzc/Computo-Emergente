#include "fuzzy_core/fuzzy_rule.hpp"
#include <stdexcept>
#include <algorithm>
#include <limits>

namespace fuzzy_core {

FuzzyRule::FuzzyRule(int id, double weight) : id_(id), weight_(weight) {
    if (weight < 0.0 || weight > 1.0)
        throw std::invalid_argument("Peso de regla debe estar en [0, 1]");
}

void FuzzyRule::addAntecedent(const std::string& var_name, const std::string& term_name) {
    antecedents_.emplace_back(var_name, term_name);
}

void FuzzyRule::setConsequent(const std::string& var_name, const std::string& term_name) {
    consequent_ = {var_name, term_name};
}

double FuzzyRule::evaluateDegree(
    const std::map<std::string, double>& crisp_inputs,
    const std::map<std::string, LinguisticVariable>& variables) const {
    
    if (antecedents_.empty()) return 0.0;
    
    double min_degree = std::numeric_limits<double>::max();
    for (const auto& [var_name, term_name] : antecedents_) {
        auto it_var = variables.find(var_name);
        if (it_var == variables.end())
            throw std::invalid_argument("Variable no encontrada: " + var_name);
        
        auto it_input = crisp_inputs.find(var_name);
        if (it_input == crisp_inputs.end())
            throw std::invalid_argument("Entrada no encontrada: " + var_name);
        
        const auto& mf = it_var->second.getTermByName(term_name);
        double degree = mf.evaluate(it_input->second);
        min_degree = std::min(min_degree, degree);
    }
    
    return min_degree * weight_;
}

std::pair<std::string, std::string> FuzzyRule::getConsequent() const { return consequent_; }
const std::vector<std::pair<std::string, std::string>>& FuzzyRule::getAntecedents() const { return antecedents_; }
double FuzzyRule::getWeight() const { return weight_; }
int FuzzyRule::getId() const { return id_; }

} // namespace fuzzy_core