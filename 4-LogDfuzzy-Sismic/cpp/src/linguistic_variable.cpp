#include "fuzzy_core/linguistic_variable.hpp"
#include <stdexcept>
#include <algorithm>

namespace fuzzy_core {

LinguisticVariable::LinguisticVariable(std::string name, std::pair<double, double> universe)
    : name_(std::move(name)), universe_(universe) {
    if (universe.first >= universe.second)
        throw std::invalid_argument("LinguisticVariable: universo inválido");
}

void LinguisticVariable::addTerm(std::shared_ptr<MembershipFunction> mf) {
    if (!mf) throw std::invalid_argument("MF no puede ser nula");
    terms_.push_back(std::move(mf));
}

std::vector<double> LinguisticVariable::fuzzify(double x) const {
    std::vector<double> degrees;
    degrees.reserve(terms_.size());
    for (const auto& term : terms_)
        degrees.push_back(term->evaluate(x));
    return degrees;
}

const MembershipFunction& LinguisticVariable::getTerm(size_t index) const {
    if (index >= terms_.size()) throw std::out_of_range("Índice de término fuera de rango");
    return *terms_[index];
}

const MembershipFunction& LinguisticVariable::getTermByName(const std::string& name) const {
    for (const auto& term : terms_)
        if (term->getName() == name) return *term;
    throw std::invalid_argument("Término no encontrado: " + name);
}

size_t LinguisticVariable::getTermIndex(const std::string& name) const {
    for (size_t i = 0; i < terms_.size(); ++i)
        if (terms_[i]->getName() == name) return i;
    throw std::invalid_argument("Término no encontrado: " + name);
}

size_t LinguisticVariable::size() const { return terms_.size(); }
std::string LinguisticVariable::getName() const { return name_; }
std::pair<double, double> LinguisticVariable::getUniverse() const { return universe_; }

} // namespace fuzzy_core