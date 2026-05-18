#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <iomanip>

#include "automata1d.hpp"
#include "analizador.hpp"

namespace fs = std::filesystem;

void mostrarTabla(const std::vector<MetricasGeneracion>& datos, int numFilas) {
    std::cout << "\n  Gen | Densidad | Entropia | Hamming(vs inicial)" << std::endl;
    std::cout << "  ----|----------|----------|--------------------" << std::endl;

    int total = static_cast<int>(datos.size());
    int paso  = std::max(1, total / numFilas);

    for (int i = 0; i < total; i += paso) {
        const auto& m = datos[i];
        std::cout << std::right
                  << std::setw(4) << m.generacion << " | "
                  << std::fixed << std::setprecision(6)
                  << std::setw(8) << m.densidad << " | "
                  << std::setw(8) << m.entropiaEspacial << " | "
                  << std::setw(19) << m.hammingVsInicial << '\n';
    }

    // Siempre mostrar la última generación
    if (total > 0) {
        const auto& ultimo = datos.back();
        std::cout << std::right
                  << std::setw(4) << ultimo.generacion << " | "
                  << std::fixed << std::setprecision(6)
                  << std::setw(8) << ultimo.densidad << " | "
                  << std::setw(8) << ultimo.entropiaEspacial << " | "
                  << std::setw(19) << ultimo.hammingVsInicial << '\n';
    }
}

int main() {
    const int ANCHO        = 200;
    const int GENERACIONES = 500;
    const std::vector<int> REGLAS = {0, 30, 90, 110, 184};

    std::cout << "========================================\n";
    std::cout << "  ANÁLISIS DE AUTÓMATAS CELULARES 1D\n";
    std::cout << "========================================\n\n";

    // Crear directorio de resultados si no existe
    fs::create_directories("resultados");

    for (int regla : REGLAS) {
        std::string clase = AnalizadorAC::obtenerClaseWolfram(regla);

        std::cout << "\n========================================\n";
        std::cout << "  REGLA " << regla << " - " << clase << "\n";
        std::cout << "========================================\n";

        // 1. Parámetro de Langton
        double lambda = AnalizadorAC::parametroLangton(regla);
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "-> Parametro Langton (lambda): " << lambda << "\n";
        std::cout << "   (lambda ~ 0: orden | lambda ~ 0.5: complejidad | lambda ~ 1: caos)\n";

        // 2. Evolución batch
        Automata1D ac(ANCHO, regla, true);
        ac.reiniciarCentro();

        const auto& estadoInicial = ac.getEstado();
        std::vector<MetricasGeneracion> metricas;
        metricas.reserve(GENERACIONES);

        std::cout << "-> Evolucionando " << GENERACIONES << " generaciones...\n";

        for (int t = 0; t < GENERACIONES; ++t) {
            const auto& estado = ac.getEstado();
            MetricasGeneracion m;
            m.generacion         = t;
            m.densidad           = AnalizadorAC::densidad(estado);
            m.entropiaEspacial   = AnalizadorAC::entropiaShannon(estado);
            m.hammingVsInicial   = AnalizadorAC::hamming(estado, estadoInicial);
            metricas.push_back(m);
            ac.evolucionar();
        }

        // 3. Entropía temporal promedio
        double entTempProm = AnalizadorAC::entropiaTemporalPromedio(ac.getHistorial());

        // 4. Exponente de Lyapunov
        double lyap = AnalizadorAC::lyapunovAproximado(regla, ANCHO, GENERACIONES, ANCHO / 2 + 1);

        // 5. Resumen final
        const auto& estadoFinal = ac.getEstado();
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "-> Densidad final: "               << AnalizadorAC::densidad(estadoFinal)   << "\n";
        std::cout << "-> Entropia espacial final: "       << AnalizadorAC::entropiaShannon(estadoFinal) << " bits\n";
        std::cout << "-> Entropia temporal promedio: "    << entTempProm                            << " bits\n";
        std::cout << "-> Exponente Lyapunov (aprox): "    << lyap                                   << "\n";
        std::cout << "   (lambda_L > 0: sensible a CI | lambda_L approx 0: estable | lambda_L < 0: converge)\n";

        // 6. Exportar CSV
        std::string rutaCSV = "resultados/regla_" + std::to_string(regla) + "_metricas.csv";
        AnalizadorAC::exportarCSV(rutaCSV, metricas);

        // 7. Tabla resumen
        mostrarTabla(metricas, 10);
    }

    std::cout << "\n========================================\n";
    std::cout << "  ANALISIS COMPLETADO\n";
    std::cout << "========================================\n";

    return 0;
}
