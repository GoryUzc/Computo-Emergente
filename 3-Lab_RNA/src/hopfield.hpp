#pragma once
#include "RNA_Engine.hpp"
#include <Eigen/Dense>
#include <vector>

class HopfieldNet : public RNA_Base {
private:
    Eigen::MatrixXd W_;
    Eigen::MatrixXd patterns_;
    std::vector<double> energy_history_;
    std::vector<int> noise_history_;
    std::vector<double> recovery_history_;
    std::vector<Eigen::VectorXd> noisy_history_;
    std::vector<Eigen::VectorXd> recovered_history_;
    std::vector<int> pattern_idx_history_;
    int current_pattern_idx_;
    int current_noise_pct_;

    void define_patterns();
    void recover_pattern(const Eigen::VectorXd& noisy, Eigen::VectorXd& recovered);

public:
    explicit HopfieldNet(const RNA_Params& p);
    
    std::string get_model_name() const override;
    std::string get_theory_summary() const override;
    void initialize() override;
    double train_epoch() override;
    void compute_metrics() override;
    void export_csv() const override;
};