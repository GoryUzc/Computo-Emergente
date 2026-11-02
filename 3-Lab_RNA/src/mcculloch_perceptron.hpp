#pragma once
#include "RNA_Engine.hpp"
#include <Eigen/Dense>
#include <vector>

/**
 * @brief Modelo McCulloch-Pitts & Perceptrón Simple para clasificación linealmente separable.
 * 
 * Implementa la regla de aprendizaje Δw = η(d - ŷ)x con función escalón Heaviside.
 * Soporta operaciones booleanas AND/OR mediante toggle en params.extra_1.
 * Incluye bias explícito (x₀ = 1) y exportación CSV con métricas de aciertos.
 */
class McCullochPerceptron : public RNA_Base {
private:
    Eigen::VectorXd weights_;      // [w0(bias), w1, w2]
    Eigen::MatrixXd X_;            // 4x3 (columna bias + 2 entradas)
    Eigen::VectorXd D_;            // Targets [0/1]
    std::vector<int> accuracy_;    // Aciertos por época
    bool use_or_;                  // Toggle AND/OR controlado por params.extra_1

    double heaviside(double z) const;
    void setup_patterns();

public:
    explicit McCullochPerceptron(const RNA_Params& p);
    
    std::string get_model_name() const override;
    std::string get_theory_summary() const override;
    void initialize() override;
    double train_epoch() override;
    void compute_metrics() override;
    void export_csv() const override;
    void print_final_predictions() const;
};