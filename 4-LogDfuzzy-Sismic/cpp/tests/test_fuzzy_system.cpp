#include <gtest/gtest.h>
#include "fuzzy_core/fuzzy_system.hpp"
#include "fuzzy_core/triangular_mf.hpp"
#include "fuzzy_core/trapezoidal_mf.hpp"
#include <cmath>

using namespace fuzzy_core;

TEST(TrapezoidalMFTest, EvaluatesLeftShoulderWithoutDivisionByZero) {
    TrapezoidalMF mf("left", 1.0, 1.0, 3.0, 5.0);

    EXPECT_DOUBLE_EQ(mf.evaluate(0.0), 0.0);
    EXPECT_DOUBLE_EQ(mf.evaluate(1.0), 1.0);
    EXPECT_DOUBLE_EQ(mf.evaluate(2.0), 1.0);
    EXPECT_DOUBLE_EQ(mf.evaluate(4.0), 0.5);
    EXPECT_DOUBLE_EQ(mf.evaluate(5.0), 0.0);
    EXPECT_DOUBLE_EQ(mf.evaluate(6.0), 0.0);
}

TEST(TrapezoidalMFTest, EvaluatesRightShoulderWithoutDivisionByZero) {
    TrapezoidalMF mf("right", 1.0, 3.0, 5.0, 5.0);

    EXPECT_DOUBLE_EQ(mf.evaluate(0.0), 0.0);
    EXPECT_DOUBLE_EQ(mf.evaluate(1.0), 0.0);
    EXPECT_DOUBLE_EQ(mf.evaluate(2.0), 0.5);
    EXPECT_DOUBLE_EQ(mf.evaluate(3.0), 1.0);
    EXPECT_DOUBLE_EQ(mf.evaluate(5.0), 1.0);
    EXPECT_DOUBLE_EQ(mf.evaluate(6.0), 0.0);
}

TEST(TrapezoidalMFTest, RejectsInvalidOrDegenerateParameters) {
    EXPECT_THROW(TrapezoidalMF("invalid-order", 2.0, 1.0, 3.0, 4.0),
                 std::invalid_argument);
    EXPECT_THROW(TrapezoidalMF("degenerate", 2.0, 2.0, 2.0, 2.0),
                 std::invalid_argument);
}

// Sistema mínimo de prueba: 2 entradas, 1 salida, 2 reglas
class FuzzySystemTest : public ::testing::Test {
protected:
    FuzzySystem build_system() {
        // Salida: Intensidad [1, 12]
        LinguisticVariable output("Intensidad", {1.0, 12.0});
        output.addTerm(std::make_shared<TrapezoidalMF>("Baja",  1.0, 1.0, 3.0, 5.0));
        output.addTerm(std::make_shared<TriangularMF>("Media", 4.0, 6.5, 9.0));
        output.addTerm(std::make_shared<TrapezoidalMF>("Alta", 8.0, 10.0, 12.0, 12.0));
        
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
    EXPECT_GT(r.crisp_output, 7.0);
}

TEST_F(FuzzySystemTest, LowMagProducesLowIntensity) {
    auto sys = build_system();
    FuzzyResult r = sys.compute({{"Magnitud", 3.5}, {"Profundidad", 50.0}});
    EXPECT_LT(r.crisp_output, 5.0);
}

TEST_F(FuzzySystemTest, InvalidInputThrows) {
    auto sys = build_system();
    EXPECT_THROW(sys.compute({{"Magnitud", 10.0}, {"Profundidad", 30.0}}), std::invalid_argument);
}
