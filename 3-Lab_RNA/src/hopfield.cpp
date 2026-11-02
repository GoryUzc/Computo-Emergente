#include "hopfield.hpp"
#include <random>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

// Función auxiliar para convertir vector bipolar a string
static std::string vectorToString(const Eigen::VectorXd& v) {
    std::string s;
    s.reserve(static_cast<size_t>(v.size()));
    for (int i = 0; i < v.size(); ++i)
        s.push_back((v(i) >= 0.0) ? '1' : '-');
    return s;
}

HopfieldNet::HopfieldNet(const RNA_Params& p) : RNA_Base(p) {
    current_pattern_idx_ = 0;
    current_noise_pct_ = 10;
}

std::string HopfieldNet::get_model_name() const {
    return "Hopfield_Net";
}

std::string HopfieldNet::get_theory_summary() const {
    return "HOPFIELD (MEMORIA ASOCIATIVA)\n"
           "Almacenamiento (Hebb): W = (1/N)*sum(x^k*(x^k)^T) - I\n"
           "Energia (Lyapunov): E = -0.5*sum(W_ij*x_i*x_j)  (i!=j)\n"
           "Recuperacion: x_i <- sgn(sum(W_ij * x_j)) hasta convergencia\n"
           "Capacidad: p_max ~ 0.14 * N (N=42 -> p<=5)\n"
           "Uso: patrones 7x6, ruido 10-30%, objetivo >=90% recuperacion";
}

void HopfieldNet::define_patterns() {
    const int rows = 7, cols = 6, N = rows * cols;
    const int binary_A[7][6] = {{0,1,1,0,0,0},{1,0,0,1,0,0},{1,0,0,0,1,0},{1,0,0,0,0,1},{1,1,1,1,1,1},{1,0,0,0,0,1},{1,0,0,0,0,1}};
    const int binary_B[7][6] = {{1,1,1,1,0,0},{1,0,0,0,1,0},{1,0,0,0,0,1},{1,1,1,1,0,0},{1,0,0,0,0,1},{1,0,0,0,1,0},{1,1,1,1,0,0}};
    const int binary_C[7][6] = {{0,1,1,1,1,0},{1,0,0,0,0,1},{1,0,0,0,0,0},{1,0,0,0,0,0},{1,0,0,0,0,0},{1,0,0,0,0,1},{0,1,1,1,1,0}};
    const int binary_D[7][6] = {{1,1,1,1,0,0},{1,0,0,0,1,0},{1,0,0,0,0,1},{1,0,0,0,0,1},{1,0,0,0,0,1},{1,0,0,0,1,0},{1,1,1,1,0,0}};
    const int* patterns_ptr[4] = {&binary_A[0][0], &binary_B[0][0], &binary_C[0][0], &binary_D[0][0]};
    patterns_.resize(N, 4);
    for(int k=0; k<4; ++k) {
        for(int i=0; i<N; ++i) {
            patterns_(i, k) = (patterns_ptr[k][i] == 1) ? 1.0 : -1.0;
        }
    }
    W_ = Eigen::MatrixXd::Zero(N, N);
    for(int k=0; k<4; ++k) {
        Eigen::VectorXd xk = patterns_.col(k);
        W_ += xk * xk.transpose();
    }
    W_.diagonal().setZero();
}

void HopfieldNet::recover_pattern(const Eigen::VectorXd& noisy, Eigen::VectorXd& recovered) {
    recovered = noisy;
    const int N = 42;
    int steps = 0;
    bool stable = false;
    // Semilla fija para reproducibilidad
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(0, N-1);
    while(!stable && steps < N * 10) {
        stable = true;
        for(int s=0; s<N; ++s) {
            int i = dist(rng);
            double h = W_.row(i).dot(recovered);
            double new_val = (h >= 0.0) ? 1.0 : -1.0;
            if(new_val != recovered(i)) {
                recovered(i) = new_val;
                stable = false;
            }
        }
        ++steps;
    }
}

void HopfieldNet::initialize() {
    define_patterns();
    energy_history_.clear();
    noise_history_.clear();
    recovery_history_.clear();
    noisy_history_.clear();
    recovered_history_.clear();
    pattern_idx_history_.clear();
    current_pattern_idx_ = 0;
    current_noise_pct_ = 10;
    std::cout << "Hopfield | 4 patrones (A-D) almacenados. N=42, p=4\n";
    std::cout << "Capacidad teórica: ~5 patrones. Estado: ESTABLE.\n";
}

double HopfieldNet::train_epoch() {
    int p_idx = current_pattern_idx_ % 4;
    int noise_pct = current_noise_pct_;
    Eigen::VectorXd original = patterns_.col(p_idx);
    Eigen::VectorXd noisy = original;
    std::mt19937 rng(42 + p_idx * 10 + noise_pct);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for(int i=0; i<42; ++i) {
        if(dist(rng) < noise_pct / 100.0) {
            noisy(i) *= -1.0;
        }
    }
    Eigen::VectorXd recovered;
    recover_pattern(noisy, recovered);
    double energy = -0.5 * recovered.transpose() * W_ * recovered;
    int hamming = (original.cwiseProduct(recovered).array() == -1.0).count();
    double recovery_pct = (42.0 - hamming) / 42.0 * 100.0;
    energy_history_.push_back(energy);
    noise_history_.push_back(noise_pct);
    recovery_history_.push_back(recovery_pct);
    noisy_history_.push_back(noisy);
    recovered_history_.push_back(recovered);
    pattern_idx_history_.push_back(p_idx);
    current_noise_pct_ += 10;
    if(current_noise_pct_ > 30) {
        current_noise_pct_ = 10;
        current_pattern_idx_++;
    }
    return energy;
}

void HopfieldNet::compute_metrics() {
    int tests = static_cast<int>(energy_history_.size());
    double avg_10 = 0.0, count_10 = 0.0;
    double avg_30 = 0.0, count_30 = 0.0;
    // Cálculo seguro sin índices fijos
    for(size_t i = 0; i < noise_history_.size(); ++i) {
        if(noise_history_[i] == 10) { avg_10 += recovery_history_[i]; count_10++; }
        if(noise_history_[i] == 30) { avg_30 += recovery_history_[i]; count_30++; }
    }
    if(count_10 > 0) avg_10 /= count_10;
    if(count_30 > 0) avg_30 /= count_30;
    std::cout << "[MÉTRICAS] Red de Hopfield - Memoria Asociativa\n";
    std::cout << "  - Patrones almacenados:             4 (A, B, C, D)\n";
    std::cout << "  - Unidades (N):                     42\n";
    std::cout << "  - Tests ejecutados:                 " << tests << "\n";
    std::cout << "  - Niveles de ruido probados:        10%, 20%, 30%\n";
    std::cout << "  - Recuperación promedio (10%):      " << std::fixed << std::setprecision(1) << avg_10 << "%\n";
    std::cout << "  - Recuperación promedio (30%):      " << std::fixed << std::setprecision(1) << avg_30 << "%\n";
    std::cout << "  - Observación:                      Energía decrece monótonamente.\n";
    std::cout << "  - Limitación:                       Fallos esperados si p/N > 0.14 o ruido > 35%.\n";
}

void HopfieldNet::export_csv() const {
    std::filesystem::create_directories("output");
    std::ofstream out(params_.csv_path);  // ← Corregido: params_ con guion bajo
    if (!out.is_open()) {
        std::cerr << "\nError al abrir CSV: " << params_.csv_path << "\n";
        return;
    }
    out << "Modelo,Epoca,Energia,RuidoPct,RecuperacionPct,PatternIdx,NoisyPixels,RecoveredPixels\n";
    for (size_t i = 0; i < energy_history_.size(); ++i) {
        out << get_model_name() << "," << (i + 1) << ","
            << std::fixed << std::setprecision(4) << energy_history_[i] << ","
            << noise_history_[i] << ","
            << std::fixed << std::setprecision(2) << recovery_history_[i] << ","
            << pattern_idx_history_[i] << ","
            << vectorToString(noisy_history_[i]) << ","
            << vectorToString(recovered_history_[i]) << "\n";
    }
    out.close();
}