#include "fuzzy_core/membership_function.hpp"

namespace fuzzy_core {

MembershipFunction::MembershipFunction(std::string name) : name_(std::move(name)) {}

std::string MembershipFunction::getName() const { return name_; }

} // namespace fuzzy_core