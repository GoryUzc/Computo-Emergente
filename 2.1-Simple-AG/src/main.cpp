#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <cstdlib>
#include "ag_core.hpp"
#include "operadores.hpp"
#include "exportador.hpp"

struct Experimento {
    std::string nombre;
    int n;
    int L;
    double Pc;
    double Pm;
    int G_max;
    bool es_real;
    int precision;
};

std::vector<MetricasGeneracion> ejecutarAGS(const Experimento& exp) {
    std::mt19937 rng(std::random_device{}());
    auto poblacion = inicializarPoblacion(exp.n, exp.L, exp.es_real, exp.precision);
    std::vector<MetricasGeneracion> metricas;

    for (int g = 0; g < exp.G_max; ++g) {
        auto padres = seleccionRuleta(poblacion, rng);
        auto hijos = cruceUnPunto(padres, exp.Pc, rng);
        mutarPoblacion(hijos, exp.Pm, rng);

        poblacion.individuos = hijos;

        for (auto& ind : poblacion.individuos) {
            ind.x_fenotipo = decodificarCromosoma(ind.cromosoma, exp.es_real, exp.precision);
            ind.fitness = evaluarFitness(ind.x_fenotipo);
        }

        double mejor = -1.0;
        double suma = 0.0;
        double mejor_x = 0.0;
        for (const auto& ind : poblacion.individuos) {
            suma += ind.fitness;
            if (ind.fitness > mejor) {
                mejor = ind.fitness;
                mejor_x = ind.x_fenotipo;
            }
        }
        metricas.push_back({g, mejor, suma / exp.n, mejor_x});
    }
    return metricas;
}

int main() {
    // ============================================
    // RUTA: desde build/Debug/ subir a la raíz
    // ============================================
    const std::string RUTA_RESULTADOS = "../resultados/";
    
    // Verificar que el directorio exista (simple)
    // Si no existe, lo creamos con system()
    system("if not exist ..\\resultados mkdir ..\\resultados");

    std::cout << "========================================\n";
    std::cout << "  ALGORITMO GENETICO SIMPLE (AGS)\n";
    std::cout << "  Maximizacion de F(x) = x^2\n";
    std::cout << "========================================\n\n";

    std::vector<Experimento> experimentos = {
        {"experimento_base",  20, 6,  0.8, 0.002, 100, false, 0},
        {"experimento_pc06",  20, 6,  0.6, 0.002, 100, false, 0},
        {"experimento_pc04",  20, 6,  0.4, 0.002, 100, false, 0},
        {"experimento_pm001", 20, 6,  0.8, 0.01,  100, false, 0},
        {"experimento_pm01",  20, 6,  0.8, 0.1,   100, false, 0},
        {"experimento_real",  50, 13, 0.8, 0.002, 100, true,  2}
    };

    std::cout << "[i] Total de experimentos: " << experimentos.size() << "\n\n";

    for (size_t idx = 0; idx < experimentos.size(); ++idx) {
        const auto& exp = experimentos[idx];
        
        std::cout << "[" << (idx + 1) << "/" << experimentos.size() << "] "
                  << exp.nombre << "\n";
        std::cout << "    n=" << exp.n << ", L=" << exp.L 
                  << ", Pc=" << exp.Pc << ", Pm=" << exp.Pm << "\n";

        auto t_inicio = std::chrono::high_resolution_clock::now();
        auto metricas = ejecutarAGS(exp);
        auto t_fin = std::chrono::high_resolution_clock::now();
        auto duracion_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t_fin - t_inicio);

        const auto& ultima = metricas.back();
        std::cout << "    -> Mejor fitness: " << ultima.mejor_fitness 
                  << " (x=" << ultima.mejor_x << ")\n";
        std::cout << "    -> Promedio final: " << ultima.fitness_promedio << "\n";
        std::cout << "    -> Tiempo: " << duracion_ms.count() << " ms\n";

        std::string ruta_csv = RUTA_RESULTADOS + exp.nombre + ".csv";
        exportarMetricas(ruta_csv, metricas);
        std::cout << "    -> CSV: " << ruta_csv << "\n\n";
    }

    std::cout << "========================================\n";
    std::cout << "  COMPLETADO - Revisa ../resultados/\n";
    std::cout << "========================================\n";

    return 0;
}