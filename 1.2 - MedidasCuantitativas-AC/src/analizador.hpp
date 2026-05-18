#ifndef ANALIZADOR_HPP
#define ANALIZADOR_HPP

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>

#include "automata1d.hpp"

// ── Estructura de exportación ──────────────────────────────────────────────
struct MetricasGeneracion {
    int generacion;
    double densidad;
    double entropiaEspacial;
    double hammingVsInicial;
};

// ── Clase estática de análisis ─────────────────────────────────────────────
class AnalizadorAC {
public:
    // 1. Parámetro de Langton (λ)
    static double parametroLangton(int regla);

    // 2. Densidad (ρ)
    static double densidad(const std::vector<int>& estado);

    // 3. Distancia de Hamming
    static double hamming(const std::vector<int>& a, const std::vector<int>& b);       // Normalizado [0,1]
    static int    hammingCrudo(const std::vector<int>& a, const std::vector<int>& b);    // Entero [0,N]

    // 4. Entropía espacial de Shannon
    static double entropiaShannon(const std::vector<int>& estado);

    // 5. Entropía temporal de Shannon
    static double entropiaTemporal(const std::vector<std::vector<int>>& historial, int columna);
    static double entropiaTemporalPromedio(const std::vector<std::vector<int>>& historial);

    // 6. Exponente de Lyapunov (aproximado, divergencia de Hamming)
    static double lyapunovAproximado(int regla, int ancho, int T, int celdaPerturbada);

    // ── Utilidades ───────────────────────────────────────────────────────────
    static void exportarCSV(const std::string& nombreArchivo,
                            const std::vector<MetricasGeneracion>& datos);
    static std::string obtenerClaseWolfram(int regla);
};

#endif // ANALIZADOR_HPP
