#include "fuzzy_core/triangular_mf.hpp"
#include <stdexcept>
#include <algorithm>

namespace fuzzy_core {

TriangularMF::TriangularMF(std::string name, double a, double b, double c)
    : MembershipFunction(std::move(name)), a_(a), b_(b), c_(c) {
    if (a >= b || b >= c)
        throw std::invalid_argument("TriangularMF: requiere a < b < c");
}

double TriangularMF::evaluate(double x) const {
    if (x <= a_ || x >= c_) return 0.0;
    if (x == b_) return 1.0;
    double left  = (x - a_) / (b_ - a_);
    double right = (c_ - x) / (c_ - b_);
    return std::min(left, right);
}

std::pair<double,double> TriangularMF::getSupport() const { return {a_, c_}; }

} // namespace fuzzy_core