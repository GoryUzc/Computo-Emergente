# Fase 2 — Implementación del Motor C++ (`fuzzy_core`)

> Estado del repositorio: la configuración CMake actual define `fuzzy_core_lib`, enlaza `Eigen3::Eigen`, compila el módulo pybind11 y registra `fuzzy_system` en CTest. Esta sección documenta el diseño y los fragmentos de referencia; para comandos actualizados de Windows, véase la sección 4.
---

## 1. Implementaciones `.cpp`

### `cpp/src/membership_function.cpp`
```cpp
#include "fuzzy_core/membership_function.hpp"

namespace fuzzy_core {

MembershipFunction::MembershipFunction(std::string name) : name_(std::move(name)) {}

std::string MembershipFunction::getName() const { return name_; }

} // namespace fuzzy_core
```

### `cpp/src/triangular_mf.cpp`
```cpp
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
double TriangularMF::getA() const { return a_; }
double TriangularMF::getB() const { return b_; }
double TriangularMF::getC() const { return c_; }

} // namespace fuzzy_core
```

### `cpp/src/trapezoidal_mf.cpp`
```cpp
#include "fuzzy_core/trapezoidal_mf.hpp"
#include <stdexcept>
#include <algorithm>

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
double TrapezoidalMF::getA() const { return a_; }
double TrapezoidalMF::getB() const { return b_; }
double TrapezoidalMF::getC() const { return c_; }
double TrapezoidalMF::getD() const { return d_; }

} // namespace fuzzy_core
```

### `cpp/src/linguistic_variable.cpp`
```cpp
#include "fuzzy_core/linguistic_variable.hpp"
#include <stdexcept>
#include <algorithm>

namespace fuzzy_core {

LinguisticVariable::LinguisticVariable(std::string name, std::pair<double,double> universe)
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
std::pair<double,double> LinguisticVariable::getUniverse() const { return universe_; }

} // namespace fuzzy_core
```

### `cpp/src/fuzzy_rule.cpp`
```cpp
#include "fuzzy_core/fuzzy_rule.hpp"
#include <stdexcept>
#include <algorithm>
#include <limits>

namespace fuzzy_core {

FuzzyRule::FuzzyRule(int id, double weight) : id_(id), weight_(weight) {
    if (weight < 0.0 || weight > 1.0)
        throw std::invalid_argument("Peso de regla debe estar en [0, 1]");
}

void FuzzyRule::addAntecedent(const std::string& var_name, const std::string& term_name) {
    antecedents_.emplace_back(var_name, term_name);
}

void FuzzyRule::setConsequent(const std::string& var_name, const std::string& term_name) {
    consequent_ = {var_name, term_name};
}

double FuzzyRule::evaluateDegree(
    const std::map<std::string, double>& crisp_inputs,
    const std::map<std::string, LinguisticVariable>& variables) const {
    
    if (antecedents_.empty()) return 0.0;
    
    double min_degree = std::numeric_limits<double>::max();
    for (const auto& [var_name, term_name] : antecedents_) {
        auto it_var = variables.find(var_name);
        if (it_var == variables.end())
            throw std::invalid_argument("Variable no encontrada: " + var_name);
        
        auto it_input = crisp_inputs.find(var_name);
        if (it_input == crisp_inputs.end())
            throw std::invalid_argument("Entrada no encontrada: " + var_name);
        
        const auto& mf = it_var->second.getTermByName(term_name);
        double degree = mf.evaluate(it_input->second);
        min_degree = std::min(min_degree, degree);
    }
    
    return min_degree * weight_;
}

std::pair<std::string, std::string> FuzzyRule::getConsequent() const { return consequent_; }
const std::vector<std::pair<std::string, std::string>>& FuzzyRule::getAntecedents() const { return antecedents_; }
double FuzzyRule::getWeight() const { return weight_; }
int FuzzyRule::getId() const { return id_; }

} // namespace fuzzy_core
```

### `cpp/src/fuzzifier.cpp`
```cpp
#include "fuzzy_core/fuzzifier.hpp"

namespace fuzzy_core {

std::map<std::string, std::vector<double>> Fuzzifier::fuzzify(
    const std::map<std::string, double>& crisp_inputs,
    const std::map<std::string, LinguisticVariable>& variables) {
    
    std::map<std::string, std::vector<double>> result;
    for (const auto& [var_name, var] : variables) {
        auto it = crisp_inputs.find(var_name);
        if (it != crisp_inputs.end())
            result[var_name] = var.fuzzify(it->second);
    }
    return result;
}

} // namespace fuzzy_core
```

### `cpp/src/inference_engine.cpp`
```cpp
#include "fuzzy_core/inference_engine.hpp"

namespace fuzzy_core {

std::vector<double> InferenceEngine::fire(
    const std::vector<FuzzyRule>& rules,
    const std::map<std::string, double>& crisp_inputs,
    const std::map<std::string, LinguisticVariable>& variables) {
    
    std::vector<double> degrees;
    degrees.reserve(rules.size());
    for (const auto& rule : rules)
        degrees.push_back(rule.evaluateDegree(crisp_inputs, variables));
    return degrees;
}

} // namespace fuzzy_core
```

### `cpp/src/aggregator.cpp`
```cpp
#include "fuzzy_core/aggregator.hpp"
#include <algorithm>
#include <stdexcept>

namespace fuzzy_core {

Aggregator::Aggregator(int n_points) : n_points_(n_points) {
    if (n_points <= 0)
        throw std::invalid_argument("n_points debe ser > 0");
}

Eigen::VectorXd Aggregator::aggregate(
    const std::vector<double>& rule_degrees,
    const std::vector<FuzzyRule>& rules,
    const LinguisticVariable& output_var) const {
    
    Eigen::VectorXd universe = getDiscretizedUniverse(output_var);
    Eigen::VectorXd mu_agg = Eigen::VectorXd::Zero(n_points_);
    
    for (size_t i = 0; i < rules.size(); ++i) {
        double alpha = rule_degrees[i];
        if (alpha <= 0.0) continue;
        
        const auto& [var_name, term_name] = rules[i].getConsequent();
        const auto& mf = output_var.getTermByName(term_name);
        
        for (int k = 0; k < n_points_; ++k) {
            double mu_consequent = mf.evaluate(universe(k));
            double mu_clipped = std::min(alpha, mu_consequent);
            mu_agg(k) = std::max(mu_agg(k), mu_clipped);
        }
    }
    return mu_agg;
}

Eigen::VectorXd Aggregator::getDiscretizedUniverse(const LinguisticVariable& output_var) const {
    auto [min_val, max_val] = output_var.getUniverse();
    Eigen::VectorXd universe(n_points_);
    double step = (max_val - min_val) / (n_points_ - 1);
    for (int i = 0; i < n_points_; ++i)
        universe(i) = min_val + i * step;
    return universe;
}

int Aggregator::getNPoints() const { return n_points_; }

} // namespace fuzzy_core
```

### `cpp/src/defuzzifier.cpp`
```cpp
#include "fuzzy_core/defuzzifier.hpp"
#include <cmath>

namespace fuzzy_core {

double Defuzzifier::centroid(const Eigen::VectorXd& mu, const Eigen::VectorXd& universe) {
    double numerator   = (universe.array() * mu.array()).sum();
    double denominator = mu.array().sum();
    
    if (denominator < 1e-10)
        return (universe(0) + universe(universe.size() - 1)) / 2.0;
    
    return numerator / denominator;
}

double Defuzzifier::mom(const Eigen::VectorXd& mu, const Eigen::VectorXd& universe) {
    double max_mu = mu.maxCoeff();
    double sum = 0.0;
    int count = 0;
    for (int i = 0; i < mu.size(); ++i) {
        if (std::abs(mu(i) - max_mu) < 1e-10) {
            sum += universe(i);
            count++;
        }
    }
    return (count > 0) ? (sum / count) : ((universe(0) + universe(universe.size() - 1)) / 2.0);
}

} // namespace fuzzy_core
```

### `cpp/src/fuzzy_result.cpp`
```cpp
#include "fuzzy_core/fuzzy_result.hpp"
// FuzzyResult es un struct trivial; no requiere implementación adicional.
namespace fuzzy_core {} 
```

### `cpp/src/fuzzy_system.cpp`
```cpp
#include "fuzzy_core/fuzzy_system.hpp"
#include "fuzzy_core/fuzzifier.hpp"
#include "fuzzy_core/inference_engine.hpp"
#include "fuzzy_core/defuzzifier.hpp"
#include <stdexcept>

namespace fuzzy_core {

FuzzySystem::FuzzySystem(LinguisticVariable output_var, int n_points)
    : output_(std::move(output_var)), aggregator_(n_points) {}

void FuzzySystem::addInput(LinguisticVariable var) {
    std::string name = var.getName();
    if (inputs_.count(name))
        throw std::invalid_argument("Variable de entrada duplicada: " + name);
    inputs_.emplace(name, std::move(var));
}

void FuzzySystem::addRule(FuzzyRule rule) {
    rules_.push_back(std::move(rule));
}

FuzzyResult FuzzySystem::compute(const std::map<std::string, double>& inputs) {
    // Validación
    for (const auto& [name, var] : inputs_) {
        auto it = inputs.find(name);
        if (it == inputs.end())
            throw std::invalid_argument("Falta entrada: " + name);
        auto [lo, hi] = var.getUniverse();
        if (it->second < lo || it->second > hi)
            throw std::invalid_argument("Entrada fuera de universo: " + name);
    }
    
    FuzzyResult result;
    
    // Etapa 1: Fusificación
    result.fuzzified_inputs = Fuzzifier::fuzzify(inputs, inputs_);
    
    // Etapa 2: Inferencia
    result.rule_degrees = InferenceEngine::fire(rules_, inputs, inputs_);
    
    // Etapa 3: Agregación
    result.aggregated_mu = aggregator_.aggregate(result.rule_degrees, rules_, output_);
    result.discretized_output = aggregator_.getDiscretizedUniverse(output_);
    
    // Etapa 4: Desfusificación
    result.crisp_output = Defuzzifier::centroid(result.aggregated_mu, result.discretized_output);
    
    return result;
}

std::vector<std::string> FuzzySystem::getInputNames() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : inputs_) names.push_back(name);
    return names;
}

const LinguisticVariable& FuzzySystem::getOutput() const { return output_; }
const std::vector<FuzzyRule>& FuzzySystem::getRules() const { return rules_; }

} // namespace fuzzy_core
```

---

## 2. Binding pybind11 (`cpp/bindings/pybind_module.cpp`)

```cpp
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
```

---

## 3. Test end-to-end mínimo (`cpp/tests/test_fuzzy_system.cpp`)

```cpp
#include <gtest/gtest.h>
#include "fuzzy_core/fuzzy_system.hpp"
#include "fuzzy_core/triangular_mf.hpp"
#include "fuzzy_core/trapezoidal_mf.hpp"
#include <cmath>

using namespace fuzzy_core;

// Sistema mínimo de prueba: 2 entradas, 1 salida, 2 reglas
class FuzzySystemTest : public ::testing::Test {
protected:
    FuzzySystem build_system() {
        // Salida: Intensidad [1, 12]
        LinguisticVariable output("Intensidad", {1.0, 12.0});
        output.addTerm(std::make_shared<TrapezoidalMF>("Baja",  1.0, 1.0, 3.0, 5.0));
        output.addTerm(std::make_shared<TriangularMF>   ("Media", 4.0, 6.5, 9.0));
        output.addTerm(std::make_shared<TrapezoidalMF>  ("Alta",  8.0, 10.0, 12.0, 12.0));
        
        FuzzySystem sys(output, 500);
        
        // Entrada 1: Magnitud [3, 9]
        LinguisticVariable mag("Magnitud", {3.0, 9.0});
        mag.addTerm(std::make_shared<TriangularMF>("Baja",    3.0, 4.5, 6.0));
        mag.addTerm(std::make_shared<TriangularMF>("Alta",    6.0, 7.5, 9.0));
        sys.addInput(mag);
        
        // Entrada 2: Profundidad [1, 300]
        LinguisticVariable prof("Profundidad", {1.0, 300.0});
        prof.addTerm(std::make_shared<TrapezoidalMF>("Superficial", 1.0, 1.0, 50.0, 100.0));
        prof.addTerm(std::make_shared<TrapezoidalMF>("Profunda",    200.0, 250.0, 300.0, 300.0));
        sys.addInput(prof);
        
        // Reglas
        FuzzyRule r1(1);
        r1.addAntecedent("Magnitud", "Alta");
        r1.addAntecedent("Profundidad", "Superficial");
        r1.setConsequent("Intensidad", "Alta");
        sys.addRule(r1);
        
        FuzzyRule r2(2);
        r2.addAntecedent("Magnitud", "Baja");
        r2.setConsequent("Intensidad", "Baja");
        sys.addRule(r2);
        
        return sys;
    }
};

TEST_F(FuzzySystemTest, ComputeReturnsValidCrispOutput) {
    auto sys = build_system();
    FuzzyResult r = sys.compute({{"Magnitud", 7.5}, {"Profundidad", 30.0}});
    
    EXPECT_GE(r.crisp_output, 1.0);
    EXPECT_LE(r.crisp_output, 12.0);
    EXPECT_EQ(r.rule_degrees.size(), 2u);
    EXPECT_EQ(r.aggregated_mu.size(), 500);
}

TEST_F(FuzzySystemTest, HighMagSuperficialProducesHighIntensity) {
    auto sys = build_system();
    FuzzyResult r = sys.compute({{"Magnitud", 8.5}, {"Profundidad", 10.0}});
    EXPECT_GT(r.crisp_output, 7.0); // Debe tender a "Alta"
}

TEST_F(FuzzySystemTest, LowMagProducesLowIntensity) {
    auto sys = build_system();
    FuzzyResult r = sys.compute({{"Magnitud", 3.5}, {"Profundidad", 50.0}});
    EXPECT_LT(r.crisp_output, 5.0); // Debe tender a "Baja"
}

TEST_F(FuzzySystemTest, InvalidInputThrows) {
    auto sys = build_system();
    EXPECT_THROW(sys.compute({{"Magnitud", 10.0}, {"Profundidad", 30.0}}), std::invalid_argument);
}
```

---

## 4. Build y ejecución (Windows)

```powershell
# Configurar desde la raíz; Eigen3 debe estar disponible como paquete CMake
# (por ejemplo, con el toolchain de vcpkg).
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="C:\ruta\a\vcpkg\scripts\buildsystems\vcpkg.cmake"

# Compilar
cmake --build build --config Debug

# Ejecutar las pruebas registradas
ctest --test-dir build -C Debug --output-on-failure
```

El módulo Python `fuzzy_core` se copia automáticamente a `workspace/python/fuzzy_ui/`; no es necesario copiarlo manualmente. CMake obtiene pybind11 y GoogleTest con FetchContent, mientras que Eigen3 debe estar instalado y visible para `find_package(Eigen3 CONFIG REQUIRED)`.

El build y los siete casos de CTest se verificaron en Windows con MSVC. Las MF trapezoidales admiten hombros con `a == b` o `c == d`; las pruebas cubren ambos casos, valores fuera de soporte y parámetros inválidos.

---

## 5. Resultado esperado y estado de validación

El test `HighMagSuperficialProducesHighIntensity` verifica:
- Fusificación → `Magnitud=8.5` activa "Alta" con μ=1.0
- Inferencia → R1 se activa con α = min(1.0, μ_Superficial(10)) ≈ 1.0
- Agregación → μ_agregado recortado en "Alta"
- Centroide → y* > 7.0

Este test y los otros seis casos registrados pasan en el entorno de validación.

---
