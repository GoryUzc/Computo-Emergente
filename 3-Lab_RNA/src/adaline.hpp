#pragma once
#include "RNA_Engine.hpp"
#include <Eigen/Dense>
#include <vector>

/**
 * @brief Modelo ADALINE (ADAptive LInear NEuron) con Regla Delta.
 * 
 * Implementa el aprendizaje supervisado para regresión lineal mediante descenso de gradiente estocástico.
 * Aplicación: Descodificador binario (3 bits) a decimal (0-7).
 * Sin función de activación umbral durante el entrenamiento.
 */
class Adaline : public RNA_Base {
private:
    Eigen::VectorXd weights_;         // [w1, w2, w3] (sin bias, según guía)
    Eigen::MatrixXd X_;               // 8x3 (patrones binarios)
    Eigen::VectorXd D_;               // [0, 1, 2, 3, 4, 5, 6, 7]
    std::vector<double> mae_history_; // MAE por época (para CSV y métricas)
    std::vector<Eigen::VectorXd> noisy_history_;
    std::vector<Eigen::VectorXd> recovered_history_;
    std::vector<int> pattern_idx_history_;
    
    void setup_patterns();            // Carga estática de X y D

public:
    explicit Adaline(const RNA_Params& p);
    
    std::string get_model_name() const override;
    std::string get_theory_summary() const override;
    void initialize() override;
    double train_epoch() override;
    void compute_metrics() override;
    void export_csv() const override;
    void print_final_predictions() const;
};