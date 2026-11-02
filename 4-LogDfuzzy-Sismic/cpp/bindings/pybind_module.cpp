#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>
#include "fuzzy_core/fuzzy_system.hpp"
#include "fuzzy_core/triangular_mf.hpp"
#include "fuzzy_core/trapezoidal_mf.hpp"

namespace py = pybind11;
using namespace fuzzy_core;

PYBIND11_MODULE(fuzzy_core, m) {
    m.doc() = "Motor de cálculo para sistema difuso sísmico";
    
    py::class_<FuzzyResult>(m, "FuzzyResult")
        .def_readonly("crisp_output", &FuzzyResult::crisp_output)
        .def_readonly("fuzzified_inputs", &FuzzyResult::fuzzified_inputs)
        .def_readonly("rule_degrees", &FuzzyResult::rule_degrees)
        .def_readonly("aggregated_mu", &FuzzyResult::aggregated_mu)
        .def_readonly("discretized_output", &FuzzyResult::discretized_output);
    
    py::class_<MembershipFunction, std::shared_ptr<MembershipFunction>>(m, "MembershipFunction")
        .def("evaluate", &MembershipFunction::evaluate)
        .def("get_support", &MembershipFunction::getSupport)
        .def("get_name", &MembershipFunction::getName);
    
    py::class_<TriangularMF, MembershipFunction, std::shared_ptr<TriangularMF>>(m, "TriangularMF")
        .def(py::init<std::string, double, double, double>())
        .def("get_a", &TriangularMF::getA)
        .def("get_b", &TriangularMF::getB)
        .def("get_c", &TriangularMF::getC);
    
    py::class_<TrapezoidalMF, MembershipFunction, std::shared_ptr<TrapezoidalMF>>(m, "TrapezoidalMF")
        .def(py::init<std::string, double, double, double, double>())
        .def("get_a", &TrapezoidalMF::getA)
        .def("get_b", &TrapezoidalMF::getB)
        .def("get_c", &TrapezoidalMF::getC)
        .def("get_d", &TrapezoidalMF::getD);
    
    py::class_<LinguisticVariable>(m, "LinguisticVariable")
        .def(py::init<std::string, std::pair<double,double>>())
        .def("add_term", &LinguisticVariable::addTerm)
        .def("fuzzify", &LinguisticVariable::fuzzify)
        .def("get_name", &LinguisticVariable::getName)
        .def("get_universe", &LinguisticVariable::getUniverse)
        .def("size", &LinguisticVariable::size);
    
    py::class_<FuzzyRule>(m, "FuzzyRule")
        .def(py::init<int, double>(), py::arg("id"), py::arg("weight") = 1.0)
        .def("add_antecedent", &FuzzyRule::addAntecedent)
        .def("set_consequent", &FuzzyRule::setConsequent)
        .def("get_id", &FuzzyRule::getId)
        .def("get_weight", &FuzzyRule::getWeight);
    
    py::class_<FuzzySystem>(m, "FuzzySystem")
        .def(py::init<LinguisticVariable, int>(), py::arg("output_var"), py::arg("n_points") = 1000)
        .def("add_input", &FuzzySystem::addInput)
        .def("add_rule", &FuzzySystem::addRule)
        .def("compute", &FuzzySystem::compute)
        .def("get_input_names", &FuzzySystem::getInputNames)
        .def("get_output", &FuzzySystem::getOutput, py::return_value_policy::reference_internal)
        .def("get_rules", &FuzzySystem::getRules, py::return_value_policy::reference_internal);
}
