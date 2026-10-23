#include "utils.hpp"
#include "ag_core.hpp"
#include "operadores.hpp"
#include "renormalizacion.hpp"
#include "exportador.hpp"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    // Parámetros por defecto
    double pc = PC_DEFAULT;
    double pm = PM_DEFAULT;
    int sims = NUM_SIMULACIONES;
    int seed = SEED_DEFAULT;
    bool grid = false;

    // Parseo de argumentos
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--pc" && i + 1 < argc) pc = std::atof(argv[++i]);
        else if (arg == "--pm" && i + 1 < argc) pm = std::atof(argv[++i]);
        else if (arg == "--sims" && i + 1 < argc) sims = std::atoi(argv[++i]);
        else if (arg == "--seed" && i + 1 < argc) seed = std::atoi(argv[++i]);
        else if (arg == "--grid") grid = true;
    }

    // LOS 3 TIPOS DE AG QUE DEBEMOS COMPARAR
    std::vector<std::string> tipos_ag = {"simple", "lineal", "ventana"};
    
    std::vector<MetricasGen> metricas_totales;
    
    // === MODO GRID (para parte c) ===
    if (grid) {
        std::vector<double> pc_values = {0.6, 0.7, 0.8, 0.9};
        std::vector<double> pm_values = {0.001, 0.005, 0.01};
        
        for (const auto& tipo : tipos_ag) {
            std::cout << "\n=== Ejecutando AG tipo: " << tipo << " ===\n";
            
            for (double test_pc : pc_values) {
                for (double test_pm : pm_values) {
                    std::cout << "  Pc=" << test_pc << ", Pm=" << test_pm << "\n";
                    
                    for (int s = 0; s < sims; ++s) {
                        auto gen = crear_generador(seed + s);
                        Poblacion pop = inicializar_poblacion(gen);
                        metricas_totales.push_back(
                            obtener_metricas(pop, s, 0, tipo, test_pc, test_pm));
                        
                        for (int g = 1; g <= MAX_GENERACIONES; ++g) {
                            pop = evolucionar_generacion(pop, test_pc, test_pm, tipo, gen);
                            metricas_totales.push_back(
                                obtener_metricas(pop, s, g, tipo, test_pc, test_pm));
                        }
                    }
                }
            }
        }
    }
    // === MODO NORMAL (para partes a y b) ===
    else {
        for (const auto& tipo : tipos_ag) {
            std::cout << "\n=== Ejecutando AG tipo: " << tipo 
                      << " (Pc=" << pc << ", Pm=" << pm << ") ===\n";
            
            for (int s = 0; s < sims; ++s) {
                auto gen = crear_generador(seed + s);
                Poblacion pop = inicializar_poblacion(gen);
                metricas_totales.push_back(
                    obtener_metricas(pop, s, 0, tipo, pc, pm));
                
                for (int g = 1; g <= MAX_GENERACIONES; ++g) {
                    pop = evolucionar_generacion(pop, pc, pm, tipo, gen);
                    metricas_totales.push_back(
                        obtener_metricas(pop, s, g, tipo, pc, pm));
                }
                
                std::cout << "  Simulacion " << s << " completada\n";
            }
        }
    }

    // Exportar TODO en un solo CSV
    exportar_csv(metricas_totales, "../resultados/results_comparativa.csv");
    std::cout << "\nResultados exportados a resultados/results_comparativa.csv\n";
    std::cout << "Total de registros: " << metricas_totales.size() << "\n";
    
    return 0;
}