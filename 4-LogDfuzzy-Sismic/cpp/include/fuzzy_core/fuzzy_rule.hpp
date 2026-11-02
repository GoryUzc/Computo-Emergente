#pragma once
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "fuzzy_core/linguistic_variable.hpp"

namespace fuzzy_core {

class FuzzyRule {
public:
    FuzzyRule(int id, double weight = 1.0);
    ~FuzzyRule() = default;
    
    void addAntecedent(const std::string& var_name, const std::string& term_name);
    void setConsequent(const std::string& var_name, const std::string& term_name);
    
    double evaluateDegree(
        const std::map<std::string, double>& crisp_inputs,
        const std::map<std::string, LinguisticVariable>& variables) const;
    
    std::pair<std::string, std::string> getConsequent() const;
    const std::vector<std::pair<std::string, std::string>>& getAntecedents() const;
    double getWeight() const;
    int getId() const;

private:
    int id_;
    double weight_;
    std::vector<std::pair<std::string, std::string>> antecedents_;
    std::pair<std::string, std::string> consequent_;
};

} // namespace fuzzy_core