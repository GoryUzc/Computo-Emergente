#pragma once
#include <Eigen/Core>

namespace fuzzy_core {

class Defuzzifier {
public:
    static double centroid(const Eigen::VectorXd& mu, const Eigen::VectorXd& universe);
    static double mom(const Eigen::VectorXd& mu, const Eigen::VectorXd& universe);
};

} // namespace fuzzy_core