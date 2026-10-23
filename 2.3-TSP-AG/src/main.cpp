#include <iostream>
#include <vector>
#include <limits>
#include "tsp_core.hpp"
#include "ag_core.hpp"
#include "exportador.hpp"

int main() {
    std::cout << "=== ALGORITMO GENETICO PARA TSP ===" << std::endl;

    // ============================================
    // PARAMETROS CONFIGURABLES
    // ============================================
    const int N_CIUDADES = 30;
    const int TAM_POBLACION = 100;
    const double PROB_CRUCE = 0.9;
    const double PROB_MUTACION = 0.02;
    const int MAX_GENERACIONES = 200;
    const int NUM_SIMULACIONES = 30;
    const unsigned int SEED_BASE = 42;

    std::cout << "\nCiudades: " << N_CIUDADES << std::endl;
    std::cout << "Poblacion: " << TAM_POBLACION << std::endl;
    std::cout << "Generaciones: " << MAX_GENERACIONES << std::endl;
    std::cout << "Simulaciones: " << NUM_SIMULACIONES << std::endl;

    // ============================================
    // GENERAR CIUDADES ALEATORIAS
    // ============================================
    std::cout << "\nGenerando " << N_CIUDADES << " ciudades aleatorias...\n";
    auto ciudades = generar_ciudades_aleatorias(N_CIUDADES, 100.0, 100.0, SEED_BASE);

    TSP tsp(ciudades);

    // Exportar ciudades
    Exportador::exportar_ciudades("../resultados/ciudades.csv", ciudades);
    std::cout << "Ciudades exportadas a resultados/ciudades.csv\n";

    // ============================================
    // EJECUTAR 30 SIMULACIONES INDEPENDIENTES
    // ============================================
    std::vector<std::vector<MetricasGeneracion>> todas_simulaciones;
    Individuo mejor_global;
    mejor_global.distancia = std::numeric_limits<double>::max();

    for (int sim = 0; sim < NUM_SIMULACIONES; ++sim) {
        std::cout << "\n--- Simulacion " << sim + 1 << "/" << NUM_SIMULACIONES << " ---\n";

           AlgoritmoGenetico ag(tsp,
                            TAM_POBLACION,
                            PROB_CRUCE,
                            PROB_MUTACION,
                            MAX_GENERACIONES,
                            SEED_BASE + sim);

        ag.ejecutar();

        todas_simulaciones.push_back(ag.get_historial());

        // Actualizar mejor global
        if (ag.get_mejor_distancia() < mejor_global.distancia) {
            mejor_global = ag.get_mejor_individuo();
        }

        std::cout << "  Mejor distancia: " << ag.get_mejor_distancia() << std::endl;
    }

    // ============================================
    // EXPORTAR RESULTADOS
    // ============================================
    std::cout << "\n=== EXPORTANDO RESULTADOS ===\n";

    Exportador::exportar_mejor_ruta("../resultados/mejor_ruta.csv",
                                     ciudades, mejor_global);

    Exportador::exportar_historial("../resultados/historial_sim0.csv",
                                    todas_simulaciones[0]);

    Exportador::exportar_simulaciones("../resultados/simulaciones.csv",
                                        todas_simulaciones);

    std::cout << "\n=== RESULTADO FINAL ===\n";
    std::cout << "Mejor distancia global: " << mejor_global.distancia << std::endl;
    std::cout << "Mejor ruta: ";
    for (int ciudad : mejor_global.ruta) {
        std::cout << ciudad << " ";
    }
    std::cout << std::endl;

    return 0;
}