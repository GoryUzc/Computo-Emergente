#pragma once
#include "fuzzy_core/membership_function.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace fuzzy_core {

class LinguisticVariable {
public:
    LinguisticVariable(std::string name, std::pair<double, double> universe);
    ~LinguisticVariable() = default;
    
    void addTerm(std::shared_ptr<MembershipFunction> mf);
    std::vector<double> fuzzify(double x) const;
    const MembershipFunction& getTerm(size_t index) const;
    const MembershipFunction& getTermByName(const std::string& name) const;
    size_t getTermIndex(const std::string& name) const;
    size_t size() const;
    std::string getName() const;
    std::pair<double, double> getUniverse() const;

private:
    std::string name_;
    std::pair<double, double> universe_;
    std::vector<std::shared_ptr<MembershipFunction>> terms_;
};

} // namespace fuzzy_core