#include "fuzzy_core/trapezoidal_mf.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace fuzzy_core {

TrapezoidalMF::TrapezoidalMF(std::string name, double a, double b, double c, double d)
    : MembershipFunction(std::move(name)), a_(a), b_(b), c_(c), d_(d) {
    if (!std::isfinite(a) || !std::isfinite(b) || !std::isfinite(c) ||
        !std::isfinite(d) || a > b || b > c || c > d || a == d) {
        throw std::invalid_argument(
            "TrapezoidalMF: requiere valores finitos con a <= b <= c <= d y a < d");
    }
}

double TrapezoidalMF::evaluate(double x) const {
    if (x < a_ || x > d_) return 0.0;
    if (x >= b_ && x <= c_) return 1.0;
    if (x < b_) return (x - a_) / (b_ - a_);
    if (x > c_) return (d_ - x) / (d_ - c_);
    return 0.0;
}

std::pair<double,double> TrapezoidalMF::getSupport() const { return {a_, d_}; }

} // namespace fuzzy_core