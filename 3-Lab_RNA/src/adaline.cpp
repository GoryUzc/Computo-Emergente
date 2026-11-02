#include "adaline.hpp"
#include <random>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

Adaline::Adaline(const RNA_Params& p) : RNA_Base(p) {}

void Adaline::setup_patterns() {
    X_.resize(8, 3);
    // Patrones binarios de 3 bits (x1, x2, x3)
    X_ << 0, 0, 0,
          0, 0, 1,
          0, 1, 0,
          0, 1, 1,
          1, 0, 0,
          1, 0, 1,
          1, 1, 0,
          1, 1, 1;

    D_.resize(8);
    // Valores decimales equivalentes
    D_ << 0, 1, 2, 3, 4, 5, 6, 7;
}

std::string Adaline::get_model_name() const {
    return "ADALINE";
}

std::string Adaline::get_theory_summary() const {
    return "ADALINE (REGLA DELTA)\n"
           "Salida lineal: y = sum(w_j * x_j)\n"
           "Error MSE: E = 0.5 * sum(d_p - y_p)^2\n"
           "Regla Delta: delta_w_j = gamma * (d_p - y_p) * x_j\n"
           "Uso: descodificador binario -> decimal (d=4x1+2x2+x3)\n"
           "Param: gamma=0.005, eps=1e-4";
}

void Adaline::initialize() {
    // [Reproducibilidad] Semilla fija 42
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    
    weights_.resize(3);
    for (int i = 0; i < 3; ++i) weights_(i) = dist(rng);

    setup_patterns();
    mae_history_.clear();
    
    std::cout << "[ADALINE] Tarea: Descodificador 3-bit → Decimal\n";
    std::cout << "Pesos iniciales: [" << weights_.transpose() << "]\n";
}

double Adaline::train_epoch() {
    double mse = 0.0;
    double mae = 0.0;
    const int N = X_.rows();

    for (int i = 0; i < N; ++i) {
        Eigen::VectorXd x = X_.row(i).transpose();
        double d = D_(i);

        // [Fórmula] y_p = w·x (salida lineal, sin umbral)
        double y = weights_.dot(x);
        // [Fórmula] e_p = d_p - y_p
        double err = d - y;

        // [Fórmula] Δw = γ * e_p * x (actualización de pesos)
        weights_ += params_.learning_rate * err * x;
        
        mse += err * err;
        mae += std::abs(err);
    }

    mse /= N; // MSE = (1/N) Σ e²
    mae /= N; // MAE = (1/N) Σ |e|
    
    mae_history_.push_back(mae);
    return mse;
}

void Adaline::print_final_predictions() const {
    std::cout << "\n[PRUEBA] Predicciones finales con pesos convergidos (w ≈ [4,2,1]):\n";
    std::cout << " x1  x2  x3  | D  Ŷ  | Correcto | Error\n";
    std::cout << "---------------------------------------------\n";
    for (int i = 0; i < 8; ++i) {
        Eigen::VectorXd x = X_.row(i).transpose();
        double d = D_(i);
        double y_hat = weights_.dot(x);  // Salida lineal (sin umbral)
        bool ok = (std::abs(d - y_hat) < 0.05); // Tolerancia para convergencia
        double err = std::abs(d - y_hat);
        std::cout << " " << (int)X_(i,0) << "   " 
                  << (int)X_(i,1) << "   " << (int)X_(i,2) << "  | "
                  << std::fixed << std::setprecision(1)
                  << d << "  " << std::setprecision(4) << y_hat
                  << "  | " << (ok ? "Sí" : "No")
                  << " | " << std::scientific << std::setprecision(2) << err << "\n";
    }
}

void Adaline::compute_metrics() {
    int epochs = static_cast<int>(error_history_.size());
    double final_mse = error_history_.back();
    double final_mae = mae_history_.back();

    // Pesos teóricos analíticos: d = 4*x1 + 2*x2 + 1*x3
    Eigen::VectorXd w_teo(3);
    w_teo << 4.0, 2.0, 1.0;

    Eigen::VectorXd abs_err = (weights_ - w_teo).cwiseAbs();
    double max_abs_err = abs_err.maxCoeff();

    std::cout << "[MÉTRICAS] ADALINE - Descodificador Binario\n";
    std::cout << " - Épocas hasta convergencia/parada: " << epochs << "\n";
    std::cout << "  - MSE final:                        " << std::scientific << std::setprecision(4) << final_mse << "\n";
    std::cout << "  - MAE final:                        " << std::scientific << std::setprecision(4) << final_mae << "\n";
    std::cout << "  - Pesos finales [w1, w2, w3]:       [" 
              << std::fixed << std::setprecision(4) 
              << weights_(0) << ", " << weights_(1) << ", " << weights_(2) << "]\n";
    std::cout << "  - Pesos teóricos [4, 2, 1]:         [4.0000, 2.0000, 1.0000]\n";
    std::cout << "  - Desviación absoluta máxima:       " << std::scientific << std::setprecision(4) << max_abs_err << "\n";
    std::cout << "  - Estado de convergencia:           " << (max_abs_err < 0.05 ? "EXITOSA (≈ teóricos)" : "REQUIERE AJUSTE DE γ o ÉPOCAS") << "\n";

        if (max_abs_err < 0.05) {
        std::cout << "\n✅ Convergencia confirmada (desviación < 0.05). Mostrando predicciones finales:\n";
        print_final_predictions();
    } else {
        std::cout << "\n⚠️  No alcanzó convergencia teórica (desviación ≥ 0.05). Ajustar γ o épocas.\n";
    }
}

void Adaline::export_csv() const {
    std::filesystem::create_directories("output");
    std::ofstream out(params_.csv_path);
    if (!out.is_open()) {
        std::cerr << "[ERR] Error al abrir CSV: " << params_.csv_path << "\n";
        return;
    }

    // Formato aprobado: Modelo,Epoca,MSE,MAE
    out << "Modelo,Epoca,MSE,MAE\n";
    for (size_t i = 0; i < error_history_.size(); ++i) {
        out << get_model_name() << "," << (i + 1) << ","
            << std::fixed << std::setprecision(6) << error_history_[i] << ","
            << std::fixed << std::setprecision(6) << mae_history_[i] << "\n";
    }
    out.close();
}