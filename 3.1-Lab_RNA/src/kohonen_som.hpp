#pragma once
#include "RNA_Engine.hpp"
#include <Eigen/Dense>
#include <vector>

class KohonenSOM : public RNA_Base {
private:
    Eigen::MatrixXd weights_;
    Eigen::MatrixXd data_;
    Eigen::MatrixXd grid_coords_;
    Eigen::MatrixXd grid_dist_sq_;
    int map_size_;
    double sigma_, eta_, tau_;
    std::vector<double> sigma_history_, eta_history_;
    std::vector<int> bmu_counts_;

    void generate_clusters();
    void precompute_grid();
    int find_bmu(const Eigen::VectorXd& x) const;

public:
    explicit KohonenSOM(const RNA_Params& p);
    
    std::string get_model_name() const override;
    std::string get_theory_summary() const override;
    void initialize() override;
    double train_epoch() override;
    void compute_metrics() override;
    void export_csv() const override;
    void export_snapshot(int epoch) const;
};