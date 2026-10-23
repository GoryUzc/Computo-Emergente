#include "kohonen_som.hpp"
#include <random>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

KohonenSOM::KohonenSOM(const RNA_Params& p) : RNA_Base(p) {
    map_size_ = 10;   // Mapa 10x10 = 100 neuronas
    sigma_ = 5.0;     // σ₀ (radio inicial de vecindad)
    eta_ = 0.5;       // η₀ (tasa de aprendizaje inicial)
}

std::string KohonenSOM::get_model_name() const {
    return "Kohonen_SOM";
}

std::string KohonenSOM::get_theory_summary() const {
    return "KOHONEN SOM (MAPA AUTOORGANIZADO)\n"
           "Competencia: c = argmin ||x - w_i||\n"
           "Vecindad gaussiana: h_ij = exp(-||r_i-r_j||^2 / (2*sigma^2))\n"
           "Actualizacion: w_i(t+1) = w_i(t) + eta(t)*h_ij*(x - w_i)\n"
           "Decaimiento: eta(t)=eta0*exp(-t/tau), sigma(t)=sigma0*exp(-t/tau)\n"
           "Mapa: 10x10, eta0=0.5, sigma0=5, T=1000";
}

void KohonenSOM::generate_clusters() {
    std::mt19937 rng(42);
    std::normal_distribution<double> dist(0.0, 0.06);
    double centers[4][2] = {{0.2, 0.2}, {0.8, 0.2}, {0.2, 0.8}, {0.8, 0.8}};
    int pts_per_cluster = 50;
    int N = pts_per_cluster * 4;
    data_.resize(N, 2);
    int idx = 0;
    for(int c=0; c<4; ++c) {
        for(int i=0; i<pts_per_cluster; ++i) {
            data_(idx, 0) = std::max(0.0, std::min(1.0, centers[c][0] + dist(rng)));
            data_(idx, 1) = std::max(0.0, std::min(1.0, centers[c][1] + dist(rng)));
            ++idx;
        }
    }
}

void KohonenSOM::precompute_grid() {
    int M2 = map_size_ * map_size_;
    grid_coords_.resize(M2, 2);
    grid_dist_sq_.resize(M2, M2);
    for (int i = 0; i < map_size_; ++i) {
        for (int j = 0; j < map_size_; ++j) {
            int k = i * map_size_ + j;
            grid_coords_(k, 0) = i;
            grid_coords_(k, 1) = j;
        }
    }
    for (int c = 0; c < M2; ++c) {
        for (int i = 0; i < M2; ++i) {
            double dx = grid_coords_(c, 0) - grid_coords_(i, 0);
            double dy = grid_coords_(c, 1) - grid_coords_(i, 1);
            grid_dist_sq_(c, i) = dx * dx + dy * dy;
        }
    }
}

int KohonenSOM::find_bmu(const Eigen::VectorXd& x) const {
    double min_dist = 1e18;
    int bmu = 0;
    int M2 = weights_.rows();
    for (int i = 0; i < M2; ++i) {
        double dx = x(0) - weights_(i, 0);
        double dy = x(1) - weights_(i, 1);
        double d = dx * dx + dy * dy;
        if (d < min_dist) {
            min_dist = d;
            bmu = i;
        }
    }
    return bmu;
}

void KohonenSOM::initialize() {
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    int M2 = map_size_ * map_size_;
    weights_.resize(M2, 2);
    for (int i = 0; i < M2; ++i) {
        weights_(i, 0) = dist(rng);
        weights_(i, 1) = dist(rng);
    }

    // Snapshot inicial (época 0)
    std::filesystem::create_directories("output");
    std::ofstream snap("output/kohonen_snapshots.csv");
    if (snap.is_open()) {
        snap << "Modelo,SnapshotEpoch,NeuronIdx,WeightX,WeightY\n";
        for (int i = 0; i < M2; ++i) {
            snap << get_model_name() << ",0,"
                 << i << ","
                 << std::fixed << std::setprecision(6) << weights_(i, 0) << ","
                 << std::fixed << std::setprecision(6) << weights_(i, 1) << "\n";
        }
        snap.close();
    }

    generate_clusters();
    precompute_grid();
    bmu_counts_.assign(M2, 0);
    error_history_.clear();
    sigma_history_.clear();
    eta_history_.clear();

    // τ = T / ln(σ₀) para decaimiento suave hasta σ(T) ≈ 1
    tau_ = params_.max_epochs / std::log(sigma_);

    std::cout << " Kohonen | Mapa " << map_size_ << "x" << map_size_
              << " | 4 Clusters sintéticos (N=" << data_.rows() << ")\n";
}

double KohonenSOM::train_epoch() {
    int N = data_.rows();
    int M2 = weights_.rows();
    double total_qe = 0.0;

    // Índice de época actual (0-based)
    int epoch_idx = static_cast<int>(error_history_.size());
    double current_epoch = static_cast<double>(epoch_idx);

    // Decaimiento exponencial de η y σ
    eta_ = params_.learning_rate * std::exp(-current_epoch / tau_);
    sigma_ = 5.0 * std::exp(-current_epoch / tau_);

    // Entrenamiento de una época completa
    for (int p = 0; p < N; ++p) {
        Eigen::VectorXd x = data_.row(p).transpose();
        int bmu = find_bmu(x);

        // Actualizar pesos de la BMU y sus vecinos
        for (int i = 0; i < M2; ++i) {
            double h = std::exp(-grid_dist_sq_(bmu, i) / (2.0 * sigma_ * sigma_));
            // Actualización vectorizada correcta
            Eigen::VectorXd diff = x - weights_.row(i).transpose();
            weights_.row(i) += (eta_ * h * diff).transpose();
        }

        // Contar activaciones de la BMU
        bmu_counts_[bmu]++;

        // Calcular QE para este patrón
        double dx = x(0) - weights_(bmu, 0);
        double dy = x(1) - weights_(bmu, 1);
        total_qe += std::sqrt(dx * dx + dy * dy);
    }

    double qe = total_qe / N;

    // Guardar historiales
    error_history_.push_back(qe);
    sigma_history_.push_back(sigma_);
    eta_history_.push_back(eta_);

    // Exportar snapshot cada 100 épocas
    if (epoch_idx % 100 == 0 && epoch_idx > 0) {
        export_snapshot(epoch_idx);
    }

    return qe;  // ← CRÍTICO: retornar el QE
}

void KohonenSOM::compute_metrics() {
    int epochs = static_cast<int>(error_history_.size());
    double final_qe = error_history_.empty() ? 0.0 : error_history_.back();
    int active_neurons = 0;
    int M2 = weights_.rows();
    for (int c : bmu_counts_) {
        if (c > 0) ++active_neurons;
    }

    std::cout << "[MÉTRICAS] Kohonen SOM - Agrupamiento No Supervisado\n";
    std::cout << "  - Épocas completadas:               " << epochs << "\n";
    std::cout << "  - QE final:                         " << std::fixed << std::setprecision(4) << final_qe << "\n";
    std::cout << "  - Neuronas activadas:               " << active_neurons << "/" << M2 << "\n";
    std::cout << "  - Preservación topológica:          " << (active_neurons > M2 * 0.6 ? "ALTA" : "MEDIA") << "\n";
}

void KohonenSOM::export_snapshot(int epoch) const {
    std::filesystem::create_directories("output");
    std::ofstream out("output/kohonen_snapshots.csv", std::ios::app);
    if (!out.is_open()) {
        std::cerr << "[ERR] No se pudo abrir kohonen_snapshots.csv\n";
        return;
    }
    int M2 = weights_.rows();
    for (int i = 0; i < M2; ++i) {
        out << get_model_name() << "," << epoch << ","
            << i << ","
            << std::fixed << std::setprecision(6) << weights_(i, 0) << ","
            << std::fixed << std::setprecision(6) << weights_(i, 1) << "\n";
    }
    out.close();
}

void KohonenSOM::export_csv() const {
    std::filesystem::create_directories("output");
    std::ofstream out(params_.csv_path);
    if (!out.is_open()) {
        std::cerr << "[ERR] Error al abrir CSV: " << params_.csv_path << "\n";
        return;
    }
    out << "Modelo,Epoca,QuantizationError,Sigma,LearningRate\n";
    for (size_t i = 0; i < error_history_.size(); ++i) {
        out << get_model_name() << "," << (i + 1) << ","
            << std::fixed << std::setprecision(6) << error_history_[i] << ","
            << std::fixed << std::setprecision(6) << sigma_history_[i] << ","
            << std::fixed << std::setprecision(6) << eta_history_[i] << "\n";
    }
    out.close();
}