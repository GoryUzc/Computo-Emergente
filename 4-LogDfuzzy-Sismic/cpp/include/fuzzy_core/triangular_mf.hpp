#pragma once
#include "fuzzy_core/membership_function.hpp"
#include <string>

namespace fuzzy_core {

class TriangularMF : public MembershipFunction {
public:
    TriangularMF(std::string name, double a, double b, double c);
    double evaluate(double x) const override;
    std::pair<double, double> getSupport() const override;
    std::string getName() const override { return name_; }
    double getA() const { return a_; }
    double getB() const { return b_; }
    double getC() const { return c_; }

private:
    double a_, b_, c_;
};

} // namespace fuzzy_core