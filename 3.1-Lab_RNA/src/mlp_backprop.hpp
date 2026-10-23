#pragma once
#include "RNA_Engine.hpp"
#include <Eigen/Dense>
#include <vector>

/**
 * @brief Perceptrón Multicapa con Backpropagation y Momentum.
 * 
 * Soporta dos tareas: clasificación XOR (2-2-1) y aproximación de sin(x) (1-10-1).
 * Usa activación sigmoide en capas ocultas y salida lineal para regresión.
 * Implementa actualización de pesos con momento para acelerar convergencia.
 */
class MLPBackprop : public RNA_Base {
private:
    std::vector<Eigen::MatrixXd> weights_, biases_;
    std::vector<Eigen::MatrixXd> prev_delta_W_, prev_delta_b_;
    Eigen::MatrixXd X_, D_;
    bool task_is_sin_;
    double sin_scale_min_ = 0.1, sin_scale_max_ = 0.9;

    void setup_xor();
    void setup_sin();
    double sigmoid(double z) const;
    double sigmoid_deriv(double a) const;

public:
    explicit MLPBackprop(const RNA_Params& p);
    
    std::string get_model_name() const override;
    std::string get_theory_summary() const override;
    void initialize() override;
    double train_epoch() override;
    void compute_metrics() override;
    void export_csv() const override;
    void print_predictions() const;
};