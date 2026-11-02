#pragma once
#include "fuzzy_core/membership_function.hpp"
#include <string>

namespace fuzzy_core {

class TrapezoidalMF : public MembershipFunction {
public:
    TrapezoidalMF(std::string name, double a, double b, double c, double d);
    double evaluate(double x) const override;
    std::pair<double, double> getSupport() const override;
    std::string getName() const override { return name_; }
    double getA() const { return a_; }
    double getB() const { return b_; }
    double getC() const { return c_; }
    double getD() const { return d_; }

private:
    double a_, b_, c_, d_;
};

} // namespace fuzzy_core