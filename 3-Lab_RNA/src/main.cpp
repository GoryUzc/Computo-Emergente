#include "RNA_Engine.hpp"
#include <iostream>
#include <string>
#include <filesystem>
#include <memory>
#include <limits>
#define NOMINMAX
#include <windows.h>
#include <algorithm>

#include "mcculloch_perceptron.hpp"
#include "adaline.hpp"
#include "mlp_backprop.hpp"
#include "kohonen_som.hpp"
#include "hopfield.hpp"

template<typename T>
T get_input(const std::string& prompt, T min_val, T max_val) {
    T value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= min_val && value <= max_val) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << "[WARN] Valor invalido. Rango: [" << min_val << ", " << max_val << "]\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

void show_menu() {
    std::cout << "\n+--------------------------------------------------+\n";
    std::cout << "|  LABORATORIO DE REDES NEURONALES ARTIFICIALES    |\n";
    std::cout << "+--------------------------------------------------+\n";
    std::cout << "|  1. McCulloch-Pitts & Perceptron Simple          |\n";
    std::cout << "|  2. ADALINE (Regla Delta)                        |\n";
    std::cout << "|  3. Perceptron Multicapa (Backpropagation)       |\n";
    std::cout << "|  4. Mapas Autoorganizados (Kohonen)              |\n";
    std::cout << "|  5. Red de Hopfield (Memoria Asociativa)         |\n";
    std::cout << "|  0. Salir                                        |\n";
    std::cout << "+--------------------------------------------------+\n";
}

int main() {
    
    SetConsoleCP(CP_UTF8); // Para mostrar correctamente acentos y caracteres especiales en Windows
    SetConsoleOutputCP(CP_UTF8); // Para mostrar correctamente acentos y caracteres especiales en Windows

    // Verificación rápida de directorio de trabajo
    if (!std::filesystem::exists("./python/plot_auto.py") || !std::filesystem::exists("./CMakeLists.txt")) {
        std::cerr << "[ERR] Ejecuta este programa desde la carpeta RAIZ del proyecto (3.1-Lab_RNA/)\n";
        std::cerr << "      No lo ejecutes desde dentro de build/.\n";
        return 1;
    }

    std::filesystem::create_directories("./output");
    std::filesystem::create_directories("./python");

    int choice;
    while (true) {
        show_menu();
        choice = get_input<int>("\nSeleccione modelo [0-5]: ", 0, 5);
        if (choice == 0) {
            std::cout << "\n>> Sesion finalizada.\n";
            break;
        }

        double lr        = get_input<double>("[*] Tasa de aprendizaje (0.001-1.0): ", 0.001, 1.0);
        int max_ep       = get_input<int>("[*] Max epocas (10-100000): ", 10, 100000);
        double threshold = get_input<double>("[*] Umbral error (1e-8 a 1.0): ", 1e-8, 1.0);

        std::string csv = "./output/rna_model_" + std::to_string(choice) + ".csv";
        RNA_Params p{lr, max_ep, threshold, csv};

        std::unique_ptr<RNA_Base> net;
        switch(choice) {
            case 1: net = std::make_unique<McCullochPerceptron>(p); break;
            case 2: net = std::make_unique<Adaline>(p); break;
            case 3: net = std::make_unique<MLPBackprop>(p); break;
            case 4: net = std::make_unique<KohonenSOM>(p); break;
            case 5: net = std::make_unique<HopfieldNet>(p); break;
            default: 
                std::cerr << " \n[ERR] Opcion no implementada.\n"; 
                continue;
        }

        try {
            net->run();
        } catch (const std::exception& e) {
            std::cerr << "\n[ERR] Error critico: " << e.what() << "\n";
        }

        std::cout << "\nPresione ENTER para continuar...";
        std::cin.get();
    }
    return 0;
}
