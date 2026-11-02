#pragma once
#include <string>
#include <utility>
#include <memory>

namespace fuzzy_core {

class MembershipFunction {
public:
    explicit MembershipFunction(std::string name);
    virtual ~MembershipFunction() = default;
    virtual double evaluate(double x) const = 0;
    virtual std::pair<double, double> getSupport() const = 0;
    virtual std::string getName() const = 0;

protected:
    std::string name_;
};

} // namespace fuzzy_core
