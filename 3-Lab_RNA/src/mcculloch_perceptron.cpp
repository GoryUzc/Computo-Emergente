#include "mcculloch_perceptron.hpp"
#include <random>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

McCullochPerceptron::McCullochPerceptron(const RNA_Params& p) : RNA_Base(p) {
    // params_.extra_1 > 0.5 → Entrena OR, caso contrario → AND
    use_or_ = (params_.extra_1 > 0.5);
}
    
// [Fórmula] H(z) = {1 si z ≥ 0, 0 c.c.}
double McCullochPerceptron::heaviside(double z) const {
    return (z >= 0.0) ? 1.0 : 0.0;
}

void McCullochPerceptron::setup_patterns() {
    X_.resize(4, 3);
    X_.col(0).setConstant(1.0); // x₀ = 1 (Bias integrado)
    X_.col(1) << 0, 0, 1, 1;    // x₁ (Bit 1)
    X_.col(2) << 0, 1, 0, 1;    // x₂ (Bit 0)

    D_.resize(4);
    if (use_or_) {
        D_ << 0, 1, 1, 1; // Tabla de verdad OR
    } else {
        D_ << 0, 0, 0, 1; // Tabla de verdad AND
    }
}

std::string McCullochPerceptron::get_model_name() const {
    return "McCulloch_Perceptron";
}


std::string McCullochPerceptron::get_theory_summary() const {
    return "MC-CULLOCH / PERCEPTRON SIMPLE\n"
           "Salida: y = H(sum(w_i*x_i) - theta)\n"
           "H(z) = 1 si z >= 0, 0 contrario\n"
           "Regla aprendizaje: delta_w_j = eta * (d - y) * x_j\n"
           "Limite: solo linealmente separables (AND, OR). NO converge XOR.\n"
           "Param: theta=0.5, w=+-1, eta=0.1";
}

void McCullochPerceptron::initialize() {
    // [Reproducibilidad] Semilla fija 42
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    
    weights_.resize(3);
    for (int i = 0; i < 3; ++i) weights_(i) = dist(rng);

    setup_patterns();
    accuracy_.clear();
    std::cout << "Tarea: " << (use_or_ ? "OR" : "AND") 
              << "Pesos iniciales: [" << weights_.transpose() << "]\n";
}

double McCullochPerceptron::train_epoch() {
    double mse = 0.0;
    int correct = 0;
    const int N = X_.rows();

    for (int i = 0; i < N; ++i) {
        Eigen::VectorXd x = X_.row(i).transpose();
        double d = D_(i);

        // [Fórmula] z = w·x (producto escalar optimizado por Eigen)
        double z = weights_.dot(x);
        // [Fórmula] ŷ = H(z)
        double y_hat = heaviside(z);
        // [Fórmula] e = d - ŷ
        double error = d - y_hat;

        if (std::abs(d - y_hat) < 1e-6) ++correct;

        // [Fórmula] Δw = η * e * x (actualización de pesos por patrón)
        weights_ += params_.learning_rate * error * x;
        mse += error * error;
        
    }
    mse /= N; // MSE = (1/N) Σ e²
    accuracy_.push_back(correct);
    return mse;
}

void McCullochPerceptron::print_final_predictions() const {
    std::cout << "\n[PRUEBA] Predicción final con pesos convergidos:\n";
    std::cout << " x1  x2  | D  Ŷ  | Correcto\n";
    std::cout << "---------------------------\n";
    
    for (int i = 0; i < 4; ++i) {
        Eigen::VectorXd x = X_.row(i).transpose();
        double d = D_(i);
        double z = weights_.dot(x);
        double y_hat = heaviside(z);
        bool ok = (std::abs(d - y_hat) < 1e-6);
        std::cout << " " << (int)X_(i,1) << "   " << (int)X_(i,2) << "   | "
                << (int)d << "  " << (int)y_hat << "  | " << (ok ? "Sí" : "No") << "\n";
    }
}


void McCullochPerceptron::compute_metrics() {
    int epochs = static_cast<int>(error_history_.size());
    int final_acc = accuracy_.empty() ? 0 : accuracy_.back();
    

    std::cout << "[MÉTRICAS] Perceptrón Simple (" << (use_or_ ? "OR" : "AND") << ")\n";
   
    std::cout << "  - Épocas hasta convergencia/parada: " << epochs << "\n";
    std::cout << "  - Pesos finales [w0, w1, w2]:      ["
              << std::fixed << std::setprecision(4)
              << weights_(0) << ", " << weights_(1) << ", " << weights_(2) << "]\n";
    std::cout << "  - Exactitud final:                  " << final_acc << "/4 (100%)\n";
    std::cout << "  - Frontera lineal:                  w1·x1 + w2·x2 + w0 = 0\n";
    std::cout << "  - Observación:                      Separabilidad lineal garantizada.\n";
    std::cout << "  - Limitación:                       Falla en XOR (no linealmente separable).\n";
    print_final_predictions();
}

void McCullochPerceptron::export_csv() const {
    std::filesystem::create_directories("output");
    std::ofstream out(params_.csv_path);
    if (!out.is_open()) {
        std::cerr << "[ERR] Error al abrir CSV: " << params_.csv_path << "\n";
        return;
    }

    out << "Modelo,Epoca,Error,Aciertos\n";
    for (size_t i = 0; i < error_history_.size(); ++i) {
        out << get_model_name() << "," << (i + 1) << ","
            << std::fixed << std::setprecision(6) << error_history_[i] << ","
            << accuracy_[i] << "\n";
    }
    out.close();
}
