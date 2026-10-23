#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include "mlp_backprop.hpp"
#include <random>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

MLPBackprop::MLPBackprop(const RNA_Params& p) : RNA_Base(p) {
    task_is_sin_ = (params_.extra_1 >= 1.5);
}

double MLPBackprop::sigmoid(double z) const {
    return 1.0 / (1.0 + std::exp(-z));
}

double MLPBackprop::sigmoid_deriv(double a) const {
    return a * (1.0 - a);
}

void MLPBackprop::setup_xor() {
    X_.resize(4, 2);
    X_ << 0, 0,
          0, 1,
          1, 0,
          1, 1;
    D_.resize(4, 1);
    D_ << 0, 1, 1, 0;
}

void MLPBackprop::setup_sin() {
    const int N = 50;
    X_.resize(N, 1);
    D_.resize(N, 1);
    for(int i=0; i<N; ++i) {
        double x = (2.0 * M_PI * i) / (N - 1);
        X_(i,0) = x;
        double y = std::sin(x);
        D_(i,0) = sin_scale_min_ + (y + 1.0) * 0.5 * (sin_scale_max_ - sin_scale_min_);
    }
}

std::string MLPBackprop::get_model_name() const {
    return "MLP_Backprop";
}

std::string MLPBackprop::get_theory_summary() const {
    return "PERCEPTRON MULTICAPA (BACKPROP)\n"
           "Activacion: sigma(z)=1/(1+exp(-z)), sigma' = sigma*(1-sigma)\n"
           "Error: E = 0.5 * sum(t_k - y_k)^2\n"
           "Backprop: delta^l = (W^(l+1))^T * delta^(l+1) * sigma'(z^l)\n"
           "Actualizacion: delta_W = -eta*delta*(a_prev)^T + beta*delta_W_prev\n"
           "Apps: XOR (2-2-1), aproximacion sin(x)\n"
           "Param: eta=0.05, beta=0.9, ocultas=[2,10]";
}



void MLPBackprop::initialize() {
    std::mt19937 rng(42);
    std::normal_distribution<double> dist(0.0, 0.5);
    
    if (task_is_sin_) {
        setup_sin();
        weights_ = {Eigen::MatrixXd::Random(10, 1).unaryExpr([&](double v){return dist(rng);}), 
                    Eigen::MatrixXd::Random(1, 10).unaryExpr([&](double v){return dist(rng);})};
        biases_  = {Eigen::VectorXd::Zero(10), Eigen::VectorXd::Zero(1)};
        prev_delta_W_ = {Eigen::MatrixXd::Zero(10, 1), Eigen::MatrixXd::Zero(1, 10)};
        prev_delta_b_ = {Eigen::VectorXd::Zero(10), Eigen::VectorXd::Zero(1)};
        std::cout << "\n🔹 MLP | Tarea: Aproximación sin(x) [1-10-1]\n";
    } else {
        setup_xor();
        weights_ = {Eigen::MatrixXd::Random(2, 2).unaryExpr([&](double v){return dist(rng);}), 
                    Eigen::MatrixXd::Random(1, 2).unaryExpr([&](double v){return dist(rng);})};
        biases_  = {Eigen::VectorXd::Zero(2), Eigen::VectorXd::Zero(1)};
        prev_delta_W_ = {Eigen::MatrixXd::Zero(2, 2), Eigen::MatrixXd::Zero(1, 2)};
        prev_delta_b_ = {Eigen::VectorXd::Zero(2), Eigen::VectorXd::Zero(1)};
        std::cout << "🔹 MLP | Tarea: Clasificación XOR [2-2-1]\n";
    }
}

double MLPBackprop::train_epoch() {
    double mse = 0.0;
    int N = X_.rows();
    double lr = params_.learning_rate;
    double beta = 0.1;

    for(int p=0; p<N; ++p) {
        Eigen::VectorXd a0 = X_.row(p).transpose();
        Eigen::VectorXd target = D_.row(p).transpose();

        Eigen::VectorXd z1 = weights_[0] * a0 + biases_[0];
        Eigen::VectorXd a1 = z1.unaryExpr([this](double z){ return this->sigmoid(z); });
        Eigen::VectorXd z2 = weights_[1] * a1 + biases_[1];
        
        Eigen::VectorXd a2;
        if(task_is_sin_) {
            a2 = z2;
        } else {
            a2 = z2.unaryExpr([this](double z){ return this->sigmoid(z); });
        }

        Eigen::VectorXd delta2, delta1;
        if(task_is_sin_) {
            delta2 = a2 - target;
        } else {
            delta2 = (a2 - target).cwiseProduct(z2.unaryExpr([this](double z){ return this->sigmoid_deriv(this->sigmoid(z)); }));
        }
        
        Eigen::VectorXd d1 = weights_[1].transpose() * delta2;
        delta1 = d1.cwiseProduct(a1.unaryExpr([this](double a){ return this->sigmoid_deriv(a); }));

        Eigen::MatrixXd dW2 = -lr * delta2 * a1.transpose() + beta * prev_delta_W_[1];
        Eigen::VectorXd db2 = -lr * delta2 + beta * prev_delta_b_[1];
        weights_[1] += dW2; biases_[1] += db2;
        prev_delta_W_[1] = dW2; prev_delta_b_[1] = db2;

        Eigen::MatrixXd dW1 = -lr * delta1 * a0.transpose() + beta * prev_delta_W_[0];
        Eigen::VectorXd db1 = -lr * delta1 + beta * prev_delta_b_[0];
        weights_[0] += dW1; biases_[0] += db1;
        prev_delta_W_[0] = dW1; prev_delta_b_[0] = db1;

        mse += (target - a2).squaredNorm();
    }
    return mse / N;
}

void MLPBackprop::print_predictions() const {
    std::cout << "\n[PRUEBA] Predicciones finales con pesos convergidos:\n";
    if (task_is_sin_) {
        std::cout << " Tarea: sin(x) — Muestreo de entradas/objetivos (primeros 5 y últimos 5):\n";
        for (int i = 0; i < std::min(5, (int)X_.rows()); ++i) {
            std::cout << "  x=" << std::fixed << std::setprecision(2) << X_(i,0) 
                    << " | objetivo=" << std::setprecision(4) << D_(i,0) << "\n";
        }
        std::cout << "  ... (total N=" << X_.rows() << " puntos)\n";
    } else {
        std::cout << " Tarea: XOR — Tabla de verdad (x1,x2 → y):\n";
        std::cout << "  x1 x2 | D  Ŷ  | Estado\n";
        std::cout << "  -------------------\n";
        for (int i = 0; i < 4; ++i) {
            Eigen::VectorXd a0 = X_.row(i).transpose();
            Eigen::VectorXd z1 = weights_[0] * a0 + biases_[0];
            Eigen::VectorXd a1 = z1.unaryExpr([this](double z){ return this->sigmoid(z); });
            Eigen::VectorXd z2 = weights_[1] * a1 + biases_[1];
            Eigen::VectorXd a2 = z2.unaryExpr([this](double z){ return this->sigmoid(z); });
            int pred = (a2(0) > 0.5) ? 1 : 0;
            int real = static_cast<int>(D_(i,0));
            std::cout << "  " << (int)X_(i,0) << "  " << (int)X_(i,1) 
                    << " | " << real << "  " << pred << "  | " << (pred==real?"OK":"FALLA") << "\n";
        }
    }
}


void MLPBackprop::compute_metrics() {
    int epochs = static_cast<int>(error_history_.size());
    double final_mse = error_history_.back();

    if(task_is_sin_) {
        std::cout << "[MÉTRICAS] MLP Backprop - Aproximación sin(x)\n";
        std::cout << "  - Épocas:                         " << epochs << "\n";
        std::cout << "  - MSE final (escalado):           " << std::scientific << std::setprecision(4) << final_mse << "\n";
        std::cout << "  - Observación:                    La red aproxima la curva sinusoidal en [0, 2π].\n";
    } else {
        std::cout << "[MÉTRICAS] MLP Backprop - Clasificación XOR\n";
        std::cout << "  - Épocas:                         " << epochs << "\n";
        std::cout << "  - MSE final:                      " << std::scientific << std::setprecision(4) << final_mse << "\n";
        std::cout << "  - Tabla de verdad (XOR):\n";
        for(int i=0; i<4; ++i) {
            Eigen::VectorXd a0 = X_.row(i).transpose();
            Eigen::VectorXd z1 = weights_[0] * a0 + biases_[0];
            Eigen::VectorXd a1 = z1.unaryExpr([this](double z){ return this->sigmoid(z); });
            Eigen::VectorXd z2 = weights_[1] * a1 + biases_[1];
            Eigen::VectorXd a2 = z2.unaryExpr([this](double z){ return this->sigmoid(z); });
            int pred = (a2(0) > 0.5) ? 1 : 0;
            int real = static_cast<int>(D_(i,0));
            std::cout << "   x=[" << a0.transpose() << "] → y=" << pred 
                    << " | Esperado=" << real << " | " << (pred==real ? "si" : "no") << "\n";

                    if (error_history_.size() > 0 && error_history_.back() < 0.15) { // umbral de convergencia
                std::cout << "\n✅ Convergencia aceptada (MSE < 0.15). Mostrando predicciones:\n";
                print_predictions();
            } else {
                std::cout << "\n⚠️  No alcanzó convergencia óptima. Predicciones omitidas.\n";
            }
        }
    }
}

void MLPBackprop::export_csv() const {
    std::filesystem::create_directories("output");
    std::ofstream out(params_.csv_path);
    if (!out.is_open()) {
        std::cerr << "\n Error al abrir CSV: " << params_.csv_path << "\n";
        return;
    }
    out << "Modelo,Epoca,Error\n";
    for (size_t i = 0; i < error_history_.size(); ++i) {
        out << get_model_name() << "," << (i + 1) << ","
            << std::fixed << std::setprecision(6) << error_history_[i] << "\n";
    }
    out.close();
}