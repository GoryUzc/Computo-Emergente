#pragma once
#include <Eigen/Dense>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <iostream>
#include <filesystem>

struct RNA_Params {
    double learning_rate = 0.0;
    int max_epochs = 0;
    double error_threshold = 0.0;
    std::string csv_path = "./output/result.csv";
    double extra_1 = 0.0;
    double extra_2 = 0.0;
};

class RNA_Base {
protected:
    RNA_Params params_;
    std::vector<double> error_history_;

private:
           void trigger_plot() const {
        
        std::string cmd = "python .\\python\\plot_auto.py \"" + params_.csv_path + "\"";
        std::cout << "[PLOT] Lanzando: " << cmd << "\n";
        int ret = std::system(cmd.c_str());
        std::cout << "[PLOT] Python ret=" << ret << "\n";
        if (ret != 0) {
            std::cerr << "[ERR] Python no ejecutó plot. Intenta con 'py' en vez de 'python'.\n";
        }
    }

public:
    explicit RNA_Base(const RNA_Params& p) : params_(p) {}
    virtual ~RNA_Base() = default;

    virtual std::string get_model_name() const = 0;
    virtual std::string get_theory_summary() const = 0;
    virtual void initialize() = 0;
    virtual double train_epoch() = 0;
    virtual void compute_metrics() = 0;

    // SOLO esta declaración; NO duplicar con cuerpo aparte
    virtual void export_csv() const;

    void run() {
        std::cout << "[MODELO] " << get_model_name() << "\n";
        std::cout << get_theory_summary() << "\n\n";
        std::cout << "[TRAIN] Iniciando entrenamiento...\n";
        initialize();
        for(int ep = 0; ep < params_.max_epochs; ++ep) {
            double error = train_epoch();
            error_history_.push_back(error);
            if (error >= 0 &&error < params_.error_threshold) break;
        }
        compute_metrics();
        export_csv();
        std::cout << "\n[CSV]   Exportado: " << params_.csv_path << "\n";
        std::cout << "[PLOT]  Generando grafica...\n";
        trigger_plot();
    }
};

// Definición fuera de la clase para evitar C2535
inline void RNA_Base::export_csv() const {
    std::filesystem::create_directories("./output");
    std::ofstream file(params_.csv_path);
    if (!file) {
        throw std::runtime_error("[ERR] No se pudo abrir: " + params_.csv_path);
    }
    file << "Modelo,Epoca,Error\n";
    for (size_t i = 0; i < error_history_.size(); ++i) {
        file << get_model_name() << "," << (i + 1) << "," 
             << std::fixed << std::setprecision(6) << error_history_[i] << "\n";
    }
};